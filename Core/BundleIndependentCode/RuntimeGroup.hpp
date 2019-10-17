#pragma once
#include <unordered_map>
#include "Object.hpp"

namespace ResourceSubsystem
{
class RuntimeGroup
{
public:
    explicit RuntimeGroup (uint32_t id) noexcept;
    virtual ~RuntimeGroup ();

    uint32_t GetId () const noexcept;
    Object *Query (uint32_t id) const noexcept;
    bool Add (Object *object) noexcept;
    void DisposeUnused () noexcept;

    void UseObject (Object *object) noexcept;
    void UnuseObject (Object *object) noexcept;

private:
    uint32_t id_;
    std::unordered_map <uint32_t, Object *> used_;
    std::unordered_map <uint32_t, Object *> unused_;
};
}
