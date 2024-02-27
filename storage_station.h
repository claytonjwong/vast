#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <queue>
#include <utility>
#include <vector>
#include "joining_thread.h"
#include "logger.h"
#include "threadsafe_queue.h"
#include "truck.h"

using TruckPtr = std::shared_ptr<Truck>;
using Queue = threadsafe_queue<TruckPtr>;
using QueuePtr = std::shared_ptr<Queue>;

class storage_station {
public:
    storage_station() = delete;
    ~storage_station() noexcept = default;
    explicit storage_station(int queue_count) noexcept
            : _queue_count{ queue_count } {
        init();
    }
    storage_station(const storage_station&) = delete;
    storage_station(const storage_station&&) = delete;
    storage_station& operator=(const storage_station&) = delete;
    storage_station& operator=(const storage_station&&) = delete;
    void enqueue(const TruckPtr truck) {
        logger::log(logger::log_level::low, "storage_station", "enqueuing truck to storage station");
        auto [best_size, best_index] = std::make_pair(1234567890, -1);
        for (auto i{ 0 }; i < _queues.size(); ++i) {
            if (best_size > _queues[i]->size()) {
                best_size = _queues[i]->size(), best_index = i;
            }
        }
        logger::log(logger::log_level::low, "storage_station",
            "enqueuing truck to storage station -> shortest queue[", best_index, "] of size ",
            _queues[best_index]->size());
        _queues[best_index]->push(truck);
    }
    void init() {
        logger::log(logger::log_level::low, "storage_station", "initializing storage station queues");
        generate_n(back_inserter(_queues), _queue_count, []{ return std::make_shared<Queue>(); });
        for (auto i{ 0 }; i < _queues.size(); ++i) {
            std::thread t{ &storage_station::process, this, i };
            t.detach();
        }
    }
    void process(int i) {
        logger::log(logger::log_level::low, "storage_station", "process storage station queue ", i);
        for (;;) {
            TruckPtr truckPtr;
            _queues[i]->wait_and_pop(truckPtr);
            truckPtr->unload();
        }
    }
private:
    int i, _queue_count;
    std::vector<QueuePtr> _queues;
};
