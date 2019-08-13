add_library(DataObjects SHARED PluginAPI.cpp PluginAPI.hpp DataClass.cpp DataClass.hpp
        DataObject.cpp DataObject.hpp DataObjectField.cpp DataObjectField.hpp
        Plugin.cpp DataObjectSimpleField.hpp DataObjectArrayField.cpp DataObjectArrayField.hpp DataObjectReferenceField.cpp DataObjectReferenceField.hpp DataObjectValueField.cpp DataObjectValueField.hpp)
set_target_properties(DataObjects PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")
target_link_libraries(DataObjects GamedevResourcePackerShared)
