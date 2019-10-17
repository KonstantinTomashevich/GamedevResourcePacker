#include "ObjectManager.hpp"
#include "Exception.hpp"

#include <Shared/StringHash.hpp>
#include <Shared/MultithreadedLog.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/log/trivial.hpp>
#include <unordered_set>
#include <cstdio>

namespace GamedevResourcePacker
{
using DirRecursiveIterator = boost::filesystem::recursive_directory_iterator;

ObjectManager::ObjectManager ()
    : resourceClassMap_ (), contentListOverwritten_ (false), repackerDetected_ (false)
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

bool ObjectManager::IsContentListOverwritten () const
{
    return contentListOverwritten_;
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
    MT_LOG (info, "Loading assets from folder " << assetsFolder << ".");
    DirRecursiveIterator iterator (assetsFolder);
    DirRecursiveIterator end;
    std::vector <boost::filesystem::path> possibleAssets;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            possibleAssets.push_back (iterator->path ());
        }

        ++iterator;
    }

    // TODO: It will speedup process only if there is a lot of plugins (maybe 10 or more). Think if its useful.
    boost::mutex resourceClassMapInsert;
#pragma omp parallel for
    for (int32_t index = 0; index < possibleAssets.size (); ++index)
    {
        boost::filesystem::path &asset = possibleAssets[index];
        MT_LOG (info, "Trying to capture asset " << asset << ".");
        Object *object = pluginManager->Capture (asset);

        if (object != nullptr)
        {
            resourceClassMapInsert.lock ();
            resourceClassMap_[object->GetResourceClassName ()].insert (
                std::make_pair (object->GetUniqueName (), object));
            resourceClassMapInsert.unlock ();

            MT_LOG (info, "Plugin " << object->GetOwnerAPI ()->GetName () <<
                                    " captured asset " << object->GetUniqueName () << " of type "
                                    << object->GetResourceClassName () << ".");
        }
        else
        {
            MT_LOG (info, "Asset " << iterator->path () << " isn't captured by any plugin.");
        }
    }
}

void ObjectManager::ResolveObjectReferences ()
{
    MT_LOG (info, "Resolving objects outer references...");
    std::unordered_set <uint32_t> usedClassNames;
    std::vector <Object *> plainObjectList;

    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        uint32_t classNameHash = StringHash (resourceClassObjectMapPair.first);
        if (usedClassNames.count (classNameHash))
        {
            BOOST_THROW_EXCEPTION (Exception <ClassNameHashCollision> ("Hash " + std::to_string (classNameHash) +
                "(for class \"" + resourceClassObjectMapPair.first + "\") already exists in set!"));
        }

        usedClassNames.insert (classNameHash);
        std::unordered_set <uint32_t> usedObjectNames;
        ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;

        for (auto &nameObjectPair : objectNameMap)
        {
            uint32_t objectNameHash = StringHash (nameObjectPair.first);

            if (usedObjectNames.count (objectNameHash))
            {
                BOOST_THROW_EXCEPTION (Exception <ObjectNameHashCollision> ("Hash " + std::to_string (objectNameHash) +
                    "(for object \"" + resourceClassObjectMapPair.first + "\") already exists in set!"));
            }

            usedObjectNames.insert (objectNameHash);
            Object *object = nameObjectPair.second;
            plainObjectList.push_back (object);
        }
    }

    // TODO: Does it really speedups process?
#pragma omp parallel for
    for (int32_t index = 0; index < plainObjectList.size (); ++index)
    {
        Object *object = plainObjectList[index];
        MT_LOG (info, "Resolving outer references for object \"" << object->GetResourceClassName () << "::" <<
                                                                 object->GetUniqueName () << "\"...");

        for (ObjectReference *reference : object->GetOuterReferences ())
        {
            ResolveObjectReference (reference);
        }
    }
}

bool ObjectManager::WriteBinaries (const boost::filesystem::path &outputFolder)
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

bool ObjectManager::WriteContentList (const boost::filesystem::path &outputFolder)
{
    boost::filesystem::path target = outputFolder / "content.list";
    std::unordered_map <uint32_t, std::unordered_set <uint32_t> > existingHashes;

    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        uint32_t classNameHash = StringHash (resourceClassObjectMapPair.first);
        existingHashes.insert (std::make_pair (classNameHash, std::unordered_set <uint32_t> ()));
        const ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;

        for (auto &nameObjectPair : objectNameMap)
        {
            uint32_t objectNameHash = StringHash (nameObjectPair.first);
            existingHashes[classNameHash].insert (objectNameHash);
        }
    }

    contentListOverwritten_ = IsContentListChanged (target, existingHashes);
    if (!contentListOverwritten_)
    {
        return true;
    }

    FILE *output = fopen (target.string ().c_str (), "wb");

    if (output == nullptr)
    {
        MT_LOG (fatal, "Unable to open " << target << " for content list output.");
        return false;
    }

    MT_LOG (info, "Generating " << target << "...");
    size_t sizeContainer = existingHashes.size ();
    fwrite (&sizeContainer, sizeof (sizeContainer), 1, output);

    for (auto &nameHashHashSetPair : existingHashes)
    {
        uint32_t classNameHash = nameHashHashSetPair.first;
        fwrite (&classNameHash, sizeof (classNameHash), 1, output);

        const std::unordered_set <uint32_t> &objectsHashSet = nameHashHashSetPair.second;
        sizeContainer = objectsHashSet.size ();
        fwrite (&sizeContainer, sizeof (sizeContainer), 1, output);

        for (auto &hash : objectsHashSet)
        {
            uint32_t objectNameHash = hash;
            std::string assetFile = GetObjectOutputFileName (objectNameHash, classNameHash);
            uint32_t assetFileStringSize = assetFile.size ();
            uint32_t offset = 0;

            fwrite (&objectNameHash, sizeof (objectNameHash), 1, output);
            fwrite (&assetFileStringSize, sizeof (assetFileStringSize), 1, output);
            fwrite (assetFile.c_str (), sizeof (char), assetFileStringSize, output);
            fwrite (&offset, sizeof (offset), 1, output);
        }
    }

    fclose (output);
    MT_LOG (info, "Done " << target << " generation.");
    return true;
}

bool ObjectManager::WriteObjects (const boost::filesystem::path &rootOutputFolder) const
{
    std::vector <const Object *> plainObjectList;
    for (auto &resourceClassObjectMapPair : resourceClassMap_)
    {
        const ObjectNameMap &objectNameMap = resourceClassObjectMapPair.second;
        for (auto &nameObjectPair : objectNameMap)
        {
            const Object *object = nameObjectPair.second;
            plainObjectList.push_back (object);
        }
    }

    bool failed = false;
    std::unordered_map <std::string, FILE *> outputs;

#pragma omp parallel for
    for (int32_t index = 0; index < plainObjectList.size (); ++index)
    {
        const Object *object = plainObjectList[index];
        boost::filesystem::path outputFile = GetObjectOutputPath (rootOutputFolder, object);

        if (repackerDetected_ || object->NeedsExecution (outputFile))
        {
            FILE *output = fopen (outputFile.string ().c_str (), "wb");
            if (!output)
            {
                MT_LOG (fatal, "Unable to open " << outputFile << " for object \"" <<
                                                 object->GetUniqueName () << "\" of type \"" <<
                                                 object->GetResourceClassName () << "\" binary output.");
                failed = true;
            }

            MT_LOG (info, "Generating " << outputFile << "...");
            if (!failed && !object->Execute (output))
            {
                MT_LOG (fatal, "Unable to write object \"" << object->GetUniqueName () <<
                                                           "\" of type \"" << object->GetResourceClassName () <<
                                                           "\" because of internal error.");
                failed = true;
            }

            if (!failed)
            {
                MT_LOG (info, "Done " << outputFile << " generation...");
                fclose (output);
            }
        }
    }

    return !failed;
}

bool ObjectManager::IsContentListChanged (const boost::filesystem::path &contentListPath,
                                          const std::unordered_map <uint32_t, std::unordered_set <uint32_t> > &
                                          existingHashes)
{
    if (!boost::filesystem::exists (contentListPath))
    {
        return true;
    }

    FILE *contentList = fopen (contentListPath.string ().c_str (), "rb");
    if (!contentList)
    {
        return true;
    }

    size_t groupCount;
    fread (&groupCount, sizeof (groupCount), 1, contentList);

    if (groupCount != resourceClassMap_.size ())
    {
        return true;
    }

    std::unordered_map <uint32_t, std::unordered_set <uint32_t> > foundHashes;
    for (int32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        uint32_t groupId;
        fread (&groupId, sizeof (groupId), 1, contentList);
        foundHashes.insert (std::make_pair (groupId, std::unordered_set <uint32_t> ()));

        size_t groupSize;
        fread (&groupSize, sizeof (groupSize), 1, contentList);

        for (int32_t objectIndex = 0; objectIndex < groupSize; ++objectIndex)
        {
            uint32_t objectId;
            fread (&objectId, sizeof (objectId), 1, contentList);
            foundHashes[groupId].insert (objectId);

            uint32_t pathSize;
            std::string assetFile;
            fread (&pathSize, sizeof (pathSize), 1, contentList);

            assetFile.resize (pathSize);
            fread (&assetFile[0], sizeof (char), pathSize, contentList);
            std::string expectedAssetFile = GetObjectOutputFileName (objectId, groupId);

            uint32_t offset;
            fread (&offset, sizeof (offset), 1, contentList);

            if (offset != 0 || expectedAssetFile != assetFile)
            {
                // If assets were repacked, we should overwrite them.
                repackerDetected_ = true;
                fclose (contentList);
                return false;
            }
        }
    }

    fclose (contentList);
    return existingHashes != foundHashes;
}

boost::filesystem::path ObjectManager::GetObjectOutputPath (const boost::filesystem::path &rootOutputFolder,
                                                            const Object *object) const
{
    return rootOutputFolder / GetObjectOutputFileName (
        StringHash (object->GetUniqueName ()), StringHash (object->GetResourceClassName ()));
}

std::string ObjectManager::GetObjectOutputFileName (uint32_t objectId, uint32_t classId) const
{
    return std::to_string (objectId) + "." + std::to_string (classId);
}
}
