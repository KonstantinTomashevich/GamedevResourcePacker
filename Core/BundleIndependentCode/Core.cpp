#include "Core.hpp"
#include "Exception.hpp"
#include "CheckedFileRead.hpp"

#include <boost/log/trivial.hpp>
#include <cstdio>
#include <unordered_map>
#include <string>

namespace ResourceSubsystem
{
typedef struct
{
    RuntimeGroup runtime;
    std::unordered_map <unsigned int, std::string> loadPaths;
} Group;

static std::unordered_map <unsigned int, Group> groups_;

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
    for (int groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        unsigned int groupId;
        CheckedFileRead <BrokenContentList> (&groupId, sizeof (groupId), 1, contentList,
                                             "Unable to read group hash!");

        auto insertionResult = groups_.insert (std::make_pair (groupId, Group {RuntimeGroup (groupId), {}}));
        if (!insertionResult.second)
        {
            BOOST_THROW_EXCEPTION (Exception <BrokenContentList> ("Found group with duplicate hash " +
                std::to_string (groupId) + "."));
        }

        boost::filesystem::path groupFolder = assetFolder / std::to_string (groupId);
        Group &group = insertionResult.first->second;
        size_t groupSize;

        CheckedFileRead <BrokenContentList> (&groupSize, sizeof (groupSize), 1, contentList,
                                             "Unable to read group size!");
        for (int objectIndex = 0; objectIndex < groupSize; ++objectIndex)
        {
            unsigned int objectId;
            CheckedFileRead <BrokenContentList> (&objectId, sizeof (objectId), 1, contentList,
                                                 "Unable to read object hash!");

            boost::filesystem::path loadPath = groupFolder / std::to_string (objectId);
            if (!group.loadPaths.insert (std::make_pair (objectId, loadPath.string ())).second)
            {
                BOOST_THROW_EXCEPTION (Exception <BrokenContentList> ("Found object with duplicate hash " +
                    std::to_string (objectId) + " in group " + std::to_string (groupId) + "."));
            }
        }
    }

    fclose (contentList);
}

Object *GetResource (const Loader &loader, unsigned int group, unsigned int id)
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

        object = loader (id, loadPathInfo->second);
        if (object == nullptr)
        {
            BOOST_THROW_EXCEPTION (Exception <LoadedObjectIsNull> ("Loaded object is null!"));
        }

        groupInfo.runtime.Add (object);
        return object;
    }
    catch (std::out_of_range &exception)
    {
        BOOST_THROW_EXCEPTION (Exception <GroupNotExists> ("Group " + std::to_string (group) + "not found!"));
    }
}

RuntimeGroup *GetRuntimeGroup (unsigned int group) noexcept
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
