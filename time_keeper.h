#pragma once

#include <chrono>
#include <random>
#include <sstream>
#include "logger.h"

using namespace std::chrono_literals;

class time_keeper {
public:
    time_keeper(double time_ratio) noexcept;
    ~time_keeper() noexcept = default;
    time_keeper(const time_keeper&) = delete;
    time_keeper(const time_keeper&&) = delete;
    time_keeper& operator=(const time_keeper&) = delete;
    time_keeper& operator=(const time_keeper&&) = delete;
    std::chrono::hours getMineTime();
    std::chrono::minutes getDriveTime();
    std::chrono::minutes getUnloadTime();
private:
    static constexpr auto MIN_MINE_TIME = 1h;
    static constexpr auto MAX_MINE_TIME = 5h;
    static constexpr auto DRIVE_TIME = 30min;
    static constexpr auto UNLOAD_TIME = 5min;
    double _time_ratio;
    std::default_random_engine _generator;
    std::chrono::hours _getMineTime();
};
