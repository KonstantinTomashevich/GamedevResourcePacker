#include "DataObject.hpp"
#include "PluginAPI.hpp"
#include "DataObjectValueField.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObject::DataObject (const std::string &name, const std::string &rootName, DataObject::PTree &root, PluginAPI *owner)
    : Object (owner, name, rootName), rootField_ (new DataObjectValueField (owner, rootName, root))
{

}

DataObject::~DataObject ()
{
    delete rootField_;
}

void DataObject::Print (std::ostream &output) const
{
    rootField_->Print (output);
}
}
}
