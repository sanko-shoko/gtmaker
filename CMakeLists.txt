cmake_minimum_required(VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(gtmaker)

include_directories(${CMAKE_SOURCE_DIR})

## simplesp
find_package(SIMPLESP REQUIRED)

include_directories(${SIMPLESP_INCLUDE_DIR})

## openCV
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIRS})


## openGL
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)

include_directories(${GLFW_INCLUDE_DIR})
set(OpenGL_LIBS ${OPENGL_LIBRARY} ${GLFW_LIB})


## folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")


add_subdirectory(src)

