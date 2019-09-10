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

const char *PluginAPI::GetName () const
{
    return "DataObjects";
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
                if (dataClassProvider_.AddDataClass (dataClass))
                {
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
            DataObject *object = new DataObject (name->data (), rootObjectName, rootObject, this, &dataClassProvider_);
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

void PluginAPI::GenerateCode (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path classesFolder = outputFolder / GetName ();
    boost::filesystem::create_directories (classesFolder);

    for (const auto &nameClassPair : dataClassProvider_.GetDataClasses ())
    {
        nameClassPair.second->GenerateCode (classesFolder);
    }

    GenerateLoadersCode (outputFolder);
}

std::vector <std::string> PluginAPI::GenerateDefines () const
{
    std::vector <std::string> result;
    for (const auto &nameClassPair : dataClassProvider_.GetDataClasses ())
    {
        result.emplace_back ("object_class_header_" + nameClassPair.first + " <" + GetName () +
            "/" + nameClassPair.first + ".hpp>");
    }

    return result;
}

DataClass *PluginAPI::GetClassByName (const std::string &name) const
{
    return dataClassProvider_.GetDataClass (name);
}

void PluginAPI::GenerateLoadersCode (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path loadersPath = outputFolder / (GetName () + std::string ("Loaders.hpp"));
    BOOST_LOG_TRIVIAL (info) << "Generating  " << loadersPath << "...";
    std::ofstream loaders (loadersPath.string ());

    loaders << "#pragma once" << std::endl <<
            "#include <cstdio>" << std::endl <<
            "#include <boost/filesystem.hpp>" << std::endl;

    for (auto &nameDataClass : dataClassProvider_.GetDataClasses ())
    {
        loaders << "#include \"" << GetName () << "/" << nameDataClass.first << ".hpp\"" << std::endl;
    }

    loaders << std::endl << "namespace ResourceSubsystem" << std::endl << "{" << std::endl <<
            "namespace DataObjects" << std::endl << "{" << std::endl <<
            "template <typename T> Object *DataObjectLoader ("
            "int id, const boost::filesystem::path &path)" << std::endl <<
            "{" << std::endl <<
            "    FILE *input = fopen (path.string ().c_str (), \"rb\");" << std::endl <<
            "    T *object = new T (id, input);" << std::endl <<
            "    fclose (input);" << std::endl <<
            "    return object;" << std::endl <<
            "}" << std::endl << std::endl;

    for (auto &nameDataClass : dataClassProvider_.GetDataClasses ())
    {
        loaders << "template <> Loader GetLoader <" << nameDataClass.first << "> ()" << std::endl <<
                "{" << std::endl <<
                "    return DataObjectLoader <" << nameDataClass.first << ">;" << std::endl <<
                "}" << std::endl << std::endl;
    }

    loaders << "}" << std::endl << "}" << std::endl;
    loaders.close ();
    BOOST_LOG_TRIVIAL (info) << "Done " << loadersPath << " generation.";
}
}
}
