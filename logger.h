#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

class logger {
public:
    template <typename ...Args>
    static void log(int line, const std::string& filename, Args&& ...args) {
        std::ostringstream os;
        os << get_time() << " line " << line << " " << filename << " - ";
        (os << ... << std::forward<Args>(args)) << '\n';
        std::cout << os.str(), flush(std::cout);
    }
    static std::string get_time() {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string time_string = ctime(&time);
        time_string.pop_back(); // newline
        return time_string;
    }
};
