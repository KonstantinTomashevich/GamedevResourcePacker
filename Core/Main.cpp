#include <fstream>
#include <cstdio>

#include <boost/dll.hpp>
#include <boost/log/trivial.hpp>
#include <boost/exception/all.hpp>

#include <Shared/PluginAPI.hpp>
#include <Shared/MultithreadedLog.hpp>

#include "PluginManager.hpp"
#include "ObjectManager.hpp"
#include "CodeGenerator.hpp"

#define INCORRECT_ARGS_CODE -1
#define UNABLE_TO_LOAD_CONFIG -2
#define PACKAGING_FATAL_ERROR -3
#define OUTPUT_FATAL_ERROR -4

// TODO: Don't use int's as it may cause problems with 32bit/64bin conversion!
namespace GamedevResourcePacker
{
int Main (int argCount, char **argValues)
{
    if (argCount != 5)
    {
        printf ("Expected arguments:\n"
                "    Variant 1. 4 args:\n"
                "        - plugin config folder.\n"
                "        - assets folder.\n"
                "        - package output folder.\n"
                "        - code output folder.\n");
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

    try
    {
        objectManager.ResolveObjectReferences ();
    }
    catch (boost::exception &exception)
    {
        MT_LOG (fatal, "Exception caught: " << boost::diagnostic_information (exception));
        return PACKAGING_FATAL_ERROR;
    }

    boost::filesystem::path outputFolder (argValues[3]);
    if (!objectManager.WriteBinaries (outputFolder))
    {
        return OUTPUT_FATAL_ERROR;
    }

    boost::filesystem::path codeOutputFolder (argValues[4]);
    CodeGenerator codeGenerator (&objectManager, &pluginManager);
    codeGenerator.Generate (codeOutputFolder);
    return 0;
}
}

int main (int argCount, char **argValues)
{
    return GamedevResourcePacker::Main (argCount, argValues);
}
