cmake_minimum_required(VERSION 3.10)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

# glad
add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c)

# stb_image
add_library(stb ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.cpp)

# Dear ImGui
file(GLOB IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp)
add_library(imgui ${IMGUI_SRC})
