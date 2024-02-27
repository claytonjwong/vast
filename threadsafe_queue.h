#pragma once

#include <exception>
#include <condition_variable>
#include <mutex>
#include <queue>

struct empty_queue : std::exception {
    const char* what() const throw() {
        return "queue is empty";
    }
};

template <typename T>
class threadsafe_queue {
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable ok;
public:
    threadsafe_queue() noexcept = default;
    ~threadsafe_queue() noexcept = default;
    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue(const threadsafe_queue&&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&&) = delete;
    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        q.push(new_value);
        ok.notify_one();
    }
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m);
        ok.wait(lock, [this]{ return !q.empty(); });
        value = q.front(), q.pop();
    }
    size_t size() const {
        return q.size();
    }
    bool empty() const {
        return q.empty();
    }
};
