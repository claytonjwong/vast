load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "threadsafe_queue",
    hdrs = ["threadsafe_queue.h"],
)

cc_library(
    name = "time_keeper",
    hdrs = ["time_keeper.h"],
)

cc_library(
    name = "truck",
    hdrs = ["truck.h"],
    deps = [
        ":time_keeper",
        ":storage_station",
    ],
)

cc_library(
    name = "storage_station",
    hdrs = ["storage_station.h"],
    deps = [
        ":threadsafe_queue",
    ],
)

cc_library(
    name = "logger",
    hdrs = ["logger.h"],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":logger",
        ":storage_station",
        ":threadsafe_queue",
        ":truck",
    ],
)
