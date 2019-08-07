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

private:
    std::unordered_map <std::string, DataClass *> dataClasses_;
};

extern "C" BOOST_SYMBOL_EXPORT PluginAPI plugin;

PluginAPI plugin;
}
}


