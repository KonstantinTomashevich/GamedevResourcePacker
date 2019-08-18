#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Shared/PluginAPI.hpp>

#define PLUGINS_LIST_INITIAL_CAPACITY 10

namespace GamedevResourcePacker
{
using PluginVector = boost::container::small_vector <boost::shared_ptr <PluginAPI>, PLUGINS_LIST_INITIAL_CAPACITY>;
class PluginManager
{
public:
    PluginManager ();
    virtual ~PluginManager ();

    void LoadPlugins ();
    bool LoadPluginsConfig (const boost::filesystem::path &configFolder);
    const PluginVector &GetPluginsVector () const;

    /// Calls Capture method for each plugin until one of them returns valid object.
    /// Returns nullptr if all plugins rejected to given capture this asset.
    Object *Capture (const boost::filesystem::path &asset);

private:
    PluginVector plugins_;
};
}

