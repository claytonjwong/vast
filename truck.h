#pragma once

#include <condition_variable>
#include <thread>
#include "logger.h"
#include "time_keeper.h"

class Truck : public std::enable_shared_from_this<Truck> {
public:
    Truck(threadsafe_queue<std::shared_ptr<Truck>>& unload_queue, double time_ratio) noexcept
            : _unload_queue{ unload_queue }, _time{ time_ratio } {
    }
    ~Truck() noexcept = default;
    Truck(const Truck&) = delete;
    Truck(const Truck&&) = delete;
    Truck& operator=(const Truck&) = delete;
    Truck& operator=(const Truck&&) = delete;
    void do_work() {
        mine();     // initially each truck is already at a mining site
        drive();    // drive from mine site to unloading station
        enqueue();  // storage station queue
        wait_for_unload_ok();  // the storage station will eventually dequeue this truck and invoke unload, so wait until that's done
        drive();   // drive from unloading station to mine site
    }
    void unload() {
        logger::log(__LINE__, __FILE__, "unloading begin");
        _state = State::Unloading;
        auto wait = _time.getUnloadTime();
        std::this_thread::sleep_for(wait);
        logger::log(__LINE__, __FILE__, "unloading end");
        _state = State::Unloaded;
        unload_ok.notify_one();
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
    enum class State {
        Driving,
        Enqueuing,
        Unloading,
        Unloaded,
        Mining,
    } _state;
    State getState() const {
        return _state;
    }
private:
    threadsafe_queue<std::shared_ptr<Truck>>& _unload_queue;
    time_keeper _time;
    mutable std::mutex m;
    std::condition_variable unload_ok;
    void enqueue() {
        logger::log(__LINE__, __FILE__, "enqueue() begin");
        _state = State::Enqueuing;
        _unload_queue.push(shared_from_this());
        logger::log(__LINE__, __FILE__, "enqueue() end");
    }
    void wait_for_unload_ok() {
        logger::log(__LINE__, __FILE__, "wait_for_unload_ok() begin");
        std::unique_lock<std::mutex> lock(m);
        unload_ok.wait(lock, [this]{ return _state == State::Unloaded; });
        logger::log(__LINE__, __FILE__, "wait_for_unload_ok() end");
    }
    void drive() {
        logger::log(__LINE__, __FILE__, "drive() begin");
        _state = State::Driving;
        auto wait = _time.getDriveTime();
        std::this_thread::sleep_for(wait);
        logger::log(__LINE__, __FILE__, "drive() end");
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
    void mine() {
        logger::log(__LINE__, __FILE__, "mine() begin");
        _state = State::Mining;
        auto wait = _time.getMineTime();
        std::this_thread::sleep_for(wait);
        logger::log(__LINE__, __FILE__, "mine() end");
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
};
