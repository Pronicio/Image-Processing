cmake_minimum_required(VERSION 3.30)
project(Image_Processing C)

set(CMAKE_C_STANDARD 99)

add_executable(Image_Processing main.c
        src/bmp8.h
        src/bmp8.c
        src/color.h
        src/color.c
        src/histogram.c
        src/histogram.h)
