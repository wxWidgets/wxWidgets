///////////////////////////////////////////////////////////////////////////////
// Name:        univ/frame.cpp
// Purpose:     wxFrame class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.05.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univframe.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

    #include "wx/menu.h"
#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxFrame, wxFrameNative)
    EVT_SIZE(wxFrame::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)

// ----------------------------------------------------------------------------
// ctors
// ----------------------------------------------------------------------------

wxFrame::wxFrame()
{
}

wxFrame::wxFrame(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
       : wxFrameNative(parent, id, title, pos, size, style, name)
{
    m_renderer = NULL;
}

// ----------------------------------------------------------------------------
// menu support
// ----------------------------------------------------------------------------

void wxFrame::OnSize(wxSizeEvent& event)
{
#if wxUSE_MENUS
    PositionMenuBar();
#endif // wxUSE_MENUS

    event.Skip();
}

#if wxUSE_MENUS

void wxFrame::PositionMenuBar()
{
    if ( m_frameMenuBar )
    {
        // the menubar is positioned above the client size, hence the negative
        // y coord
        m_frameMenuBar->SetSize(0, -m_frameMenuBar->GetSize().y,
                                GetClientSize().x, -1);
    }
}
#endif // wxUSE_MENUS

wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint pt = wxFrameNative::GetClientAreaOrigin();

#if wxUSE_MENUS
    if ( m_frameMenuBar )
    {
        pt.y += m_frameMenuBar->GetSize().y;
    }
#endif // wxUSE_MENUS

    return pt;
}

bool wxFrame::Enable( bool enable )
{
    if (!wxFrameNative::Enable(enable))
	return FALSE;
#ifdef __WXMICROWIN__
    if (m_frameMenuBar)
        m_frameMenuBar->Enable(enable);
#endif
    return TRUE;
}
