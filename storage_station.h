#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>
#include "joining_thread.h"
#include "logger.h"
#include "threadsafe_queue.h"
#include "truck.h"

// TODO: move to common definitions header?
using TruckPtr = std::shared_ptr<Truck>;
using Queue = threadsafe_queue<TruckPtr>;
using QueuePtr = std::shared_ptr<Queue>;

class storage_station {
public:
    storage_station() = delete;
    ~storage_station() noexcept = default;
    storage_station(const int truck_cnt, const int queue_cnt, const int simulation_hours) noexcept;
    storage_station(const storage_station&) = delete;
    storage_station(const storage_station&&) = delete;
    storage_station& operator=(const storage_station&) = delete;
    storage_station& operator=(const storage_station&&) = delete;
    void enqueue(const TruckPtr truck);
    std::string get_metrics() const;
private:
    const int _truck_cnt, _queue_cnt, _simulation_hours;
    std::default_random_engine _generator;
    std::mutex m;
    std::vector<QueuePtr> _queues;
    void init();
    void process(int i);
    using Counter = std::unordered_map<int, int>;
    mutable Counter _truck_loads, _queue_loads;
};
