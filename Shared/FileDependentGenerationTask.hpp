#pragma once
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include "GenerationTask.hpp"

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_EXPORT FileDependentGenerationTask : public GenerationTask
{
public:
    virtual bool NeedsExecution (const boost::filesystem::path &outputFolder) const;

protected:
    std::vector <std::string> generationTargets_;
    std::vector <boost::filesystem::path> generationDependencies_;
};
}
