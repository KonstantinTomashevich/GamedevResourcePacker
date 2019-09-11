#pragma once
#include <boost/filesystem/path.hpp>

namespace ResourceSubsystem
{
class RuntimeGroup;
class Object
{
public:
    /// All private objects will have that id.
    const static int PRIVATE_OBJECT_ID = 0;

    Object (int id) noexcept;
    virtual ~Object () = default;

    unsigned int GetId () const noexcept;
    RuntimeGroup *GetGroup () const noexcept;
    void SetGroup (RuntimeGroup *group) noexcept;

    int GetRefCount () const noexcept;
    void Ref () noexcept;
    void Unref () noexcept;

private:
    unsigned int id_;
    RuntimeGroup *group_;
    unsigned int refCount_;
};
}
