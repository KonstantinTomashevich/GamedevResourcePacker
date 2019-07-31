cmake_minimum_required(VERSION 3.14)
project(GamedevResourcePacker)

set(CMAKE_CXX_STANDARD_REQUIRED 11)
include_directories(${CMAKE_SOURCE_DIR})
find_package(Boost COMPONENTS filesystem system headers REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(Core)
add_subdirectory(Plugins)
