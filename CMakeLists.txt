cmake_minimum_required(VERSION 3.12)
project(CHIP_8_Emu)

set(CMAKE_CXX_STANDARD 17)

add_executable(CHIP_8_Emu src/main.cpp src/CHIP8.h src/CHIP8.cpp
        src/simple-sdl2-audio/audio.h src/simple-sdl2-audio/audio.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/Buzz.wav
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})