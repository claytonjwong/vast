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
    auto SIMULATION_DURATION = parser.get<int>("duration");
    assert(0 < N);
    assert(0 < M);
    assert(0 < TIME_RATIO);
    assert(0 < SIMULATION_DURATION);
    return std::make_tuple(N, M, TIME_RATIO, SIMULATION_DURATION);
}

int main(int argc, const char* argv[]) {
    auto [N, M, TIME_RATIO, SIMULATION_DURATION] = getArgs(argc, argv);
    logger::log(__LINE__, __FILE__, "starting simulation with ",
        N, " truck(s) and ", M, " unloading queue(s) using time warp ratio ", TIME_RATIO,
        " for duration ", SIMULATION_DURATION, " hours");
    auto simulation_duration = std::chrono::seconds(SIMULATION_DURATION * 60 * 60) / TIME_RATIO;  // FIXME? multiply by 60 * 60 to convert hours to seconds
    logger::log(__LINE__, __FILE__, "simulation duration ", simulation_duration.count(), " seconds (our world's real-time)");
    std::vector<joining_thread> truck_threads;
    storage_station station(M);
    threadsafe_queue<std::shared_ptr<Truck>> unload_queue;
    auto unload_queue_work = [&unload_queue, &station, simulation_duration]() {
        logger::log(__LINE__, __FILE__, "running thread for unload_queue_work");
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        auto delta = [&now, start]{ return std::chrono::duration<double>(now() - start); }; // seconds
        while (delta() < simulation_duration) {
            std::shared_ptr<Truck> truckPtr;
            unload_queue.wait_and_pop(truckPtr);
            station.enqueue(truckPtr);
            logger::log(__LINE__, __FILE__, "runtime = ", delta().count(),
                " seconds < ", simulation_duration.count(), " simulation seconds (our world's real-time)");
        }
        logger::log(__LINE__, __FILE__, "unload_queue_work() finished simulation");
    };
    auto truck_work = [&unload_queue, simulation_duration, TIME_RATIO = TIME_RATIO]() {
        logger::log(__LINE__, __FILE__, "running thread for truck_work");
        auto truckPtr = std::make_shared<Truck>(unload_queue, TIME_RATIO);
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        auto delta = [&now, start]{ return std::chrono::duration<double>(now() - start); }; // seconds
        while (delta() < simulation_duration) {
            truckPtr->do_work();
        }
        logger::log(__LINE__, __FILE__, "truck_work() finished simulation");
    };
    joining_thread unload_queue_thread{ unload_queue_work };
    generate_n(back_inserter(truck_threads), N, [&truck_work]{ return joining_thread{ truck_work }; });
    for (auto& thread: truck_threads) {
        thread.join();
    }
    unload_queue_thread.join();
    return 0;
}
