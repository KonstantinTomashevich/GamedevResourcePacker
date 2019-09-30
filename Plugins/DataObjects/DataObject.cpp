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

bool DataObject::NeedsExecution (const boost::filesystem::path &outputFile) const
{
    return FileDependentBinaryGenerationTask::NeedsExecution (outputFile);
}

bool DataObject::Execute (FILE *output) const
{
    return rootField_->Write (output);
}
}
}
