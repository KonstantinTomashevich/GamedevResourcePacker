#pragma once
#include <Shared/Object.hpp>
#include <boost/property_tree/ptree.hpp>
#include "DataObjectField.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class DataObject : public Object
{
public:
    using PTree = boost::property_tree::ptree;

    DataObject (const std::string &name, const std::string &rootName, PTree &root, PluginAPI *owner);
    virtual ~DataObject ();

private:
    DataObjectField *rootField_;
};
}
}
