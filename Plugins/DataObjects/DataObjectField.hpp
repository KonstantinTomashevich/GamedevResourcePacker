#pragma once
#include <boost/property_tree/ptree.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class DataObjectField
{
public:
    using PTree = boost::property_tree::ptree;

    virtual ~DataObjectField ();
};
}
}
