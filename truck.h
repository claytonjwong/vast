#pragma once

#include <thread>
#include "storage_station.h"
#include "time_keeper.h"

class Truck : public std::enable_shared_from_this<Truck> {
public:
    Truck(storage_station& station, double time_ratio) : _station{ station }, _time{ time_ratio } {
    }
    ~Truck() = default;
    Truck(const Truck&) = delete;
    Truck(const Truck&&) = delete;
    Truck& operator=(const Truck&) = delete;
    Truck& operator=(const Truck&&) = delete;
    void do_work() {
        mine();    // initially each truck is already at a mining site
        drive();   // drive from mine site to unloading station
        unload();  // unload truck contents of Helium-3
        drive();   // drive from unloading station to mine site
    }
    enum class State {
        Driving,
        Unloading,
        Mining,
    } _state;
    State getState() const {
        return _state;
    }
private:
    storage_station& _station;
    time_keeper _time;
    void enqueue() {
        _station.enqueue(shared_from_this());
    }
    void drive() {
        _state = State::Driving;
        auto wait = _time.getDriveTime();
        std::this_thread::sleep_for(wait);
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
    void unload() {
        _state = State::Unloading;
        auto wait = _time.getUnloadTime();
        std::this_thread::sleep_for(wait);
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
    void mine() {
        _state = State::Mining;
        auto wait = _time.getMineTime();
        std::this_thread::sleep_for(wait);
        // TODO: invoke logging module for performance metrics (how long each truck is in each state)
        //       actually, maybe just notify a listener?
    }
};
