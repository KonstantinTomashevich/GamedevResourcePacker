#include "ObjectManager.hpp"
#include "Exception.hpp"

#include <Shared/StringHash.hpp>
#include <boost/log/trivial.hpp>
#include <unordered_set>
#include <cstdio>

namespace GamedevResourcePacker
{
using DirRecursiveIterator = boost::filesystem::recursive_directory_iterator;

ObjectManager::ObjectManager ()
    : resourceClassMap_ ()
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

void ObjectManager::ResolveObjectReferences ()
{
    BOOST_LOG_TRIVIAL (info) << "Resolving objects outer references...";
    std::unordered_set <unsigned int> usedClassNames;

    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        BOOST_LOG_TRIVIAL (info) << "Resolving outer references for objects of class \"" <<
                                 resourceClassObjectMapPair.first << "\"...";
        unsigned int classNameHash = StringHash (resourceClassObjectMapPair.first);

        if (usedClassNames.count (classNameHash))
        {
            BOOST_THROW_EXCEPTION (Exception <ClassNameHashCollision> ("Hash " + std::to_string (classNameHash) +
                "(for class \"" + resourceClassObjectMapPair.first + "\") already exists in set!"));
        }

        usedClassNames.insert (classNameHash);
        std::unordered_set <unsigned int> usedObjectNames;
        ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;

        for (auto &nameObjectPair : objectNameMap)
        {
            BOOST_LOG_TRIVIAL (info) << "Resolving outer references for object \"" << nameObjectPair.first << "\"...";
            unsigned int objectNameHash = StringHash (nameObjectPair.first);

            if (usedObjectNames.count (objectNameHash))
            {
                BOOST_THROW_EXCEPTION (Exception <ObjectNameHashCollision> ("Hash " + std::to_string (objectNameHash) +
                    "(for object \"" + resourceClassObjectMapPair.first + "\") already exists in set!"));
            }

            usedObjectNames.insert (objectNameHash);
            Object *object = nameObjectPair.second;

            for (ObjectReference *reference : object->GetOuterReferences ())
            {
                ResolveObjectReference (reference);
            }
        }
    }
}

bool ObjectManager::WriteBinaries (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::create_directories (outputFolder);
    return WriteContentList (outputFolder) && WriteObjects (outputFolder);
}

void ObjectManager::ResolveObjectReference (ObjectReference *reference)
{
    const ObjectNameMap *map = GetObjectMap (reference->GetClassName ());
    if (map == nullptr)
    {
        BOOST_THROW_EXCEPTION (Exception <OuterReferenceCanNotBeResolved> ("Unable to find objects with class name \"" +
            reference->GetClassName () + "\"!"));
    }

    if (map->count (reference->GetObjectName ()) == 0)
    {
        BOOST_THROW_EXCEPTION (Exception <OuterReferenceCanNotBeResolved> ("Unable to find object of class \"" +
            reference->GetClassName () + "\" and name \"" + reference->GetObjectName () + "\"!"));
    }

    reference->Resolve (StringHash (reference->GetClassName ()), StringHash (reference->GetObjectName ()));
}

bool ObjectManager::WriteContentList (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path target = outputFolder / "content.list";
    FILE *output = fopen (target.string ().c_str (), "wb");

    if (output == nullptr)
    {
        BOOST_LOG_TRIVIAL (fatal) << "Unable to open " << target << " for content list output.";
        return false;
    }

    BOOST_LOG_TRIVIAL (info) << "Generating " << target << "...";
    size_t sizeContainer = resourceClassMap_.size ();
    fwrite (&sizeContainer, sizeof (sizeContainer), 1, output);

    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        unsigned int classNameHash = StringHash (resourceClassObjectMapPair.first);
        fwrite (&classNameHash, sizeof (classNameHash), 1, output);

        const ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;
        sizeContainer = objectNameMap.size ();
        fwrite (&sizeContainer, sizeof (sizeContainer), 1, output);

        for (auto &nameObjectPair : objectNameMap)
        {
            unsigned int objectNameHash = StringHash (nameObjectPair.first);
            fwrite (&objectNameHash, sizeof (objectNameHash), 1, output);
        }
    }

    fclose (output);
    BOOST_LOG_TRIVIAL (info) << "Done " << target << " generation.";
    return true;
}

bool ObjectManager::WriteObjects (const boost::filesystem::path &rootOutputFolder) const
{
    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        unsigned int classNameHash = StringHash (resourceClassObjectMapPair.first);
        boost::filesystem::path classOutputFolder = rootOutputFolder / std::to_string (classNameHash);
        boost::filesystem::create_directories (classOutputFolder);
        const ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;

        for (auto &nameObjectPair : objectNameMap)
        {
            unsigned int objectNameHash = StringHash (nameObjectPair.first);
            const Object *object = nameObjectPair.second;
            boost::filesystem::path target = classOutputFolder / std::to_string (objectNameHash);
            FILE *output = fopen (target.string ().c_str (), "wb");

            if (output == nullptr)
            {
                BOOST_LOG_TRIVIAL (fatal) << "Unable to open " << target << " for object \"" <<
                                          object->GetUniqueName () << "\" of type \"" <<
                                          resourceClassObjectMapPair.first << "\" binary output.";
                return false;
            }

            BOOST_LOG_TRIVIAL (info) << "Generating " << target << "...";
            if (!object->Write (output))
            {
                BOOST_LOG_TRIVIAL (fatal) << "Unable to write object \"" << object->GetUniqueName () <<
                                          "\" of type \"" << resourceClassObjectMapPair.first <<
                                          "\" because of internal error.";
                return false;
            }

            fclose (output);
            BOOST_LOG_TRIVIAL (info) << "Done " << target << " generation.";
        }
    }

    return true;
}
}
