///////////////////////////////////////////////////////////////////////////////
// Name:        common/popupcmn.cpp
// Purpose:     implementation of wxPopupTransientWindow
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

#ifdef __GNUG__
    #pragma implementation "popupwinbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_POPUPWIN && !defined(__WXMOTIF__)

#include "wx/popupwin.h"

#ifndef WX_PRECOMP
    #include "wx/combobox.h"        // wxComboControl
#endif //WX_PRECOMP

#ifdef __WXUNIVERSAL__
    #include "wx/univ/renderer.h"
#endif // __WXUNIVERSAL__

// there is no src/{msw,mgl}/popupwin.cpp to put this in, so we do it here - BTW we
// probably could do it for all ports here just as well
#if defined(__WXMSW__) || defined(__WXMGL__)
    IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)
#endif // __WXMSW__

IMPLEMENT_DYNAMIC_CLASS(wxPopupTransientWindow, wxPopupWindow)

#if wxUSE_COMBOBOX && defined(__WXUNIVERSAL__)
    IMPLEMENT_DYNAMIC_CLASS(wxPopupComboWindow, wxPopupTransientWindow)
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// event handlers which we use to intercept events which cause the popup to
// disappear
class wxPopupWindowHandler : public wxEvtHandler
{
public:
    wxPopupWindowHandler(wxPopupTransientWindow *popup) { m_popup = popup; }

protected:
    // event handlers
    void OnLeftDown(wxMouseEvent& event);

private:
    wxPopupTransientWindow *m_popup;

    DECLARE_EVENT_TABLE()
};

class wxPopupFocusHandler : public wxEvtHandler
{
public:
    wxPopupFocusHandler(wxPopupTransientWindow *popup)
    {
        m_popup = popup;

#ifdef __WXGTK__
        // ignore the next few OnKillFocus() calls
        m_creationTime = time(NULL);
#endif // __WXGTK__
    }

protected:
    // event handlers
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyUp(wxKeyEvent& event);

private:
    wxPopupTransientWindow *m_popup;

    // hack around wxGTK bug: we always get several kill focus events
    // immediately after creation!
#ifdef __WXGTK__
    time_t m_creationTime;
#endif // __WXGTK__

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
    EVT_KEY_UP(wxPopupFocusHandler::OnKeyUp)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPopupWindowBase
// ----------------------------------------------------------------------------

wxPopupWindowBase::~wxPopupWindowBase()
{
    // this destructor is required for Darwin
}

bool wxPopupWindowBase::Create(wxWindow* WXUNUSED(parent), int WXUNUSED(flags))
{
    return TRUE;
}

void wxPopupWindowBase::Position(const wxPoint& ptOrigin,
                                 const wxSize& size)
{
    wxSize sizeScreen = wxGetDisplaySize(),
           sizeSelf = GetSize();

    // is there enough space to put the popup below the window (where we put it
    // by default)?
    wxCoord y = ptOrigin.y + size.y;
    if ( y + sizeSelf.y > sizeScreen.y )
    {
        // check if there is enough space above
        if ( ptOrigin.y > sizeSelf.y )
        {
            // do position the control above the window
            y -= size.y + sizeSelf.y;
        }
        //else: not enough space below nor above, leave below
    }

    // now check left/right too
    wxCoord x = ptOrigin.x + size.x;
    if ( x + sizeSelf.x > sizeScreen.x )
    {
        // check if there is enough space to the left
        if ( ptOrigin.x > sizeSelf.x )
        {
            // do position the control to the left
            x -= size.x + sizeSelf.x;
        }
        //else: not enough space there neither, leave in default position
    }

    Move(x, y, wxSIZE_NO_ADJUSTMENTS);
}

// ----------------------------------------------------------------------------
// wxPopupTransientWindow
// ----------------------------------------------------------------------------

void wxPopupTransientWindow::Init()
{
    m_child =
    m_focus = (wxWindow *)NULL;
}

wxPopupTransientWindow::wxPopupTransientWindow(wxWindow *parent, int style)
{
    Init();

    (void)Create(parent, style);
}

wxPopupTransientWindow::~wxPopupTransientWindow()
{
    PopHandlers();
}

void wxPopupTransientWindow::PopHandlers()
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

void wxPopupTransientWindow::Popup(wxWindow *winFocus)
{
    const wxWindowList& children = GetChildren();
    if ( children.GetCount() )
    {
        m_child = children.GetFirst()->GetData();
    }
    else
    {
        m_child = this;
    }

    // we can't capture mouse before the window is shown in wxGTK, so do it
    // first
    Show();

    m_child->CaptureMouse();
    m_child->PushEventHandler(new wxPopupWindowHandler(this));

    m_focus = winFocus ? winFocus : this;
    m_focus->SetFocus();

#ifdef __WXMSW__
    // FIXME: I don't know why does this happen but sometimes SetFocus() simply
    //        refuses to work under MSW - no error happens but the focus is not
    //        given to the window, i.e. the assert below is triggered
    //
    //        Try work around this as we can...

    //wxASSERT_MSG( FindFocus() == m_focus, _T("setting focus failed") );
    m_focus = FindFocus();
    if ( m_focus )
#endif // __WXMSW__
    {
        m_focus->PushEventHandler(new wxPopupFocusHandler(this));
    }
}

void wxPopupTransientWindow::Dismiss()
{
    PopHandlers();

    Hide();
}

void wxPopupTransientWindow::DismissAndNotify()
{
    Dismiss();

    OnDismiss();
}

void wxPopupTransientWindow::OnDismiss()
{
    // nothing to do here - but it may be interesting for derived class
}

bool wxPopupTransientWindow::ProcessLeftDown(wxMouseEvent& WXUNUSED(event))
{
    // no special processing here
    return FALSE;
}

#if wxUSE_COMBOBOX && defined(__WXUNIVERSAL__)

// ----------------------------------------------------------------------------
// wxPopupComboWindow
// ----------------------------------------------------------------------------

wxPopupComboWindow::wxPopupComboWindow(wxComboControl *parent)
                  : wxPopupTransientWindow(parent)
{
    m_combo = parent;
}

bool wxPopupComboWindow::Create(wxComboControl *parent)
{
    m_combo = parent;

    return wxPopupWindow::Create(parent);
}

void wxPopupComboWindow::PositionNearCombo()
{
    // the origin point must be in screen coords
    wxPoint ptOrigin = m_combo->ClientToScreen(wxPoint(0, 0));

#if 0 //def __WXUNIVERSAL__
    // account for the fact that (0, 0) is not the top left corner of the
    // window: there is also the border
    wxRect rectBorders = m_combo->GetRenderer()->
                            GetBorderDimensions(m_combo->GetBorder());
    ptOrigin.x -= rectBorders.x;
    ptOrigin.y -= rectBorders.y;
#endif // __WXUNIVERSAL__

    // position below or above the combobox: the width is 0 to put it exactly
    // below us, not to the left or to the right
    Position(ptOrigin, wxSize(0, m_combo->GetSize().y));
}

void wxPopupComboWindow::OnDismiss()
{
    m_combo->OnDismiss();
}

#endif // wxUSE_COMBOBOX && defined(__WXUNIVERSAL__)

// ----------------------------------------------------------------------------
// wxPopupWindowHandler
// ----------------------------------------------------------------------------

void wxPopupWindowHandler::OnLeftDown(wxMouseEvent& event)
{
    // let the window have it first (we're the first event handler in the chain
    // of handlers for this window)
    if ( m_popup->ProcessLeftDown(event) )
    {
        return;
    }

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

#ifdef __WXUNIVERSAL__
        case wxHT_WINDOW_HORZ_SCROLLBAR:
            sbar = win->GetScrollbar(wxHORIZONTAL);
            break;

        case wxHT_WINDOW_VERT_SCROLLBAR:
            sbar = win->GetScrollbar(wxVERTICAL);
            break;
#endif

        default:
            // forgot to update the switch after adding a new hit test code?
            wxFAIL_MSG( _T("unexpected HitTest() return value") );
            // fall through

        case wxHT_WINDOW_CORNER:
            // don't actually know if this one is good for anything, but let it
            // pass just in case

        case wxHT_WINDOW_INSIDE:
            // let the normal processing take place
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
    }
}

// ----------------------------------------------------------------------------
// wxPopupFocusHandler
// ----------------------------------------------------------------------------

void wxPopupFocusHandler::OnKillFocus(wxFocusEvent& event)
{
#ifdef __WXGTK__
    // ignore the next OnKillFocus() call
    if ( time(NULL) < m_creationTime + 1 )
    {
        event.Skip();

        return;
    }
#endif // __WXGTK__

    // when we lose focus we always disappear - unless it goes to the popup (in
    // which case we don't really lose it)
    if ( event.GetWindow() != m_popup )
        m_popup->DismissAndNotify();
}

void wxPopupFocusHandler::OnKeyUp(wxKeyEvent& event)
{
    // let the window have it first, it might process the keys
    if ( !m_popup->ProcessEvent(event) )
    {
        // by default, dismiss the popup
        m_popup->DismissAndNotify();
    }
}

#endif // wxUSE_POPUPWIN

