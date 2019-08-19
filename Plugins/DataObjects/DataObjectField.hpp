#pragma once
#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace GamedevResourcePacker
{
class ObjectReference;

namespace DataObjectsPlugin
{
class DataObjectField
{
public:
    using ReferenceIterationCallback = std::function <void (ObjectReference *)>;

    using PTree = boost::property_tree::ptree;

    virtual ~DataObjectField ();

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback) = 0;
    virtual void Print (std::ostream &output, int indentation = 0) const = 0;

protected:
    static std::ostream &Indent (std::ostream &output, int size);
};
}
}
