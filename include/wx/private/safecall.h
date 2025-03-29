///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/safecall.h
// Purpose:     Call a function "safely", i.e. potentially catching exceptions.
// Author:      Vadim Zeitlin
// Created:     2025-03-20
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SAFECALL_H_
#define _WX_PRIVATE_SAFECALL_H_

#include "wx/app.h"

#if wxUSE_EXCEPTIONS

#include "wx/sysopt.h"

// General version calls the given function or function-like object and
// executes the provided handler if an exception is thrown.
//
// Both the function and the handler must return the value of the same type R,
// possibly void.
template <typename R, typename T1, typename T2>
inline R wxSafeCall(const T1& func, const T2& handler)
{
    // This special option exists in order to avoid having try/catch blocks
    // around potentially throwing code.
    if ( wxSystemOptions::IsFalse("catch-unhandled-exceptions") )
    {
        return func();
    }

    try
    {
        return func();
    }
    catch ( ... )
    {
        return handler();
    }
}

// Simplified version for the common case when the function doesn't return
// anything and we just want to call wxApp::OnUnhandledException() if it
// throws.
template <typename T>
inline void wxSafeCall(const T& func)
{
    wxSafeCall<void>(func, wxApp::CallOnUnhandledException);
}

#else // !wxUSE_EXCEPTIONS

template <typename R, typename T1, typename T2>
inline R wxSafeCall(const T1& func, const T2& WXUNUSED(handler))
{
    return func();
}

template <typename T>
inline void wxSafeCall(const T& func)
{
    func();
}

#endif // wxUSE_EXCEPTIONS/!wxUSE_EXCEPTIONS

#endif // _WX_PRIVATE_SAFECALL_H_
