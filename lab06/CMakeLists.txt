cmake_minimum_required(VERSION 3.0)
project(lab6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -pthread")
set(CMAKE_CXX_COMPILER g++)

add_executable(semaphores semaphores.cpp)
