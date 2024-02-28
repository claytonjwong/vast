#include "storage_station.h"

storage_station::storage_station(int queue_count) noexcept
        : _queue_count{ queue_count } {
    init();
}

void storage_station::enqueue(const TruckPtr truck) {
    logger::log(__LINE__, __FILE__, "enqueuing truck to storage station");
    auto [best_size, best_index] = std::make_pair(1234567890, -1);
    for (auto i{ 0 }; i < _queues.size(); ++i) {
        if (best_size > _queues[i]->size()) {
            best_size = _queues[i]->size(), best_index = i;
        }
    }
    logger::log(__LINE__, __FILE__,
        "enqueuing truck to storage station -> shortest queue[", best_index, "] of size ",
        _queues[best_index]->size());
    _queues[best_index]->push(truck);
}

void storage_station::init() {
    logger::log(__LINE__, __FILE__, "initializing storage station queues");
    generate_n(back_inserter(_queues), _queue_count, []{ return std::make_shared<Queue>(); });
    for (auto i{ 0 }; i < _queues.size(); ++i) {
        std::thread t{ &storage_station::process, this, i };
        t.detach();
    }
}

void storage_station::process(int i) {
    logger::log(__LINE__, __FILE__, "process storage station queue ", i);
    for (;;) {
        TruckPtr truckPtr;
        _queues[i]->wait_and_pop(truckPtr);
        truckPtr->unload();
    }
}
