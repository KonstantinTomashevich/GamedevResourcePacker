#pragma once
#include <Shared/PluginAPI.hpp>
#include <unordered_map>
#include "DataClassProvider.hpp"

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class PluginAPI : public GamedevResourcePacker::PluginAPI
{
public:
    virtual ~PluginAPI () = default;

    virtual const char *GetName () const;
    virtual bool Load (const boost::filesystem::path &configFolder);
    virtual Object *Capture (const boost::filesystem::path &asset);
    virtual void GenerateCode (const boost::filesystem::path &outputFolder);

    DataClass *GetClassByName (const std::string &name) const;

private:
    DataClassProvider dataClassProvider_;
};
}
}


