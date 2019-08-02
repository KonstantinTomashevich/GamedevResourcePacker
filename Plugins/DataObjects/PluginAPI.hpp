#pragma once
#include <Core/PluginAPI.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI : public GamedevResourcePacker::PluginAPI
{
public:
    virtual ~PluginAPI () = default;

    virtual std::string GetName () const;
    virtual bool Load (const boost::filesystem::path &configFolder);
};

extern "C" BOOST_SYMBOL_EXPORT PluginAPI plugin;

PluginAPI plugin;
}
}


