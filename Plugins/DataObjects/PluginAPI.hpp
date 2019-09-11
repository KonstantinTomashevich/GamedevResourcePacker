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

    virtual void GenerateCode (const boost::filesystem::path &outputFolder) const;
    virtual std::vector <std::string> GenerateDefines () const;

private:
    void GenerateLoadersCode (const boost::filesystem::path &outputFolder) const;

    DataClassProvider dataClassProvider_;
};
}
}


