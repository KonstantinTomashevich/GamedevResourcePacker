add_executable(GamedevResourcePacker Main.cpp PluginManager.cpp PluginManager.hpp
        ObjectManager.cpp ObjectManager.hpp Exception.hpp CodeGenerator.cpp CodeGenerator.hpp)

set_target_properties(GamedevResourcePacker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePacker GamedevResourcePackerShared)
add_dependencies(GamedevResourcePacker DataObjects)
