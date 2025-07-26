cmake_minimum_required(VERSION 3.15)
project(MuxEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(
    ${PROJECT_SOURCE_DIR}/external/include
    ${PROJECT_SOURCE_DIR}/external/include/SDL2
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/Graphics
)

# Fuentes
set(ENGINE_SRC
    ${PROJECT_SOURCE_DIR}/src/Core/Me_Main.cpp
    ${PROJECT_SOURCE_DIR}/src/Graphics/gl.c
    ${PROJECT_SOURCE_DIR}/src/Graphics/SplashScene.cpp
    ${PROJECT_SOURCE_DIR}/src/Graphics/rtx_to_bitmap.cpp
    ${PROJECT_SOURCE_DIR}/src/Graphics/RTXLoader.cpp
)

add_executable(ME1Editor ${ENGINE_SRC})

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(ME1Editor
    SDL2
    SDL2main
    opengl32
)

set_target_properties(ME1Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/Debug
)

