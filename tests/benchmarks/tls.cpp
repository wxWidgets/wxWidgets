/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/strings.cpp
// Purpose:     String-related benchmarks
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "bench.h"

#ifdef __UNIX__
    #define HAVE_PTHREAD
    #include <pthread.h>
#endif

#if wxCHECK_GCC_VERSION(3, 3)
    #define HAVE_COMPILER_THREAD
    #define wxTHREAD_SPECIFIC __thread
#endif

// uncomment this to also test Boost version (you will also need to link with
// libboost_threads)
//#define HAVE_BOOST_THREAD
#ifdef HAVE_BOOST_THREAD
    #include <boost/thread/tss.hpp>
#endif


static const int NUM_ITER = 1000;

// this is just a baseline
BENCHMARK_FUNC(DummyTLS)
{
    static int s_global = 0;

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            s_global = 0;
        else
            s_global = n;
    }

    return !s_global;
}

#ifdef HAVE_COMPILER_THREAD

BENCHMARK_FUNC(CompilerTLS)
{
    static wxTHREAD_SPECIFIC int s_global = 0;

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            s_global = 0;
        else
            s_global = n;
    }

    return !s_global;
}

#endif // HAVE_COMPILER_THREAD

#ifdef HAVE_PTHREAD

class PthreadKey
{
public:
    PthreadKey()
    {
        pthread_key_create(&m_key, NULL);
    }

    ~PthreadKey()
    {
        pthread_key_delete(m_key);
    }

    operator pthread_key_t() const { return m_key; }

private:
    pthread_key_t m_key;

    DECLARE_NO_COPY_CLASS(PthreadKey)
};

BENCHMARK_FUNC(PosixTLS)
{
    static PthreadKey s_key;

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            pthread_setspecific(s_key, 0);
        else
            pthread_setspecific(s_key, &n);
    }

    return !pthread_getspecific(s_key);
}

#endif // HAVE_PTHREAD

#ifdef HAVE_BOOST_THREAD

BENCHMARK_FUNC(BoostTLS)
{
    static boost::thread_specific_ptr<int> s_ptr;
    if ( !s_ptr.get() )
        s_ptr.reset(new int(0));

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            *s_ptr = 0;
        else
            *s_ptr = n;
    }

    return !*s_ptr;
}

#endif // HAVE_BOOST_THREAD
