/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/dcclient.h"
#endif

#include "wx/control.h"

#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__)
#include <commctrl.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()
#endif

// Item members
wxControl::wxControl()
{
  m_backgroundColour = *wxWHITE;
  m_foregroundColour = *wxBLACK;

#if WXWIN_COMPATIBILITY
  m_callback = 0;
#endif // WXWIN_COMPATIBILITY
}

wxControl::~wxControl()
{
    m_isBeingDeleted = TRUE;

    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxWindow *parent = (wxWindow *)GetParent();
    if (parent)
    {
        if (parent->GetDefaultItem() == (wxButton*) this)
            parent->SetDefaultItem(NULL);
    }
}

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}

#ifdef __WIN95__
bool wxControl::MSWOnNotify(int idCtrl,
                            WXLPARAM lParam,
                            WXLPARAM* result)
{
    wxCommandEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;
    NMHDR *hdr1 = (NMHDR*) lParam;
    switch ( hdr1->code )
    {
        case NM_CLICK:
            eventType = wxEVT_COMMAND_LEFT_CLICK;
            break;

        case NM_DBLCLK:
            eventType = wxEVT_COMMAND_LEFT_DCLICK;
            break;

        case NM_RCLICK:
            eventType = wxEVT_COMMAND_RIGHT_CLICK;
            break;

        case NM_RDBLCLK:
            eventType = wxEVT_COMMAND_RIGHT_DCLICK;
            break;

        case NM_SETFOCUS:
            eventType = wxEVT_COMMAND_SET_FOCUS;
            break;

        case NM_KILLFOCUS:
            eventType = wxEVT_COMMAND_KILL_FOCUS;
            break;

        case NM_RETURN:
            eventType = wxEVT_COMMAND_ENTER;
            break;

        default:
            return wxWindow::MSWOnNotify(idCtrl, lParam, result);
    }

    event.SetEventType(eventType);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}
#endif // Win95

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
    // In general, you don't want to erase the background of a control,
    // or you'll get a flicker.
    // TODO: move this 'null' function into each control that
    // might flicker.

    RECT rect;
    ::GetClientRect((HWND) GetHWND(), &rect);

    HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(),
                                                  GetBackgroundColour().Green(),
                                                  GetBackgroundColour().Blue()));
    int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

    ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
}

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// Call this repeatedly for several wnds to find the overall size
// of the widget.
// Call it initially with -1 for all values in rect.
// Keep calling for other widgets, and rect will be modified
// to calculate largest bounding rectangle.
void wxFindMaxSize(WXHWND wnd, RECT *rect)
{
    int left = rect->left;
    int right = rect->right;
    int top = rect->top;
    int bottom = rect->bottom;

    GetWindowRect((HWND) wnd, rect);

    if (left < 0)
        return;

    if (left < rect->left)
        rect->left = left;

    if (right > rect->right)
        rect->right = right;

    if (top < rect->top)
        rect->top = top;

    if (bottom > rect->bottom)
        rect->bottom = bottom;
}

