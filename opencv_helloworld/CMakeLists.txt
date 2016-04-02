cmake_minimum_required(VERSION 3.3)
project(opencvHelloWorld)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/opencv)
set(SOURCE_FILES main.cpp)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(opencvHelloWorld ${SOURCE_FILES})
target_link_libraries( opencvHelloWorld ${OpenCV_LIBS} )