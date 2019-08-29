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

    std::string name_;
    FieldVector fields_;
};
}
}
