#include "StringHash.hpp"

namespace GamedevResourcePacker
{
uint32_t StringHash (const std::string &str)
{
    uint32_t hash = 0;
    const char *cStr = str.c_str ();
    while (*cStr)
    {
        hash = *cStr + (hash << 6u) + (hash << 16u) - hash;
        ++cStr;
    }

    return hash;
}
}
