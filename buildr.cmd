mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --config Release --parallel 20
copy external\bin\*.dll bin\Release
cd ..
