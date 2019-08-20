#pragma once
#include <Shared/Object.hpp>
#include "PluginManager.hpp"
#include <unordered_map>
#include <boost/filesystem.hpp>

namespace GamedevResourcePacker
{
class ObjectManager
{
public:
    using ObjectNameMap = std::unordered_map <std::string, Object *>;
    using ResourceClassMap = std::unordered_map <std::string, ObjectNameMap>;

    ObjectManager ();
    virtual ~ObjectManager ();

    const ResourceClassMap &GetResourceClassMap () const;
    /// Returns null if there is no object name map for given resource class.
    const ObjectNameMap *GetObjectMap (const std::string &resourceClass) const;
    /// Recursively scans assets folder trying to capture objects.
    void ScanAssetsDir (const boost::filesystem::path &assetsFolder, PluginManager *pluginManager);
    /// Tries to resolve outer references for all objects, throws exception on failure.
    void ResolveObjectReferences ();
    // TODO: Implement up-to-date cheking.
    /// Tries to write index and assets to given output folder.
    bool WriteBinaries (const boost::filesystem::path &outputFolder) const;

    // Exceptions.
    class ClassNameHashCollision;
    class ObjectNameHashCollision;
    class OuterReferenceCanNotBeResolved;

private:
    void ResolveObjectReference (ObjectReference *reference);
    bool WriteContentList (const boost::filesystem::path &outputFolder) const;
    bool WriteObjects (const boost::filesystem::path &rootOutputFolder) const;

    ResourceClassMap resourceClassMap_;
};
}
