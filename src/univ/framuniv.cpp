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

    EVT_CHAR(wxFrame::OnChar)
    EVT_KEY_UP(wxFrame::OnKeyUp)
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
        // the menubar is positioned above the client size, hence the negative
        // y coord
        m_frameMenuBar->SetSize(0, -m_frameMenuBar->GetSize().y,
                                GetClientSize().x, -1);
    }
#endif // wxUSE_MENUS
}

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

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

void wxFrame::OnChar(wxKeyEvent& event)
{
#if wxUSE_MENUS
    if ( event.AltDown() && !event.ControlDown() )
    {
        int key = event.GetKeyCode();

        if ( m_frameMenuBar )
        {
            int item = m_frameMenuBar->FindNextItemForAccel(-1, key);
            if ( item != -1 )
            {
                m_frameMenuBar->SetFocus();
                m_frameMenuBar->PopupMenu((size_t)item);

                // skip "event.Skip()" below
                return;
            }
        }
    }
#endif // wxUSE_MENUS

    event.Skip();
}

void wxFrame::OnKeyUp(wxKeyEvent& event)
{
#if wxUSE_MENUS
    int key = event.GetKeyCode();

    if ( !event.HasModifiers() && (key == WXK_MENU || key == WXK_F10) )
    {
        if ( m_frameMenuBar )
        {
            m_frameMenuBar->SetFocus();
            m_frameMenuBar->SelectMenu(0);
        }
    }
    else
#endif // wxUSE_MENUS
    {
        event.Skip();
    }
}

