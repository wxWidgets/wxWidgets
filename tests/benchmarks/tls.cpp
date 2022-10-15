/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/strings.cpp
// Purpose:     String-related benchmarks
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "bench.h"

#include "wx/tls.h"

#if defined(__UNIX__)
    #define HAVE_PTHREAD
    #include <pthread.h>
#elif defined(__WIN32__)
    #define HAVE_WIN32_THREAD
    #include "wx/msw/wrapwin.h"
#endif

#ifdef __GNUC__
    #define HAVE_COMPILER_THREAD
    #define wxTHREAD_SPECIFIC __thread
#elif defined(__VISUALC__)
    #define HAVE_COMPILER_THREAD
    #define wxTHREAD_SPECIFIC __declspec(thread)
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
        pthread_key_create(&m_key, nullptr);
    }

    ~PthreadKey()
    {
        pthread_key_delete(m_key);
    }

    operator pthread_key_t() const { return m_key; }

private:
    pthread_key_t m_key;

    wxDECLARE_NO_COPY_CLASS(PthreadKey);
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

#ifdef HAVE_WIN32_THREAD

class TlsSlot
{
public:
    TlsSlot()
    {
        m_slot = ::TlsAlloc();
    }

    ~TlsSlot()
    {
        ::TlsFree(m_slot);
    }

    operator DWORD() const { return m_slot; }

private:
    DWORD m_slot;

    wxDECLARE_NO_COPY_CLASS(TlsSlot);
};

BENCHMARK_FUNC(Win32TLS)
{
    static TlsSlot s_slot;

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            ::TlsSetValue(s_slot, 0);
        else
            ::TlsSetValue(s_slot, &n);
    }

    return !::TlsGetValue(s_slot);
}

#endif // HAVE_WIN32_THREAD

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

BENCHMARK_FUNC(wxTLS)
{
    static wxTLS_TYPE(int) s_globalVar;
    #define s_global wxTLS_VALUE(s_globalVar)

    for ( int n = 0; n < NUM_ITER; n++ )
    {
        if ( n % 2 )
            s_global = 0;
        else
            s_global = n;
    }

    return !s_global;
}
