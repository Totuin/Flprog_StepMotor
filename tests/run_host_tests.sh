#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
CXX=${CXX:-g++}
"$CXX" -std=c++11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Itests/fakes -Isrc tests/test_stepmotor.cpp src/*.cpp -o /tmp/flprog_stepmotor_host_tests
ASAN_OPTIONS=detect_leaks=1 /tmp/flprog_stepmotor_host_tests
