#include "PluginAPI.hpp"
#include <boost/log/trivial.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
using Path = boost::filesystem::path;
using DirIterator = boost::filesystem::directory_iterator;

PluginAPI::~PluginAPI ()
{
    for (auto &nameDataClassPair : dataClasses_)
    {
        delete nameDataClassPair.second;
    }
}

std::string PluginAPI::GetName () const
{
    return "Data Objects";
}

bool PluginAPI::Load (const Path &configFolder)
{
    Path classesFolder = configFolder / "DataObjectClasses";
    DirIterator iterator (classesFolder);
    DirIterator end;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            BOOST_LOG_TRIVIAL (info) << "Trying to load data class from " << iterator->path () << ".";
            try
            {
                DataClass *dataClass = new DataClass (iterator->path ());
                if (dataClasses_.count (dataClass->GetName ()) == 0)
                {
                    dataClasses_.insert (std::make_pair (dataClass->GetName (), dataClass));
                    BOOST_LOG_TRIVIAL (info) << "Loaded class " << dataClass->GetName () << " from file " <<
                                             iterator->path () << ".";
                }
                else
                {
                    BOOST_LOG_TRIVIAL (error) << "Unable to load class from " << iterator->path ()
                                              << " because class with name " <<
                                              dataClass->GetName () << " already loaded.";
                }
            }
            catch (boost::exception &exception)
            {
                BOOST_LOG_TRIVIAL (error) << "Unable to load because of exception: "
                                          << boost::diagnostic_information (exception);
            }
        }

        ++iterator;
    }

    return true;
}
}
}
