project(ResourceSubsystem)
find_package(Boost COMPONENTS filesystem log headers REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE Sources *.cpp *.hpp)
add_library(ResourceSubsystem SHARED ${Sources})
target_link_libraries(ResourceSubsystem Boost::filesystem Boost::log)
