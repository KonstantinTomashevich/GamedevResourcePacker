#pragma once
#include <string>
#include <boost/config.hpp>

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_VISIBLE PluginAPI
{
public:
    virtual std::string GetName () const = 0;
    virtual ~PluginAPI () = default;
};
}
