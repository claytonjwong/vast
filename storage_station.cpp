#include <algorithm>
#include <random>
#include <sstream>
#include <string>
#include "storage_station.h"

storage_station::storage_station(const int truck_cnt, const int queue_cnt, const int simulation_hours) noexcept
        : _truck_cnt{ truck_cnt }, _queue_cnt{ queue_cnt }, _simulation_hours{ simulation_hours } {
    init();
}

void storage_station::enqueue(const TruckPtr truck) {
    std::unique_lock<std::mutex> lock(m);
    logger::log(__LINE__, __FILE__, "🛰️ enqueuing truck to storage station");
    auto [best_size, best_index] = std::make_pair(1234567890, -1); // FIXME? large constant lib + best effort linear scan for minimum queue size (which is always changing)
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::ranges::shuffle(_index, gen);
    for (auto i: _index) {
        if (best_size > _queues[i]->size()) {
            best_size = _queues[i]->size(), best_index = i;
        }
    }
    logger::log(__LINE__, __FILE__, "🛰️ enqueuing truck to storage station ",
        "-> shortest 🪣 queue[", best_index, "] of size ", _queues[best_index]->size());
    _queues[best_index]->push(truck);
}

void storage_station::init() {
    logger::log(__LINE__, __FILE__, "🛰️ initializing storage station queues");
    generate_n(back_inserter(_queues), _queue_cnt, []{ return std::make_shared<Queue>(); });
    for (auto i{ 0 }; i < _queues.size(); ++i) {
        std::thread t{ &storage_station::process, this, i };
        t.detach();
        _index.push_back(i);
    }
}

void storage_station::process(int i) {
    for (;;) {
        TruckPtr truck;
        _queues[i]->wait_and_pop(truck);
        truck->unload();
        auto j = truck->get_id();
        ++_truck_loads[j];
        ++_queue_loads[i];
        logger::log(__LINE__, __FILE__, "🛰️ unloaded 🚚 truck[", j, "] at storage station 🪣 queue[", i, "]");
    }
    assert(false && "🛰️ Error: unreachable code!  The storage_station queue process must remain running until all trucks have finished! 🛑");
}

std::string storage_station::get_metrics() const {
    std::ostringstream os;
    auto separator = std::string(123, '*');
    os << "🛰️ storage station metrics for the performance and efficiency of each mining truck 🚚 and unload station queue 🪣" << '\n'
       << separator << '\n'
       << '\n'
       << "🎮 simulation hours: " << _simulation_hours << '\n'
       << "🚚 truck count: " << _truck_cnt << '\n'
       << "🪣 queue count: " << _queue_cnt << '\n'
       << '\n';
    for (auto j{ 0 }; j < _truck_cnt; ++j) {
        auto loads = _truck_loads[j];
        auto rate = loads / double(_simulation_hours);
        os << "🚚 truck[" << j << "] unloaded " << loads << " loads of Helium-3 at a rate of " << rate << " loads per hour" << '\n';
    }
    os << '\n';
    for (auto i{ 0 }; i < _queue_cnt; ++i) {
        auto loads = _queue_loads[i];
        auto rate = loads / double(_simulation_hours);
        os << "🪣 queue[" << i << "] unloaded " << loads << " loads of Helium-3 at a rate of " << rate << " loads per hour" << '\n';
    }
    os << '\n'
       << "👍 GREAT WORK, TEAM!  CHEERS! 🍺🍻" << '\n'
       << '\n'
       << separator << '\n';
    return os.str();
}
