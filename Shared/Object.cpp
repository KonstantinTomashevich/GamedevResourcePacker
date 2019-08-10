#include "Object.hpp"

GamedevResourcePacker::PluginAPI *GamedevResourcePacker::Object::GetOwnerAPI () const
{
    return ownerAPI_;
}

const std::string &GamedevResourcePacker::Object::GetUniqueName () const
{
    return uniqueName_;
}

const std::string &GamedevResourcePacker::Object::GetResourceClassName () const
{
    return resourceClassName_;
}

GamedevResourcePacker::Object::Object (PluginAPI *ownerAPI, const std::string &uniqueName,
                                       const std::string &resourceClassName)
    : ownerAPI_ (ownerAPI), uniqueName_ (uniqueName), resourceClassName_ (resourceClassName)
{

}
