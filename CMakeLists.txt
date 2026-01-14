cmake_minimum_required(VERSION 3.20)

project(nes
    LANGUAGES CXX
)

add_executable(nes
    src/main.cpp
    src/nes.cpp
    src/cpubus.cpp
    src/cpu.cpp
    src/ppubus.cpp
    src/ppu.cpp
    src/ram.cpp
    src/rom.cpp
    src/memory.cpp
    src/register.cpp
    src/display.cpp
)

target_include_directories(nes
    PRIVATE include
)

target_compile_features(nes
    PRIVATE cxx_std_20
)
