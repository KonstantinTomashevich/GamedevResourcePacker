#include <fstream>
#include <iostream>
#include <boost/dll.hpp>

#include "PluginAPI.hpp"

namespace GamedevResourcePacker
{
int Main (int argCount, char **argValues)
{
    boost::dll::fs::path pluginDir ("plugins");
    boost::shared_ptr <PluginAPI> testPlugin =
        boost::dll::import <PluginAPI> (pluginDir / "DataObjects", "plugin", boost::dll::load_mode::append_decorations);

    std::cout << testPlugin->GetName ();
    return 0;
}
}

int main (int argCount, char **argValues)
{
    return GamedevResourcePacker::Main (argCount, argValues);
}
