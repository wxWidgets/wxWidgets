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

#include "wx/univ/renderer.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// event handlers which we use to intercept events which cause the popup to
// disappear
class wxPopupWindowHandler : public wxEvtHandler
{
public:
    wxPopupWindowHandler(wxPopupComboWindow *popup) { m_popup = popup; }

protected:
    // event handlers
    void OnLeftDown(wxMouseEvent& event);

private:
    wxPopupComboWindow *m_popup;

    DECLARE_EVENT_TABLE()
};

class wxPopupFocusHandler : public wxEvtHandler
{
public:
    wxPopupFocusHandler(wxPopupComboWindow *popup) { m_popup = popup; }

protected:
    // event handlers
    void OnKillFocus(wxFocusEvent& event);

private:
    wxPopupComboWindow *m_popup;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPopupWindowHandler, wxEvtHandler)
    EVT_LEFT_DOWN(wxPopupWindowHandler::OnLeftDown)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxPopupFocusHandler, wxEvtHandler)
    EVT_KILL_FOCUS(wxPopupFocusHandler::OnKillFocus)
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

#ifdef __WXUNIVERSAL__
    wxRect rectBorders = m_winParent->GetRenderer()->
                            GetBorderDimensions(m_winParent->GetBorder());
    ptOrigin.x -= rectBorders.x;
    y -= rectBorders.y;
#endif // __WXUNIVERSAL__

    Move(ptOrigin.x, y);
}

// ----------------------------------------------------------------------------
// wxPopupComboWindow
// ----------------------------------------------------------------------------

wxPopupComboWindow::wxPopupComboWindow(wxComboControl *parent)
{
    m_child =
    m_focus = (wxWindow *)NULL;

    (void)Create(parent);
}

wxPopupComboWindow::~wxPopupComboWindow()
{
    PopHandlers();
}

bool wxPopupComboWindow::Create(wxComboControl *parent)
{
    return wxPopupWindow::Create(parent);
}

void wxPopupComboWindow::PopHandlers()
{
    if ( m_child )
    {
        m_child->PopEventHandler(TRUE /* delete it */);
        m_child->ReleaseMouse();
        m_child = NULL;
    }

    if ( m_focus )
    {
        m_focus->PopEventHandler(TRUE /* delete it */);
        m_focus = NULL;
    }
}

void wxPopupComboWindow::Popup(wxWindow *winFocus)
{
    const wxWindowList& children = GetChildren();
    if ( children.GetCount() )
    {
        m_child = children.GetFirst()->GetData();
        m_child->CaptureMouse();
        m_child->PushEventHandler(new wxPopupWindowHandler(this));
    }

    Show();

    m_focus = winFocus ? winFocus : this;
    m_focus->SetFocus();
    m_focus->PushEventHandler(new wxPopupFocusHandler(this));
}

void wxPopupComboWindow::Dismiss()
{
    PopHandlers();

    Hide();
}

void wxPopupComboWindow::DismissAndNotify()
{
    Dismiss();

    ((wxComboControl *)m_winParent)->OnDismiss();
}

// ----------------------------------------------------------------------------
// wxPopupWindowHandler
// ----------------------------------------------------------------------------

void wxPopupWindowHandler::OnLeftDown(wxMouseEvent& event)
{
    // clicking the mouse outside of this window makes us disappear
    wxCoord x, y;
    event.GetPosition(&x, &y);

    wxWindow *win = (wxWindow *)event.GetEventObject();
    wxHitTest ht = win->HitTest(x, y);

    if ( ht == wxHT_WINDOW_OUTSIDE )
    {
        m_popup->DismissAndNotify();
    }
    else
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// wxPopupFocusHandler
// ----------------------------------------------------------------------------

void wxPopupFocusHandler::OnKillFocus(wxFocusEvent& event)
{
    // when we lose focus we always disappear
    m_popup->DismissAndNotify();
}

#endif // wxUSE_POPUPWIN
