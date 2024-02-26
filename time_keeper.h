#pragma once

#include <chrono>
#include <random>

using namespace std::chrono_literals;

class time_keeper {
public:
    time_keeper(double time_ratio) : _time_ratio{ time_ratio } {
    }
    ~time_keeper() = default;
    time_keeper(const time_keeper&) = delete;
    time_keeper(const time_keeper&&) = delete;
    time_keeper& operator=(const time_keeper&) = delete;
    time_keeper& operator=(const time_keeper&&) = delete;
    std::chrono::hours getMineTime() {
        auto time = _getMineTime() * _time_ratio;
        return std::chrono::duration_cast<std::chrono::hours>(std::chrono::duration<double>(time));
    }
    std::chrono::minutes getDriveTime() {
        auto time = DRIVE_TIME * _time_ratio;
        return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
    }
    std::chrono::minutes getUnloadTime() {
        auto time = UNLOAD_TIME * _time_ratio;
        return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
    }
    void setScale(double time_ratio) {  // to enable time-warping to speed up the simulation
        _time_ratio = time_ratio;
    }
private:
    static constexpr auto MIN_MINING_TIME = 1h;
    static constexpr auto MAX_MINING_TIME = 5h;
    static constexpr auto DRIVE_TIME = 30min;
    static constexpr auto UNLOAD_TIME = 5min;
    double _time_ratio;
    std::default_random_engine _generator;
    double _getMineTime() {
        std::uniform_real_distribution<double> distribution(MIN_MINING_TIME.count(), MAX_MINING_TIME.count());
        auto random = distribution(_generator);
        return random;
    }
};
