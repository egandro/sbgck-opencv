#!/bin/bash
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Debug
make test -j20
cd ..