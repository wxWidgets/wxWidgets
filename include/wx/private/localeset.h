///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/localeset.h
// Purpose:     Define helper wxLocaleSetter class.
// Author:      Vadim Zeitlin
// Created:     2021-08-03 (extracted from tests/testprec.h)
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_LOCALESET_H_
#define _WX_PRIVATE_LOCALESET_H_

#include "wx/crt.h"     // wxStrdupA()

#include <locale.h>

// Helper class setting the locale to the given one for its lifetime.
class wxLocaleSetter
{
public:
    wxLocaleSetter(const char *loc)
        : m_locOld(wxStrdupA(setlocale(LC_ALL, nullptr)))
    {
        setlocale(LC_ALL, loc);
    }

    ~wxLocaleSetter()
    {
        setlocale(LC_ALL, m_locOld);
        free(m_locOld);
    }

private:
    char * const m_locOld;

    wxDECLARE_NO_COPY_CLASS(wxLocaleSetter);
};

// An even simpler helper for setting the locale to "C" one during its lifetime.
class wxCLocaleSetter : private wxLocaleSetter
{
public:
    wxCLocaleSetter() : wxLocaleSetter("C") { }

private:
    wxDECLARE_NO_COPY_CLASS(wxCLocaleSetter);
};

// This function must be called on program startup and after changing
// locale to ensure LC_CTYPE is set correctly under macOS (it does nothing
// under the other platforms currently).
inline void wxEnsureLocaleIsCompatibleWithCRT()
{
#if defined(__DARWIN__)
    // In OS X and iOS, wchar_t CRT functions convert to char* and fail under
    // some locales. The safest fix is to set LC_CTYPE to UTF-8 to ensure that
    // they can handle any input.
    //
    // Note that this must be done for any app, Cocoa or console, whether or
    // not it uses wxLocale.
    //
    // See https://stackoverflow.com/questions/11713745/why-does-the-printf-family-of-functions-care-about-locale
    setlocale(LC_CTYPE, "UTF-8");
#endif // defined(__DARWIN__)
}

#endif // _WX_PRIVATE_LOCALESET_H_
