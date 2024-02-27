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
    explicit storage_station(int queue_count) noexcept;
    storage_station(const storage_station&) = delete;
    storage_station(const storage_station&&) = delete;
    storage_station& operator=(const storage_station&) = delete;
    storage_station& operator=(const storage_station&&) = delete;
    void enqueue(const TruckPtr truck);
private:
    int _queue_count;
    std::vector<QueuePtr> _queues;
    void init();
    void process(int i);
};
