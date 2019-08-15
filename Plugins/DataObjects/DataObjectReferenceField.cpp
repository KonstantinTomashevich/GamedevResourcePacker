#include "DataObjectReferenceField.hpp"
#include "PluginAPI.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectReferenceField::DataObjectReferenceField (const std::string &typeName, DataObjectField::PTree &source)
    : typeName_ (typeName), value_ (source.data ())
{

}

void DataObjectReferenceField::Print (std::ostream &output, int indentation) const
{
    Indent (output, indentation) << "Reference to \"" << value_ << "\" of type \"" << typeName_ << "\"" << std::endl;
}
}
}
