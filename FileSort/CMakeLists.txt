﻿# CMakeList.txt : CMake project for FileSort, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DUNICODE -D_UNICODE)

# Add source to this project's executable.
add_executable (FileSort "FileSort.cpp" "FileSort.hpp" "main.cpp"  "SegmentHandler.hpp" "SegmentHandler.cpp")

# TODO: Add tests and install targets if needed.
