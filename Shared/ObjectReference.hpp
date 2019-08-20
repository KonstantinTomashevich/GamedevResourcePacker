#pragma once
#include <boost/config.hpp>
#include <string>

namespace GamedevResourcePacker
{
class BOOST_SYMBOL_EXPORT ObjectReference
{
public:
    ObjectReference (const std::string &className, const std::string &objectName);
    virtual ~ObjectReference () = default;

    const std::string &GetClassName () const;
    const std::string &GetObjectName () const;
    unsigned int GetClassNameHash () const;
    unsigned int GetObjectNameHash () const;
    void Resolve (unsigned int classNameHash, unsigned int objectNameHash);

private:
    std::string className_;
    std::string objectName_;
    unsigned int classNameHash_;
    unsigned int objectNameHash_;
};
}
