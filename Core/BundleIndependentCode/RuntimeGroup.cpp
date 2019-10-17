#include "RuntimeGroup.hpp"

namespace ResourceSubsystem
{
RuntimeGroup::RuntimeGroup (uint32_t id) noexcept
    : id_ (id), used_ (), unused_ ()
{

}

RuntimeGroup::~RuntimeGroup ()
{
    DisposeUnused ();
    for (auto idObjectPair : used_)
    {
        delete idObjectPair.second;
    }
}

uint32_t RuntimeGroup::GetId () const noexcept
{
    return id_;
}

Object *RuntimeGroup::Query (uint32_t id) const noexcept
{
    try
    {
        return used_.at (id);
    }
    catch (std::out_of_range &exception)
    {
        // Do nothing, try check unused map.
    }

    try
    {
        return unused_.at (id);
    }
    catch (std::out_of_range &exception)
    {
        return nullptr;
    }
}

bool RuntimeGroup::Add (Object *object) noexcept
{
    bool result;
    if (object->GetRefCount ())
    {
        result = used_.insert (std::make_pair (object->GetId (), object)).second;
    }
    else
    {
        result = unused_.insert (std::make_pair (object->GetId (), object)).second;
    }

    if (result)
    {
        object->SetGroup (this);
    }

    return result;
}

void RuntimeGroup::DisposeUnused () noexcept
{
    for (auto idObjectPair : unused_)
    {
        delete idObjectPair.second;
    }

    unused_.clear ();
}

void RuntimeGroup::UseObject (Object *object) noexcept
{
    if (unused_.erase (object->GetId ()))
    {
        used_.insert (std::make_pair (object->GetId (), object));
    }
}

void RuntimeGroup::UnuseObject (Object *object) noexcept
{
    if (used_.erase (object->GetId ()))
    {
        unused_.insert (std::make_pair (object->GetId (), object));
    }
}
}
