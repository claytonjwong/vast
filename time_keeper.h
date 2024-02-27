#pragma once

#include <chrono>
#include <random>
#include <sstream>
#include "logger.h"

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
        auto result = std::chrono::duration_cast<std::chrono::hours>(std::chrono::duration<double>(time));
        logger::log(logger::log_level::low, "time_keeper", "getMineTime() raw time = ", time.count());
        logger::log(logger::log_level::low, "time_keeper", "getMineTime() = ", result.count());
        return result;
    }
    std::chrono::minutes getDriveTime() {
        auto time = DRIVE_TIME * _time_ratio;
        auto result = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
        logger::log(logger::log_level::low, "time_keeper", "getDriveTime() raw time = ", time.count());
        logger::log(logger::log_level::low, "time_keeper", "getDriveTime() = ", result.count());
        return result;
    }
    std::chrono::minutes getUnloadTime() {
        auto time = UNLOAD_TIME * _time_ratio;
        auto result = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
        logger::log(logger::log_level::low, "time_keeper", "getUnloadTime() raw time = ", time.count());
        logger::log(logger::log_level::low, "time_keeper", "getUnloadTime() = ", result.count());
        return result;
    }
private:
    static constexpr auto MIN_MINE_TIME = 1h;
    static constexpr auto MAX_MINE_TIME = 5h;
    static constexpr auto DRIVE_TIME = 30min;
    static constexpr auto UNLOAD_TIME = 5min;
    double _time_ratio;
    std::default_random_engine _generator;
    std::chrono::hours _getMineTime() {
        std::uniform_int_distribution<int> distribution(MIN_MINE_TIME.count(), MAX_MINE_TIME.count());
        auto random = distribution(_generator);
        std::ostringstream os;
        logger::log(logger::log_level::low, "time_keeper", "_getMineTime() = ", random);
        return std::chrono::hours(random);
    }
};
