cmake_minimum_required(VERSION 3.10)
project(vast VERSION 1.0)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cmake-bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cmake-bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cmake-bin")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(${CMAKE_C_COMPILER})
message(${CMAKE_CXX_COMPILER})
add_executable(vast main.cpp)
configure_file(version.h.in version.h)
add_library(storage_station storage_station.h storage_station.cpp common.h joining_thread.h logger.h threadsafe_queue.h)
add_library(time_keeper time_keeper.h time_keeper.cpp logger.h)
add_library(truck truck.h truck.cpp common.h logger.h threadsafe_queue.h)
target_link_libraries(storage_station PRIVATE truck)
target_link_libraries(truck PRIVATE time_keeper)
target_link_libraries(vast PRIVATE storage_station truck)
target_include_directories(vast PRIVATE "${PROJECT_SOURCE_DIR}")