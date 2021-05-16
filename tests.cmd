mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Debug --parallel 20
cmake --build . --config Debug --target RUN_TESTS --parallel 20
cd ..
