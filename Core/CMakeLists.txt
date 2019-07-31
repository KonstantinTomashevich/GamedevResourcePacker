add_executable(GamedevResourcePacker Main.cpp)
set_target_properties(GamedevResourcePacker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePacker Boost::filesystem)
