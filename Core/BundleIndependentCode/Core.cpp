#include "Core.hpp"
#include "Exception.hpp"
#include "CheckedFileRead.hpp"

#include <boost/log/trivial.hpp>
#include <cstdio>
#include <unordered_map>
#include <string>

namespace ResourceSubsystem
{
class LoadPath
{
public:
    LoadPath (const std::string &fileName, uint32_t offset)
        : fileName_ (fileName), offset_ (offset)
    {

    }

    std::string fileName_;
    uint32_t offset_;
};

typedef struct
{
    RuntimeGroup runtime;
    std::unordered_map <uint32_t, LoadPath> loadPaths;
} Group;

static std::unordered_map <uint32_t, Group> groups_;

void Init (const boost::filesystem::path &assetFolder)
{
    boost::filesystem::path contentListPath = assetFolder / "content.list";
    FILE *contentList = fopen (contentListPath.string ().c_str (), "rb");

    if (!contentList)
    {
        BOOST_THROW_EXCEPTION (Exception <NoContentList> ("Unable to open content.list."));
    }

    size_t groupCount;
    CheckedFileRead <BrokenContentList> (&groupCount, sizeof (groupCount), 1, contentList,
                                         "Unable to read group count!");

    BOOST_LOG_TRIVIAL (info) << "ResourceSubsystem: Found " << groupCount << " groups.";
    for (int32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        uint32_t groupId;
        CheckedFileRead <BrokenContentList> (&groupId, sizeof (groupId), 1, contentList,
                                             "Unable to read group hash!");

        BOOST_LOG_TRIVIAL (info) << "ResourceSubsystem: Found group " << groupId << ".";
        auto insertionResult = groups_.insert (std::make_pair (groupId, Group {RuntimeGroup (groupId), {}}));
        if (!insertionResult.second)
        {
            BOOST_THROW_EXCEPTION (Exception <BrokenContentList> ("Found group with duplicate hash " +
                std::to_string (groupId) + "."));
        }

        Group &group = insertionResult.first->second;
        size_t groupSize;
        CheckedFileRead <BrokenContentList> (&groupSize, sizeof (groupSize), 1, contentList,
                                             "Unable to read group size!");

        for (int32_t objectIndex = 0; objectIndex < groupSize; ++objectIndex)
        {
            uint32_t objectId;
            uint32_t fileNameSize;
            std::string fileName;
            uint32_t offset;

            CheckedFileRead <BrokenContentList> (&objectId, sizeof (objectId), 1, contentList,
                                                 "Unable to read object hash!");
            CheckedFileRead <BrokenContentList> (&fileNameSize, sizeof (fileNameSize), 1, contentList,
                                                 "Unable to read file name size!");

            fileName.resize (fileNameSize);
            CheckedFileRead <BrokenContentList> (&fileName[0], sizeof (char), fileNameSize, contentList,
                                                 "Unable to read file name!");
            CheckedFileRead <BrokenContentList> (&offset, sizeof (offset), 1, contentList,
                                                 "Unable to read offset!");

            boost::filesystem::path loadPath = assetFolder / fileName;
            if (!group.loadPaths.insert (std::make_pair (objectId, LoadPath (loadPath.string (), offset))).second)
            {
                BOOST_THROW_EXCEPTION (Exception <BrokenContentList> ("Found object with duplicate hash " +
                    std::to_string (objectId) + " in group " + std::to_string (groupId) + "."));
            }
        }
    }

    fclose (contentList);
}

Object *GetResource (Loader loader, uint32_t group, uint32_t id)
{
    try
    {
        Group &groupInfo = groups_.at (group);
        Object *object = groupInfo.runtime.Query (id);

        if (object != nullptr)
        {
            return object;
        }

        auto loadPathInfo = groupInfo.loadPaths.find (id);
        if (loadPathInfo == groupInfo.loadPaths.end ())
        {
            BOOST_THROW_EXCEPTION (Exception <ObjectNotExists> ("Object " + std::to_string (id) + "not found!"));
        }

        object = loader (id, loadPathInfo->second.fileName_, loadPathInfo->second.offset_);
        if (object == nullptr)
        {
            BOOST_THROW_EXCEPTION (Exception <LoadedObjectIsNull> ("Loaded object is null!"));
        }

        object->Ref ();
        groupInfo.runtime.Add (object);
        return object;
    }
    catch (std::out_of_range &exception)
    {
        BOOST_THROW_EXCEPTION (Exception <GroupNotExists> ("Group " + std::to_string (group) + " not found!"));
    }
}

RuntimeGroup *GetRuntimeGroup (uint32_t group) noexcept
{
    try
    {
        Group &groupInfo = groups_.at (group);
        return &groupInfo.runtime;
    }
    catch (std::out_of_range &exception)
    {
        return nullptr;
    }
}

void DisposeAllUnused () noexcept
{
    for (auto &idGroupPair : groups_)
    {
        idGroupPair.second.runtime.DisposeUnused ();
    }
}
}
