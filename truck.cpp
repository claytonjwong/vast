#include "truck.h"

Truck::Truck(int i, threadsafe_queue<std::shared_ptr<Truck>>& unload_queue, double time_ratio) noexcept
        : _state{ Truck::State::Initializing }, _id{ i }, _unload_queue{ unload_queue }, _time_keeper{ time_ratio } {
}

void Truck::do_work() {
    _mine();                // 1. initially each truck is already at a mining site, so we begin by mining
    _drive_to_station();    // 2. drive from mine site to unloading station
    _enqueue();             // 3. storage station queue (emphemeral, meta queue, ie. its a single parent queue into storage station's children queues)
    _wait_for_unload_ok();  // 4. the storage station will eventually enqueue and dequeue this truck, then invoke unload upon it, so wait until that's done
    _drive_to_mine();       // 5. drive from unloading station to mine site
}

void Truck::_drive_to_mine() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] _drive_to_mine() begin");
    _drive();
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] _drive_to_mine() end");
}

void Truck::_drive_to_station() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] _drive_to_station() begin");
    _drive();
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] _drive_to_station() end");
}

void Truck::_drive() {
    _state = State::Driving;
    auto wait = _time_keeper.getDriveTime();
    std::this_thread::sleep_for(wait);
}

void Truck::_enqueue() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] enqueue() begin");
    _state = State::Enqueuing;
    _unload_queue.push(shared_from_this());
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] enqueue() end");
}

int Truck::get_id() const {
    return _id;
}

Truck::State Truck::getState() const {
    return _state;
}

void Truck::_mine() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] mine() begin");
    _state = State::Mining;
    auto wait = _time_keeper.getMineTime();
    std::this_thread::sleep_for(wait);
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] mine() end");
}

void Truck::unload() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] unloading begin");
    _state = State::Unloading;
    auto wait = _time_keeper.getUnloadTime();
    std::this_thread::sleep_for(wait);
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] unloading end");
    _state = State::Unloaded;
    _unload_ok.notify_one();
}

void Truck::_wait_for_unload_ok() {
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] wait_for_unload_ok() begin");
    std::unique_lock<std::mutex> lock(_m);
    _unload_ok.wait(lock, [this]{ return _state == State::Unloaded; });
    logger::log(__LINE__, __FILE__, "🚚 truck[", get_id(), "] wait_for_unload_ok() end");
}
