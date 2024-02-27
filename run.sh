#!/bin/bash
bazel build :main && ./bazel-bin/main >> 'output.txt'
