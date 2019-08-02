#include "PluginAPI.hpp"
std::string GamedevResourcePacker::DataObjectsPlugin::PluginAPI::GetName () const
{
    return "Data Objects";
}

bool GamedevResourcePacker::DataObjectsPlugin::PluginAPI::Load (const boost::filesystem::path &configFolder)
{
    return false;
}
