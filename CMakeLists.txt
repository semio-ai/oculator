cmake_minimum_required(VERSION 3.0)
project(oculator)

option (BUILD_EXAMPLES "Build examples" ON)
option (DEBUG "Build debug version" ON)
option (RELEASE "Build release version" OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)



if (DEBUG)
  add_definitions(-g -pg)
endif ()

if (RELEASE)
  add_definitions(-Ofast)
endif ()

find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


file (GLOB_RECURSE SOURCES src/*.cpp)

add_library(oculator SHARED ${SOURCES})

target_include_directories(oculator PUBLIC include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(oculator PUBLIC ${OpenCV_LIBS} ${TORCH_LIBRARIES})

add_subdirectory(third_party)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
