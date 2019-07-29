cmake_minimum_required(VERSION 3.15)
project(sudokusolver)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

add_executable(sudokusolver main.cpp)