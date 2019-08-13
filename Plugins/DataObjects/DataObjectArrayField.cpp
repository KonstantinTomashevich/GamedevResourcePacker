#include "DataObjectArrayField.hpp"
#include "DataObjectSimpleField.hpp"
#include "DataObjectReferenceField.hpp"
#include "DataObjectValueField.hpp"
#include "PluginAPI.hpp"
#include <Core/Exception.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectArrayField::DataObjectArrayField (
    PluginAPI *api, const DataClass::Field &sourceField, DataObjectField::PTree *source)
    : objects_ ()
{
    DataObjectArrayField::ElementConstructor constructor = GetElementConstructor (api, sourceField);

    for (PTree::value_type &node : *source)
    {
        if (node.first == "<xmlattr>") continue;
        if (node.first != sourceField.typeName)
        {
            throw Exception <InnerElementHasOtherType> ("Found inner element with type " + node.first + " instead of " +
                sourceField.typeName + ".");
        }

        objects_.push_back (constructor (&node.second));
    }
}

DataObjectArrayField::~DataObjectArrayField ()
{
    for (DataObjectField *object : objects_)
    {
        delete object;
    }
}

static DataObjectField *IntConstructor (DataObjectField::PTree *source)
{
    return new DataObjectIntField (source);
}

static DataObjectField *FloatConstructor (DataObjectField::PTree *source)
{
    return new DataObjectFloatField (source);
}

static DataObjectField *StringConstructor (DataObjectField::PTree *source)
{
    return new DataObjectStringField (source);
}

DataObjectArrayField::ElementConstructor DataObjectArrayField::GetElementConstructor (
    PluginAPI *api, const DataClass::Field &sourceField)
{
    if (sourceField.typeName == "int")
    {
        return IntConstructor;
    }
    else if (sourceField.typeName == "float")
    {
        return FloatConstructor;
    }
    else if (sourceField.typeName == "string")
    {
        return StringConstructor;
    }
    else if (sourceField.reference)
    {
        return [api, &sourceField] (PTree *source) -> DataObjectField *
        { return new DataObjectReferenceField (sourceField.typeName, source); };
    }
    else
    {
        return [api, &sourceField] (PTree *source) -> DataObjectField *
        { return new DataObjectValueField (api, sourceField.typeName, source); };
    }
}
}
}