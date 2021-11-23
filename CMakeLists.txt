project(oculator)

cmake_minimum_required(VERSION 3.0)

option (BUILD_EXAMPLES "Build examples" ON)
option (DEBUG "Build debug version" ON)
option (RELEASE "Build release version" OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_COMPILER  "/usr/bin/g++-7")
set (CMAKE_CXX_FLAGS "-std=c++1z")

if (DEBUG)
  add_definitions(-g -pg)
endif ()

if (RELEASE)
  add_definitions(-Ofast)
endif ()

find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)

file (GLOB_RECURSE SOURCES src/*.cpp)

add_library(oculator SHARED ${SOURCES})

target_include_directories(oculator PUBLIC include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(oculator PUBLIC ${OpenCV_LIBS})

add_subdirectory(third_party)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
