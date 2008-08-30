///////////////////////////////////////////////////////////////////////////////
// Name:        wx/tls.h
// Purpose:     Implementation of thread local storage
// Author:      Vadim Zeitlin
// Created:     2008-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TLS_H_
#define _WX_TLS_H_

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// check for compiler support of thread-specific variables
// ----------------------------------------------------------------------------

// when not using threads at all, there is no need for thread-specific
// values to be really thread-specific
#if !wxUSE_THREADS
    #define wxHAS_COMPILER_TLS
    #define wxTHREAD_SPECIFIC_DECL
// __thread keyword is supported if configure detected it or when using mingw32
// >= 4.3 which is known to have it too
#elif defined(HAVE___THREAD_KEYWORD) || \
        (defined(__MINGW32__) && wxCHECK_GCC_VERSION(4, 3))
    #define wxHAS_COMPILER_TLS
    #define wxTHREAD_SPECIFIC_DECL __thread
// MSVC has its own version which might be supported by some other Windows
// compilers, to be tested
#elif wxCHECK_VISUALC_VERSION(7)
    #define wxHAS_COMPILER_TLS
    #define wxTHREAD_SPECIFIC_DECL __declspec(thread)
#endif

// ----------------------------------------------------------------------------
// define wxTLS_TYPE()
// ----------------------------------------------------------------------------

#ifdef wxHAS_COMPILER_TLS
    #define wxTLS_TYPE(T) wxTHREAD_SPECIFIC_DECL T
    #define wxTLS_PTR(var) (&(var))
    #define wxTLS_VALUE(var) (var)
#else // !wxHAS_COMPILER_TLS
    #ifdef __WXMSW__
        #include "wx/msw/tls.h"
    #elif defined(__OS2__)
        #include "wx/os2/tls.h"
    #elif defined(__UNIX__)
        #include "wx/unix/tls.h"
    #else
        // TODO: we could emulate TLS for such platforms...
        #error Neither compiler nor OS support thread-specific variables.
    #endif

    #include <stdlib.h> // for calloc()

    // wxTlsValue<T> represents a thread-specific value of type T but, unlike
    // with native compiler thread-specific variables, it behaves like a
    // (never NULL) pointer to T and so needs to be dereferenced before use
    template <typename T>
    class wxTlsValue
    {
    public:
        typedef T ValueType;

        // ctor doesn't do anything, the object is created on first access
        //
        // FIXME: the thread-specific values are currently not freed under
        //        Windows, resulting in memory leaks, this must be implemented
        //        there somehow (probably by keeping a list of all TLS objects
        //        and cleaning them up in wxThread cleanup)
        wxTlsValue()
#if !defined(__OS2__) && defined(__UNIX__)
            : m_key(free)
#endif
        {
        }

        // dtor is only called in the main thread context and so is not enough
        // to free memory allocated by us for the other threads, we use
        // destructor function when using Pthreads for this (which is not
        // called for the main thread as it doesn't call pthread_exit() but
        // just to be safe we also reset the key anyhow) and simply leak the
        // memory under Windows (see the FIXME above)
        ~wxTlsValue()
        {
            void * const value = m_key.Get();
            if ( value)
            {
                free(value);
                m_key.Set(NULL);
            }
        }

        // access the object creating it on demand
        ValueType *Get()
        {
            void *value = m_key.Get();
            if ( !value )
            {
                // ValueType must be POD to be used in wxHAS_COMPILER_TLS case
                // anyhow (at least gcc doesn't accept non-POD values being
                // declared with __thread) so initialize it as a POD too
                value = calloc(1, sizeof(ValueType));

                if ( !m_key.Set(value) )
                {
                    free(value);

                    // this will probably result in a crash in the caller but
                    // it's arguably better to crash immediately instead of
                    // slowly dying from out-of-memory errors which would
                    // happen as the next access to this object would allocate
                    // another ValueType instance and so on forever
                    value = NULL;
                }
            }

            return static_cast<ValueType *>(value);
        }

        // pointer-like accessors
        ValueType *operator->() { return Get(); }
        ValueType& operator*() { return *Get(); }

    private:
        wxTlsKey m_key;

        DECLARE_NO_COPY_TEMPLATE_CLASS(wxTlsValue, T)
    };

    #define wxTLS_TYPE(T) wxTlsValue<T>
    #define wxTLS_PTR(var) (var)
    #define wxTLS_VALUE(var) (*(var))
#endif // wxHAS_COMPILER_TLS/!wxHAS_COMPILER_TLS

#endif // _WX_TLS_H_

