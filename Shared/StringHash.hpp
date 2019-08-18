#pragma once
#include <boost/config.hpp>
#include <string>

namespace GamedevResourcePacker
{
/// Classic SDBM string hash.
BOOST_SYMBOL_EXPORT unsigned int StringHash (const std::string &str);
}
