#!/bin/bash

#cmake -S . -B build -D CMAKE_C_COMPILER=clang-11 -D CMAKE_CXX_COMPILER=clang++-11 -D CMAKE_BUILD_TYPE=Release
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug -G 'Unix Makefiles'
cmake --build build -j $(nproc)

