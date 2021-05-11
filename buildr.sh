#!/bin/bash
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release ..
make -j20
