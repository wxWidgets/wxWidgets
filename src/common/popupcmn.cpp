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
    wxPoint pos = event.GetPosition();

    // scrollbar on which the click occured
    wxWindow *sbar = NULL;

    wxWindow *win = (wxWindow *)event.GetEventObject();
    switch ( win->HitTest(pos.x, pos.y) )
    {
        case wxHT_WINDOW_OUTSIDE:
            // clicking outside a popup dismisses it
            m_popup->DismissAndNotify();
            break;

        case wxHT_WINDOW_HORZ_SCROLLBAR:
            sbar = win->GetScrollbar(wxHORIZONTAL);
            break;

        case wxHT_WINDOW_VERT_SCROLLBAR:
            sbar = win->GetScrollbar(wxVERTICAL);
            break;

        default:
            // forgot to update the switch after adding a new hit test code?
            wxFAIL_MSG( _T("unexpected HitTest() return value") );
            // fall through

        case wxHT_WINDOW_CORNER:
            // don't actually know if this one is good for anything, but let it
            // pass just in case

        case wxHT_WINDOW_INSIDE:
            event.Skip();
            break;
    }

    if ( sbar )
    {
        // translate the event coordinates to the scrollbar ones
        pos = sbar->ScreenToClient(win->ClientToScreen(pos));

        // and give the event to it
        wxMouseEvent event2 = event;
        event2.m_x = pos.x;
        event2.m_y = pos.y;

        (void)sbar->GetEventHandler()->ProcessEvent(event2);

        if ( wxWindow::GetCapture() != win )
        {
            // scrollbar has captured the mouse so we need to ensure it
            // will be restored to us when it releases it
            wxWindow::SetStickyCapture(win);
        }
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
