///////////////////////////////////////////////////////////////////////////////
// Name:        univ/frame.cpp
// Purpose:     wxFrame class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.05.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
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

#if defined(__WXMSW__)
    IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxFrameMSW)
#elif defined(__WXGTK__)
    IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxFrameGTK)
#endif

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
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

void wxFrame::OnSize(wxSizeEvent& event)
{
    PositionMenuBar();

    event.Skip();
}

void wxFrame::PositionMenuBar()
{
#if wxUSE_MENUS
    if ( m_frameMenuBar )
    {
        m_frameMenuBar->SetSize(0, 0, GetClientSize().x, -1);
    }
#endif // wxUSE_MENUS
}

