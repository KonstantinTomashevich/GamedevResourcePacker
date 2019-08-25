#pragma once
#include <boost/filesystem.hpp>
#include "ObjectManager.hpp"
#include "PluginManager.hpp"

namespace GamedevResourcePacker
{
class CodeGenerator
{
public:
    CodeGenerator (ObjectManager *objectManager, PluginManager *pluginManager);
    virtual ~CodeGenerator () = default;

    void Generate (const boost::filesystem::path &outputFolder) const;

private:
    ObjectManager *objectManager_;
    PluginManager *pluginManager_;
};
}
