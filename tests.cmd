mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Debug --target RUN_TESTS
cd ..
