cmake_minimum_required(VERSION 3.14)
project(GamedevResourcePacker)

set(CMAKE_CXX_STANDARD_REQUIRED 11)
include_directories(${CMAKE_SOURCE_DIR})
find_package(Boost COMPONENTS filesystem system log headers unit_test_framework REQUIRED)
find_package(OpenMP REQUIRED)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_subdirectory(Shared)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Tests)
