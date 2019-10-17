#pragma once
#include <boost/filesystem/path.hpp>

namespace ResourceSubsystem
{
class RuntimeGroup;
class Object
{
public:
    /// All private objects will have that id.
    const static int32_t PRIVATE_OBJECT_ID = 0;

    Object (int32_t id) noexcept;
    virtual ~Object () = default;

    uint32_t GetId () const noexcept;
    RuntimeGroup *GetGroup () const noexcept;
    void SetGroup (RuntimeGroup *group) noexcept;

    int32_t GetRefCount () const noexcept;
    void Ref () noexcept;
    void Unref () noexcept;

private:
    uint32_t id_;
    RuntimeGroup *group_;
    uint32_t refCount_;
};
}
