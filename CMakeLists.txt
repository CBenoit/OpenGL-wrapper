cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(IN55_solids)

project(example_getting_started)
project(example_lights)
project(example_model_loading)

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

file(GLOB_RECURSE OW_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/ow/*.cpp)
file(GLOB_RECURSE GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.cpp)

set(IN55_SOURCES src/in55/main.cpp src/in55/parametrical_object.cpp src/in55/lamp.cpp src/in55/imgui_windows.cpp)
set(TX52_SOURCES src/tx52/main.cpp)
set(GETTING_STARTED_SOURCES src/examples/getting_started.cpp)
set(LIGHTS_SOURCES src/examples/lights.cpp)
set(MODEL_LOADING_SOURCES src/examples/model_loading.cpp)

include_directories(
        include/
        SYSTEM external/
)

add_executable(
        IN55_solids
        ${IN55_SOURCES}
        ${OW_SOURCES}
        ${GUI_SOURCES}
)

add_executable(
        TX52_eye_tracking
        ${TX52_SOURCES}
        ${OW_SOURCES}
        ${GUI_SOURCES}
)

add_executable(
        example_getting_started
        ${GETTING_STARTED_SOURCES}
        ${OW_SOURCES}
)

add_executable(
        example_lights
        ${LIGHTS_SOURCES}
        ${OW_SOURCES}
)

add_executable(
        example_model_loading
        ${MODEL_LOADING_SOURCES}
        ${OW_SOURCES}
)

target_link_libraries(IN55_solids imgui stb glad ${ASSIMP_LIBRARIES} glfw ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(TX52_eye_tracking imgui stb glad ${ASSIMP_LIBRARIES} glfw ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(example_getting_started stb glad ${ASSIMP_LIBRARIES} glfw ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(example_lights stb glad ${ASSIMP_LIBRARIES} glfw ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(example_model_loading stb glad ${ASSIMP_LIBRARIES} glfw ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})
