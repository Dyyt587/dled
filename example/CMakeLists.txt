cmake_minimum_required(VERSION 3.26)
project(untitled1 C)

set(CMAKE_C_STANDARD 11)

SET(CMAKE_C_FLAGS -pthread)
add_executable(untitled1 main.c
        ../src/list.h
        ../src/dled.c)
