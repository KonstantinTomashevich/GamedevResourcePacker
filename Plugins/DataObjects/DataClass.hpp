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
class DataClass
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

    DataClass (const boost::filesystem::path &xmlPath);
    virtual ~DataClass ();

    const std::string &GetName () const;
    const FieldVector &GetFields () const;

    class XMLParseException : public boost::exception, public std::exception
    {
    public:
        explicit XMLParseException (const std::string &info);
        std::string info_;

        virtual const char *what () const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;
    };

private:
    void LoadFromTree (boost::property_tree::ptree &tree);

    std::string name_;
    FieldVector fields_;
};
}
}
