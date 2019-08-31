#pragma once
#include "Object.hpp"
#include "RuntimeGroup.hpp"
#include "Loaders.hpp"
#include <functional>

namespace ResourceSubsystem
{
void Init (const boost::filesystem::path &assetFolder);
Object *GetResource (const Loader &loader, unsigned int group, unsigned int id);
template <typename T> T *GetResources (unsigned int group, unsigned int id)
{
    return (T *) GetResource (GetLoader <T> (), group, id);
}

RuntimeGroup *GetRuntimeGroup (unsigned int group) noexcept;
void DisposeAllUnused () noexcept;

// Exception classes.
class NoContentList;
class BrokenContentList;
class GroupNotExists;
class ObjectNotExists;
class LoadedObjectIsNull;
}
