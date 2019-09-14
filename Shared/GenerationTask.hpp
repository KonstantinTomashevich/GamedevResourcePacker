#pragma once
#include <boost/config.hpp>
#include <boost/filesystem.hpp>

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_EXPORT GenerationTask
{
public:
    virtual ~GenerationTask () = default;

    virtual bool NeedsExecution (const boost::filesystem::path &outputFolder) const = 0;
    virtual bool Execute(const boost::filesystem::path &outputFolder) const = 0;
};
}
