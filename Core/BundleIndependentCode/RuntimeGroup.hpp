#pragma once
#include <unordered_map>
#include "Object.hpp"

namespace ResourceSubsystem
{
class RuntimeGroup
{
public:
    explicit RuntimeGroup (unsigned int id) noexcept;
    virtual ~RuntimeGroup ();

    unsigned int GetId () const noexcept;
    Object *Query (unsigned int id) const noexcept;
    bool Add (Object *object) noexcept;
    void DisposeUnused () noexcept;

    void UseObject (Object *object) noexcept;
    void UnuseObject (Object *object) noexcept;

private:
    unsigned int id_;
    std::unordered_map <unsigned int, Object *> used_;
    std::unordered_map <unsigned int, Object *> unused_;
};
}
