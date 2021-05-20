#!/bin/bash

cmake -S . -B build -D CMAKE_C_COMPILER=clang-11 -D CMAKE_CXX_COMPILER=clang++-11 -D CMAKE_BUILD_TYPE=Release
cmake --build build -j4
