cmake_minimum_required(VERSION 3.15)
project(hello_world_opencv)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(hello_world_opencv hello_world_opencv.cpp)
target_link_libraries(hello_world_opencv ${OpenCV_LIBS})