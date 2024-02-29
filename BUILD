load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "logger",
    hdrs = [
        "logger.h",
    ],
)

cc_library(
    name = "argparse",
    hdrs = [
        "argparse.h",
    ],
)

cc_library(
    name = "joining_thread",
    hdrs = [
        "joining_thread.h",
    ],
)

cc_library(
    name = "threadsafe_queue",
    hdrs = [
        "threadsafe_queue.h",
    ],
)

cc_library(
    name = "time_keeper",
    hdrs = [
        "time_keeper.h",
    ],
    srcs = [
        "time_keeper.cpp",
    ],
    deps = [
        ":logger",
    ],
)

cc_library(
    name = "truck",
    hdrs = [
        "truck.h",
    ],
    deps = [
        ":logger",
        ":time_keeper",
    ],
)

cc_library(
    name = "storage_station",
    hdrs = [
        "storage_station.h",
    ],
    srcs = [
        "storage_station.cpp",
    ],
    deps = [
        ":joining_thread",
        ":logger",
        ":threadsafe_queue",
        ":truck",
    ],
)

cc_library(
    name = "star_wars",
    hdrs = [
        "star_wars.h",
    ],
)

cc_binary(
    name = "main",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":argparse",
        ":joining_thread",
        ":logger",
        ":star_wars",
        ":storage_station",
        ":threadsafe_queue",
        ":truck",
    ],
)

cc_test(
  name = "joining_thread_test",
  size = "small",
  srcs = ["joining_thread_test.cpp"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
  name = "logger_test",
  size = "small",
  srcs = ["logger_test.cpp"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
  name = "storage_station_test",
  size = "small",
  srcs = ["storage_station_test.cpp"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
  name = "threadsafe_queue_test",
  size = "small",
  srcs = ["threadsafe_queue_test.cpp"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
  name = "time_keeper_test",
  size = "small",
  srcs = ["time_keeper_test.cpp"],
  deps = ["@com_google_googletest//:gtest_main"],
)
