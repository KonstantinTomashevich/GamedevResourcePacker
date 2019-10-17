#pragma once
#include "DataObjectField.hpp"
#include "DataClassProvider.hpp"
#include <boost/container/small_vector.hpp>

#define PREALLOCATED_FIELD_DATA_COUNT 15

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class DataObjectValueField : public DataObjectField
{
public:
    DataObjectValueField (DataClassProvider *provider, const std::string &typeName, PTree &source);
    virtual ~DataObjectValueField ();

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback);
    virtual bool Write (FILE *output);
    virtual void Print (std::ostream &output, int32_t indentation = 0) const;

    class TypeNotFound;
    class FieldNotFound;

private:
    using FieldDataVector = boost::container::small_vector <DataObjectField *, PREALLOCATED_FIELD_DATA_COUNT>;

    std::string typeName_;
    FieldDataVector fields_;
};
}
}
