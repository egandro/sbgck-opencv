mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Release --parallel 20
cd ..
