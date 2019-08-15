#pragma once
#include "DataObjectField.hpp"
#include <string>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class DataObjectReferenceField : public DataObjectField
{
public:
    DataObjectReferenceField (const std::string &typeName, PTree &source);
    virtual ~DataObjectReferenceField () = default;

    virtual void Print (std::ostream &output, int indentation = 0) const;

private:
    std::string typeName_;
    std::string value_;
};
}
}
