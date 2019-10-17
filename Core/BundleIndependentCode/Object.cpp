#include "Object.hpp"
#include "RuntimeGroup.hpp"

namespace ResourceSubsystem
{
Object::Object (int32_t id) noexcept
    : id_ (id), group_ (nullptr), refCount_ (0)
{

}

uint32_t Object::GetId () const noexcept
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

int32_t Object::GetRefCount () const noexcept
{
    return refCount_;
}

void Object::Ref () noexcept
{
    if (refCount_ < UINT_MAX)
    {
        if (++refCount_ == 1 && group_ != nullptr)
        {
            group_->UseObject (this);
        }
    }
}

void Object::Unref () noexcept
{
    if (refCount_ > 0)
    {
        if (--refCount_ == 0 && group_ != nullptr)
        {
            group_->UnuseObject (this);
        }
    }
}
}
