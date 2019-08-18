#include "Object.hpp"
#include "PluginAPI.hpp"
#include <algorithm>

namespace GamedevResourcePacker
{
Object::~Object ()
{
    for (ObjectReference *reference : controlledOuterReferences_)
    {
        delete reference;
    }
}

PluginAPI *Object::GetOwnerAPI () const
{
    return ownerAPI_;
}

const std::string &Object::GetUniqueName () const
{
    return uniqueName_;
}

const std::string &Object::GetResourceClassName () const
{
    return resourceClassName_;
}

const std::vector <ObjectReference *> &Object::GetOuterReferences () const
{
    return outerReferences_;
}

Object::Object (PluginAPI *ownerAPI, const std::string &uniqueName,
                                       const std::string &resourceClassName)
    : ownerAPI_ (ownerAPI), uniqueName_ (uniqueName), resourceClassName_ (resourceClassName),
      outerReferences_ (), controlledOuterReferences_ ()
{

}

void Object::AddOuterReference (ObjectReference *reference, bool passControl)
{
    outerReferences_.push_back (reference);
    if (passControl)
    {
        controlledOuterReferences_.push_back (reference);
    }
}

bool Object::RemoveOuterReference (ObjectReference *reference, bool checkIsControlled)
{
    auto iterator = std::find (outerReferences_.begin (), outerReferences_.end (), reference);
    if (iterator == outerReferences_.end ())
    {
        return false;
    }

    outerReferences_.erase (iterator);
    if (checkIsControlled)
    {
        iterator = std::find (controlledOuterReferences_.begin (), controlledOuterReferences_.end (), reference);
        if (iterator != controlledOuterReferences_.end ())
        {
            controlledOuterReferences_.erase (iterator);
        }
    }

    return true;
}
}
