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

void PluginManager::Load ()
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
                BOOST_LOG_TRIVIAL(info) << "Loaded plugin \"" << plugin->GetName () << "\" from " << path.string () << ".";
                plugins_.push_back (plugin);
            }
            else
            {
                BOOST_LOG_TRIVIAL(info) << "Unable to load plugin from " << path.string () << ".";
            }
        }

        iterator++;
    }
}

const PluginVector &PluginManager::GetPluginsVector () const
{
    return plugins_;
}
}
