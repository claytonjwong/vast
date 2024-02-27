#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

class logger {
public:
    static enum class log_level {
        low,
        medium,
        high,
        critical,
        error
    };
    template <typename ...Args>
    static void log(log_level level, const std::string& module, Args&& ...args) {
        std::ostringstream os;
        os << to_string(level) << ": " << get_time() << " - " << module << " - ";
        (os << ... << std::forward<Args>(args));
        std::cout << os.str() << std::endl;
    }
    static std::string to_string(const log_level& level) {
        switch(level) {
            case log_level::low: return "LOW";
            case log_level::medium: return "MEDIUM";
            case log_level::high: return "HIGH";
            case log_level::critical: return "CRITICAL";
            case log_level::error: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    static std::string get_time() {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string time_string = ctime(&time);
        time_string.pop_back(); // newline
        return time_string;
    }
};
