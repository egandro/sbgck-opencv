cmake_minimum_required (VERSION 3.2 FATAL_ERROR)
project( opencvtest )

########################################################################
# Tests - idea https://bastian.rieck.me/blog/posts/2017/simple_unit_tests/
########################################################################

# Enable testing globally. Individual tests may be found in the `tests`
# subdirectory of the repository.
ENABLE_TESTING()
ADD_SUBDIRECTORY(../tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
## set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
## set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # idea from here - https://stackoverflow.com/questions/15100351/changing-cmake-cxx-flags-in-project
    SET ( CMAKE_CXX_FLAGS "-Wno-psabi" )
ENDIF()

include_directories( ${OpenCV_INCLUDE_DIRS}  "../src/" "../include/" )
file(GLOB SOURCE_FILES "../src/*.cpp" "../src/*.hpp" "../include/*.hpp")

# add_executable( main_json_test ${SOURCE_FILES} main_json_test.cpp )
# target_link_libraries( main_json_test ${OpenCV_LIBS} )

# add_executable( main_json_maps ${SOURCE_FILES} main_json_maps.cpp )
# target_link_libraries( main_json_maps ${OpenCV_LIBS} )

# add_executable( main_assetmanager ${SOURCE_FILES} main_assetmanager.cpp )
# target_link_libraries( main_assetmanager ${OpenCV_LIBS} )

# add_executable( main_camera ${SOURCE_FILES} main_camera.cpp )
# target_link_libraries( main_camera ${OpenCV_LIBS} )

# add_executable( main_detectboardonmap ${SOURCE_FILES} main_detectboardonmap.cpp )
# target_link_libraries( main_detectboardonmap ${OpenCV_LIBS} )

# add_executable( main_detecttokens ${SOURCE_FILES} main_detecttokens.cpp )
# target_link_libraries( main_detecttokens ${OpenCV_LIBS} )

# add_executable( hello_world_opencv hello_world_opencv.cpp )
# target_link_libraries( hello_world_opencv ${OpenCV_LIBS} )

# add_executable( main_diff_images ${SOURCE_FILES} main_diff_images.cpp )
# target_link_libraries( main_diff_images ${OpenCV_LIBS} )

# add_executable( main_color_correction ${SOURCE_FILES} main_color_correction.cpp )
# target_link_libraries( main_color_correction ${OpenCV_LIBS} )

add_executable( main_token_detect_loop ${SOURCE_FILES} main_token_detect_loop.cpp )
target_link_libraries( main_token_detect_loop ${OpenCV_LIBS} )
