cmake_minimum_required(VERSION 3.6)
project(vin LANGUAGES CXX )

################################
####### PREP FOR TORCH #########
################################


# Now use the site packages to add some custom cmake directories
set(CMAKE_PREFIX_PATH "${_CELLAR_CMAKE_DIRS}")

#############################################
####### Setting up compiler options #########
#############################################
#### bin2c for models during build
include(FindPkgConfig REQUIRED)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories("./include/" "/opt/homebrew/include/")
add_compile_options("-stdlib=libc++")

option (DEBUG "Build debug version" ON)
option (RELEASE "Build release version" OFF)

if (DEBUG)
  add_definitions(-g -pg)
endif ()

if (RELEASE)
  add_definitions(-Ofast)
endif ()

#############################################
####### User centered build options #########
#############################################
option (BUILD_DEEPGAZE "Build Deepgaze support" ON)
option (BUILD_OPENPOSE "Build OpenPose support" OFF)

##########################################
####### Find the needed packages #########
##########################################
find_package(PkgConfig REQUIRED)
find_package(Catch2 REQUIRED)
find_package(functional_dag REQUIRED)
pkg_search_module(ONNX libonnxruntime)
# find_package(libonnxruntime REQUIRED)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FUNCTIONALDAG_CXX_FLAGS}")

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
# file (GLOB_RECURSE SOURCES src/oculator/model/deepgaze_entry.cpp 
#                            src/oculator/sink_tools/mcb_io.cpp 
#                            src/oculator/utils/onnx_utils.cpp)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ONNX_INCLUDE_DIRS}")


if (BUILD_TESTING)
  # message(${JSONCPP_INCLUDE_DIRS})
  # add_executable(tests test/filter_sys/dag_tests.cpp test/filter_sys/lib_tests.cpp)
  # target_include_directories(tests PRIVATE "include/" ${CATCH_INCLUDE_DIRS} ${TORCH_INCLUDE_DIRS})
  # target_link_directories(tests PRIVATE ${CATCH_LIBRARY_DIRS} ${JSONCPP_LIBRARY_DIRS})
  # target_link_libraries(tests filterdag ${CATCH_LIBRARIES} ${JSONCPP_LIBRARIES})

  # include(CTest)
  # include(Catch)
  # catch_discover_tests(tests)
endif()

add_custom_target(test_target ALL
    # COMMAND echo "This is ALL target 'zoo', and it depends on ${TEST_FILE}"
    COMMAND vin
    # If the file exists, then commands related to that file won't be executed
    # DONOT let other target depends on the same OUTPUT as current target,
    #   or it may be bad when doing parallel make
    # DEPENDS ${TEST_FILE}

    # to make quotes printable,for example
    VERBATIM
)

##################################
###### DeepGaze library  #########
##################################
if (BUILD_DEEPGAZE)
  add_library(deepgaze SHARED src/oculator/utils/onnx_utils.cpp
                              src/oculator/model/deepgaze_entry.cpp 
                              src/oculator/sink_tools/mcb_io.cpp 
                              )
  target_include_directories(deepgaze PRIVATE "include/" ${ONNX_INCLUDE_DIRS})
  target_compile_options(deepgaze PRIVATE -fPIC -fvisibility=hidden ${CMAKE_CXX_FLAGS})
  target_link_directories(deepgaze PRIVATE ${ONNX_LIBRARY_DIRS})
  target_link_libraries(deepgaze PRIVATE FunctionalDag::functional_dag ${ONNX_LIBRARIES})


  add_custom_command(
          TARGET deepgaze POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory
                  ${CMAKE_CURRENT_BINARY_DIR}/lib)

  add_custom_command(
          TARGET deepgaze POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
                  ${CMAKE_CURRENT_BINARY_DIR}/libdeepgaze.dylib
                  ${CMAKE_CURRENT_BINARY_DIR}/lib/)

  add_custom_command(
          TARGET deepgaze POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E rm
                  ${CMAKE_CURRENT_BINARY_DIR}/libdeepgaze.dylib)
  set_property(
          TARGET deepgaze APPEND
          PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libdeepgaze.dylib)
endif ()


if (BUILD_OPENPOSE)
  add_library(openpose SHARED src/oculator/model/openpose_entry.cpp 
                              src/oculator/utils/onnx_utils.cpp)
  target_include_directories(openpose PRIVATE "include/" ${ONNX_INCLUDE_DIRS})
  target_compile_options(openpose PRIVATE -fPIC -fvisibility=hidden ${CMAKE_CXX_FLAGS})
  target_link_directories(openpose PRIVATE ${ONNX_LIBRARY_DIRS})
  target_link_libraries(openpose PRIVATE FunctionalDag::functional_dag ${ONNX_LIBRARIES})


  add_custom_command(
          TARGET openpose POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory
                  ${CMAKE_CURRENT_BINARY_DIR}/lib)

  add_custom_command(
          TARGET openpose POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
                  ${CMAKE_CURRENT_BINARY_DIR}/libopenpose.dylib
                  ${CMAKE_CURRENT_BINARY_DIR}/lib/)

  add_custom_command(
          TARGET openpose POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E rm
                  ${CMAKE_CURRENT_BINARY_DIR}/libopenpose.dylib)
  set_property(
          TARGET openpose APPEND
          PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libopenpose.dylib)
endif ()
