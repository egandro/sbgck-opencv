cmake_minimum_required (VERSION 3.2 FATAL_ERROR)
project(sbgck_opencv)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(UNIX AND NOT APPLE)
    # on Linux
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
    SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
endif()

set(CMAKE_CXX_STANDARD 17)
include(ExternalProject)

#set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # idea from here - https://stackoverflow.com/questions/15100351/changing-cmake-cxx-flags-in-project
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi" )
ENDIF()

if(UNIX AND NOT APPLE)
    # on Linux
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
    SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
endif()

# if(DEFINED EXTERNAL_INSTALL_LOCATION)
#     set(EXTERNAL_INSTALL_LOCATION ${EXTERNAL_INSTALL_LOCATION})
# else()
#     set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
# endif()
# #message("using EXTERNAL_INSTALL_LOCATION: ${EXTERNAL_INSTALL_LOCATION}")

# include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
# link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

# if(UNIX)
#     # fix me
#     # build static on Linux - some bug in here - on windows it always makes a dll
#     set (BGS_CORE_STATIC "-DBGS_CORE_STATIC=ON")
# endif()

include_directories( ${OpenCV_INCLUDE_DIRS} "./src/" "./include/" )
file(GLOB SOURCE_FILES "./src/*.cpp" "./src/*.hpp" "./include/*.hpp" )

add_library( sbgck_opencv STATIC ${SOURCE_FILES} )
target_include_directories(sbgck_opencv PUBLIC include)

install(TARGETS sbgck_opencv
        INCLUDES DESTINATION include
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/ DESTINATION include/sbgck_opencv)

########################################################################
# Tests - idea https://bastian.rieck.me/blog/posts/2017/simple_unit_tests/
########################################################################

# Enable testing globally. Individual tests may be found in the `tests`
# subdirectory of the repository.

if(BUILD_TESTING)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(tests)
endif()

if(BUILD_TESTING)
    add_executable( hello_world_opencv dev/hello_world_opencv.cpp )
    target_link_libraries( hello_world_opencv ${OpenCV_LIBS} sbgck_opencv)

    add_executable( main_camera dev/main_camera.cpp )
    target_link_libraries( main_camera ${OpenCV_LIBS} sbgck_opencv)

    add_executable( main_detectboardonmap dev/main_detectboardonmap.cpp )
    target_link_libraries( main_detectboardonmap ${OpenCV_LIBS} sbgck_opencv)

    add_executable( main_token_detect_loop dev/main_token_detect_loop.cpp )
    target_link_libraries( main_token_detect_loop ${OpenCV_LIBS} sbgck_opencv)
endif()
