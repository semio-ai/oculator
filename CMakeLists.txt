cmake_minimum_required(VERSION 3.6)
project(oculator)

## Hacking the cmake paths for now. Need to figure out what to do about this later. 
set(CMAKE_PREFIX_PATH "/opt/homebrew/Caskroom/miniforge/base/envs/torch/lib/python3.9/site-packages/torch/share/cmake;/opt/homebrew/Cellar/qt@5/5.15.2_1/lib/cmake;/opt/homebrew/Cellar/devil/1.8.0_2/lib/pkgconfig")

include(FindPkgConfig REQUIRED)

option (DEBUG "Build debug version" ON)
option (RELEASE "Build release version" OFF)
option (BUILD_VIZ "Build with GUI support" OFF)

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


if (BUILD_VIZ)
  set(CMAKE_AUTOUIC ON)  
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  

  ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

  file (GLOB_RECURSE SOURCES src/*.cpp)
  set (VIZ_CXX_FLAGS "-DVIZ=1")

  find_package(Qt5 COMPONENTS Widgets REQUIRED)
  # file (GLOB_RECURSE UI_FILES src/*.ui)
else ()
  set (VIZ_CXX_FLAGS "-DVIZ=0")
  file (GLOB_RECURSE SOURCES src/oculator/*.cpp)
endif () 

# target_include_directories(oculator "include/")
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} ${IL2_CXX_FLAGS} ${VIZ_CXX_FLAGS} -I/Users/drobotnik/projects/oculator/include/")
# add_library(oculator SHARED ${SOURCES})
if (BUILD_VIZ)
  file(GLOB HEADERS_TO_MOC include/oculator/viz/*.hpp)
  qt5_wrap_cpp(PROCESSED_MOCS                                                                                                                                                                                                                                                                    
                ${HEADERS_TO_MOC}                                                   
                TARGET oculator
                OPTIONS --no-notes) # Don't display a note for the headers which don't produce a moc_*.cpp

  add_executable(oculator src/viz/mainwindow.ui ${SOURCES} ${PROCESSED_MOCS})
  target_link_libraries(oculator PUBLIC ${TORCH_LIBRARIES} PkgConfig::IL2 PkgConfig::ILU2 Qt5::Core Qt5::Widgets) 
else ()
  add_executable(oculator src/main.cpp ${SOURCES})
  target_link_libraries(oculator PUBLIC ${TORCH_LIBRARIES} PkgConfig::IL2 PkgConfig::ILU2)
endif ()
# target_link_libraries(qoculator oculator Qt5::Core Qt5::Widgets)
# add_subdirectory(third_party)

# if (BUILD_EXAMPLES)
#    include_directories("include/")
#   add_subdirectory(examples)
# endif()
