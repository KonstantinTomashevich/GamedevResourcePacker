#include "FileUtils.hpp"

namespace GamedevResourcePacker
{
bool IsFileNeedsUpdate (const boost::filesystem::path &filePath,
                        const std::vector <boost::filesystem::path> &dependencies)
{
    if (!boost::filesystem::exists (filePath))
    {
        return true;
    }

    std::time_t fileLastWrite = boost::filesystem::last_write_time (filePath);
    for (auto &dependency : dependencies)
    {
        std::time_t dependencyLastWrite = boost::filesystem::last_write_time (dependency);
        if (dependencyLastWrite > fileLastWrite)
        {
            return true;
        }
    }

    return false;
}
}
