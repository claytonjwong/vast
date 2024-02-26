#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>
#include "storage_station.h"
#include "truck.h"

using namespace std::chrono_literals;

static constexpr auto N = 3;  // trucks
static constexpr auto M = 2;  // unload station queues

static constexpr auto TIME_RATIO = 0.5;
static constexpr auto SIMULATION_DURATION = 72h;

int main() {
    auto simulation_duration = SIMULATION_DURATION * TIME_RATIO;
    storage_station station{ M };
    std::vector<std::thread> truck_threads;
    auto truck_work = [&station, simulation_duration]() {
        Truck truck{ station, TIME_RATIO };
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
    return 0;
}
