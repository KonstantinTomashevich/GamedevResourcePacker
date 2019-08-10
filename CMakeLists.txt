cmake_minimum_required(VERSION 3.14)
project(GamedevResourcePacker)

set(CMAKE_CXX_STANDARD_REQUIRED 11)
include_directories(${CMAKE_SOURCE_DIR})
find_package(Boost COMPONENTS filesystem system log headers unit_test_framework REQUIRED)

add_subdirectory(Shared)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Tests)
