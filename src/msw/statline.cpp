/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statline.cpp
// Purpose:     MSW version of wxStaticLine class
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/statline.h"

#if wxUSE_STATLINE

#ifndef WX_PRECOMP
    #include "wx/msw/private.h"
    #include "wx/msw/missing.h"
#endif

#include "wx/msw/private/darkmode.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

bool wxStaticLine::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& sizeOrig,
                          long style,
                          const wxString &name)
{
    wxSize size = AdjustSize(sizeOrig);

    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    return MSWCreateControl(wxT("STATIC"), wxEmptyString, pos, size);
}

WXDWORD wxStaticLine::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    // we never have border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // add our default styles
    msStyle |= SS_NOTIFY | WS_CLIPSIBLINGS;
    msStyle |= SS_GRAYRECT ;

    // Sunken 3D border looks too bright in dark mode as it uses white colour,
    // so we use another style there for less ostentatious appearance.
    if ( !wxMSWDarkMode::IsActive() )
        msStyle |= SS_SUNKEN;

    return msStyle ;
}

#endif // wxUSE_STATLINE
