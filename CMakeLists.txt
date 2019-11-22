cmake_minimum_required(VERSION 3.1)
project(masssprings)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### libIGL options: choose between header only and compiled static library
option(LIBIGL_USE_STATIC_LIBRARY     "Use libigl as static library" OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"         ON)
option(LIBIGL_WITH_PNG               "Use PNG"            ON)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           ON)

find_package(LIBIGL REQUIRED QUIET)

# Add your project files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/json/")
if(USE_SOLUTION)
  file(GLOB SRCFILES solution/*.cpp)
else()
  file(GLOB SRCFILES src/*.cpp)
endif()

add_library(lib ${SRCFILES})
add_executable(${PROJECT_NAME}_dense "main.cpp")
add_executable(${PROJECT_NAME}_sparse "main.cpp")
target_compile_options(${PROJECT_NAME}_sparse PUBLIC -DSPARSE)

target_include_directories(${PROJECT_NAME}_dense SYSTEM PUBLIC)
target_link_libraries(${PROJECT_NAME}_dense lib igl::core igl::opengl igl::opengl_glfw igl::png)
target_include_directories(${PROJECT_NAME}_sparse SYSTEM PUBLIC)
target_link_libraries(lib igl::core igl::opengl igl::opengl_glfw igl::png)
target_link_libraries(${PROJECT_NAME}_sparse lib igl::core igl::opengl igl::opengl_glfw igl::png)
