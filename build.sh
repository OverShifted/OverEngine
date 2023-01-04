#!/bin/bash

#cmake -S . -B build -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_BUILD_TYPE=Debug
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
cmake --build build -j $(nproc)

