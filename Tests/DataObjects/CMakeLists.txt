add_executable(DataObjectsTests Main.cpp DataClassLoad.cpp DataObjectLoad.cpp)
set_target_properties(DataObjectsTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
target_link_libraries(DataObjectsTests GamedevResourcePackerShared Boost::unit_test_framework DataObjects)
