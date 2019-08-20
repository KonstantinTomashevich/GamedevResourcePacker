#pragma once
#include "DataObjectField.hpp"
#include "DataClassProvider.hpp"
#include <vector>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class DataObjectArrayField : public DataObjectField
{
public:
    DataObjectArrayField (DataClassProvider *provider, const DataClass::Field &sourceField, PTree &source);
    virtual ~DataObjectArrayField ();

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback);
    virtual bool Write (FILE *output);
    virtual void Print (std::ostream &output, int indentation = 0) const;

    class InnerElementHasOtherType;

private:
    using ElementConstructor = std::function <DataObjectField * (PTree & )>;

    static ElementConstructor GetElementConstructor (DataClassProvider *provider, const DataClass::Field &sourceField);

    std::vector <DataObjectField *> objects_;
};
}
}
