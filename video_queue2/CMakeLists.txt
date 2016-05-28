cmake_minimum_required(VERSION 3.0)
project(camera)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -lm")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/opencv)
set(SOURCE_FILES main.cpp
        queue_system.cpp
        queue_system.h
        queue_single_lane.cpp
        queue_single_lane.h
        queue_roi.cpp
        queue_roi.h
        frameImageProcessing.cpp
        frameImageProcessing.h
        globals.h
        common_vdetect/sequences.cpp
        common_vdetect/sequences.h
        car_analysis/colourRecognition.cpp
        car_analysis/colourRecognition.h
        car_analysis/typeRecognitionLBP.cpp
        car_analysis/typeRecognitionLBP.h
        car_analysis/vAnalysis.cpp
        car_analysis/vAnalysis.h
        common/edgeDetectionFPGA.cpp
        common/edgeDetectionFPGA.h
        polygon/Polygon.cpp polygon/Polygon.h)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(camera ${SOURCE_FILES})
target_link_libraries(camera ${OpenCV_LIBS} )
