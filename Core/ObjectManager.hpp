#pragma once
#include <Shared/Object.hpp>
#include "PluginManager.hpp"
#include <unordered_map>
#include <unordered_set>
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
    bool IsContentListOverwritten () const;
    /// Returns null if there is no object name map for given resource class.
    const ObjectNameMap *GetObjectMap (const std::string &resourceClass) const;
    /// Recursively scans assets folder trying to capture objects.
    void ScanAssetsDir (const boost::filesystem::path &assetsFolder, PluginManager *pluginManager);
    /// Tries to resolve outer references for all objects, throws exception on failure.
    void ResolveObjectReferences ();
    /// Tries to write index and assets to given output folder.
    bool WriteBinaries (const boost::filesystem::path &outputFolder);

    // Exceptions.
    class ClassNameHashCollision;
    class ObjectNameHashCollision;
    class OuterReferenceCanNotBeResolved;

private:
    void ResolveObjectReference (ObjectReference *reference);
    bool WriteContentList (const boost::filesystem::path &outputFolder);
    bool WriteObjects (const boost::filesystem::path &rootOutputFolder) const;

    bool IsContentListChanged (const boost::filesystem::path &contentListPath,
                               const std::unordered_map <uint32_t, std::unordered_set <uint32_t> > &
                               existingHashes);

    boost::filesystem::path GetObjectOutputPath (const boost::filesystem::path &rootOutputFolder,
        const Object *object) const;
    std::string GetObjectOutputFileName (uint32_t objectId, uint32_t classId) const;


    ResourceClassMap resourceClassMap_;
    bool contentListOverwritten_;
    bool repackerDetected_;
};
}
