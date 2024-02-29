#pragma once

#include <memory>
#include "threadsafe_queue.h"

class Truck; // forward declaration so truck can use this header file without this header file needing to include "truck.h" (ie. avoid mutual dependence since it doesn't compile!)
using TruckPtr = std::shared_ptr<Truck>;
using Queue = threadsafe_queue<TruckPtr>;
using QueuePtr = std::shared_ptr<Queue>;
