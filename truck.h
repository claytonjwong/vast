#pragma once

#include <condition_variable>
#include <thread>
#include "common.h"
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
    Truck(int i, Queue& unload_queue, double time_ratio) noexcept;
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
    Queue& _unload_queue;
    time_keeper _time_keeper;
    mutable std::mutex _m;
    std::condition_variable _unload_ok;
    void _drive();
    void _drive_to_mine();
    void _drive_to_station();
    void _enqueue();
    void _mine();
    void _wait_for_unload_ok();
};
