#include "DataObjectValueField.hpp"
#include "PluginAPI.hpp"

#include "DataObjectReferenceField.hpp"
#include "DataObjectArrayField.hpp"
#include "DataObjectSimpleField.hpp"

#include <boost/log/trivial.hpp>
#include <Core/Exception.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectValueField::DataObjectValueField (
    DataClassProvider *provider, const std::string &typeName, DataObjectField::PTree &source)
    : typeName_ (typeName), fields_ ()
{
    DataClass *type = provider->GetDataClass (typeName);
    if (type == nullptr)
    {
        BOOST_THROW_EXCEPTION (Exception <TypeNotFound> ("Unable to find class " + typeName + "!"));
    }

    for (auto &field : type->GetFields ())
    {
        // TODO: Catch and print as exception situations where arrays and value-classes are defined by attribute.
        auto node = source.get_child_optional ("<xmlattr>." + field.name);
        if (!node.is_initialized ())
        {
            node = source.get_child_optional (field.name);
            if (!node.is_initialized ())
            {
                BOOST_THROW_EXCEPTION (Exception <FieldNotFound> ("Unable to find field " + field.name + "!"));
            }
        }

        if (field.array)
        {
            fields_.push_back (new DataObjectArrayField (provider, field, *node.get_ptr ()));
        }
        else if (field.reference)
        {
            fields_.push_back (new DataObjectReferenceField (field.typeName, *node.get_ptr ()));
        }
        else if (field.typeName == "int")
        {
            fields_.push_back (new DataObjectIntField (*node.get_ptr ()));
        }
        else if (field.typeName == "float")
        {
            fields_.push_back (new DataObjectFloatField (*node.get_ptr ()));
        }
        else if (field.typeName == "string")
        {
            fields_.push_back (new DataObjectStringField (*node.get_ptr ()));
        }
        else
        {
            fields_.push_back (new DataObjectValueField (provider, field.typeName, *node.get_ptr ()));
        }
    }
}

DataObjectValueField::~DataObjectValueField ()
{
    for (DataObjectField *field : fields_)
    {
        delete field;
    }
}

void DataObjectValueField::Print (std::ostream &output, int indentation) const
{
    Indent (output, indentation) << "Value of type \"" << typeName_ << "\"" << std::endl;
    for (DataObjectField *field : fields_)
    {
        field->Print (output, indentation + 4);
    }
}
}
}
