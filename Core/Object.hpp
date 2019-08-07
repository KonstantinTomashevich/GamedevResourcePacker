#pragma once
#include <boost/config.hpp>
#include <string>

namespace GamedevResourcePacker
{
class PluginAPI;

class BOOST_SYMBOL_VISIBLE Object
{
public:
    virtual ~Object () = default;
    PluginAPI *GetOwnerAPI () const;
    const std::string &GetUniqueName () const;
    const std::string &GetResourceClassName () const;

protected:
    Object (PluginAPI *ownerAPI, const std::string &uniqueName, const std::string &resourceClassName);

private:
    PluginAPI *ownerAPI_;
    std::string uniqueName_;
    std::string resourceClassName_;
};
}
