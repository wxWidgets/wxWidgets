///////////////////////////////////////////////////////////////////////////////
// Name:        common/popupcmn.cpp
// Purpose:     implementation of wxPopupComboWindow
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if 0 // def __GNUG__
    #pragma implementation "popupwin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

#ifndef WX_PRECOMP
    #include "wx/combobox.h"        // wxComboControl
#endif //WX_PRECOMP

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPopupComboWindow, wxPopupWindow)
    EVT_LEFT_DOWN(wxPopupComboWindow::OnLeftDown)
    //EVT_KILL_FOCUS(wxPopupComboWindow::OnKillFocus)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPopupWindowBase
// ----------------------------------------------------------------------------

bool wxPopupWindowBase::Create(wxWindow *parent)
{
    m_winParent = parent;

    return TRUE;
}

void wxPopupWindowBase::Position()
{
    wxPoint ptOrigin = m_winParent->ClientToScreen(wxPoint(0, 0));
    wxSize sizeScreen = wxGetDisplaySize();
    wxCoord heightParent = m_winParent->GetSize().y,
            heightSelf = GetSize().y;

    // is there enough space to put the popup below the window?
    wxCoord y = ptOrigin.y + heightParent;
    if ( y + heightSelf > sizeScreen.y )
    {
        // FIXME: don't assume that there is enough space above - but what else
        //        can we do?

        // position the control above the window
        y -= heightParent + heightSelf;
    }

    Move(ptOrigin.x, y);
}

// ----------------------------------------------------------------------------
// wxPopupComboWindow
// ----------------------------------------------------------------------------

wxPopupComboWindow::wxPopupComboWindow(wxComboControl *parent)
{
    m_child = (wxWindow *)NULL;

    (void)Create(parent);
}

bool wxPopupComboWindow::Create(wxComboControl *parent)
{
    return wxPopupWindow::Create(parent);
}

void wxPopupComboWindow::Popup()
{
    const wxWindowList& children = GetChildren();
    if ( children.GetCount() )
    {
        m_child = children.GetFirst()->GetData();
        m_child->SetFocus();
        m_child->CaptureMouse();
    }

    Show();
}

void wxPopupComboWindow::Dismiss()
{
    if ( m_child )
    {
        m_child->ReleaseMouse();
        m_child = NULL;
    }

    Hide();
}

void wxPopupComboWindow::DismissAndNotify()
{
    Dismiss();

    ((wxComboControl *)m_winParent)->OnDismiss();
}

void wxPopupComboWindow::OnLeftDown(wxMouseEvent& event)
{
    // clicking the mouse outside of this window makes us disappear
    wxCoord x, y;
    event.GetPosition(&x, &y);

    wxWindow *win = (wxWindow *)event.GetEventObject();
    wxHitTest ht = win->HitTest(x, y);

    if ( ht == wxHT_WINDOW_OUTSIDE )
    {
        DismissAndNotify();
    }
    else
    {
        event.Skip();
    }
}

void wxPopupComboWindow::OnKillFocus(wxFocusEvent& event)
{
    // when we lose focus we always disappear
    DismissAndNotify();
}

#endif // wxUSE_POPUPWIN
