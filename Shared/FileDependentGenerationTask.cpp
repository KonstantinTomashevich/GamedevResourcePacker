#include "FileDependentGenerationTask.hpp"
#include "FileUtils.hpp"

namespace GamedevResourcePacker
{
bool FileDependentGenerationTask::NeedsExecution (const boost::filesystem::path &outputFolder) const
{
    for (auto &target : generationTargets_)
    {
        if (IsFileNeedsUpdate (outputFolder / target, generationDependencies_))
        {
            return true;
        }
    }

    return false;
}
}
