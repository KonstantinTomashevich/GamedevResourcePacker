#include "DataObjectField.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObjectField::~DataObjectField ()
{

}

std::ostream &DataObjectField::Indent (std::ostream &output, int size)
{
    for (int index = 0; index < size; ++index)
    {
        output.put (' ');
    }

    return output;
}
}
}
