#include "time_keeper.h"

using namespace std::chrono_literals;

time_keeper::time_keeper(const double time_ratio) noexcept : _time_ratio{ time_ratio } {
}

std::chrono::duration<double> time_keeper::getMineTime() {
    auto duration = _getMineTime() / _time_ratio;
    logger::log(__LINE__, __FILE__, "⏰ getMineTime() = ",
        duration.count(), " hours = ", duration.count() * 3600, " seconds");
    return duration;
}

std::chrono::duration<double> time_keeper::getDriveTime() const {
    auto duration = DRIVE_TIME / _time_ratio;
    logger::log(__LINE__, __FILE__, "⏰ getDriveTime() = ",
        duration.count(), " minutes = ", duration.count() * 60, " seconds");
    return duration;
}

std::chrono::duration<double> time_keeper::getUnloadTime() const {
    auto duration = UNLOAD_TIME / _time_ratio;
    logger::log(__LINE__, __FILE__, "⏰ getUnloadTime() = ",
        duration.count(), " minutes = ", duration.count() * 60, " seconds");
    return duration;
}

std::chrono::hours time_keeper::_getMineTime() {
    // TODO: seed this random distribution generator so each truck's random time is unique
    std::uniform_int_distribution<int> distribution(MIN_MINE_TIME.count(), MAX_MINE_TIME.count());
    auto random = distribution(_generator);
    std::ostringstream os;
    logger::log(__LINE__, __FILE__, "⏰ _getMineTime() = ", random, " random hour(s) 🎲"); // TODO: FIXME provide per truck seed since its not so random right now, all trucks get the same deterministic values
    return std::chrono::hours(random);
}
