#include "DataClassProvider.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataClassProvider::DataClassProvider ()
    : dataClasses_ ()
{

}

DataClassProvider::~DataClassProvider ()
{
    for (auto &nameDataClassPair : dataClasses_)
    {
        delete nameDataClassPair.second;
    }
}

bool DataClassProvider::AddDataClass (DataClass *dataClass)
{
    return dataClasses_.insert (std::make_pair (dataClass->GetName (), dataClass)).second;
}

bool DataClassProvider::RemoveDataClass (DataClass *dataClass)
{
    return dataClasses_.erase (dataClass->GetName ()) > 0;
}

DataClass *DataClassProvider::GetDataClass (const std::string &name) const
{
    try
    {
        return dataClasses_.at (name);
    }
    catch (std::out_of_range &exception)
    {
        return nullptr;
    }
}
}
}
