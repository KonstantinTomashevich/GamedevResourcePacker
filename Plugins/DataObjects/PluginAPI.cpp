#include "PluginAPI.hpp"
#include "DataObject.hpp"
#include <boost/log/trivial.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

const char *PluginAPI::GetName () const
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

Object *PluginAPI::Capture (const boost::filesystem::path &asset)
{
    if (asset.extension () == ".xml")
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml (asset.string (), tree);

        auto xmlRoot = tree.get_child_optional ("data-object");
        if (!xmlRoot.is_initialized ())
        {
            return nullptr;
        }

        if (xmlRoot->size () > 2)
        {
            BOOST_LOG_TRIVIAL (warning)
                << "Data Objects: given object has more than one root child. "
                   "Only first will be parsed, others will be ignored.";
        }

        auto name = xmlRoot->get_child_optional ("<xmlattr>.name");
        if (!name.is_initialized ())
        {
            BOOST_LOG_TRIVIAL (error) << "Data Objects: given object has no name, so it will be ignored.";
            return nullptr;
        }

        boost::property_tree::ptree rootObject;
        std::string rootObjectName;

        for (auto child : *xmlRoot)
        {
            if (child.first != "<xmlattr>")
            {
                rootObject = child.second;
                rootObjectName = child.first;
                break;
            }
        }

        if (rootObject.empty ())
        {
            BOOST_LOG_TRIVIAL (error) << "Data Objects: given object has empty object, so it will be ignored.";
            return nullptr;
        }

        try
        {
            rootObject.get_child_optional ("<xmlattr>");
            DataObject *object = new DataObject (name->data (), rootObjectName, rootObject, this);
            return object;
        }
        catch (boost::exception &exception)
        {
            BOOST_LOG_TRIVIAL (error) << "Exception caught: " << boost::diagnostic_information (exception);
            BOOST_LOG_TRIVIAL (error) << "Data Objects: unable to parse given data object because of exception.";
            return nullptr;
        }
    }

    return nullptr;
}

DataClass *PluginAPI::GetClassByName (const std::string &name) const
{
    try
    {
        return dataClasses_.at (name);
    }
    catch (std::out_of_range &exception)
    {
        return nullptr;
    }
}
}
}
