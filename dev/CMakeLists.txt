
cmake_minimum_required(VERSION 2.8)
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

include_directories( ${OpenCV_INCLUDE_DIRS} "../opencv/" )
file(GLOB SOURCE_FILES "../opencv/*.cpp" "../opencv/*.hpp")

# add_executable( main_json_test ${SOURCE_FILES} main_json_test.cpp )
# target_link_libraries( main_json_test ${OpenCV_LIBS} )

add_executable( main_json_maps ${SOURCE_FILES} main_json_maps.cpp )
target_link_libraries( main_json_maps ${OpenCV_LIBS} )

# add_executable( main_assetmanager ${SOURCE_FILES} main_assetmanager.cpp )
# target_link_libraries( main_assetmanager ${OpenCV_LIBS} )

# add_executable( main_camera ${SOURCE_FILES} main_camera.cpp )
# target_link_libraries( main_camera ${OpenCV_LIBS} )

# add_executable( main_detectboardonmap ${SOURCE_FILES} main_detectboardonmap.cpp )
# target_link_libraries( main_detectboardonmap ${OpenCV_LIBS} )

# add_executable( main_detecttokens ${SOURCE_FILES} main_detecttokens.cpp )
# target_link_libraries( main_detecttokens ${OpenCV_LIBS} )