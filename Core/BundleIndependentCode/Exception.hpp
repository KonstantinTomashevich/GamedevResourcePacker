#pragma once
#include <boost/exception/exception.hpp>
#include <exception>
#include <string>

namespace ResourceSubsystem
{
template <typename T> class Exception : public boost::exception, public std::exception
{
public:
    explicit Exception (const std::string &info)
        : info_ (info)
    {}

    virtual const char *what () const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
    {
        return info_.c_str ();
    }

private:
    std::string info_;
};
}
