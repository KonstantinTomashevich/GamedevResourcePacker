#pragma once
#include <Shared/Object.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <Shared/FileDependentGenerationTask.hpp>

#include "DataObjectField.hpp"
#include "DataClassProvider.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class BOOST_SYMBOL_EXPORT DataObject : public Object, private FileDependentGenerationTask
{
public:
    using PTree = boost::property_tree::ptree;

    DataObject (const std::string &name, const boost::filesystem::path &sourceAsset,
        const std::string &rootName, PTree &root,
        PluginAPI *owner, DataClassProvider *provider);
    virtual ~DataObject ();

    void Print (std::ostream &output) const;
    virtual bool NeedsExecution (const boost::filesystem::path &outputFolder) const;
    virtual bool Execute(const boost::filesystem::path &outputFolder) const;

private:
    DataObjectField *rootField_;
};
}
}
