
cmake_minimum_required (VERSION 3.1.3)

project (traceview2d CXX)

SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS                "-O0 -std=c++11")
# set (CMAKE_CXX_STANDARD 11)
# set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package (NetCDF REQUIRED)
include_directories (${NETCDF_INCLUDE_DIR})

find_package (GLEW REQUIRED)
include_directories (${GLEW_INCLUDE_DIR})

find_package (OpenGL REQUIRED)

find_package (Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL)
include (${QT_USE_FILE})

# EIGEN
INCLUDE_DIRECTORIES ( "${EIGEN3_INCLUDE_DIR}" )


include_directories ("include")

add_executable(main src/main.cpp src/widget.cpp src/trackball.cpp src/Data.cpp src/interp.cpp src/advect.cpp )
set_target_properties (main PROPERTIES AUTOMOC ON)
target_link_libraries(main ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${NETCDF_LIBRARIES})





# add_subdirectory (src)