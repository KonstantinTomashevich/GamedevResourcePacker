#pragma once
#include <boost/filesystem.hpp>

namespace GamedevResourcePacker
{
bool BOOST_SYMBOL_EXPORT IsFileNeedsUpdate (const boost::filesystem::path &filePath,
                                            const std::vector <boost::filesystem::path> &dependencies);
}
