#pragma once
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_EXPORT BinaryGenerationTask
{
public:
    virtual ~BinaryGenerationTask () = default;

    virtual bool NeedsExecution (const boost::filesystem::path &outputFile) const = 0;
    virtual bool Execute(FILE *output) const = 0;
};
}
