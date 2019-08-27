#pragma once
#include "Exception.hpp"
#include <boost/exception/all.hpp>

namespace ResourceSubsystem
{
template <typename ExceptionT> void CheckedFileRead (void *dst, size_t size, size_t count,
    FILE *file, const char *errorMessage)
{
    if (fread (dst, size, count, file) != size * count)
    {
        BOOST_THROW_EXCEPTION (Exception <ExceptionT> (errorMessage));
    }
}
}
