cmake_minimum_required(VERSION 3.6)
project(oculator)

if(NOT DEFINED ENV{CONDA_PREFIX})
  message(FATAL_ERROR " Conda must be activated for this build to proceed.")
  return()
else()
  set(_python_exe $ENV{CONDA_PREFIX}/bin/python)
endif ()

function(getsitepackages outvar)
  set(_fixup_cmd "import sys;print(list(filter(lambda x: x.endswith('site-packages'), sys.path))[0])")
  execute_process(
    COMMAND "${_python_exe}" -c "${_fixup_cmd}"
    RESULT_VARIABLE _getpath_exitcode
    OUTPUT_VARIABLE _getpath_text)
  if(NOT _getpath_exitcode EQUAL 0)
    message(ERROR "Failed to get the cmake path for torch. Are you sure torch is installed for python?")
    message(FATAL_ERROR " Python site-packages failed with error code: ${_getpath_exitcode}")
  endif()
  string(STRIP "${_getpath_text}" _path_stripped)
  set(${outvar} "${_path_stripped}" PARENT_SCOPE)
endfunction()

getsitepackages(PYTHON_SITE_PACKAGES)
set(_TORCH_CMAKE_DIR "${PYTHON_SITE_PACKAGES}/torch/share/cmake")

if(DEFINED ENV{HOMEBREW_CELLAR})
  message("Found homebrew")
  set(_CELLAR_CMAKE_DIRS "$ENV{HOMEBREW_CELLAR}/qt@5/5.15.2_1/lib/cmake;$ENV{HOMEBREW_CELLAR}/devil/1.8.0_2/lib/pkgconfig")
endif() 

## Hacking the cmake paths for now. Need to figure out what to do about this later. 
set(CMAKE_PREFIX_PATH "${_TORCH_CMAKE_DIR};${_CELLAR_CMAKE_DIRS}")
# set(CMAKE_PREFIX_PATH "/opt/homebrew/Caskroom/miniforge/base/lib/python3.9/site-packages/torch/share/cmake;/opt/homebrew/Cellar/qt@5/5.15.2_1/lib/cmake;/opt/homebrew/Cellar/devil/1.8.0_2/lib/pkgconfig")

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

pkg_check_modules(IL REQUIRED IMPORTED_TARGET IL)
pkg_check_modules(ILU REQUIRED IMPORTED_TARGET ILU)


if (BUILD_VIZ)
  set(CMAKE_AUTOUIC ON)  
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  
  ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

  file (GLOB_RECURSE SOURCES src/*.cpp)
  set (VIZ_CXX_FLAGS "-DVIZ=1")

  find_package(Qt5 COMPONENTS Widgets REQUIRED)
else ()
  set (VIZ_CXX_FLAGS "-DVIZ=0")
  file (GLOB_RECURSE SOURCES src/oculator/*.cpp)
endif () 

include_directories("include/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} ${IL2_CXX_FLAGS} ${VIZ_CXX_FLAGS}")

include_directories("third_party/cxxopts")

if (BUILD_VIZ)
  file(GLOB HEADERS_TO_MOC include/oculator/viz/*.hpp)
  qt5_wrap_cpp(PROCESSED_MOCS                                                                                                                                                                                                                                                                    
                ${HEADERS_TO_MOC}                                                   
                TARGET oculator
                OPTIONS --no-notes) # Don't display a note for the headers which don't produce a moc_*.cpp

  add_executable(oculator src/viz/mainwindow.ui ${SOURCES} ${PROCESSED_MOCS})
  target_link_libraries(oculator PUBLIC ${TORCH_LIBRARIES} PkgConfig::IL PkgConfig::ILU Qt5::Core Qt5::Widgets) 
else ()
  add_executable(oculator src/main.cpp ${SOURCES})
  target_link_libraries(oculator PUBLIC ${TORCH_LIBRARIES} PkgConfig::IL PkgConfig::ILU)
endif ()