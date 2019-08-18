#include "StringHash.hpp"

namespace GamedevResourcePacker
{
unsigned int StringHash (const std::string &str)
{
    unsigned int hash = 0;
    const char *cStr = str.c_str ();
    while (*cStr)
    {
        hash = *cStr + (hash << 6u) + (hash << 16u) - hash;
        ++cStr;
    }

    return hash;
}
}
