#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "argparse.h"
#include "joining_thread.h"
#include "logger.h"
#include "star_wars.h"
#include "storage_station.h"
#include "truck.h"

std::tuple<int, int, int, int> getArgs(int argc, const char* argv[]) {
    argparse::ArgumentParser parser("main", "argument parser");
    parser.add_argument()
        .names({"-t", "--trucks"})
        .description("Quantity of mining trucks")
        .required(true);
    parser.add_argument()
        .names({"-q", "--queues"})
        .description("Quantity of unloading queues")
        .required(true);
    parser.add_argument()
        .names({"-r", "--ratio"})
        .description("Time warp ratio (example: -r=2 is 2x speed)")
        .required(true);
    parser.add_argument()
        .names({"-d", "--duration"})
        .description("Simulation duration in hours")
        .required(true);
    parser.enable_help();
    auto error = parser.parse(argc, argv);
    if (error) {
        std::cout << error << std::endl;
        exit(1);
    }
    if (parser.exists("help")) {
        parser.print_help();
        std::cout << "    --starwars             May the force be with you" << std::endl;
        exit(0);
    }
    const auto TRUCK_CNT = parser.get<int>("trucks");
    const auto QUEUE_CNT = parser.get<int>("queues");
    const auto TIME_RATIO = parser.get<double>("ratio");
    const auto SIMULATION_HOURS = parser.get<int>("duration");
    assert(0 < TRUCK_CNT && TRUCK_CNT <= 1000);
    assert(0 < QUEUE_CNT && QUEUE_CNT <= 1000);
    assert(0 < TIME_RATIO);
    assert(0 < SIMULATION_HOURS);
    return std::make_tuple(TRUCK_CNT, QUEUE_CNT, TIME_RATIO, SIMULATION_HOURS);
}

void star_wars(int argc, const char* argv[]) {
    auto found = false;
    std::string target = "--starwars";
    for (auto i{ 0 }; i < argc; ++i)
        if (target == std::string(argv[i]))
            found = true;
    if (!found)
        return;
    std::cout << STAR_WARS << std::endl;
    for (auto i{ 0 }; /* loop forever */ ; i = (i + 1) % STAR_WARS_IMAGES.size()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        system("clear");
        std::cout << STAR_WARS_IMAGES[i] << std::endl;
    }
}

void run(int argc, const char* argv[]) {
    //
    // simulation input params
    //
    auto [TRUCK_CNT, QUEUE_CNT, TIME_RATIO, SIMULATION_HOURS] = getArgs(argc, argv);
    logger::log(__LINE__, __FILE__, "🎮 starting simulation with ",
        TRUCK_CNT, " truck(s) and ", QUEUE_CNT, " unloading queue(s) using time warp ratio ",
        TIME_RATIO, " for duration ", SIMULATION_HOURS, " hours");
    auto simulation_duration = std::chrono::seconds(SIMULATION_HOURS * 60 * 60) / TIME_RATIO;  // multiply by 60 * 60 to convert hours to seconds
    logger::log(__LINE__, __FILE__, "🎮 simulation duration ", simulation_duration.count(), " seconds (our world's real-time)");
    //
    // servers
    //
    storage_station station(TRUCK_CNT, QUEUE_CNT, SIMULATION_HOURS);
    threadsafe_queue<std::shared_ptr<Truck>> unload_queue;  // Trucks push themselves onto this emphemeral queue to the storage station, and the unload_queue thread acts as a server Mediator/Controller between the Truck and Storage Station
    auto unload_queue_work = [&] {
        logger::log(__LINE__, __FILE__, "🥡 running detached thread for unload_queue_work 🚚 ... 🚚 ... 🚚 ... 🚚 ... 🚚, ie. 🥡 and then? 🥡 and then? 🥡 and then?");
        for (;;) {
            std::shared_ptr<Truck> truckPtr;
            unload_queue.wait_and_pop(truckPtr);
            station.enqueue(truckPtr);
        }
        logger::log(__LINE__, __FILE__, "🎮 Error: running detached thread for unload_queue_work should NOT reach this code leg! 🛑");
        assert(false && "🎮 Error: unreachable code!  The unload queue must remain running until all trucks have finished! 🛑");
    };
    joining_thread unload_queue_thread{ unload_queue_work };
    unload_queue_thread.detach();
    //
    // clients
    //
    std::vector<joining_thread> truck_threads;
    auto now = std::chrono::steady_clock::now;
    auto start = now();
    auto delta = [&now](auto start) { return std::chrono::duration<double>(now() - start); }; // seconds
    auto truck_work = [&, TIME_RATIO = TIME_RATIO](auto i) {
        logger::log(__LINE__, __FILE__, "🚚 truck[", i, "] work start 🚀");
        auto truckPtr = std::make_shared<Truck>(i, unload_queue, TIME_RATIO);
        while (delta(start) < simulation_duration) {
            truckPtr->do_work();
        }
        logger::log(__LINE__, __FILE__, "🎮 simulation duration acquired, 🚚 truck[", i, "] work done 🎉");
    };
    generate_n(back_inserter(truck_threads), TRUCK_CNT, [&truck_work, i = -1]() mutable { return joining_thread{ truck_work, ++i }; });
    for (auto& thread: truck_threads) {
        thread.join();
    }
    //
    // metrics
    //
    logger::log(__LINE__, __FILE__, station.get_metrics());
}

int main(int argc, const char* argv[]) {
    star_wars(argc, argv);
    run(argc, argv);
    return 0;
}
