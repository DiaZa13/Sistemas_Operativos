cmake_minimum_required(VERSION 3.0)
project(lab3_1)

set(CMAKE_C_STANDARD 99)
set(PTHREAD_FLAGS "-pthread")
set(OPENMP_FLAGS "-fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PTHREAD_FLAGS} ${OPENMP_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PTHREAD_FLAGS} ${OPENMP_FLAGS}")

add_executable(lab3 SudokuValidator.c)
