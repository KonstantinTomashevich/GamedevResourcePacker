#pragma once
#include <omp.h>
#include <boost/log/trivial.hpp>
#include <boost/thread/mutex.hpp>

extern boost::mutex multithreadedLogMutex;

#define MT_LOG(severity, input) \
    multithreadedLogMutex.lock (); \
    BOOST_LOG_TRIVIAL (severity) << "[Thread " << omp_get_thread_num () << "] " << input; \
    multithreadedLogMutex.unlock ()
