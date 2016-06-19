cmake_minimum_required(VERSION 3.3)
project(egaugesimulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -std=c++11")

set(SOURCE_FILES main.c mailbox.h gpu_fft.h)
add_executable(egaugesimulator ${SOURCE_FILES})