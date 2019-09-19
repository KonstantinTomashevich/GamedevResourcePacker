#include "CodeGenerator.hpp"
#include <boost/log/trivial.hpp>

#include <Shared/StringHash.hpp>
#include <Shared/FileUtils.hpp>
#include <Shared/MultithreadedLog.hpp>

namespace GamedevResourcePacker
{
CodeGenerator::CodeGenerator (ObjectManager *objectManager, PluginManager *pluginManager)
    : objectManager_ (objectManager), pluginManager_ (pluginManager)
{

}

void CodeGenerator::Generate (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::create_directories (outputFolder);
    CopyBundleIndependentCode (outputFolder);

    std::vector <GenerationTask *> tasks;

    for (auto &plugin : pluginManager_->GetPluginsVector ())
    {
        plugin->GenerateCode (outputFolder, tasks);
    }

    bool anyCodeChanged = false;
    for (auto task : tasks)
    {
        if (task->NeedsExecution (outputFolder))
        {
            anyCodeChanged = true;
            task->Execute (outputFolder);
        }
    }

    boost::filesystem::path idsHeaderPath = outputFolder / "Ids.hpp";
    boost::filesystem::path definesHeaderPath = outputFolder / "Defines.hpp";

    if (objectManager_->IsContentListOverwritten () || !boost::filesystem::exists (idsHeaderPath))
    {
        GenerateIdsHeader (idsHeaderPath);
    }

    if (anyCodeChanged || !boost::filesystem::exists (definesHeaderPath))
    {
        GenerateDefinesHeader (definesHeaderPath);
    }
}

void CodeGenerator::CopyBundleIndependentCode (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path bundleIndependentCodeRoot ("BundleIndependentCode");
    boost::filesystem::recursive_directory_iterator iterator (bundleIndependentCodeRoot);
    boost::filesystem::recursive_directory_iterator end;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            boost::filesystem::path output =
                outputFolder / iterator->path ().lexically_relative (bundleIndependentCodeRoot);

            if (IsFileNeedsUpdate (output, {iterator->path ()}))
            {
                MT_LOG (info, "Generating " << output << "...");
                boost::filesystem::copy_file (iterator->path (), output,
                                              boost::filesystem::copy_option::overwrite_if_exists);
                MT_LOG (info, "Done " << output << " generation.");
            }
        }
        else if (iterator->status ().type () == boost::filesystem::directory_file)
        {
            boost::filesystem::path output =
                outputFolder / iterator->path ().lexically_relative (bundleIndependentCodeRoot);
            boost::filesystem::create_directories (output);
        }

        ++iterator;
    }
}

void CodeGenerator::GenerateIdsHeader (const boost::filesystem::path &idsHeaderPath) const
{
    MT_LOG (info, "Generating  " << idsHeaderPath << "...");
    std::ofstream idsHeader (idsHeaderPath.string ());

    idsHeader << "#pragma once" << std::endl <<
              "namespace ResourceSubsystem" << std::endl <<
              "{" << std::endl <<
              "namespace Ids" << std::endl <<
              "{" << std::endl << std::endl;

    for (auto &classNameObjectsPair : objectManager_->GetResourceClassMap ())
    {
        idsHeader << "unsigned int " << classNameObjectsPair.first << "GroupId = " <<
                  StringHash (classNameObjectsPair.first) << ";" << std::endl <<
                  "namespace " << classNameObjectsPair.first << std::endl <<
                  "{" << std::endl;

        for (auto &objectNameInstancePair : classNameObjectsPair.second)
        {
            idsHeader << "unsigned int " << objectNameInstancePair.first << " = " <<
                      StringHash (objectNameInstancePair.first) << ";" << std::endl;
        }

        idsHeader << "}" << std::endl << std::endl;
    }


    idsHeader << "}" << std::endl << "}" << std::endl;
    MT_LOG (info, "Done " << idsHeaderPath << " generation.");
}

void CodeGenerator::GenerateDefinesHeader (const boost::filesystem::path &definesHeaderPath) const
{
    MT_LOG (info, "Generating  " << definesHeaderPath << "...");
    std::ofstream definesHeader (definesHeaderPath.string ());
    definesHeader << "#pragma once" << std::endl << std::endl;

    for (auto &plugin : pluginManager_->GetPluginsVector ())
    {
        std::vector <std::string> pluginDefines = plugin->GenerateDefines ();
        definesHeader << "// " << plugin->GetName () << std::endl;

        for (auto &define : pluginDefines)
        {
            definesHeader << "#define " << define << std::endl;
        }

        definesHeader << std::endl;
    }

    MT_LOG (info, "Done " << definesHeaderPath << " generation.");
}
}
