#!/bin/bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Debug
make test
cd ..