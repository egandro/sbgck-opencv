#!/bin/bash
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON .. || exit 1
cmake --build . --config Debug || exit 1
make test -j20 || exit 1
cd ..