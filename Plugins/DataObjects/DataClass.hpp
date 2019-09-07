#pragma once
#include <string>
#include <boost/container/small_vector.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>
#include <boost/property_tree/ptree.hpp>

#define PREALLOCATED_FIELDS_COUNT 15

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
class BOOST_SYMBOL_EXPORT DataClass
{
public:
    using Field = struct
    {
        std::string name;
        std::string typeName;
        bool array;
        bool reference;
        bool readonly;
    };

    using FieldVector = boost::container::small_vector <Field, PREALLOCATED_FIELDS_COUNT>;

    explicit DataClass (const boost::filesystem::path &xmlPath);
    virtual ~DataClass ();

    const std::string &GetName () const;
    const FieldVector &GetFields () const;
    void GenerateCode (const boost::filesystem::path &outputFolder) const;

    class XMLParseException;

private:
    void LoadFromTree (boost::property_tree::ptree &tree);
    std::string GenerateCxxFieldType (const Field &field) const;
    bool IsSimpleField (const Field &field) const;
    bool IsRawPointerField (const Field &field) const;

    std::string GenerateCapitalizedFieldName (const Field &field) const;
    std::string GenerateConstGetterCxxType (const Field &field) const;
    std::string GenerateReferenceGetterCxxType (const Field &field) const;
    std::string GenerateSetterCxxType (const Field &field) const;

    void GenerateHeader (const boost::filesystem::path &outputFolder) const;
    void GenerateHeaderAccessors (std::ofstream &header) const;
    void GenerateHeaderFields (std::ofstream &header) const;

    void GenerateObject (const boost::filesystem::path &outputFolder) const;
    void GenerateObjectAccessors (std::ofstream &object) const;
    void GenerateObjectConstructor (std::ofstream &object) const;
    void GenerateObjectDestructor (std::ofstream &object) const;

    void InsertArrayReader (std::ofstream &object, const Field &field) const;
    void InsertValueReader (std::ofstream &object, const Field &field, const std::string &indentation) const;

    std::string name_;
    FieldVector fields_;
};
}
}
