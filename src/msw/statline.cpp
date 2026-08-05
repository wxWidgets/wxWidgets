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
    msStyle |= SS_SUNKEN | SS_NOTIFY | WS_CLIPSIBLINGS;
    msStyle |= SS_GRAYRECT ;

    return msStyle ;
}

void wxStaticLine::MSWSetDarkOrLightMode(SetMode setmode)
{
    wxStaticLineBase::MSWSetDarkOrLightMode(setmode);

    // Sunken 3D border looks too bright in dark mode as it uses white colour.
    // Style SS_SUNKEN enables WS_EX_STATICEDGE, so update them both.
    auto style = ::GetWindowLong(m_hWnd, GWL_STYLE);
    auto exStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
    if ( wxMSWDarkMode::IsActive() )
    {
        style &= ~SS_SUNKEN;
        exStyle &= ~WS_EX_STATICEDGE;
    }
    else
    {
        style |= SS_SUNKEN;
        exStyle |= WS_EX_STATICEDGE;
    }
    ::SetWindowLong(m_hWnd, GWL_STYLE, style);
    ::SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);
    ::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED |
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

#endif // wxUSE_STATLINE
