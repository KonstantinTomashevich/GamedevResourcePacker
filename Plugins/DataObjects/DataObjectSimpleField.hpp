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
    explicit DataObjectSimpleField (PTree *source)
    {
        value_ = boost::lexical_cast <T> (source->data ());
    }

    virtual ~DataObjectSimpleField () = default;

    virtual void Print (std::ostream &output, int indentation = 0) const
    {
        Indent (output, indentation) << "(simple field) " << value_ << std::endl;
    }

private:
    T value_;
};

template <> class DataObjectSimpleField <std::string> : public DataObjectField
{
public:
    explicit DataObjectSimpleField (PTree *source)
        : value_ (source->data ())
    {

    }

    virtual ~DataObjectSimpleField () = default;

    virtual void Print (std::ostream &output, int indentation = 0) const
    {
        Indent (output, indentation) << "(simple field) \"" << value_ << "\"" << std::endl;
    }

private:
    std::string value_;
};

using DataObjectIntField = DataObjectSimpleField <int>;
using DataObjectFloatField = DataObjectSimpleField <float>;
using DataObjectStringField = DataObjectSimpleField <std::string>;
}
}
