#include "DataObject.hpp"
#include "PluginAPI.hpp"
#include "DataObjectValueField.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObject::DataObject (const std::string &name, const std::string &rootName, DataObject::PTree &root,
    PluginAPI *owner, DataClassProvider *provider)
    : Object (owner, name, rootName), rootField_ (new DataObjectValueField (provider, rootName, root))
{

}

DataObject::~DataObject ()
{
    delete rootField_;
}

void DataObject::Print (std::ostream &output) const
{
    output << "DataObject \"" << GetUniqueName () << "\" of type \"" << GetResourceClassName () << "\"" << std::endl;
    rootField_->Print (output, 4);
}
}
}
