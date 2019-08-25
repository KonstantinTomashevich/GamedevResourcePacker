#include "CodeGenerator.hpp"

namespace GamedevResourcePacker
{
CodeGenerator::CodeGenerator (ObjectManager *objectManager, PluginManager *pluginManager)
    : objectManager_ (objectManager), pluginManager_ (pluginManager)
{

}

void CodeGenerator::Generate (const boost::filesystem::path &outputFolder) const
{
    // TODO: Implement basic code generation.
    for (auto &plugin : pluginManager_->GetPluginsVector ())
    {
        plugin->GenerateCode (outputFolder);
    }
}
}
