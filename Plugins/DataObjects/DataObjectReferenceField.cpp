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

bool DataObjectReferenceField::Write (FILE *output)
{
    uint32_t classHash = reference_.GetClassNameHash ();
    uint32_t objectHash = reference_.GetObjectNameHash ();

    fwrite (&classHash, sizeof (classHash), 1, output);
    fwrite (&objectHash, sizeof (objectHash), 1, output);
    return true;
}

void DataObjectReferenceField::Print (std::ostream &output, int32_t indentation) const
{
    Indent (output, indentation) << "Reference to \"" << reference_.GetObjectName () <<
                                 "\" of type \"" << reference_.GetClassName () << "\"" << std::endl;
}
}
}
