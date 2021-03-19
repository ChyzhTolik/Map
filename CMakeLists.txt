cmake_minimum_required (VERSION 3.19.6 FATAL_ERROR)

set(PROJECT_NAME Map)
project (${PROJECT_NAME})

add_executable(Map Map/src/Application.cpp)

include_directories(Dependencies/glew-2.1.0/include/GL/)