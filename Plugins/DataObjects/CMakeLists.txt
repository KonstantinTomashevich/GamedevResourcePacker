add_library(DataObjects SHARED PluginAPI.cpp PluginAPI.hpp)
set_target_properties(DataObjects PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")
target_link_libraries(DataObjects Boost::filesystem)
