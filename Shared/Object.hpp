#pragma once
#include <boost/config.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include "ObjectReference.hpp"

namespace GamedevResourcePacker
{
class PluginAPI;

class BOOST_SYMBOL_EXPORT Object
{
public:
    virtual ~Object ();
    PluginAPI *GetOwnerAPI () const;
    const std::string &GetUniqueName () const;
    const std::string &GetResourceClassName () const;
    const std::vector <ObjectReference *> &GetOuterReferences () const;
    virtual bool Write (FILE *output) const = 0;

protected:
    Object (PluginAPI *ownerAPI, const std::string &uniqueName, const std::string &resourceClassName);
    void AddOuterReference (ObjectReference *reference, bool passControl);
    bool RemoveOuterReference (ObjectReference *reference, bool checkIsControlled = true);

private:
    PluginAPI *ownerAPI_;
    std::string uniqueName_;
    std::string resourceClassName_;

    std::vector <ObjectReference *> outerReferences_;
    /// Outer references, that must be deleted on object destruction.
    std::vector <ObjectReference *> controlledOuterReferences_;
};
}
