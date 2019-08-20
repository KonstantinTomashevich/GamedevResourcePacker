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

unsigned int ObjectReference::GetClassNameHash () const
{
    return classNameHash_;
}

unsigned int ObjectReference::GetObjectNameHash () const
{
    return objectNameHash_;
}

void ObjectReference::Resolve (unsigned int classNameHash, unsigned int objectNameHash)
{
    classNameHash_ = classNameHash;
    objectNameHash_ = objectNameHash;
}
}
