#pragma once
#include "DataObjectField.hpp"
#include <boost/container/small_vector.hpp>

#define PREALLOCATED_FIELD_DATA_COUNT 15

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI;
class DataObjectValueField : public DataObjectField
{
public:
    // TODO: Maybe use references instead of pointers?
    DataObjectValueField (PluginAPI *api, const std::string &typeName, PTree *source);
    virtual ~DataObjectValueField ();

    virtual void Print (std::ostream &output, int indentation = 0) const;

    class TypeNotFound;
    class FieldNotFound;

private:
    using FieldDataVector = boost::container::small_vector <DataObjectField *, PREALLOCATED_FIELD_DATA_COUNT>;

    std::string typeName_;
    FieldDataVector fields_;
};
}
}
