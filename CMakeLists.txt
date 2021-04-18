
cmake_minimum_required(VERSION 3.2)
project( opencvtest )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
## set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
## set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # idea from here - https://stackoverflow.com/questions/15100351/changing-cmake-cxx-flags-in-project
    SET ( CMAKE_CXX_FLAGS "-Wno-psabi" )
ENDIF()

include_directories( ${OpenCV_INCLUDE_DIRS} "./src/" "./include/" )
file(GLOB SOURCE_FILES "./src/*.cpp" "./src/*.hpp" "./include/*.hpp" )

add_library( stgck_opencv ${SOURCE_FILES} )
