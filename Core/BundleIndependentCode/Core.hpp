#pragma once
#include "Object.hpp"
#include "RuntimeGroup.hpp"
#include <functional>

namespace ResourceSubsystem
{
typedef Object *(*Loader) (int id, const boost::filesystem::path &);
void Init (const boost::filesystem::path &assetFolder);
Object *GetResource (Loader loader, unsigned int group, unsigned int id);

RuntimeGroup *GetRuntimeGroup (unsigned int group) noexcept;
void DisposeAllUnused () noexcept;

// Exception classes.
class NoContentList;
class BrokenContentList;
class GroupNotExists;
class ObjectNotExists;
class LoadedObjectIsNull;
}
