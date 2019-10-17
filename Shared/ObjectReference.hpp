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
    uint32_t GetClassNameHash () const;
    uint32_t GetObjectNameHash () const;
    void Resolve (uint32_t classNameHash, uint32_t objectNameHash);

private:
    std::string className_;
    std::string objectName_;
    uint32_t classNameHash_;
    uint32_t objectNameHash_;
};
}
