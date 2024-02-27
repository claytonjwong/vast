#pragma once

#include <algorithm>
#include <chrono>
// #include <execution>
#include <memory>
#include <queue>
#include <utility>
#include <vector>
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
        // for now, just immediately unload each truck
        truck->unload();
        // TODO: create a priority queue with custom comparator (based on min queue size)
        // for logarithmic indels (insertions/deletions)... basically take the top and pop it off
        // then push this truck onto that queue, and push the queue back onto the priority queue
    }
    void init() {
        logger::log(logger::log_level::low, "storage_station", "initializing storage station queues");
        generate_n(back_inserter(_queues), _queue_count, []{ return std::make_shared<Queue>(); });
    }
    void process() {
        logger::log(logger::log_level::low, "storage_station", "process storage station queues");
        // TODO: process queues in parallel... maybe just create 1 thread per queue?

        // std::for_each(std::execution::par, _queues.begin(), _queues.end(), [](auto& queue) {
        //     if (!queue->empty()) {
        //         std::shared_ptr<Truck> truckPtr;
        //         queue->wait_and_pop(truckPtr);
        //         truckPtr->unload();
        //     }
        // });
    }
private:
    int _queue_count;
    std::vector<QueuePtr> _queues;
};
