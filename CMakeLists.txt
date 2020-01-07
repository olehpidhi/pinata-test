cmake_minimum_required(VERSION 3.5)

project(test-task LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test-task main.cpp)
add_executable(test-task-bokeh bokeh.cpp)

find_package(OpenCV REQUIRED PATHS opencv/)
#include_directories(opencv/include/opencv4)

target_link_libraries( test-task ${OpenCV_LIBS} )
target_link_libraries( test-task-bokeh ${OpenCV_LIBS} )
