cmake_minimum_required(VERSION 3.8)

# PROGRAM
project(zlib)

# PROJECT SETUP
file(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.[hc]")
add_library(zlib STATIC ${ALL_FILES})