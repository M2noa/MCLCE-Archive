cmake_minimum_required(VERSION 3.8)

# PROGRAM
project(minizip)

# PROJECT SETUP
file(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.[hc]")
add_library(minizip STATIC ${ALL_FILES})