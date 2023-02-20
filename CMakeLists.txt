cmake_minimum_required(VERSION 3.5)
project(Thesis)

find_package(depthai CONFIG REQUIRED)
find_package(OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(segmentation src/segmentation.cpp )

target_link_libraries(segmentation PUBLIC ${OpenCV_LIBS} PRIVATE depthai::opencv)