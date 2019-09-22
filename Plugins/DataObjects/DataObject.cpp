#include "DataObject.hpp"
#include "PluginAPI.hpp"
#include "DataObjectValueField.hpp"

#include <boost/log/trivial.hpp>
#include <Shared/StringHash.hpp>
#include <Shared/MultithreadedLog.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
DataObject::DataObject (const std::string &name, const boost::filesystem::path &sourceAsset,
                        const std::string &rootName, DataObject::PTree &root,
                        PluginAPI *owner, DataClassProvider *provider)
    : Object (owner, name, rootName), rootField_ (new DataObjectValueField (provider, rootName, root))
{
    generationTargets_.push_back (std::to_string (StringHash (GetUniqueName ())) + "." +
        std::to_string (StringHash (GetResourceClassName ())));

    generationDependencies_.push_back (sourceAsset);
    rootField_->IterateOuterReferences ([this] (ObjectReference *reference)
                                        { AddOuterReference (reference, false); });
}

DataObject::~DataObject ()
{
    delete rootField_;
}

void DataObject::Print (std::ostream &output) const
{
    output << "DataObject \"" << GetUniqueName () << "\" of type \"" << GetResourceClassName () << "\"" << std::endl;
    rootField_->Print (output, 4);
}

bool DataObject::NeedsExecution (const boost::filesystem::path &outputFolder) const
{
    return FileDependentGenerationTask::NeedsExecution (outputFolder);
}

bool DataObject::Execute (const boost::filesystem::path &outputFolder) const
{
    // TODO: Refactor generation targets system. It'll be easier to expand if there is only one target.
    boost::filesystem::path target = outputFolder / generationTargets_[0];
    MT_LOG (info, "Generation " << target << "...");
    FILE *output = fopen (target.string ().c_str (), "wb");

    if (output == nullptr)
    {
        MT_LOG (fatal, "Unable to open " << target << " for object \"" <<
                                         GetUniqueName () << "\" of type \"" <<
                                         GetResourceClassName () << "\" binary output.");
        return false;
    }

    bool result = rootField_->Write (output);
    fclose (output);
    MT_LOG (info, "Done " << target << " generation.");
    return result;
}
}
}
