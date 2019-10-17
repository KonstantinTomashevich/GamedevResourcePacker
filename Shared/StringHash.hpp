#pragma once
#include <boost/config.hpp>
#include <string>

namespace GamedevResourcePacker
{
/// Classic SDBM string hash.
BOOST_SYMBOL_EXPORT uint32_t StringHash (const std::string &str);
}
