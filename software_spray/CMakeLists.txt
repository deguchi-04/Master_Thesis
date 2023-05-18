cmake_minimum_required(VERSION 3.5)

project(Thesis)

## Packages used in src
find_package(depthai REQUIRED )
find_package(OpenCV REQUIRED )
find_package(PythonLibs REQUIRED)


#include dirs in the project
include_directories(${OpenCV_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} depthai)

# executables 
add_executable(seg src/segmentation.cpp )
add_executable(inf src/cnn_with_depthai.cpp )

target_link_libraries(seg PUBLIC ${OpenCV_LIBS} depthai::opencv PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(inf PUBLIC ${OpenCV_LIBS}  depthai::opencv )
