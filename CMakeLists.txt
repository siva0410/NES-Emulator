cmake_minimum_required(VERSION 3.20)

project(nes
    LANGUAGES CXX
)

add_executable(nes
    src/main.cpp
)

target_include_directories(nes
    PRIVATE include
)

target_compile_features(nes
    PRIVATE cxx_std_20
)
