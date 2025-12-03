cmake_minimum_required(VERSION 3.20)

# 1. VCPKG Setup
# We force the toolchain here to ensure CMake finds the packages
set(CMAKE_TOOLCHAIN_FILE "C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(anthony)

set(CMAKE_CXX_STANDARD 17)

# 2. Find SFML (Config mode)
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System)

add_executable(anthony retroGame.cpp)

# 3. Link correctly using Namespaced Targets
# usage of "SFML::" automatically handles include directories
target_link_libraries(anthony PRIVATE SFML::Graphics SFML::Window SFML::System)
