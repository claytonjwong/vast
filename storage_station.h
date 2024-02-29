#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include "common.h"
#include "joining_thread.h"
#include "logger.h"
#include "threadsafe_queue.h"
#include "truck.h"

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
    std::vector<QueuePtr> _queues;
    std::mutex m;
    std::vector<int> _index; // remove minimum index queue bias via randomized index shuffle before each linear scan
    void init();
    void process(int i);
    using Counter = std::unordered_map<int, int>;
    mutable Counter _truck_loads, _queue_loads;
};
