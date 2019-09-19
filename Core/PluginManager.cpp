#include "PluginManager.hpp"
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <boost/log/trivial.hpp>

#include <cstdio>
#include <Shared/MultithreadedLog.hpp>

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
            MT_LOG(info, "Trying to load plugin from " << path.string () << "...");

            boost::shared_ptr <PluginAPI> plugin = boost::dll::import <PluginAPI> (path, "plugin");
            if (plugin)
            {
                MT_LOG(info, "Loaded plugin \"" << plugin->GetName () << "\" from " << path.string ()
                                        << ".");
                plugins_.push_back (plugin);
            }
            else
            {
                MT_LOG(error, "Unable to load plugin from " << path.string () << ".");
            }
        }

        iterator++;
    }
}

bool PluginManager::LoadPluginsConfig (const Path &configFolder)
{
    MT_LOG(info, "Loading plugins config from " << configFolder.string () << ".");
    bool failed = false;

#pragma omp parallel for
    for (int index = 0; index < plugins_.size (); ++index)
    {
        auto &plugin = plugins_[index];
        MT_LOG(info, "Loading config of plugin \"" << plugin->GetName () << "\".");

        if (plugin->Load (configFolder))
        {
            MT_LOG(info, "Done loading config for plugin \"" << plugin->GetName () << "\".");
        }
        else
        {
            MT_LOG(fatal, "Unable to load config for plugin \"" << plugin->GetName () << "\".");
            failed = true;
        }
    }

    return !failed;
}

const PluginVector &PluginManager::GetPluginsVector () const
{
    return plugins_;
}

Object *PluginManager::Capture (const boost::filesystem::path &asset)
{
    Object *object = nullptr;
    for (const auto &pluginApiPtr : plugins_)
    {
        object = pluginApiPtr->Capture (asset);
        if (object != nullptr)
        {
            return object;
        }
    }

    return nullptr;
}
}
