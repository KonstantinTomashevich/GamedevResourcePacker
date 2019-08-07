#pragma once
#include "Object.hpp"
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

private:
    ResourceClassMap resourceClassMap_;
};
}
