#pragma once
#include <boost/filesystem/path.hpp>

namespace ResourceSubsystem
{
class RuntimeGroup;
class Object
{
public:
    Object (int id) noexcept;
    virtual ~Object () = default;

    unsigned int GetId () const noexcept;
    RuntimeGroup *GetGroup () const noexcept;
    void SetGroup (RuntimeGroup *group) noexcept;

    virtual void Load (const boost::filesystem::path &path) = 0;
    int GetRefCount () const noexcept;
    void Ref () noexcept;
    void Unref () noexcept;

private:
    unsigned int id_;
    RuntimeGroup *group_;
    unsigned int refCount_;
};
}