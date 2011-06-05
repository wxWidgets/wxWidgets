/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_scrollwin_g.h
// Purpose:     wxMoScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/popupwin.h"

#include "wx/mobile/generic/scrollwin.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoScrolledWindow, wxMoPanel)

IMPLEMENT_DYNAMIC_CLASS(wxTouchScrollEvent, wxEvent)
DEFINE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_DRAG)
DEFINE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES)

BEGIN_EVENT_TABLE(wxMoScrolledWindow, wxMoPanel)
    EVT_PAINT(wxMoScrolledWindow::OnPaint)
END_EVENT_TABLE()

wxMoScrolledWindow::wxMoScrolledWindow() : wxScrollHelperNative(this), m_touchScrollHelper(this)
{
    Init();
}

void wxMoScrolledWindow::Init()
{
    m_showHorizontalScrollIndicator = true;
    m_showVerticalScrollIndicator = true;
}

bool wxMoScrolledWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    m_targetWindow = this;

#ifdef __WXMAC__
    MacSetClipChildren( true ) ;
#endif

    bool ok = wxMoPanel::Create(parent, id, pos, size, style/*|wxHSCROLL|wxVSCROLL*/, name);

    wxWindow::SetScrollbar(wxHORIZONTAL, 0, 0, 0, false);
    wxWindow::SetScrollbar(wxVERTICAL, 0, 0, 0, false);

    return ok;
}

wxMoScrolledWindow::~wxMoScrolledWindow()
{
}

void wxMoScrolledWindow::OnPaint(wxPaintEvent& event)
{
    // the user code didn't really draw the window if we got here, so set this
    // flag to try to call OnDraw() later

    // JACS: can't do this, because m_handler's class is only defined in scrolwing.cpp.
    // m_handler->ResetDrawnFlag();

    event.Skip();
}

// Forward mouse event from another window to this window (useful within simulator
// when the scrolled window isn't able to capture events sent to a content window)
void wxMoScrolledWindow::ForwardMouseEvent(wxWindow* from, wxMouseEvent& event)
{
    int oldId = event.GetId();
    wxPoint oldPt = event.GetPosition();
    wxObject* oldObject = event.GetEventObject();

    wxPoint fakedPt = from->ClientToScreen(oldPt);
    fakedPt = ScreenToClient(fakedPt);

    event.m_x = fakedPt.x;
    event.m_y = fakedPt.y;
    event.SetEventObject(this);
    event.SetId(GetId());

    GetEventHandler()->ProcessEvent(event);

    event.m_x = oldPt.x;
    event.m_y = oldPt.y;
    event.SetEventObject(oldObject);
    event.SetId(oldId);
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
    Init();

    m_scrolledWindow = scrolledWindow;
    m_scrolledWindow->PushEventHandler(this);
}

wxMoTouchScrollHelperBase::~wxMoTouchScrollHelperBase()
{
    if (m_scrolledWindow)
        m_scrolledWindow->RemoveEventHandler(this);
    m_timer.Stop();

    delete m_scrollBarXContainer;
    delete m_scrollBarYContainer;
}

void wxMoTouchScrollHelperBase::Init()
{
    m_scrolledWindow = NULL;
    m_touchStatus = TouchStatusNone;
    m_startPos = wxPoint(0, 0);
    m_lastPos = wxPoint(0, 0);
    m_timer.SetOwner(this);
    m_inMouseEvent = false;
    m_contentProcessingDelay = 200;
    m_scrollBarX = NULL;
    m_scrollBarY = NULL;
    m_scrollBarXContainer = NULL;
    m_scrollBarYContainer = NULL;
}

// Process mouse events specially
bool wxMoTouchScrollHelperBase::ProcessEvent(wxEvent& event)
{
    bool processed = false;
    bool isMouse = event.IsKindOf(CLASSINFO(wxMouseEvent));
    if (isMouse)
        processed = DoMouseEvent((wxMouseEvent&) event);

    if (!processed)
        processed = wxEvtHandler::ProcessEvent(event);

    if (isMouse && ((wxMouseEvent&) event).LeftUp())
    {
        if (wxWindow::GetCapture() == m_scrolledWindow)
            m_scrolledWindow->ReleaseMouse();
    }

    return processed;
}

bool wxMoTouchScrollHelperBase::DoMouseEvent(wxMouseEvent& event)
{
    // What we want to do is: detect mouse down and set off a timer.
    // Then, if no (significant) movement is detected within this time,
    // stop the timer, and send a mouse-down event to the target,
    // disabling interception in this class until the next down event
    // occurs. If movement _was_ detected, start scrolling.

#ifdef __WXDEBUG__
    static int s_counter = 0;
    s_counter ++;
#endif

    if (m_inMouseEvent)
    {
        // We generated a mouse event and so we're being called again
        // recursively - skip to next handler
        //wxLogDebug(wxT("In mouse event, skipping. %d"), s_counter);
        return false;
    }

    if (event.LeftDown())
    {
        m_touchStatus = TouchStatusHaveTouchDown;
        m_originalTouchEvent = event;

        m_scrolledWindow->CaptureMouse();

        if (m_timer.IsRunning())
            m_timer.Stop();

        m_timer.Start(m_contentProcessingDelay, true /* one shot */);
        m_startPos = event.GetPosition();
        m_lastPos = event.GetPosition();

        OnStartScrolling(m_startPos);

        // Don't skip - eat the event
        return true;
    }
    else if (event.Dragging() && event.LeftIsDown())
    {
#ifdef __WXDEBUG__
        //wxLogDebug(wxT("Dragging %d"), s_counter);
#endif

        // Might have dragged into this window without a left click.
        // In which case, start scrolling.
        if (m_touchStatus == TouchStatusNone && !wxWindow::GetCapture() && !m_timer.IsRunning())
        {
            m_touchStatus = TouchStatusScrolling;
            m_startPos = event.GetPosition();
            m_lastPos = event.GetPosition();
            m_scrolledWindow->CaptureMouse();

            m_timer.Start(m_contentProcessingDelay, true /* one shot */);

            OnStartScrolling(m_startPos);
        }
        else if (m_touchStatus == TouchStatusForwardToTargetButDetectScrolling)
        {
            // See whether we could start scrolling
            // TODO: don't do this test if we should not do scroll detection (settable option)
            if (MovedSignificantly(m_startPos, event.GetPosition()))
            {
                wxTouchScrollEvent cancelEvent(wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES);
                cancelEvent.SetId(m_scrolledWindow->GetId());
                cancelEvent.SetEventObject(m_scrolledWindow);
                if (m_scrolledWindow->GetEventHandler()->ProcessEvent(cancelEvent))
                {
                    if (cancelEvent.IsAllowed())
                    {
                        // Start scrolling
                        m_touchStatus = TouchStatusScrolling;

                        if (wxWindow::GetCapture() != m_scrolledWindow)
                            m_scrolledWindow->CaptureMouse();
                    }
                    else
                    {
                        m_touchStatus = TouchStatusForwardToTarget;
                    }
                }
                else
                {
                    // If we don't process the event, forward to the target from now on.
                    m_touchStatus = TouchStatusForwardToTarget;
                }
            }
        }

        // When status is TouchStatusForwardToTarget, we definitely
        // don't want to do any more scrolling - do app-specific stuff.
        if (m_touchStatus == TouchStatusForwardToTarget)
        {
            m_lastPos = event.GetPosition();
            return false;
        }

        if ((m_touchStatus == TouchStatusScrolling) ||
            ((m_touchStatus != TouchStatusForwardToTarget) && MovedSignificantly(m_startPos, event.GetPosition())))
        {
            m_touchStatus = TouchStatusScrolling;

            DoScrolling(m_lastPos, event.GetPosition());

            ShowTransientScrollbars(true);

            m_lastPos = event.GetPosition();
        }
    }
    else if (event.LeftUp())
    {
#ifdef __WXDEBUG__
        //wxLogDebug(wxT("Left up %d"), s_counter);
#endif

        if (m_timer.IsRunning())
            m_timer.Stop();

        TouchStatus oldStatus = m_touchStatus;
        if (wxWindow::GetCapture() == m_scrolledWindow)
        {
            m_scrolledWindow->ReleaseMouse();
            m_touchStatus = oldStatus;
        }

        ShowTransientScrollbars(false);

        if (m_touchStatus == TouchStatusHaveTouchDown || m_touchStatus == TouchStatusForwardToTargetButDetectScrolling)
        {
            m_touchStatus = TouchStatusForwardToTarget;
            m_inMouseEvent = true;
            m_scrolledWindow->GetEventHandler()->ProcessEvent(m_originalTouchEvent);
            m_inMouseEvent = false;
        }

        if (m_touchStatus != TouchStatusScrolling)
        {
            m_touchStatus = TouchStatusNone;
            m_lastPos = event.GetPosition();

            return false;
        }

        m_touchStatus = TouchStatusNone;
    }
    else
    {
        if (m_touchStatus == TouchStatusForwardToTarget || m_touchStatus == TouchStatusForwardToTargetButDetectScrolling) // || m_touchStatus == TouchStatusNone)
        {
            return false;
        }
    }

    m_lastPos = event.GetPosition();

    return true;
}

// Cancels scroll overriding, at least in the generic implementation.
void wxMoTouchScrollHelperBase::CancelScrolling()
{
    m_touchStatus = TouchStatusForwardToTarget;
    if (m_timer.IsRunning())
        m_timer.Stop();
    ShowTransientScrollbars(false);
}

void wxMoTouchScrollHelperBase::OnCaptureChanged(wxMouseCaptureChangedEvent& event)
{
    wxUnusedVar(event);
#if 0
    if (event.GetCapturedWindow() != m_scrolledWindow)
    {
        //wxLogDebug(wxT("Capture changed."));
        ShowTransientScrollbars(false);
        m_touchStatus = TouchStatusNone;
    }
#endif
}

void wxMoTouchScrollHelperBase::OnIdle(wxIdleEvent& event)
{
    if (m_touchStatus == TouchStatusScrolling && m_scrolledWindow && wxWindow::GetCapture() != m_scrolledWindow)
    {
        m_touchStatus = TouchStatusNone;
        ShowTransientScrollbars(false);
    }
    event.Skip();
}

bool wxMoTouchScrollHelperBase::MovedSignificantly(const wxPoint& pt1, const wxPoint& pt2)
{
    int tolerance = 2;
    return (abs(pt2.x - pt1.x) > tolerance || abs(pt2.y - pt1.y) > tolerance);
}

void wxMoTouchScrollHelperBase::OnTimerNotify(wxTimerEvent& WXUNUSED(event))
{
    if (!MovedSignificantly(m_startPos, m_lastPos) &&
        m_touchStatus == TouchStatusHaveTouchDown)
    {
        if (wxWindow::GetCapture() == m_scrolledWindow)
            m_scrolledWindow->ReleaseMouse();

        ShowTransientScrollbars(false);
        if (1) // TODO: make optional, i.e. may not wish to ever cancel initial touch processing.
            m_touchStatus = TouchStatusForwardToTargetButDetectScrolling;
        else
            m_touchStatus = TouchStatusForwardToTarget;
        m_inMouseEvent = true;
        m_scrolledWindow->GetEventHandler()->ProcessEvent(m_originalTouchEvent);
        m_inMouseEvent = false;
    }
}

void wxMoTouchScrollHelperBase::ShowTransientScrollbars(bool show)
{
    int scrollBarWidth = 10;
    int scrollBarMargin = 4;

    int horizRange = 0, horizThumbPosition = 0, horizThumbSize = 0;
    int vertRange = 0, vertThumbPosition = 0, vertThumbSize = 0;

    bool useHorizScrollbar = GetScrollBarDetails(wxHORIZONTAL,
        & horizRange, & horizThumbPosition, & horizThumbSize);

    bool useVertScrollbar = GetScrollBarDetails(wxVERTICAL,
        & vertRange, & vertThumbPosition, & vertThumbSize);

    // If we need to show scrollbars, release capture first and then recapture.
    bool restoreCapture = false;
    if (m_scrolledWindow == wxWindow::GetCapture())
    {
        if ((useHorizScrollbar && (!m_scrollBarXContainer || !m_scrollBarXContainer->IsShown())) ||
            (useVertScrollbar && (!m_scrollBarYContainer || !m_scrollBarYContainer->IsShown())))
        {
            m_scrolledWindow->ReleaseMouse();
            restoreCapture = true;
        }
    }

    if (show)
    {
        if (useHorizScrollbar)
        {
            bool changePosition = true;
            if (m_scrollBarXContainer)
            {
                if (!m_scrollBarXContainer->IsShown())
                    m_scrollBarXContainer->Show(true);
                else
                    changePosition = false;
            }
            else
            {
#if wxUSE_POPUPWIN
                m_scrollBarXContainer = new wxPopupWindow(m_scrolledWindow->GetParent(), wxNO_BORDER);
#else
                m_scrollBarXContainer = new wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
                    wxDefaultSize, wxNO_BORDER|wxFRAME_NO_TASKBAR);
#endif

                m_scrollBarX = new wxScrollBar(m_scrollBarXContainer, wxID_ANY,
                    wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
            }

            m_scrollBarX->SetRange(horizRange);
            m_scrollBarX->SetThumbPosition(horizThumbPosition);
            m_scrollBarX->SetThumbSize(horizThumbSize);

            if (changePosition)
            {
                wxSize scrollBarSize = wxSize(m_scrolledWindow->GetSize().x
                    - scrollBarMargin*2 - (useVertScrollbar ? scrollBarWidth : 0),
                    scrollBarWidth);
                wxPoint scrollBarPosition = wxPoint(scrollBarMargin, m_scrolledWindow->GetSize().y - scrollBarSize.y - scrollBarMargin);

                // Convert to parent coordinates
                scrollBarPosition = m_scrolledWindow->ClientToScreen(scrollBarPosition);
                wxRect scrollBarRect(wxPoint(0, 0), scrollBarSize);

#if wxUSE_POPUPWIN
                wxRect frameRect(scrollBarPosition, scrollBarSize);
#else
                wxRect frameRect(scrollBarPosition, scrollBarSize);
#endif
                m_scrollBarX->SetSize(scrollBarRect);
                m_scrollBarXContainer->SetSize(frameRect);
            }

            m_scrollBarX->Update();
        }

        if (useVertScrollbar)
        {
            bool changePosition = true;
            if (m_scrollBarYContainer)
            {
                if (!m_scrollBarYContainer->IsShown())
                    m_scrollBarYContainer->Show(true);
                else
                    changePosition = false;
            }
            else
            {
#if wxUSE_POPUPWIN
                m_scrollBarYContainer = new wxPopupWindow(m_scrolledWindow->GetParent(), wxNO_BORDER);
#else
                m_scrollBarYContainer = new wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
                    wxDefaultSize, wxNO_BORDER|wxFRAME_NO_TASKBAR);
#endif

                m_scrollBarY = new wxScrollBar(m_scrollBarYContainer, wxID_ANY,
                    wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
            }

            m_scrollBarY->SetRange(vertRange);
            m_scrollBarY->SetThumbPosition(vertThumbPosition);
            m_scrollBarY->SetThumbSize(vertThumbSize);

            if (changePosition)
            {
                wxSize scrollBarSize = wxSize(scrollBarWidth,
                    m_scrolledWindow->GetSize().y - scrollBarMargin*2 -
                      (useHorizScrollbar ? scrollBarWidth : 0));
                wxPoint scrollBarPosition = wxPoint(m_scrolledWindow->GetSize().x - scrollBarSize.x - scrollBarMargin, scrollBarMargin);

                // Convert to parent coordinates
                scrollBarPosition = m_scrolledWindow->ClientToScreen(scrollBarPosition);
                wxRect scrollBarRect(wxPoint(0, 0), scrollBarSize);

#if wxUSE_POPUPWIN
                wxRect frameRect(scrollBarPosition, scrollBarSize);
#else
                wxRect frameRect(scrollBarPosition, scrollBarSize);
#endif
                m_scrollBarY->SetSize(scrollBarRect);
                m_scrollBarYContainer->SetSize(frameRect);
            }

            m_scrollBarY->Update();
        }
    }
    else
    {
        if (m_scrollBarXContainer)
            m_scrollBarXContainer->Hide();
        if (m_scrollBarYContainer)
            m_scrollBarYContainer->Hide();
    }

    if (restoreCapture)
        m_scrolledWindow->CaptureMouse();
}

// Implements scrolling when a drag is detected
void wxMoTouchScrollHelperBase::OnTouchScroll(wxTouchScrollEvent& event)
{
    wxMoScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoScrolledWindow);
    if (scrolledWindow)
    {
        scrolledWindow->Scroll(event.GetPosition().x, event.GetPosition().y);
        scrolledWindow->Update();
    }
}

IMPLEMENT_CLASS(wxMoTouchScrollHelper, wxMoTouchScrollHelperBase)

bool wxMoTouchScrollHelper::GetScrollBarDetails(int orientation,
        int* range, int* thumbPosition, int* thumbSize)
{
    wxMoScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoScrolledWindow);
    if (scrolledWindow)
    {
        int currentPixelsX, currentPixelsY, currentUnitsX, currentUnitsY,
            unitsPerPixelX, unitsPerPixelY, virtualSizeX, virtualSizeY;
        scrolledWindow->GetViewStart(& currentUnitsX, & currentUnitsY);
        scrolledWindow->GetScrollPixelsPerUnit(& unitsPerPixelX, & unitsPerPixelY);
        scrolledWindow->GetVirtualSize(& virtualSizeX, & virtualSizeY);

        if (unitsPerPixelX > 0)
            currentPixelsX = currentUnitsX * unitsPerPixelX;
        else
            currentPixelsX = 0;

        if (unitsPerPixelY > 0)
            currentPixelsY = currentUnitsY * unitsPerPixelY;
        else
            currentPixelsY = 0;

        if (orientation == wxHORIZONTAL)
        {
            if (unitsPerPixelX == 0 || !scrolledWindow->GetShowHorizontalScrollIndicator())
                return false;

            * range = virtualSizeX/unitsPerPixelX;
            * thumbPosition = currentUnitsX;
            * thumbSize = m_scrolledWindow->GetSize().x/unitsPerPixelX;

            return true;
        }
        else if (orientation == wxVERTICAL)
        {
            if (unitsPerPixelY == 0 || !scrolledWindow->GetShowVerticalScrollIndicator())
                return false;

            * range = virtualSizeY/unitsPerPixelY;
            * thumbPosition = currentUnitsY;
            * thumbSize = m_scrolledWindow->GetSize().y/unitsPerPixelY;

            return true;
        }
    }
    return false;
}

// Does the scrolling for the specific scrolled window type.
bool wxMoTouchScrollHelper::DoScrolling(const wxPoint& lastPos, const wxPoint& newPos)
{
    wxMoScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoScrolledWindow);
    if (scrolledWindow)
    {
        int incX = lastPos.x - newPos.x;
        int incY = lastPos.y - newPos.y;

        if (scrolledWindow)
        {
            int currentPixelsX, currentPixelsY, currentUnitsX, currentUnitsY,
                unitsPerPixelX, unitsPerPixelY;
            scrolledWindow->GetViewStart(& currentUnitsX, & currentUnitsY);
            scrolledWindow->GetScrollPixelsPerUnit(& unitsPerPixelX, & unitsPerPixelY);

            if (unitsPerPixelX > 0)
                currentPixelsX = currentUnitsX * unitsPerPixelX;
            else
                currentPixelsX = 0;

            if (unitsPerPixelY > 0)
                currentPixelsY = currentUnitsY * unitsPerPixelY;
            else
                currentPixelsY = 0;

            int newScrollPosX = 0;
            int newScrollPosY = 0;
            if (unitsPerPixelX > 0)
                newScrollPosX = (currentPixelsX + incX)/unitsPerPixelX;
            if (unitsPerPixelY > 0)
                newScrollPosY = (currentPixelsY + incY)/unitsPerPixelY;

            if (newScrollPosX < 0)
                newScrollPosX = 0;
            if (newScrollPosY < 0)
                newScrollPosY = 0;

            if (incX != 0 || incY != 0)
            {
                // scrolledWindow->Scroll(newScrollPosX, newScrollPosY);

                // Since we're scrolling by means other than the scrollbars,
                // we must send a special scroll event.
                wxTouchScrollEvent event(wxEVT_TOUCH_SCROLL_DRAG);
                event.SetId(scrolledWindow->GetId());
                event.SetPosition(wxPoint(newScrollPosX, newScrollPosY));
                event.SetEventObject(scrolledWindow);

                scrolledWindow->GetEventHandler()->ProcessEvent(event);

                // scrolledWindow->Update();
            }
        }
    }

    return true;
}

