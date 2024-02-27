#include "time_keeper.h"

using namespace std::chrono_literals;

time_keeper::time_keeper(double time_ratio) noexcept : _time_ratio{ time_ratio } {
}

std::chrono::hours time_keeper::getMineTime() {
    auto time = _getMineTime() * _time_ratio;
    auto result = std::chrono::duration_cast<std::chrono::hours>(std::chrono::duration<double>(time));
    logger::log(logger::log_level::low, "time_keeper", "getMineTime() raw time = ", time.count());
    logger::log(logger::log_level::low, "time_keeper", "getMineTime() = ", result.count());
    return result;
}

std::chrono::minutes time_keeper::getDriveTime() {
    auto time = DRIVE_TIME * _time_ratio;
    auto result = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
    logger::log(logger::log_level::low, "time_keeper", "getDriveTime() raw time = ", time.count());
    logger::log(logger::log_level::low, "time_keeper", "getDriveTime() = ", result.count());
    return result;
}

std::chrono::minutes time_keeper::getUnloadTime() {
    auto time = UNLOAD_TIME * _time_ratio;
    auto result = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(time));
    logger::log(logger::log_level::low, "time_keeper", "getUnloadTime() raw time = ", time.count());
    logger::log(logger::log_level::low, "time_keeper", "getUnloadTime() = ", result.count());
    return result;
}

std::chrono::hours time_keeper::_getMineTime() {
    std::uniform_int_distribution<int> distribution(MIN_MINE_TIME.count(), MAX_MINE_TIME.count());
    auto random = distribution(_generator);
    std::ostringstream os;
    logger::log(logger::log_level::low, "time_keeper", "_getMineTime() = ", random);
    return std::chrono::hours(random);
}
