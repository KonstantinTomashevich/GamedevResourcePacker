#include "DataObjectReferenceField.hpp"
#include "PluginAPI.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectReferenceField::DataObjectReferenceField (const std::string &typeName, DataObjectField::PTree *source)
    : typeName_ (typeName), value_ (source->data ())
{

}
}
}
