/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_scrollwin_g.h
// Purpose:     wxMoScrolledWindow class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*
    Notes

    Currently we process left down only if the derived class
    doesn't intercept it first. So could either use event.Skip()
    in mouse event handler, or, process mouse events differently.

    We probably need to override ProcessEvent, and handle
    mouse events for scrolling purposes before handing
    on the event for further processing.

    Delay initial left click processing. If we start moving very soon,
    we never let the events reach the app (unless there's a setting
    in the scrolled window such that the app gets all the events
    and doesn't scroll; you might have e.g. a painting/scroll toggle in a painting
    app).

    If there's no movement after a short time, release the capture and send the original
    event to the app. Now we're in TouchStatusForwardToTarget mode.
    If we subsequently get a mouse up event, we again forward the event set status
    to TouchStatusNone, but if we're starting to drag, we send EVT_TOUCH_SCROLL_CANCEL_TOUCHES
    to the window. If this doesn't veto the operation, we then take over the dragging
    to scroll the window.



 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/log.h"
#include "wx/frame.h"

#include "wx/mobile/native/scrollwin.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoScrolledWindow, wxMoPanel)

IMPLEMENT_DYNAMIC_CLASS(wxTouchScrollEvent, wxEvent)
DEFINE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_DRAG)
DEFINE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES)

BEGIN_EVENT_TABLE(wxMoScrolledWindow, wxMoPanel)
    EVT_PAINT(wxMoScrolledWindow::OnPaint)
END_EVENT_TABLE()

wxMoScrolledWindow::wxMoScrolledWindow() : wxScrollHelper(this), m_touchScrollHelper(this)
{
    // FIXME stub
}

wxMoScrolledWindow::~wxMoScrolledWindow()
{
}

void wxMoScrolledWindow::Init()
{
    // FIXME stub
}

bool wxMoScrolledWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    // FIXME stub

	return true;
}

void wxMoScrolledWindow::OnPaint(wxPaintEvent& event)
{
    // FIXME stub
}

// Forward mouse event from another window to this window (useful within simulator
// when the scrolled window isn't able to capture events sent to a content window)
void wxMoScrolledWindow::ForwardMouseEvent(wxWindow* from, wxMouseEvent& event)
{
    // FIXME stub
}

IMPLEMENT_CLASS(wxMoTouchScrollHelperBase, wxEvtHandler)

BEGIN_EVENT_TABLE(wxMoTouchScrollHelperBase, wxEvtHandler)
    EVT_MOUSE_CAPTURE_CHANGED(wxMoTouchScrollHelper::OnCaptureChanged)
    EVT_TIMER(wxID_ANY, wxMoTouchScrollHelperBase::OnTimerNotify)
    EVT_IDLE(wxMoTouchScrollHelperBase::OnIdle)
    EVT_TOUCH_SCROLL_DRAG(wxMoTouchScrollHelperBase::OnTouchScroll)
END_EVENT_TABLE()

wxMoTouchScrollHelperBase::wxMoTouchScrollHelperBase(wxWindow* scrolledWindow)
{
    // FIXME stub
}

wxMoTouchScrollHelperBase::~wxMoTouchScrollHelperBase()
{
    // FIXME stub
}

void wxMoTouchScrollHelperBase::Init()
{
    // FIXME stub
}

// Process mouse events specially
bool wxMoTouchScrollHelperBase::ProcessEvent(wxEvent& event)
{
    // FIXME stub

	return true;
}

bool wxMoTouchScrollHelperBase::DoMouseEvent(wxMouseEvent& event)
{
    // FIXME stub

	return true;
}

// Cancels scroll overriding, at least in the generic implementation.
void wxMoTouchScrollHelperBase::CancelScrolling()
{
    // FIXME stub
}

void wxMoTouchScrollHelperBase::OnCaptureChanged(wxMouseCaptureChangedEvent& event)
{
    // FIXME stub
}

void wxMoTouchScrollHelperBase::OnIdle(wxIdleEvent& event)
{
    // FIXME stub
}

bool wxMoTouchScrollHelperBase::MovedSignificantly(const wxPoint& pt1, const wxPoint& pt2)
{
    // FIXME stub

	return true;
}

void wxMoTouchScrollHelperBase::OnTimerNotify(wxTimerEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoTouchScrollHelperBase::ShowTransientScrollbars(bool show)
{
    // FIXME stub
}

// Implements scrolling when a drag is detected
void wxMoTouchScrollHelperBase::OnTouchScroll(wxTouchScrollEvent& event)
{
    // FIXME stub
}

IMPLEMENT_CLASS(wxMoTouchScrollHelper, wxMoTouchScrollHelperBase)

bool wxMoTouchScrollHelper::GetScrollBarDetails(int orientation,
        int* range, int* thumbPosition, int* thumbSize)
{
    // FIXME stub

    return false;
}

// Does the scrolling for the specific scrolled window type.
bool wxMoTouchScrollHelper::DoScrolling(const wxPoint& lastPos, const wxPoint& newPos)
{
    // FIXME stub

    return true;
}
