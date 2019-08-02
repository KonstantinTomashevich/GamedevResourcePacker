#include <fstream>
#include <iostream>
#include <boost/dll.hpp>

#include "PluginAPI.hpp"
#include "PluginManager.hpp"

namespace GamedevResourcePacker
{
int Main (int argCount, char **argValues)
{
    PluginManager pluginManager;
    pluginManager.Load ();
    return 0;
}
}

int main (int argCount, char **argValues)
{
    return GamedevResourcePacker::Main (argCount, argValues);
}
