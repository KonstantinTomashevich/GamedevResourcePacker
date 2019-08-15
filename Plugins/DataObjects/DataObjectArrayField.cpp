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
    DataClassProvider *provider, const DataClass::Field &sourceField, DataObjectField::PTree &source)
    : objects_ ()
{
    DataObjectArrayField::ElementConstructor constructor = GetElementConstructor (provider, sourceField);

    for (PTree::value_type &node : source)
    {
        if (node.first == "<xmlattr>") continue;
        if (node.first != sourceField.typeName)
        {
            throw Exception <InnerElementHasOtherType> ("Found inner element with type " + node.first + " instead of " +
                sourceField.typeName + ".");
        }

        objects_.push_back (constructor (node.second));
    }
}

DataObjectArrayField::~DataObjectArrayField ()
{
    for (DataObjectField *object : objects_)
    {
        delete object;
    }
}

void DataObjectArrayField::Print (std::ostream &output, int indentation) const
{
    Indent (output, indentation) << "Array of size " << objects_.size () << std::endl;
    for (DataObjectField *object : objects_)
    {
        object->Print (output, indentation + 4);
    }
}

static DataObjectField *IntConstructor (DataObjectField::PTree &source)
{
    return new DataObjectIntField (source);
}

static DataObjectField *FloatConstructor (DataObjectField::PTree &source)
{
    return new DataObjectFloatField (source);
}

static DataObjectField *StringConstructor (DataObjectField::PTree &source)
{
    return new DataObjectStringField (source);
}

DataObjectArrayField::ElementConstructor DataObjectArrayField::GetElementConstructor (
    DataClassProvider *provider, const DataClass::Field &sourceField)
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
        return [&sourceField] (PTree &source) -> DataObjectField *
        { return new DataObjectReferenceField (sourceField.typeName, source); };
    }
    else
    {
        return [provider, &sourceField] (PTree &source) -> DataObjectField *
        { return new DataObjectValueField (provider, sourceField.typeName, source); };
    }
}
}
}
