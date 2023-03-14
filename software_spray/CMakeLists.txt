cmake_minimum_required(VERSION 3.5)

project(Thesis)

## Packages used in src
find_package(depthai CONFIG REQUIRED PATHS /usr/local/include/depthai )
find_package(OpenCV REQUIRED )
find_package(PythonLibs REQUIRED)


#include dirs in the project
include_directories(${OpenCV_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} depthai::opencv)

# executables 
add_executable(segmentation src/segmentation.cpp )
add_executable(neural src/cnn_train.cpp )
add_executable(dep src/cnn_with_depthai.cpp )

target_link_libraries(segmentation PUBLIC ${OpenCV_LIBS} depthai::opencv PRIVATE  ${PYTHON_LIBRARIES})
target_link_libraries(neural PUBLIC ${OpenCV_LIBS}  depthai::opencv )
target_link_libraries(dep PUBLIC ${OpenCV_LIBS}  depthai::opencv )