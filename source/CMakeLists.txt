cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 14)
project(mask)

find_package( OpenCV 3.3.0 REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(mask
        main.cpp
        header.h maskUtil.cpp maskUtil.h processImage.cpp processImage.h segmentFrame.cpp segmentFrame.h)
target_link_libraries( mask ${OpenCV_LIBS})