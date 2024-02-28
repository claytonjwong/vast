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

using namespace std::chrono_literals;

static constexpr auto SIMULATION_DURATION = 72h;

std::tuple<int, int, int> getArgs(int argc, const char* argv[]) {
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
        .description("time warp ratio")
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
    auto N = parser.get<int>("trucks");
    auto M = parser.get<int>("queues");
    auto R = parser.get<double>("ratio");
    assert(0 < N);
    assert(0 < M);
    assert(0 < R);
    return std::make_tuple(N, M, R);
}

int main(int argc, const char* argv[]) {
    auto [N, M, TIME_RATIO] = getArgs(argc, argv);
    logger::log(logger::log_level::low, "main", "starting simulation with ",
        N, " truck(s) and ", M, " unloading queue(s) using time warp ratio ", TIME_RATIO);
    auto simulation_duration = SIMULATION_DURATION / TIME_RATIO;
    std::vector<joining_thread> truck_threads;
    storage_station station(M);
    threadsafe_queue<std::shared_ptr<Truck>> unload_queue;
    auto unload_queue_work = [&unload_queue, &station, simulation_duration]() {
        logger::log(logger::log_level::low, "main", "running thread for unload_queue_work");
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        while (now() - start < simulation_duration) {
            std::shared_ptr<Truck> truckPtr;
            unload_queue.wait_and_pop(truckPtr);
            station.enqueue(truckPtr);
        }
        logger::log(logger::log_level::low, "main", "unload_queue_work() finished simulation");
    };
    auto truck_work = [&unload_queue, simulation_duration, TIME_RATIO = TIME_RATIO]() {
        logger::log(logger::log_level::low, "main", "running thread for truck_work");
        auto truckPtr = std::make_shared<Truck>(unload_queue, TIME_RATIO);
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        while (now() - start < simulation_duration) {
            truckPtr->do_work();
        }
        logger::log(logger::log_level::low, "main", "truck_work() finished simulation");
    };
    joining_thread unload_queue_thread{ unload_queue_work };
    generate_n(back_inserter(truck_threads), N, [&truck_work]{ return joining_thread{ truck_work }; });
    return 0;
}
