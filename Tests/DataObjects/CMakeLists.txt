add_executable(DataObjectsTests DataClassLoad.cpp)
set_target_properties(DataObjectsTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
target_link_libraries(DataObjectsTests Boost::filesystem Boost::log Boost::unit_test_framework DataObjects)
