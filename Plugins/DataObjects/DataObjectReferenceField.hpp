#pragma once
#include "DataObjectField.hpp"
#include <Shared/ObjectReference.hpp>
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

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback);
    virtual bool Write (FILE *output);
    virtual void Print (std::ostream &output, int32_t indentation = 0) const;

private:
    ObjectReference reference_;
};
}
}
