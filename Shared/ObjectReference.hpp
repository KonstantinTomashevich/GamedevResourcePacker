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
    void Resolve (int classNameHash, int objectNameHash);

private:
    std::string className_;
    std::string objectName_;
    int classNameHash_;
    int objectNameHash_;
};
}
