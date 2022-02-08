cmake_minimum_required(VERSION 3.6)
project(oculator)

## Hacking the cmake paths for now. Need to figure out what to do about this later. 
set(CMAKE_PREFIX_PATH "/opt/homebrew/Caskroom/miniforge/base/envs/torch/lib/python3.9/site-packages/torch/share/cmake;/opt/homebrew/Cellar/qt@5/5.15.2_1/lib/cmake;/opt/homebrew/Cellar/devil/1.8.0_2/lib/pkgconfig;./third_party/")


include(FindPkgConfig REQUIRED)

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

find_package(PkgConfig REQUIRED)
find_package(Torch REQUIRED)
# find_package(cxxopts REQUIRED)

pkg_check_modules(IL2 REQUIRED IMPORTED_TARGET IL)
pkg_check_modules(ILU2 REQUIRED IMPORTED_TARGET ILU)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} ${IL2_CXX_FLAGS}")

file (GLOB_RECURSE SOURCES src/*.cpp)

add_library(oculator SHARED ${SOURCES})

target_link_libraries(oculator PUBLIC ${OpenCV_LIBS} ${TORCH_LIBRARIES} PkgConfig::IL2 PkgConfig::ILU2)

add_subdirectory(third_party)

if (BUILD_EXAMPLES)
   include_directories("include/")
  add_subdirectory(examples)
endif()
