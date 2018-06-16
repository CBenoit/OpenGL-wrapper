cmake_minimum_required(VERSION 3.10)
project(opengl_wrapper)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

add_subdirectory(external/)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization -Wvector-operation-performance -Winvalid-pch \
                            -Wunused -Wconversion -Wlogical-op -Wuninitialized -Wsuggest-final-types -Wdouble-promotion \
                            -Wmissing-include-dirs -Wall -pedantic  -Wcast-qual -Wswitch-default -Wunreachable-code \
                            -Wfloat-equal -Wundef -Wcast-align -Wcast-align -Wredundant-decls -Winit-self -Wshadow \
                            -Wnon-virtual-dtor -Weffc++ -Wswitch-enum -Wmissing-declarations -fmessage-length=150 -W \
                            -Wpointer-arith -Wmain -Wwrite-strings -Wmissing-noreturn")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined -g")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
endif()

include_directories(
        include/
        SYSTEM external/
)

# == ow lib ==
file(GLOB_RECURSE OW_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/ow/*.cpp)
add_library(ow ${OW_SOURCES})
target_link_libraries(ow stb glad ${ASSIMP_LIBRARIES} ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

# == gui lib ==
file(GLOB_RECURSE GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.cpp)
add_library(gui ${GUI_SOURCES})
target_link_libraries(ow imgui glfw)

# == IN55 parametrical object project ==
set(IN55_SOURCES src/in55/main.cpp src/in55/parametrical_object.cpp src/in55/cube.cpp src/in55/imgui_windows.cpp)
add_executable(
        in55_parametrical_object
        ${IN55_SOURCES}
)
target_link_libraries(in55_parametrical_object ow gui)

# == getting started example ==
set(GETTING_STARTED_SOURCES src/examples/getting_started.cpp)
add_executable(
        example_getting_started
        ${GETTING_STARTED_SOURCES}
        ${OW_SOURCES}
)
target_link_libraries(example_getting_started ow)

# == lighting example ==
set(LIGHTS_SOURCES src/examples/lights.cpp)
add_executable(
        example_lights
        ${LIGHTS_SOURCES}
        ${OW_SOURCES}
)
target_link_libraries(example_lights ow)

# == model loading example ==
set(MODEL_LOADING_SOURCES src/examples/model_loading.cpp)
add_executable(
        example_model_loading
        ${MODEL_LOADING_SOURCES}
        ${OW_SOURCES}
)
target_link_libraries(example_model_loading ow)
