#pragma once
#include <Shared/Object.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <Shared/FileDependentBinaryGenerationTask.hpp>

#include "DataObjectField.hpp"
#include "DataClassProvider.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
// TODO: Maybe do not cache the data objects fields? In large project it may be a problem. But in EU4 they aren't big.
class BOOST_SYMBOL_EXPORT DataObject : public Object, private FileDependentBinaryGenerationTask
{
public:
    using PTree = boost::property_tree::ptree;

    DataObject (const std::string &name, const boost::filesystem::path &sourceAsset,
        const std::string &rootName, PTree &root,
        PluginAPI *owner, DataClassProvider *provider);
    virtual ~DataObject ();

    void Print (std::ostream &output) const;
    virtual bool NeedsExecution (const boost::filesystem::path &outputFile) const;
    // TODO: Think about conversion. Maybe we can write file in such way that allows us to read whole object with one fread?
    virtual bool Execute(FILE *output) const;

private:
    DataObjectField *rootField_;
};
}
}
