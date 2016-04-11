cmake_minimum_required(VERSION 3.3)
project(opencvHelloWorld)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/opencv)
set(SOURCE_FILES main.cpp
        queue_system.cpp
        queue_system.h
        queue_single_lane.cpp
        queue_single_lane.h
        queue_roi.cpp
        queue_roi.h
        processing/frameImageProcessing.cpp
        processing/frameImageProcessing.h
        processing/lbp/lbp.h
        processing/lbp/lbp.cpp
        processing/lbp/helper.h
        processing/queueROIPolygon.h)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(camera ${SOURCE_FILES})
target_link_libraries(camera ${OpenCV_LIBS} )