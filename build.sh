#!/bin/bash
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j20
