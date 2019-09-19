add_library(GamedevResourcePackerShared SHARED Object.hpp Object.cpp
        ObjectReference.cpp ObjectReference.hpp PluginAPI.hpp
        StringHash.cpp StringHash.hpp FileUtils.cpp FileUtils.hpp
        GenerationTask.hpp FileDependentGenerationTask.cpp FileDependentGenerationTask.hpp
        MultithreadedLog.hpp MultithreadedLog.cpp)
set_target_properties(GamedevResourcePackerShared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(GamedevResourcePackerShared Boost::filesystem Boost::log)
