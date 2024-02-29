#pragma once

#include <condition_variable>
#include <thread>
#include "logger.h"
#include "time_keeper.h"
#include "threadsafe_queue.h"

class Truck : public std::enable_shared_from_this<Truck> {
public:
    enum class State {
        Initializing,
        Driving,
        Enqueuing,
        Unloading,
        Unloaded,
        Mining,
    };
    Truck(int i, threadsafe_queue<std::shared_ptr<Truck>>& unload_queue, double time_ratio) noexcept;
    ~Truck() noexcept = default;
    Truck(const Truck&) = delete;
    Truck(const Truck&&) = delete;
    Truck& operator=(const Truck&) = delete;
    Truck& operator=(const Truck&&) = delete;
    void do_work();
    int get_id() const;
    State getState() const;
    void unload();
private:
    State  _state;
    int _id;  // unique identifier for per truck reporting metrics
    threadsafe_queue<std::shared_ptr<Truck>>& _unload_queue;
    time_keeper _time_keeper;
    mutable std::mutex m;
    std::condition_variable unload_ok;
    void drive();
    void enqueue();
    void mine();
    void wait_for_unload_ok();
};
