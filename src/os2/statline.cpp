/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statline.cpp
// Purpose:     OS2 version of wxStaticLine class
// Author:      David Webster
// Created:     10/23/99
// Version:     $Id$
// Copyright:   (c) 1999 David Webster
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

#include "wx/os2/private.h"
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
    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    parent->AddChild(this);

    wxSize sizeReal = AdjustSize(size);

// TODO:
/*
    m_hWnd = (WXHWND)::CreateWindow
                       (
                        wxT("STATIC"),
                        wxT(""),
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
        wxLogDebug(wxT("Failed to create static control"));
#endif
        return FALSE;
    }

    SubclassWin(m_hWnd);

    return TRUE;
*/
    return FALSE;
}
#endif

