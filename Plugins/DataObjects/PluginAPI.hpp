#pragma once
#include <Core/PluginAPI.hpp>
#include <unordered_map>
#include "DataClass.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI : public GamedevResourcePacker::PluginAPI
{
public:
    virtual ~PluginAPI ();

    virtual const char *GetName () const;
    virtual bool Load (const boost::filesystem::path &configFolder);
    virtual Object *Capture (const boost::filesystem::path &asset);

    DataClass *GetClassByName (const std::string &name) const;

private:
    std::unordered_map <std::string, DataClass *> dataClasses_;
};
}
}


