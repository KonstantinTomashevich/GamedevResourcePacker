#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "PluginAPI.hpp"

#define PLUGINS_LIST_INITIAL_CAPACITY 10

namespace GamedevResourcePacker
{
using PluginVector = boost::container::small_vector <boost::shared_ptr <PluginAPI>, PLUGINS_LIST_INITIAL_CAPACITY>;
class PluginManager
{
public:
    PluginManager ();
    virtual ~PluginManager ();

    void Load ();
    const PluginVector &GetPluginsVector () const;

private:
    PluginVector plugins_;
};
}

