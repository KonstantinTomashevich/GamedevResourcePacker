#pragma once
#include "DataObjectField.hpp"
#include <boost/lexical_cast.hpp>
#include <string>

namespace GamedevResourcePacker
{
namespace DataObjectsPlugin
{
template <typename T> class DataObjectSimpleField : public DataObjectField
{
public:
    explicit DataObjectSimpleField (PTree &source)
    {
        value_ = boost::lexical_cast <T> (source.data ());
    }

    virtual ~DataObjectSimpleField () = default;

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback)
    {

    }

    virtual bool Write (FILE *output)
    {
        fwrite (&value_, sizeof (value_), 1, output);
        return true;
    }

    virtual void Print (std::ostream &output, int32_t indentation = 0) const
    {
        Indent (output, indentation) << "(simple field) " << value_ << std::endl;
    }

private:
    T value_;
};

template <> class DataObjectSimpleField <std::string> : public DataObjectField
{
public:
    explicit DataObjectSimpleField (PTree &source)
        : value_ (source.data ())
    {

    }

    virtual ~DataObjectSimpleField () = default;

    virtual void IterateOuterReferences (const ReferenceIterationCallback &callback)
    {

    }

    virtual bool Write (FILE *output)
    {
        uint32_t size = value_.size ();
        fwrite (&size, sizeof (size), 1, output);
        fwrite (value_.c_str (), sizeof (char), size, output);
        return true;
    }

    virtual void Print (std::ostream &output, int32_t indentation = 0) const
    {
        Indent (output, indentation) << "(simple field) \"" << value_ << "\"" << std::endl;
    }

private:
    std::string value_;
};

using DataObjectIntField = DataObjectSimpleField <int32_t>;
using DataObjectFloatField = DataObjectSimpleField <float>;
using DataObjectStringField = DataObjectSimpleField <std::string>;
}
}
