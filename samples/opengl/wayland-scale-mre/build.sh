#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
mkdir -p build
"${CXX:-c++}" -std=c++17 -Wall -Wextra -g main.cpp $("${WX_CONFIG:-wx-config}" --cxxflags --libs core,base,gl) -lGL -o build/wx-wayland-scale-mre
