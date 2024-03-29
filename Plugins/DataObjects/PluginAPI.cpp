#include "PluginAPI.hpp"
#include "DataObject.hpp"
#include <Shared/MultithreadedLog.hpp>

#include <boost/log/trivial.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
using Path = boost::filesystem::path;
using RecursiveDirIterator = boost::filesystem::recursive_directory_iterator;

const char *PluginAPI::GetName () const
{
    return "DataObjects";
}

bool PluginAPI::Load (const Path &configFolder)
{
    Path classesFolder = configFolder / "DataObjectClasses";
    RecursiveDirIterator iterator (classesFolder);
    RecursiveDirIterator end;

    // TODO: Is it worth multithreading?
    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            MT_LOG (info, "Trying to load data class from " << iterator->path () << ".");
            try
            {
                DataClass *dataClass = new DataClass (iterator->path ());
                if (dataClassProvider_.AddDataClass (dataClass))
                {
                    MT_LOG (info, "Loaded class " << dataClass->GetName () << " from file " <<
                                             iterator->path () << ".");
                }
                else
                {
                    MT_LOG (error, "Unable to load class from " << iterator->path ()
                                              << " because class with name " <<
                                              dataClass->GetName () << " already loaded.");
                }
            }
            catch (boost::exception &exception)
            {
                MT_LOG (error, "Unable to load because of exception: "
                                          << boost::diagnostic_information (exception));
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
            MT_LOG (warning, "Data Objects: given object has more than one root child. "
                   "Only first will be parsed, others will be ignored.");
        }

        auto name = xmlRoot->get_child_optional ("<xmlattr>.name");
        if (!name.is_initialized ())
        {
            MT_LOG (error, "Data Objects: given object has no name, so it will be ignored.");
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
            MT_LOG (error, "Data Objects: given object has empty object, so it will be ignored.");
            return nullptr;
        }

        try
        {
            DataObject *object = new DataObject (name->data (), asset,
                                                 rootObjectName, rootObject, this, &dataClassProvider_);
            return object;
        }
        catch (boost::exception &exception)
        {
            MT_LOG (error, "Exception caught: " << boost::diagnostic_information (exception));
            MT_LOG (error, "Data Objects: unable to parse given data object because of exception.");
            return nullptr;
        }
    }

    return nullptr;
}

void PluginAPI::GenerateCode (const boost::filesystem::path &outputFolder,
                              std::vector <GenerationTask *> &outputTasks) const
{
    boost::filesystem::path classesFolder = outputFolder / GetName ();
    boost::filesystem::create_directories (classesFolder);

    for (const auto &nameClassPair : dataClassProvider_.GetDataClasses ())
    {
        outputTasks.push_back (nameClassPair.second);
    }

    boost::filesystem::path loadersPath = outputFolder / GetName () / std::string ("Loader.hpp");
    if (!boost::filesystem::exists (loadersPath))
    {
        GenerateLoadersCode (loadersPath);
    }
}

std::vector <std::string> PluginAPI::GenerateDefines () const
{
    std::vector <std::string> result;
    for (const auto &nameClassPair : dataClassProvider_.GetDataClasses ())
    {
        result.emplace_back ("object_class_header_" + nameClassPair.first + " <" + GetName () +
            "/" + nameClassPair.first + ".hpp>");

        result.emplace_back ("object_class_forward_" + nameClassPair.first + " namespace " + GetName () +
            " { class " + nameClassPair.first + "; }");

        result.emplace_back ("object_class_loader_" + nameClassPair.first + " " + GetName () +
            "::DataObjectLoader <" + GetName () + "::" + nameClassPair.first + ">");
    }

    return result;
}

void PluginAPI::GenerateLoadersCode (const boost::filesystem::path &output) const
{
    MT_LOG (info, "Generating  " << output << "...");
    std::ofstream loaders (output.string ());

    loaders << "#pragma once" << std::endl <<
            "#include <cstdio>" << std::endl <<
            "#include <boost/filesystem.hpp>" << std::endl;

    loaders << std::endl << "namespace ResourceSubsystem" << std::endl << "{" << std::endl <<
            "namespace DataObjects" << std::endl << "{" << std::endl <<
            "template <typename T> Object *DataObjectLoader ("
            "int32_t id, const std::string &path, uint32_t offset)" << std::endl <<
            "{" << std::endl <<
            "    FILE *input = fopen (path.c_str (), \"rb\");" << std::endl <<
            "    fseek (input, offset, SEEK_SET);" << std::endl <<
            "    T *object = new T (id, input);" << std::endl <<
            "    fclose (input);" << std::endl <<
            "    return object;" << std::endl <<
            "}" << std::endl << "}" << std::endl << "}" << std::endl;

    loaders.close ();
    MT_LOG (info, "Done " << output << " generation.");
}
}
}
