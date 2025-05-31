#!/usr/bin/env bash

cmake -B build -GNinja -DBUILD_SAMPLES=ON
cmake --build build
