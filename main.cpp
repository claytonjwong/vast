#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "joining_thread.h"
#include "logger.h"
#include "storage_station.h"
#include "truck.h"

using namespace std::chrono_literals;

static constexpr auto N = 1;  // trucks
static constexpr auto M = 1;  // unload station queues

static constexpr auto TIME_RATIO = 0.05;
static constexpr auto SIMULATION_DURATION = 72h;

int main() {
    logger::log(logger::log_level::low, "main", "starting simulation");
    auto simulation_duration = SIMULATION_DURATION * TIME_RATIO;
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
    };
    auto truck_work = [&unload_queue, simulation_duration]() {
        logger::log(logger::log_level::low, "main", "running thread for truck_work");
        Truck truck{ unload_queue, TIME_RATIO };
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        while (now() - start < simulation_duration) {
            truck.do_work();
        }
    };
    joining_thread unload_queue_thread{ unload_queue_work };
    generate_n(back_inserter(truck_threads), N, [&truck_work]{ return joining_thread{ truck_work }; });
    return 0;
}
