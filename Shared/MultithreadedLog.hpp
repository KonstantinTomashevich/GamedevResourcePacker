#pragma once
#include <boost/log/trivial.hpp>
#include <boost/thread/mutex.hpp>

extern boost::mutex multithreadedLogMutex;

#define MT_LOG(severity, input) \
    multithreadedLogMutex.lock (); \
    BOOST_LOG_TRIVIAL (severity) << input; \
    multithreadedLogMutex.unlock ()
