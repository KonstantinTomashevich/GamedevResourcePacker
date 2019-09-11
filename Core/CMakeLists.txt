add_executable(GamedevResourcePacker Main.cpp PluginManager.cpp PluginManager.hpp
        ObjectManager.cpp ObjectManager.hpp Exception.hpp CodeGenerator.cpp CodeGenerator.hpp)

set_target_properties(GamedevResourcePacker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePacker GamedevResourcePackerShared)
add_dependencies(GamedevResourcePacker DataObjects)

add_custom_command(TARGET GamedevResourcePacker POST_BUILD COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/BundleIndependentCode" ${CMAKE_BINARY_DIR}/bin/BundleIndependentCode/)
