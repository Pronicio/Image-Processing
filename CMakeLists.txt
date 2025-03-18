cmake_minimum_required(VERSION 3.30)
project(Image_Processing C)

set(CMAKE_C_STANDARD 99)

add_executable(Image_Processing main.c
        bmp8.h
        bmp8.c)
