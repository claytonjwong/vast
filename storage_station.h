#pragma once

#include <memory>
#include <queue>
#include <utility>
#include <vector>
#include "threadsafe_queue.h"

class Truck;
using TruckPtr = std::shared_ptr<Truck>;
using Queue = threadsafe_queue<TruckPtr>;
using QueuePtr = std::shared_ptr<Queue>;

class storage_station {
public:
    storage_station() = delete;
    ~storage_station() = default;
    explicit storage_station(int queue_count) : _queue_count{ queue_count } {
        generate_n(back_inserter(_queues), _queue_count, []{ return std::make_shared<Queue>(); });
    }
    storage_station(const storage_station&) = delete;
    storage_station(const storage_station&&) = delete;
    storage_station& operator=(const storage_station&) = delete;
    storage_station& operator=(const storage_station&&) = delete;
    void enqueue(const TruckPtr truck) {
        // TODO: create a priority queue with custom comparator (based on min queue size)
        // for logarithmic indels (insertions/deletions)... basically take the top and pop it off
        // then push this truck onto that queue, and push the queue back onto the priority queue
    }
    void process() {
        // TODO: implement me!  create 1 thread per queue, and each thread will wait until there's something in its queue
        //       when there's a TruckPtr in the queue, pop it, invoke truck unload, and increment metrics

        // for (auto& q: _queues) {

        // }
    }
private:
    int _queue_count;
    std::vector<QueuePtr> _queues;
};
