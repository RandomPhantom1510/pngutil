cmake_minimum_required(VERSION 3.18)
project(pngutil C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(pngutil
        defs.h
        pngutil.c
        tests.c
        tests.h)
