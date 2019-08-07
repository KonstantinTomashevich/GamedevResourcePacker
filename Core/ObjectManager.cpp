#include "ObjectManager.hpp"
#include <boost/log/trivial.hpp>

namespace GamedevResourcePacker
{
using DirRecursiveIterator = boost::filesystem::recursive_directory_iterator;

ObjectManager::ObjectManager ()
{

}

ObjectManager::~ObjectManager ()
{
    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;
        for (auto &nameObjectPair : objectNameMap)
        {
            delete nameObjectPair.second;
        }
    }
}

const ObjectManager::ResourceClassMap &ObjectManager::GetResourceClassMap () const
{
    return resourceClassMap_;
}

const ObjectManager::ObjectNameMap *ObjectManager::GetObjectMap (const std::string &resourceClass) const
{
    try
    {
        const ObjectNameMap &map = resourceClassMap_.at (resourceClass);
        return &map;
    }
    catch (std::out_of_range &exception)
    {
        return nullptr;
    }
}

void ObjectManager::ScanAssetsDir (const boost::filesystem::path &assetsFolder,
                                   GamedevResourcePacker::PluginManager *pluginManager)
{
    BOOST_LOG_TRIVIAL (info) << "Loading assets from folder " << assetsFolder << ".";
    DirRecursiveIterator iterator (assetsFolder);
    DirRecursiveIterator end;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            BOOST_LOG_TRIVIAL (info) << "Trying to capture asset " << iterator->path () << ".";
            Object *object = pluginManager->Capture (iterator->path ());

            if (object != nullptr)
            {
                resourceClassMap_[object->GetResourceClassName ()].insert (
                    std::make_pair (object->GetUniqueName (), object));

                BOOST_LOG_TRIVIAL (info) << "Plugin " << object->GetOwnerAPI ()->GetName () <<
                                         " captured asset " << object->GetUniqueName () << " of type "
                                         << object->GetResourceClassName () << ".";
            }
            else
            {
                BOOST_LOG_TRIVIAL (info) << "Asset " << iterator->path () << " isn't captured by any plugin.";
            }
        }

        ++iterator;
    }
}
}
