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

#endif // _WX_PRIVATE_LOCALESET_H_
