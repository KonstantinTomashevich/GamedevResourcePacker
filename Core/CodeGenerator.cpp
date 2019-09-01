#include "CodeGenerator.hpp"
#include <boost/log/trivial.hpp>

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

    boost::filesystem::path loaderUmbrellaPath = outputFolder / "Loaders.hpp";
    BOOST_LOG_TRIVIAL (info) << "Generating  " << loaderUmbrellaPath << "...";
    std::ofstream loaderUmbrella (loaderUmbrellaPath.string ());
    loaderUmbrella << "#pragma once" << std::endl <<
                   "#include <boost/filesystem.hpp" << std::endl << std::endl <<
                   "namespace ResourceSubsystem" << std::endl << "{" << std::endl <<
                   "using Loader = std::function <Object * (int id, const boost::filesystem::path &)>;" << std::endl <<
                   "template <typename T> Loader GetLoader ()" << std::endl << "{" << std::endl <<
                   "    return nullptr;" << std::endl << "}" << std::endl << "}" << std::endl << std::endl;

    for (auto &plugin : pluginManager_->GetPluginsVector ())
    {
        plugin->GenerateCode (outputFolder);
        loaderUmbrella << "#include \"" << plugin->GetName () << "Loaders.hpp\"" << std::endl;
    }

    loaderUmbrella.close ();
    BOOST_LOG_TRIVIAL (info) << "Done " << loaderUmbrellaPath << " generation.";
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
            BOOST_LOG_TRIVIAL (info) << "Generating " << output << "...";

            boost::filesystem::copy_file (iterator->path (), output,
                                          boost::filesystem::copy_option::overwrite_if_exists);
            BOOST_LOG_TRIVIAL (info) << "Done " << output << " generation.";
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
}
