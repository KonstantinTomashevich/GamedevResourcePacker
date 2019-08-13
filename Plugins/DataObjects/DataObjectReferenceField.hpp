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
    DataObjectReferenceField (const std::string &typeName, PTree *source);
    virtual ~DataObjectReferenceField () = default;

private:
    std::string typeName_;
    std::string value_;
};
}
}
