#include "DataObjectReferenceField.hpp"
#include "PluginAPI.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectReferenceField::DataObjectReferenceField (const std::string &typeName, DataObjectField::PTree &source)
    : reference_ (typeName, source.data ())
{

}

void DataObjectReferenceField::IterateOuterReferences (const DataObjectField::ReferenceIterationCallback &callback)
{
    callback (&reference_);
}

void DataObjectReferenceField::Print (std::ostream &output, int indentation) const
{
    Indent (output, indentation) << "Reference to \"" << reference_.GetObjectName () <<
                                 "\" of type \"" << reference_.GetClassName () << "\"" << std::endl;
}
}
}
