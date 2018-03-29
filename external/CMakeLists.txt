cmake_minimum_required(VERSION 3.10)
project(glad)
project(stb)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/glad.c)
add_library(stb ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.cpp)
