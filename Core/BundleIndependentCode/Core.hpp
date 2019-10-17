#pragma once
#include "Object.hpp"
#include "RuntimeGroup.hpp"
#include <functional>

namespace ResourceSubsystem
{
typedef Object *(*Loader) (int32_t id, const std::string &, uint32_t offset);
void Init (const boost::filesystem::path &assetFolder);
Object *GetResource (Loader loader, uint32_t group, uint32_t id);

RuntimeGroup *GetRuntimeGroup (uint32_t group) noexcept;
void DisposeAllUnused () noexcept;

// Exception classes.
class NoContentList;
class BrokenContentList;
class GroupNotExists;
class ObjectNotExists;
class LoadedObjectIsNull;
}
