mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Release --parallel 20
rem copy external\bin\*.dll bin\Release
cd ..
