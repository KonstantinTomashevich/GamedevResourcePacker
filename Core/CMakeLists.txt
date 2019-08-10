add_executable(GamedevResourcePacker Main.cpp PluginAPI.hpp PluginManager.cpp PluginManager.hpp
        Object.cpp Object.hpp ObjectManager.cpp ObjectManager.hpp Exception.hpp)

set_target_properties(GamedevResourcePacker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePacker Boost::filesystem Boost::log)
add_dependencies(GamedevResourcePacker DataObjects)
