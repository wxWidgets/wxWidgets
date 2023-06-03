///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uxtheme.cpp
// Purpose:     implements wxUxThemeEngine class: support for XP themes
// Author:      John Platts, Vadim Zeitlin
// Modified by:
// Created:     2003
// Copyright:   (c) 2003 John Platts, Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_UXTHEME

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/module.h"
#endif //WX_PRECOMP

#include "wx/dynlib.h"

#include "wx/msw/uxtheme.h"

bool wxUxThemeIsActive()
{
    return ::IsAppThemed() && ::IsThemeActive();
}

/* static */
HTHEME
wxUxThemeHandle::DoOpenThemeData(HWND hwnd, const wchar_t *classes, int dpi)
{
    // If DPI is the default one, we can use the old function.
    if ( dpi != STD_DPI )
    {
        // We need to use OpenThemeDataForDpi() which is not available under
        // all supported systems, so try to load it dynamically if not done yet.
        typedef HTHEME (WINAPI *OpenThemeDataForDpi_t)(HWND, LPCWSTR, UINT);
        static OpenThemeDataForDpi_t s_pfnOpenThemeDataForDpi = nullptr;
        static bool s_initDone = false;

        if ( !s_initDone )
        {
            wxLoadedDLL dllUxTheme(wxS("uxtheme.dll"));
            wxDL_INIT_FUNC(s_pfn, OpenThemeDataForDpi, dllUxTheme);
            s_initDone = true;
        }

        if ( s_pfnOpenThemeDataForDpi )
            return s_pfnOpenThemeDataForDpi(hwnd, classes, dpi);
    }

    return ::OpenThemeData(hwnd, classes);
}

wxColour wxUxThemeHandle::GetColour(int part, int prop, int state) const
{
    COLORREF col;

    HRESULT hr = ::GetThemeColor(m_hTheme, part, state, prop, &col);
    if ( FAILED(hr) )
    {
        wxLogApiError(
            wxString::Format("GetThemeColor(%i, %i, %i)", part, state, prop),
            hr
        );
        return wxColour{};
    }

    return wxRGBToColour(col);
}

wxSize wxUxThemeHandle::DoGetSize(int part, int state, THEMESIZE ts) const
{
    SIZE size;
    HRESULT hr = ::GetThemePartSize(m_hTheme, nullptr, part, state, nullptr, ts,
                                    &size);
    if ( FAILED(hr) )
    {
        wxLogApiError(
            wxString::Format("GetThemePartSize(%i, %i)", part, state),
            hr
        );
        return {};
    }

    return wxSize{size.cx, size.cy};
}

void
wxUxThemeHandle::DrawBackground(HDC hdc, const RECT& rc, int part, int state)
{
    HRESULT hr = ::DrawThemeBackground(m_hTheme, hdc, part, state, &rc, nullptr);
    if ( FAILED(hr) )
    {
        wxLogApiError(
            wxString::Format("DrawThemeBackground(%i, %i)", part, state),
            hr
        );
    }
}

void
wxUxThemeHandle::DrawBackground(HDC hdc, const wxRect& rect, int part, int state)
{
    RECT rc;
    wxCopyRectToRECT(rect, rc);

    DrawBackground(hdc, rc, part, state);
}

#else
bool wxUxThemeIsActive()
{
    return false;
}
#endif // wxUSE_UXTHEME
