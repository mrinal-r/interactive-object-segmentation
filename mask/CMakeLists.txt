cmake_minimum_required(VERSION 3.12)
project(mask)

set(CMAKE_CXX_STANDARD 14)

add_executable(mask main.cpp processImage.cpp processImage.h segmentFrame.cpp segmentFrame.h maskUtil.cpp maskUtil.h header.h)

find_package( OpenCV 3.3.0 REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries( mask ${OpenCV_LIBS})