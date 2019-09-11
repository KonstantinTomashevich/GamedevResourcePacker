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
        name_ = tree.get_child ("class.<xmlattr>.name").data ();
        for (PTree::value_type &node : tree.get_child ("class"))
        {
            if (node.first == "field")
            {
                Field field;
                PTree attr = node.second.get_child ("<xmlattr>");
                field.name = attr.get_child ("name").data ();
                field.typeName = attr.get_child ("typeName").data ();

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

bool DataClass::IsSimpleFieldType (const DataClass::Field &field) const
{
    return field.typeName == "int" || field.typeName == "float";
}

bool DataClass::IsSimpleField (const DataClass::Field &field) const
{
    return !field.array && IsSimpleFieldType (field);
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
           "#include <cstdio>" << std::endl <<
           "#include <vector>" << std::endl <<
           "#include <string>" << std::endl <<
           "#include <Object.hpp>" << std::endl <<
           "#include <Defines.hpp>" << std::endl << std::endl;

    GenerateHeaderIncludes (header);
    header << "namespace ResourceSubsystem" << std::endl <<
           "{" << std::endl;

    GenerateHeaderForwardDeclarations (header);
    header << "namespace DataObjects" << std::endl <<
           "{" << std::endl <<
           "class " << name_ << " : public Object" << std::endl <<
           "{" << std::endl <<
           "public:" << std::endl <<
           "    /// Stub constructor for std::vector, do not use!" << std::endl <<
           "    " << name_ << "() : Object (PRIVATE_OBJECT_ID) {}" << std::endl <<
           "    " << name_ << " (int id, FILE *stream);" << std::endl <<
           "    virtual ~" << name_ << " ();" << std::endl << std::endl;

    GenerateHeaderAccessors (header);
    GenerateHeaderFields (header);
    header << "};" << std::endl << "}" << std::endl << "}" << std::endl;

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
            header << "    " << GenerateReferenceGetterCxxType (field) << " Get" << capitalizedName << " ();"
                   << std::endl;

            header << "    void Set" << capitalizedName << " (" << GenerateSetterCxxType (field) << " " <<
                   field.name << ");" << std::endl;
        }

        header << std::endl;
    }
}

void DataClass::GenerateHeaderIncludes (std::ofstream &header) const
{
    std::unordered_set <std::string> alreadyIncluded = {"string"};
    header << "#include \"Loader.hpp\"" << std::endl;

    for (auto &field : fields_)
    {
        if (!field.reference && !IsSimpleFieldType (field) && alreadyIncluded.count (field.typeName) == 0)
        {
            header << "#include object_class_header_" << field.typeName << std::endl;
            alreadyIncluded.insert (field.typeName);
        }
    }

    header << std::endl;
}

void DataClass::GenerateHeaderForwardDeclarations (std::ofstream &header) const
{
    std::unordered_set <std::string> alreadyDeclared;
    for (auto &field : fields_)
    {
        if (field.reference && alreadyDeclared.count (field.typeName) == 0)
        {
            header << "object_class_forward_" << field.typeName << std::endl;
            alreadyDeclared.insert (field.typeName);
        }
    }

    header << std::endl;
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

    object << "#include \"" << name_ << ".hpp\"" << std::endl <<
           "#include <Core.hpp>" << std::endl <<
           "#include <utility>" << std::endl << std::endl;

    GenerateObjectIncludes (object);
    object << "namespace ResourceSubsystem" << std::endl <<
           "{" << std::endl <<
           "namespace DataObjects" << std::endl <<
           "{" << std::endl << std::endl;

    GenerateObjectConstructor (object);
    GenerateObjectDestructor (object);
    GenerateObjectAccessors (object);

    object << "}" << std::endl << "}" << std::endl;
    object.close ();
    BOOST_LOG_TRIVIAL (info) << "Done " << objectPath << " generation.";
}

void DataClass::GenerateObjectIncludes (std::ofstream &object) const
{
    std::unordered_set <std::string> alreadyIncluded;
    for (auto &field : fields_)
    {
        if (field.reference && alreadyIncluded.count (field.typeName) == 0)
        {
            object << "#include object_class_header_" << field.typeName << std::endl;
            alreadyIncluded.insert (field.typeName);
        }
    }

    object << std::endl;
}

void DataClass::GenerateObjectAccessors (std::ofstream &object) const
{
    for (auto &field : fields_)
    {
        std::string capitalizedName = GenerateCapitalizedFieldName (field);
        object << GenerateConstGetterCxxType (field) << " " << name_ << "::Get" << capitalizedName <<
               " () const" << std::endl << "{" << std::endl << "    return " << field.name << "_;" <<
               std::endl << "}" << std::endl << std::endl;

        if (!field.readonly)
        {
            object << GenerateReferenceGetterCxxType (field) << " " << name_ << "::Get" <<
                   capitalizedName << " ()" << std::endl << "{" << std::endl << "    return " <<
                   field.name << "_;" << std::endl << "}" << std::endl << std::endl;

            object << "void " << name_ << "::Set" << capitalizedName << " (" << GenerateSetterCxxType (field) << " " <<
                   field.name << ")" << std::endl << "{" << std::endl << "    " << field.name << "_ = " <<
                   field.name << ";" << std::endl << "}" << std::endl << std::endl;
        }
    }
}

void DataClass::GenerateObjectConstructor (std::ofstream &object) const
{
    object << name_ << "::" << name_ << " (int id, FILE *stream)" << std::endl <<
           "    : Object (id)";

    for (auto &field : fields_)
    {
        if (field.array)
        {
            object << "," << std::endl << "      " << field.name << "_ ()";
        }
    }

    object << std::endl << "{" << std::endl;

    for (auto &field : fields_)
    {
        if (field.array)
        {
            InsertArrayReader (object, field);
        }
        else
        {
            InsertValueReader (object, field, "    ");
        }
    }

    object << "}" << std::endl << std::endl;
}

void DataClass::GenerateObjectDestructor (std::ofstream &object) const
{
    object << name_ << "::~" << name_ << " ()" << std::endl << "{" << std::endl;
    for (auto &field : fields_)
    {
        if (!field.reference)
        {
            continue;
        }

        if (field.array)
        {
            object << std::endl << "for (auto &object : " << field.name << "_)" << std::endl <<
                   "{" << std::endl << "    object->Unref ();" << std::endl << "}" << std::endl << std::endl;
        }
        else
        {
            object << "    " << field.name << "_->Unref ();" << std::endl;
        }
    }

    object << "}" << std::endl << std::endl;
}

void DataClass::InsertArrayReader (std::ofstream &object, const DataClass::Field &field) const
{
    object << "    int " << field.name << "Count;" << std::endl <<
           "    fread (&" << field.name << "Count, sizeof (" << field.name << "Count), 1, stream);" << std::endl <<
           "    " << field.name << "_.resize (" << field.name << "Count);" << std::endl << std::endl <<
           "    for (int index = 0; index < " << field.name << "Count; ++index)" << std::endl <<
           "    {" << std::endl;

    InsertValueReader (object, field, "        ");
    object << "    }" << std::endl << std::endl;
}

void DataClass::InsertValueReader (std::ofstream &object,
                                   const DataClass::Field &field,
                                   const std::string &indentation) const
{
    std::string outputName = (field.array ? field.name + "_[index]" : field.name + "_");
    if (field.typeName == "string")
    {
        object << indentation << "int " << field.name << "Size;" << std::endl <<
               indentation << "fread (&" << field.name << "Size, sizeof (" << field.name << "Size), 1, stream);" <<
               std::endl << indentation << outputName << ".resize (" << field.name << "Size);" << std::endl <<
               indentation << "fread (&" << outputName << "[0], sizeof (" << field.name << "Size), 1, stream);" <<
               std::endl;
    }
    else if (IsSimpleField (field))
    {
        object << indentation << "fread (&" << outputName << ", sizeof (" <<
               outputName << "), 1, stream);" << std::endl;
    }
    else if (field.reference)
    {
        object << indentation << "unsigned int " << field.name << "GroupId;" << std::endl <<
               indentation << "unsigned int " << field.name << "ObjectId;" << std::endl << std::endl <<
               indentation << "fread (&" << field.name << "GroupId, sizeof (" << field.name << "GroupId), 1, stream);"
               << std::endl << indentation << "fread (&" << field.name << "ObjectId, sizeof (" << field.name <<
               "ObjectId), 1, stream);" << std::endl <<
               indentation << outputName << " = (" << field.typeName << 
               " *) GetResource (object_class_loader_" << field.typeName << ", " <<
               field.name << "GroupId, " << field.name << "ObjectId);" << std::endl;
    }
    else
    {
        object << indentation << outputName << " = " << field.typeName <<
               "(Object::PRIVATE_OBJECT_ID, stream);" << std::endl;
    }

    if (!field.array)
    {
        object << std::endl;
    }
}
}
}
