#include "DataClass.hpp"
#include <cstdlib>
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

void DataClass::GenerateCode (const boost::filesystem::path &outputFolder) const
{
    GenerateHeader (outputFolder);
    GenerateObject (outputFolder);
}

void DataClass::LoadFromTree (boost::property_tree::ptree &tree)
{
    try
    {
        name_ = tree.get_child ("class.<xmlattr>.name").data ().c_str ();
        for (PTree::value_type &node : tree.get_child ("class"))
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

std::string DataClass::GenerateCxxFieldType (const DataClass::Field &field) const
{
    std::string result;
    if (field.array)
    {
        result += "std::vector <";
    }

    if (field.typeName == "string")
    {
        result += "std::string";
    }
    else
    {
        result += field.typeName;
    }

    if (field.reference)
    {
        result += " *";
    }

    if (field.array)
    {
        result += ">";
    }

    return result;
}

bool DataClass::IsSimpleField (const DataClass::Field &field) const
{
    return !field.array && (field.typeName == "int" || field.typeName == "float");
}

bool DataClass::IsRawPointerField (const DataClass::Field &field) const
{
    return !field.array && field.reference;
}

std::string DataClass::GenerateCapitalizedFieldName (const DataClass::Field &field) const
{
    std::string capitalizedName = field.name;
    if (islower (capitalizedName[0]))
    {
        capitalizedName[0] = toupper (capitalizedName[0]);
    }

    return capitalizedName;
}

std::string DataClass::GenerateConstGetterCxxType (const DataClass::Field &field) const
{
    std::string type = GenerateCxxFieldType (field);
    if (!IsSimpleField (field))
    {
        type = "const " + type;
    }

    if (!IsSimpleField (field) && !IsRawPointerField (field))
    {
        type += " &";
    }

    return type;
}

std::string DataClass::GenerateReferenceGetterCxxType (const DataClass::Field &field) const
{
    return GenerateCxxFieldType (field) + " &";
}

std::string DataClass::GenerateSetterCxxType (const DataClass::Field &field) const
{
    std::string type = GenerateCxxFieldType (field);
    if (!IsSimpleField (field) && !IsRawPointerField (field))
    {
        type = "const " + type + "&";
    }

    return type;
}

void DataClass::GenerateHeader (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path headerPath = outputFolder / (name_ + ".hpp");
    BOOST_LOG_TRIVIAL (info) << "Generating  " << headerPath << "...";
    std::ofstream header (headerPath.string ());

    header << "#pragma once" << std::endl <<
           "#include <csstdio>" << std::endl <<
           "#include <vector>" << std::endl <<
           "#include <string>" << std::endl <<
           "#include <Object.hpp>" << std::endl << std::endl <<
           "namespace ResourceSubsystem" << std::endl <<
           "{" << std::endl <<
           "class " << name_ << " : public Object" << std::endl <<
           "{" << std::endl <<
           "public:" << std::endl <<
           "    " << name_ << " (int id, FILE *stream);" << std::endl <<
           "    virtual ~" << name_ << " ();" << std::endl << std::endl;

    GenerateHeaderAccessors (header);
    GenerateHeaderFields (header);
    header << "};" << std::endl << "}" << std::endl;

    header.close ();
    BOOST_LOG_TRIVIAL (info) << "Done " << headerPath << " generation.";
}

void DataClass::GenerateHeaderAccessors (std::ofstream &header) const
{
    header << "public:" << std::endl;
    for (auto &field : fields_)
    {
        std::string capitalizedName = GenerateCapitalizedFieldName (field);
        header << "    " << GenerateConstGetterCxxType (field) << " Get" << capitalizedName << " () const;"
               << std::endl;

        if (!field.readonly)
        {
            header << "    " << GenerateReferenceGetterCxxType (field) << " Get" << capitalizedName << " ()"
                   << std::endl;

            header << "    void Set" << capitalizedName << " (" << GenerateSetterCxxType (field) << " " <<
                   capitalizedName << ");" << std::endl;
        }

        header << std::endl;
    }
}

void DataClass::GenerateHeaderFields (std::ofstream &header) const
{
    header << "private:" << std::endl;
    for (auto &field : fields_)
    {
        header << "    " << GenerateCxxFieldType (field) << " " << field.name << "_;" << std::endl;
    }
}

void DataClass::GenerateObject (const boost::filesystem::path &outputFolder) const
{
    boost::filesystem::path objectPath = outputFolder / (name_ + ".cpp");
    BOOST_LOG_TRIVIAL (info) << "Generating  " << objectPath << "...";
    std::ofstream object (objectPath.string ());

    object.close ();
    BOOST_LOG_TRIVIAL (info) << "Done " << objectPath << " generation.";
}
}
}
