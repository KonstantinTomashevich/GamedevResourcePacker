#pragma once
#include <unordered_map>
#include "DataClass.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class DataClassProvider
{
public:
    DataClassProvider ();
    virtual ~DataClassProvider ();

    bool AddDataClass (DataClass *dataClass);
    bool RemoveDataClass (DataClass *dataClass);
    DataClass *GetDataClass (const std::string &name) const;

private:
    std::unordered_map <std::string, DataClass *> dataClasses_;
};
}
}

