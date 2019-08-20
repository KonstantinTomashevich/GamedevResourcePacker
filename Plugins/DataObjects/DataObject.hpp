#pragma once
#include <Shared/Object.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include "DataObjectField.hpp"
#include "DataClassProvider.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class BOOST_SYMBOL_EXPORT DataObject : public Object
{
public:
    using PTree = boost::property_tree::ptree;

    DataObject (const std::string &name, const std::string &rootName, PTree &root,
        PluginAPI *owner, DataClassProvider *provider);
    virtual ~DataObject ();

    virtual bool Write (FILE *output) const;
    void Print (std::ostream &output) const;

private:
    DataObjectField *rootField_;
};
}
}
