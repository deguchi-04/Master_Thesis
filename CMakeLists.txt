cmake_minimum_required(VERSION 3.5)

project(Thesis)

## Packages used in src
find_package(depthai CONFIG REQUIRED)
find_package(OpenCV REQUIRED )
find_package(Armadillo REQUIRED)
find_package(MLPACK REQUIRED)
find_package(PythonLibs REQUIRED)


## OpenMP to use multiple threads
if(NOT TARGET OpenMP::OpenMP_CXX)
    find_package(Threads REQUIRED)
    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
    # Only works if the same flag is passed to the linker; use CMake 3.9+ otherwise (Intel, AppleClang)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} Threads::Threads)

endif()

find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes $<TARGET_FILE:neural>)
endif()

include (CTest)
#include dirs in the project
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${ARMADILLO_INCLUDE_DIRS}) 
include_directories(${OpenCV_INCLUDE_DIRS})

# executables 
add_executable(segmentation src/segmentation.cpp )
add_executable(neural src/cnn_train.cpp )

target_link_libraries(segmentation PUBLIC ${OpenCV_LIBS} PRIVATE depthai::opencv INTERFACE Armadillo::Armadillo PUBLIC ${MLPACK_LIBRARY} PUBLIC OpenMP::OpenMP_CXX PUBLIC ${ARMADILLO_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(neural PUBLIC ${OpenCV_LIBS} PRIVATE depthai::opencv INTERFACE Armadillo::Armadillo PUBLIC ${MLPACK_LIBRARY} PUBLIC OpenMP::OpenMP_CXX PUBLIC ${ARMADILLO_LIBRARIES} ${PYTHON_LIBRARIES})

