#include <fstream>
#include <cstdio>
#include <boost/dll.hpp>

#include <Shared/PluginAPI.hpp>
#include "PluginManager.hpp"
#include "ObjectManager.hpp"

#define INCORRECT_ARGS_CODE -1
#define UNABLE_TO_LOAD_CONFIG -2

namespace GamedevResourcePacker
{
int Main (int argCount, char **argValues)
{
    if (argCount != 4)
    {
        printf ("Expected arguments:\n"
                "    Variant 1. 3 args:\n"
                "        - plugin config folder.\n"
                "        - assets folder.\n"
                "        - package output folder.\n");
        return INCORRECT_ARGS_CODE;
    }

    PluginManager pluginManager;
    pluginManager.LoadPlugins ();

    boost::filesystem::path configFolder (argValues[1]);
    if (!pluginManager.LoadPluginsConfig (configFolder))
    {
        printf ("Unable to load plugins config, aborting execution.");
        return UNABLE_TO_LOAD_CONFIG;
    }

    ObjectManager objectManager;
    boost::filesystem::path assetsFolder (argValues[2]);
    objectManager.ScanAssetsDir (assetsFolder, &pluginManager);

    return 0;
}
}

int main (int argCount, char **argValues)
{
    return GamedevResourcePacker::Main (argCount, argValues);
}
