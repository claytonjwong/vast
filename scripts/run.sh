#!/bin/bash
bazel build :main && ./bazel-bin/main --trucks=1 --queues=1 --ratio=200 --duration=72
