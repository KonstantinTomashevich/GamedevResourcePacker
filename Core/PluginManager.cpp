#include "PluginManager.hpp"
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <cstdio>
#include <boost/log/trivial.hpp>

namespace GamedevResourcePacker
{
using Path = boost::filesystem::path;
using DirIterator = boost::filesystem::directory_iterator;

PluginManager::PluginManager ()
    : plugins_ ()
{

}

PluginManager::~PluginManager ()
{

}

void PluginManager::LoadPlugins ()
{
    Path pluginDir ("plugins");
    DirIterator iterator (pluginDir);
    DirIterator end;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            const Path &path = iterator->path ();
            BOOST_LOG_TRIVIAL(info) << "Trying to load plugin from " << path.string () << "...";

            boost::shared_ptr <PluginAPI> plugin = boost::dll::import <PluginAPI> (path, "plugin");
            if (plugin)
            {
                BOOST_LOG_TRIVIAL(info) << "Loaded plugin \"" << plugin->GetName () << "\" from " << path.string ()
                                        << ".";
                plugins_.push_back (plugin);
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Unable to load plugin from " << path.string () << ".";
            }
        }

        iterator++;
    }
}

bool PluginManager::LoadPluginsConfig (const Path &configFolder)
{
    BOOST_LOG_TRIVIAL(info) << "Loading plugins config from " << configFolder.string () << ".";
    for (auto &plugin : plugins_)
    {
        BOOST_LOG_TRIVIAL(info) << "Loading config of plugin \"" << plugin->GetName () << "\".";
        if (plugin->Load (configFolder))
        {
            BOOST_LOG_TRIVIAL(info) << "Done loading config for plugin \"" << plugin->GetName () << "\".";
        }
        else
        {
            BOOST_LOG_TRIVIAL(fatal) << "Unable to load config for plugin \"" << plugin->GetName () << "\".";
            return false;
        }
    }

    return true;
}

const PluginVector &PluginManager::GetPluginsVector () const
{
    return plugins_;
}
}
