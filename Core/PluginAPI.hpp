#pragma once
#include <string>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include "Object.hpp"

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_VISIBLE PluginAPI
{
public:
    virtual const char *GetName () const = 0;
    virtual bool Load (const boost::filesystem::path &configFolder) = 0;
    /// Offers plugin to capture asset represented by given path.
    /// Returns object that represents given asset or nullptr if plugin rejects to capture asset.
    virtual Object *Capture (const boost::filesystem::path &asset) = 0;
    virtual ~PluginAPI () = default;
};
}
