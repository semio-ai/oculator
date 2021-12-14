set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(qoculator mainwindow.ui main.cpp DeviceReader.cpp ImageView.cpp)

target_link_libraries(qoculator Qt5::Core Qt5::Widgets oculator)