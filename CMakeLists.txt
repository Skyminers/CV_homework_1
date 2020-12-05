cmake_minimum_required(VERSION 3.17)
project(CV_homework_1)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(CV_homework_1 main.cpp)

target_link_libraries(CV_homework_1 ${OpenCV_LIBS})