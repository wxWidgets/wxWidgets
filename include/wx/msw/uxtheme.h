///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/uxtheme.h
// Purpose:     wxUxThemeEngine class: support for XP themes
// Author:      John Platts, Vadim Zeitlin
// Modified by:
// Created:     2003
// Copyright:   (c) 2003 John Platts, Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UXTHEME_H_
#define _WX_UXTHEME_H_

#include "wx/defs.h"

#include "wx/msw/private.h"     // we use GetHwndOf()
#include <uxtheme.h>

// Amazingly, GetThemeFont() and GetThemeSysFont() functions use LOGFONTA under
// XP but LOGFONTW (even in non-Unicode build) under later versions of Windows.
// If we declare them as taking LOGFONT below, the code would be able to
// silently pass LOGFONTA to them in ANSI build and would crash at run-time
// under Windows Vista/7 because of a buffer overrun (LOGFONTA being smaller
// than LOGFONTW expected by these functions). If we we declare them as taking
// LOGFONTW, the code wouldn't work correctly under XP. So we use a special
// wxUxThemeFont class to encapsulate this and intentionally change the LOGFONT
// output parameters of the theme functions to take it instead.

class wxUxThemeFont
{
public:
    // Trivial default ctor.
    wxUxThemeFont() { }

#if wxUSE_UNICODE
    // In Unicode build we always use LOGFONT anyhow so this class is
    // completely trivial.
    LPLOGFONTW GetPtr() { return &m_lfW; }
    const LOGFONTW& GetLOGFONT() { return m_lfW; }
#else // !wxUSE_UNICODE
    // Return either LOGFONTA or LOGFONTW pointer as required by the current
    // Windows version.
    LPLOGFONTW GetPtr()
    {
        return UseLOGFONTW() ? &m_lfW
                             : reinterpret_cast<LPLOGFONTW>(&m_lfA);
    }

    // This method returns LOGFONT (i.e. LOGFONTA in ANSI build and LOGFONTW in
    // Unicode one) which can be used with other, normal, Windows or wx
    // functions. Internally it may need to transform LOGFONTW to LOGFONTA.
    const LOGFONTA& GetLOGFONT()
    {
        if ( UseLOGFONTW() )
        {
            // Most of the fields are the same in LOGFONTA and LOGFONTW so just
            // copy everything by default.
            memcpy(&m_lfA, &m_lfW, sizeof(m_lfA));

            // But the face name must be converted from Unicode.
            WideCharToMultiByte(CP_ACP, 0, m_lfW.lfFaceName, -1,
                                m_lfA.lfFaceName, sizeof(m_lfA.lfFaceName),
                                NULL, NULL);
        }

        return m_lfA;
    }

private:
    static bool UseLOGFONTW()
    {
        return wxGetWinVersion() >= wxWinVersion_Vista;
    }

    LOGFONTA m_lfA;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

private:
    LOGFONTW m_lfW;

    wxDECLARE_NO_COPY_CLASS(wxUxThemeFont);
};

WXDLLIMPEXP_CORE bool wxUxThemeIsActive();

// ----------------------------------------------------------------------------
// wxUxThemeHandle: encapsulates ::Open/CloseThemeData()
// ----------------------------------------------------------------------------

class wxUxThemeHandle
{
public:
    wxUxThemeHandle(const wxWindow *win, const wchar_t *classes)
    {
        m_hTheme = (HTHEME)::OpenThemeData(GetHwndOf(win), classes);
    }

    operator HTHEME() const { return m_hTheme; }

    ~wxUxThemeHandle()
    {
        if ( m_hTheme )
        {
            ::CloseThemeData(m_hTheme);
        }
    }

private:
    HTHEME m_hTheme;

    wxDECLARE_NO_COPY_CLASS(wxUxThemeHandle);
};

#endif // _WX_UXTHEME_H_

