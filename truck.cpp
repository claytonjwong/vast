#include "truck.h"

Truck::Truck(int i, threadsafe_queue<std::shared_ptr<Truck>>& unload_queue, double time_ratio) noexcept
        : _state{ Truck::State::Initializing }, _id{ i }, _unload_queue{ unload_queue }, _time_keeper{ time_ratio } {
}

int Truck::get_id() const {
    return _id;
}

void Truck::do_work() {
    mine();     // initially each truck is already at a mining site
    drive();    // drive from mine site to unloading station
    enqueue();  // storage station queue
    wait_for_unload_ok();  // the storage station will eventually dequeue this truck and invoke unload, so wait until that's done
    drive();   // drive from unloading station to mine site
}

void Truck::unload() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] unloading begin");
    _state = State::Unloading;
    auto wait = _time_keeper.getUnloadTime();
    std::this_thread::sleep_for(wait);
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] unloading end");
    _state = State::Unloaded;
    unload_ok.notify_one();
}

Truck::State Truck::getState() const {
    return _state;
}

void Truck::enqueue() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] enqueue() begin");
    _state = State::Enqueuing;
    _unload_queue.push(shared_from_this());
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] enqueue() end");
}

void Truck::wait_for_unload_ok() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] wait_for_unload_ok() begin");
    std::unique_lock<std::mutex> lock(m);
    unload_ok.wait(lock, [this]{ return _state == State::Unloaded; });
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] wait_for_unload_ok() end");
}

void Truck::drive() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] drive() begin");
    _state = State::Driving;
    auto wait = _time_keeper.getDriveTime();
    std::this_thread::sleep_for(wait);
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] drive() end");
}

void Truck::mine() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] mine() begin");
    _state = State::Mining;
    auto wait = _time_keeper.getMineTime();
    std::this_thread::sleep_for(wait);
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] mine() end");
}
