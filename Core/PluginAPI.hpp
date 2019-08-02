#pragma once
#include <string>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_VISIBLE PluginAPI
{
public:
    virtual std::string GetName () const = 0;
    virtual bool Load (const boost::filesystem::path &configFolder) = 0;
    virtual ~PluginAPI () = default;
};
}
