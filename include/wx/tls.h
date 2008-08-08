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

#ifdef HAVE___THREAD_KEYWORD
    #define wxHAS_COMPILER_TLS
    #define wxTHREAD_SPECIFIC_DECL __thread
#elif wxCHECK_VISUALC_VERSION(7)
    #define wxHAS_COMPILER_TLS
    #define wxTHREAD_SPECIFIC_DECL __declspec(thread)
#endif

// ----------------------------------------------------------------------------
// define wxTLS_TYPE()
// ----------------------------------------------------------------------------

#ifdef wxHAS_COMPILER_TLS
    #define wxTLS_TYPE(T) wxTHREAD_SPECIFIC_DECL T
#else // !wxHAS_COMPILER_TLS
    #ifdef __WXMSW__
        #include "wx/msw/tls.h"
    #elif defined(__UNIX__)
        #include "wx/unix/tls.h"
    #else
        // TODO: we could emulate TLS for such platforms...
        #error Neither compiler nor OS support thread-specific variables.
    #endif

    // wxTlsValue<T> represents a thread-specific value of type T
    template <typename T>
    class wxTlsValue
    {
    public:
        typedef T ValueType;

        wxTlsValue() { *this = static_cast<T>(0); }

        wxTlsValue& operator=(T value)
        {
            m_key.Set(wxUIntToPtr(value));

            return *this;
        }

        operator T() const { return wxPtrToUInt(m_key.Get()); }

    private:
        wxTlsKey m_key;

        DECLARE_NO_COPY_TEMPLATE_CLASS(wxTlsValue, T)
    };

    #define wxTLS_TYPE(T) wxTlsValue<T>
#endif // wxHAS_COMPILER_TLS/!wxHAS_COMPILER_TLS

#endif // _WX_TLS_H_

