#include <chrono>
#include <iostream>
#include <string>

class logger {
public:
    enum class log_level {
        low,
        medium,
        high,
        critical,
        error
    };
    void log(log_level level, const std::string& message) {
        std::cout << __TIMESTAMP__ << std::endl;
    }
};
