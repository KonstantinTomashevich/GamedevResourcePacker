#pragma once
#include "DataObjectField.hpp"
#include "DataClass.hpp"
#include <vector>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class DataObjectArrayField : public DataObjectField
{
public:
    DataObjectArrayField (PluginAPI *api, const DataClass::Field &sourceField, PTree &source);
    virtual ~DataObjectArrayField ();

    virtual void Print (std::ostream &output, int indentation = 0) const;

    class InnerElementHasOtherType;

private:
    using ElementConstructor = std::function <DataObjectField * (PTree & )>;

    static ElementConstructor GetElementConstructor (PluginAPI *api, const DataClass::Field &sourceField);

    std::vector <DataObjectField *> objects_;
};
}
}
