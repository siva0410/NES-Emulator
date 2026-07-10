cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(nes
    LANGUAGES CXX
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

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
    src/controller.cpp
)

find_package(SDL2 REQUIRED)
include_directories(nes
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(nes
    ${SDL2_LIBRARIES}
)

target_include_directories(nes
    PRIVATE include
)

target_compile_features(nes
    PRIVATE cxx_std_20
)

if(ipo_supported)
    set_target_properties(nes PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
