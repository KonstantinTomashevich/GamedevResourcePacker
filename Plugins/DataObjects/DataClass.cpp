#include "DataClass.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/log/trivial.hpp>
#include <Core/Exception.hpp>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
using PTree = boost::property_tree::ptree;

DataClass::DataClass (const boost::filesystem::path &xmlPath)
    : name_ (), fields_ ()
{
    PTree tree;
    boost::property_tree::read_xml (xmlPath.string (), tree);
    LoadFromTree (tree);
}

DataClass::~DataClass ()
{

}

const std::string &DataClass::GetName () const
{
    return name_;
}

const DataClass::FieldVector &DataClass::GetFields () const
{
    return fields_;
}

void DataClass::LoadFromTree (boost::property_tree::ptree &tree)
{
    try
    {
        name_ = tree.get_child ("class.<xmlattr>.name").data ().c_str ();
        for (PTree::value_type node : tree.get_child ("class"))
        {
            if (node.first == "field")
            {
                Field field;
                PTree attr = node.second.get_child ("<xmlattr>");
                field.name = attr.get_child ("name").data ().c_str ();
                field.typeName = attr.get_child ("typeName").data ().c_str ();

                field.array = attr.get_child ("array").data () == "true";
                field.reference = attr.get_child ("reference").data () == "true";
                field.readonly = attr.get_child ("readonly").data () == "true";
                fields_.push_back (field);
            }
        }
    }
    catch (boost::exception &exception)
    {
        BOOST_LOG_TRIVIAL (fatal) << "Exception caught: " << boost::diagnostic_information (exception);
        BOOST_THROW_EXCEPTION (Exception <XMLParseException> ("Unable to parse xml because of boost error!"));
    }
}
}
}
