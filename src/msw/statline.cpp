/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statline.cpp
// Purpose:     MSW version of wxStaticLine class
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id$
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/statline.h"

#if wxUSE_STATLINE

#include "wx/msw/private.h"
#include "wx/log.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

bool wxStaticLine::Create( wxWindow *parent,
                           wxWindowID id,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString &name)
{
    if ( !CreateBase(parent, id, pos, size, style, name) )
        return FALSE;

    parent->AddChild(this);

    wxSize sizeReal = AdjustSize(size);

#ifndef WIN32
#define SS_SUNKEN 0
#endif

    m_hWnd = (WXHWND)::CreateWindow
                       (
                        _T("STATIC"),
                        "",
                        WS_VISIBLE | WS_CHILD |
                        SS_GRAYRECT | SS_SUNKEN, // | SS_ETCHEDFRAME,
                        pos.x, pos.y, sizeReal.x, sizeReal.y,
                        GetWinHwnd(parent),
                        (HMENU)m_windowId,
                        wxGetInstance(),
                        NULL
                       );

    if ( !m_hWnd )
    {
#ifdef __WXDEBUG__        
        wxLogDebug(_T("Failed to create static control"));
#endif
        return FALSE;
    }

    SubclassWin(m_hWnd);

    return TRUE;
}
#endif

