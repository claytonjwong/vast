#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "logger.h"
#include "storage_station.h"
#include "truck.h"

using namespace std::chrono_literals;

static constexpr auto N = 1;  // trucks
static constexpr auto M = 1;  // unload station queues

static constexpr auto TIME_RATIO = 0.5;
static constexpr auto SIMULATION_DURATION = 72h;

int main() {
    // TODO: get logger working for nicely formatted output
    // logger output;
    // output.log(logger::log_level::low, "Hello world!");
    std::cout << "Hello world!" << std::endl;
    auto simulation_duration = SIMULATION_DURATION * TIME_RATIO;
    std::vector<std::thread> truck_threads;
    storage_station station(M);
    threadsafe_queue<std::shared_ptr<Truck>> unload_queue;
    auto unload_queue_work = [&unload_queue, &station, simulation_duration]() {
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        while (now() - start < simulation_duration) {
            std::shared_ptr<Truck> truckPtr;
            unload_queue.wait_and_pop(truckPtr);
            station.enqueue(truckPtr);
        }
    };
    std::thread unload_queue_thread{ unload_queue_work };
    auto truck_work = [&unload_queue, simulation_duration]() {
        Truck truck{ unload_queue, TIME_RATIO };
        auto now = std::chrono::steady_clock::now;
        auto start = now();
        while (now() - start < simulation_duration) {
            truck.do_work();
        }
    };
    generate_n(back_inserter(truck_threads), N, [&]{ return std::thread{ truck_work }; });
    for (auto& thread: truck_threads) {
        thread.join();
    }
    unload_queue_thread.join();
    return 0;
}
