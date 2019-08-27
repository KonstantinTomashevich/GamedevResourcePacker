#include "Object.hpp"
#include "RuntimeGroup.hpp"

namespace ResourceSubsystem
{
Object::Object (int id) noexcept
    : id_ (id), group_ (nullptr), refCount_ (0)
{

}

unsigned int Object::GetId () const noexcept
{
    return id_;
}

RuntimeGroup *Object::GetGroup () const noexcept
{
    return group_;
}

void Object::SetGroup (RuntimeGroup *group) noexcept
{
    group_ = group;
}

int Object::GetRefCount () const noexcept
{
    return refCount_;
}

void Object::Ref () noexcept
{
    if (refCount_ < UINT_MAX)
    {
        if (++refCount_ == 1)
        {
            group_->UseObject (this);
        }
    }
}

void Object::Unref () noexcept
{
    if (refCount_ > 0)
    {
        if (--refCount_ == 0)
        {
            group_->UnuseObject (this);
        }
    }
}
}
