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

#include "wx/msw/uxtheme.h"

bool wxUxThemeIsActive()
{
    return ::IsAppThemed() && ::IsThemeActive();
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
#else
bool wxUxThemeIsActive()
{
    return false;
}
#endif // wxUSE_UXTHEME
