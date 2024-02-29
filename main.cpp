#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "argparse.h"
#include "joining_thread.h"
#include "logger.h"
#include "storage_station.h"
#include "truck.h"

std::tuple<int, int, int, int> getArgs(int argc, const char* argv[]) {
    argparse::ArgumentParser parser("main", "argument parser");
    parser.add_argument()
        .names({"-t", "--trucks"})
        .description("quantity of mining trucks")
        .required(true);
    parser.add_argument()
        .names({"-q", "--queues"})
        .description("quantity of unloading queues")
        .required(true);
    parser.add_argument()
        .names({"-r", "--ratio"})
        .description("time warp ratio (example: -r=2 is 2x speed)")
        .required(true);
    parser.add_argument()
        .names({"-d", "--duration"})
        .description("simulation duration in hours")
        .required(true);
    parser.enable_help();
    auto error = parser.parse(argc, argv);
    if (error) {
        std::cout << error << std::endl;
        exit(1);
    }
    if (parser.exists("help")) {
        parser.print_help();
        exit(0);
    }
    auto N = parser.get<int>("trucks");  // TODO: consider upper limit assertion, since we spawn a thread for each truck
    auto M = parser.get<int>("queues");  // TODO: consider upper limit assertion, since we spawn a thread for each queue
    auto TIME_RATIO = parser.get<double>("ratio");
    auto SIMULATION_HOURS = parser.get<int>("duration");
    assert(0 < N);
    assert(0 < M);
    assert(0 < TIME_RATIO);
    assert(0 < SIMULATION_HOURS);
    return std::make_tuple(N, M, TIME_RATIO, SIMULATION_HOURS);
}

int main(int argc, const char* argv[]) {
    auto [N, M, TIME_RATIO, SIMULATION_HOURS] = getArgs(argc, argv);
    logger::log(__LINE__, __FILE__, "🎮 starting simulation with ",
        N, " truck(s) and ", M, " unloading queue(s) using time warp ratio ", TIME_RATIO,
        " for duration ", SIMULATION_HOURS, " hours");
    auto simulation_duration = std::chrono::seconds(SIMULATION_HOURS * 60 * 60) / TIME_RATIO;  // FIXME? multiply by 60 * 60 to convert hours to seconds
    logger::log(__LINE__, __FILE__, "🎮 simulation duration ", simulation_duration.count(), " seconds (our world's real-time)");
    std::vector<joining_thread> truck_threads;
    storage_station station(N, M, SIMULATION_HOURS);
    threadsafe_queue<std::shared_ptr<Truck>> unload_queue;  // Trucks push themselves onto this emphemeral queue to the storage station
    auto unload_queue_work = [&] {
        logger::log(__LINE__, __FILE__, "🗑️ running detached thread for unload_queue_work 🔨");
        for (;;) {
            std::shared_ptr<Truck> truckPtr;
            unload_queue.wait_and_pop(truckPtr);
            station.enqueue(truckPtr);
        }
        logger::log(__LINE__, __FILE__, "🎮 Error: running detached thread for unload_queue_work should NOT reach this code leg! 🛑");
        assert(false && "🎮 Error: unreachable code!  The unload queue must remain running until all trucks have finished! 🛑");
    };
    auto now = std::chrono::steady_clock::now;
    auto start = now();
    auto delta = [&now](auto start) { return std::chrono::duration<double>(now() - start); }; // seconds
    auto truck_work = [&, TIME_RATIO = TIME_RATIO](auto i) {
        logger::log(__LINE__, __FILE__, "🚚 truck[", i, "] work start 🚀");
        auto truckPtr = std::make_shared<Truck>(i, unload_queue, TIME_RATIO);
        while (delta(start) < simulation_duration) {
            truckPtr->do_work();
        }
        logger::log(__LINE__, __FILE__, "🚚 truck[", i, "] work done 🎉 cheers! 🍺🍻");
    };
    joining_thread unload_queue_thread{ unload_queue_work };
    unload_queue_thread.detach();
    generate_n(back_inserter(truck_threads), N, [&truck_work, i = -1]() mutable { return joining_thread{ truck_work, ++i }; });
    for (auto& thread: truck_threads) {
        thread.join();
    }
    logger::log(__LINE__, __FILE__, station.get_metrics());
    return 0;
}
