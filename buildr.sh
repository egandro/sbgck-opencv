#!/bin/bash
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release .. || exit 1
make -j20 || exit 1
