mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Debug --parallel 20
rem copy external\bin\*.dll bin\Debug
cmake --build . --config Debug --target RUN_TESTS --parallel 20
cd ..
