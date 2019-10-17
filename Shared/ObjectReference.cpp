#include "ObjectReference.hpp"

namespace GamedevResourcePacker
{
ObjectReference::ObjectReference (const std::string &className, const std::string &objectName)
    : className_ (className), objectName_ (objectName), classNameHash_ (0), objectNameHash_ (0)
{

}

const std::string &ObjectReference::GetClassName () const
{
    return className_;
}

const std::string &ObjectReference::GetObjectName () const
{
    return objectName_;
}

uint32_t ObjectReference::GetClassNameHash () const
{
    return classNameHash_;
}

uint32_t ObjectReference::GetObjectNameHash () const
{
    return objectNameHash_;
}

void ObjectReference::Resolve (uint32_t classNameHash, uint32_t objectNameHash)
{
    classNameHash_ = classNameHash;
    objectNameHash_ = objectNameHash;
}
}
