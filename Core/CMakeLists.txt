add_executable(GamedevResourcePacker Main.cpp ../Shared/PluginAPI.hpp PluginManager.cpp PluginManager.hpp
        ObjectManager.cpp ObjectManager.hpp Exception.hpp)

set_target_properties(GamedevResourcePacker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePacker GamedevResourcePackerShared)
add_dependencies(GamedevResourcePacker DataObjects)
