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
#pragma omp parallel for
    for (int32_t index = 0; index < tasks.size (); ++index)
    {
        auto &task = tasks[index];
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

    using FileOutputPair = std::pair <boost::filesystem::path, boost::filesystem::path>;
    std::vector <FileOutputPair> filesToCopy;

    while (iterator != end)
    {
        if (iterator->status ().type () == boost::filesystem::regular_file)
        {
            boost::filesystem::path output =
                outputFolder / iterator->path ().lexically_relative (bundleIndependentCodeRoot);

            if (IsFileNeedsUpdate (output, {iterator->path ()}))
            {
                filesToCopy.emplace_back (std::make_pair (iterator->path (), output));
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

#pragma omp parallel for
    for (int32_t index = 0; index < filesToCopy.size (); ++index)
    {
        boost::filesystem::path &file = filesToCopy[index].first;
        boost::filesystem::path &output = filesToCopy[index].second;

        MT_LOG (info, "Generating " << output << "...");
        boost::filesystem::copy_file (file, output,
                                      boost::filesystem::copy_option::overwrite_if_exists);
        MT_LOG (info, "Done " << output << " generation.");
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
        idsHeader << "uint32_t " << classNameObjectsPair.first << "GroupId = " <<
                  StringHash (classNameObjectsPair.first) << ";" << std::endl <<
                  "namespace " << classNameObjectsPair.first << std::endl <<
                  "{" << std::endl;

        for (auto &objectNameInstancePair : classNameObjectsPair.second)
        {
            idsHeader << "uint32_t " << objectNameInstancePair.first << " = " <<
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
