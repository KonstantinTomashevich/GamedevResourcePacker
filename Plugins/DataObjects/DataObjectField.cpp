#include "DataObjectField.hpp"
#include <Shared/ObjectReference.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectField::~DataObjectField ()
{

}

std::ostream &DataObjectField::Indent (std::ostream &output, int32_t size)
{
    for (int32_t index = 0; index < size; ++index)
    {
        output.put (' ');
    }

    return output;
}
}
}
