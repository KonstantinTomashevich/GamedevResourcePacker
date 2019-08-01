#pragma once
#include <Core/PluginAPI.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI : public GamedevResourcePacker::PluginAPI
{
public:
    virtual std::string GetName () const;
};

extern "C" BOOST_SYMBOL_EXPORT PluginAPI plugin;
PluginAPI plugin;
}
}


