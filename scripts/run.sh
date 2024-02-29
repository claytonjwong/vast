#!/bin/bash
bazel build :vast && ./bazel-bin/vast --trucks=1 --queues=1 --ratio=200 --duration=72
