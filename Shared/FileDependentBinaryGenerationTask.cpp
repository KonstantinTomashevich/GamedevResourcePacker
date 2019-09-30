#include "FileDependentBinaryGenerationTask.hpp"
#include "FileUtils.hpp"

namespace GamedevResourcePacker
{
bool FileDependentBinaryGenerationTask::NeedsExecution (const boost::filesystem::path &outputFile) const
{
    return IsFileNeedsUpdate (outputFile, generationDependencies_);
}
}
