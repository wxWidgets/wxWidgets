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
    #include "wx/statusbr.h"
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
    EVT_SIZE(wxFrame::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)

// ----------------------------------------------------------------------------
// ctors
// ----------------------------------------------------------------------------

bool wxFrame::Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    return wxTopLevelWindow::Create(parent, id, title, pos, size, style, name);
}


// ----------------------------------------------------------------------------
// menu support
// ----------------------------------------------------------------------------

void wxFrame::OnSize(wxSizeEvent& event)
{
#if wxUSE_MENUS
    PositionMenuBar();
#endif // wxUSE_MENUS
#if wxUSE_STATUSBAR
    PositionStatusBar();
#endif // wxUSE_STATUSBAR

    event.Skip();
}

void wxFrame::SendSizeEvent()
{    
    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

#if wxUSE_MENUS

void wxFrame::PositionMenuBar()
{
    if ( m_frameMenuBar )
    {
        // the menubar is positioned above the client size, hence the negative
        // y coord
        wxCoord heightMbar = m_frameMenuBar->GetSize().y;
        m_frameMenuBar->SetSize(0, -heightMbar,
                                GetClientSize().x, heightMbar);
    }
}

void wxFrame::DetachMenuBar()
{
    wxFrameBase::DetachMenuBar();
    SendSizeEvent();
}

void wxFrame::AttachMenuBar(wxMenuBar *menubar)
{
    wxFrameBase::AttachMenuBar(menubar);
    SendSizeEvent();
}

#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR

void wxFrame::PositionStatusBar()
{
    if ( m_frameStatusBar )
    {
        wxCoord heightBar = m_frameStatusBar->GetSize().y;
        m_frameStatusBar->SetSize(0, GetClientSize().y,
                                  GetClientSize().x, heightBar);
    }
}

wxStatusBar* wxFrame::CreateStatusBar(int number, long style,
                                      wxWindowID id, const wxString& name)
{
    wxStatusBar *bar = wxFrameBase::CreateStatusBar(number, style, id, name);
    SendSizeEvent();
    return bar;
}

#endif // wxUSE_STATUSBAR

wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint pt = wxFrameBase::GetClientAreaOrigin();

#if wxUSE_MENUS
    if ( m_frameMenuBar )
    {
        pt.y += m_frameMenuBar->GetSize().y;
    }
#endif // wxUSE_MENUS

    return pt;
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxFrameBase::DoGetClientSize(width, height);

#if wxUSE_MENUS
    if ( m_frameMenuBar && height )
    {
        (*height) -= m_frameMenuBar->GetSize().y;
    }
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    if ( m_frameStatusBar && height )
    {
        (*height) -= m_frameStatusBar->GetSize().y;
    }
#endif // wxUSE_STATUSBAR
}

void wxFrame::DoSetClientSize(int width, int height)
{
#if wxUSE_MENUS
    if ( m_frameMenuBar )
    {
        height += m_frameMenuBar->GetSize().y;
    }
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    if ( m_frameStatusBar )
    {
        height += m_frameStatusBar->GetSize().y;
    }
#endif // wxUSE_STATUSBAR

    wxFrameBase::DoSetClientSize(width, height);
}

bool wxFrame::Enable(bool enable)
{
    if (!wxFrameBase::Enable(enable))
	return FALSE;
#ifdef __WXMICROWIN__
    if (m_frameMenuBar)
        m_frameMenuBar->Enable(enable);
#endif
    return TRUE;
}
