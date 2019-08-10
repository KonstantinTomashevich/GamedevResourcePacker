add_library(GamedevResourcePackerShared SHARED Object.hpp Object.cpp)
set_target_properties(GamedevResourcePackerShared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePackerShared Boost::filesystem Boost::log)
