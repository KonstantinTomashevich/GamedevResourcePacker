#pragma once
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include "BinaryGenerationTask.hpp"

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_EXPORT FileDependentBinaryGenerationTask : public BinaryGenerationTask
{
public:
    virtual bool NeedsExecution (const boost::filesystem::path &outputFile) const;

protected:
    std::vector <boost::filesystem::path> generationDependencies_;
};
}
