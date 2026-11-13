/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/evtloop.cpp
// Purpose:     wxGUIEventLoop and Event Manager events dispatching
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/toplevel.h"
    #include "wx/utils.h"
#endif

#include "wx/unix/private/timer.h"

#include "wx/retro68/private/qd.h"
#include "wx/retro68/private/bgtask.h"
#include "wx/retro68/dc.h"

#define TRACE_EVENTS "events"

// Implemented in src/retro68/menu.cpp.
extern bool wxMacHandleMenuSelection(long menuResult);

// ===========================================================================
// mouse events
// ===========================================================================

namespace
{

// The window under the mouse pointer.
wxWindow* gs_windowUnderMouse = nullptr;

// Last known mouse position, in global coordinates.
Point gs_lastMousePos = { -32768, -32768 };

// Is the mouse button currently pressed (from wx point of view)?
bool gs_buttonDown = false;

// Is the pressed button considered to be the right one?
bool gs_isRightButton = false;

// Information about the last click, used to detect double clicks.
unsigned long gs_lastClickTime = 0;
Point gs_lastClickPos = { 0, 0 };
wxWindow* gs_lastClickWindow = nullptr;

// Region used for WaitNextEvent() to get notified when the mouse moves.
RgnHandle gs_mouseRgn = nullptr;

// Return the deepest window containing the given point in global coordinates
// or null if it's not over any of our windows.
wxWindow* FindWindowAtGlobal(Point where)
{
    WindowRef win;
    if ( FindWindow(where, &win) != inContent )
        return nullptr;

    wxNonOwnedWindow* const tlw = wxMacFindWindow(win);
    if ( !tlw || !tlw->IsShown() )
        return nullptr;

    wxPoint pt(where.h, where.v);
    tlw->ScreenToClient(&pt.x, &pt.y);

    return tlw->FindDeepestChildAt(pt + tlw->GetClientAreaOrigin());
}

void InitMouseEvent(wxMouseEvent& event,
                    wxWindow* win,
                    Point where,
                    short modifiers,
                    unsigned long when)
{
    wxPoint pt(where.h, where.v);
    win->ScreenToClient(&pt.x, &pt.y);

    event.m_x = pt.x;
    event.m_y = pt.y;

    event.m_shiftDown = (modifiers & shiftKey) != 0;
    event.m_controlDown = (modifiers & cmdKey) != 0;
    event.m_altDown = (modifiers & optionKey) != 0;
    event.m_metaDown = (modifiers & wxMAC_CONTROL_KEY) != 0;

    event.m_leftDown = gs_buttonDown && !gs_isRightButton;
    event.m_rightDown = gs_buttonDown && gs_isRightButton;

    event.SetTimestamp(wxMacTicksToMillis(when));
    event.SetEventObject(win);
    event.SetId(win->GetId());
}

bool SendMouseEvent(wxWindow* win,
                    wxEventType type,
                    Point where,
                    short modifiers,
                    unsigned long when)
{
    wxMouseEvent event(type);
    InitMouseEvent(event, win, where, modifiers, when);

    return win->HandleWindowEvent(event);
}

void ChangeWindowUnderMouse(wxWindow* win,
                            Point where,
                            short modifiers,
                            unsigned long when)
{
    if ( win == gs_windowUnderMouse )
        return;

    wxWindow* const winOld = gs_windowUnderMouse;
    gs_windowUnderMouse = win;

    if ( winOld && !winOld->IsBeingDeleted() )
        SendMouseEvent(winOld, wxEVT_LEAVE_WINDOW, where, modifiers, when);

    if ( win )
    {
        SendMouseEvent(win, wxEVT_ENTER_WINDOW, where, modifiers, when);

        // The window could have been destroyed by the event handler.
        if ( gs_windowUnderMouse == win )
            win->UpdateCursor();
    }
    else if ( !wxIsBusy() )
    {
        wxSTANDARD_CURSOR->MacInstall();
    }
}

void UpdateMouseRgn(Point where)
{
    if ( !gs_mouseRgn )
        gs_mouseRgn = NewRgn();

    SetRectRgn(gs_mouseRgn, where.h, where.v, where.h + 1, where.v + 1);
}

void HandleMouseDown(const EventRecord& ev)
{
    gs_isRightButton = (ev.modifiers & wxMAC_CONTROL_KEY) != 0;

    // If the mouse is captured, all clicks go to the capturing window, even
    // if they're outside it (this is used e.g. to close popup windows).
    wxWindow* const capture = wxWindow::GetCapture();
    if ( capture )
    {
        gs_buttonDown = true;
        SendMouseEvent(capture,
                       gs_isRightButton ? wxEVT_RIGHT_DOWN : wxEVT_LEFT_DOWN,
                       ev.where, ev.modifiers, ev.when);
        return;
    }

    WindowRef win;
    const short part = FindWindow(ev.where, &win);

    wxNonOwnedWindow* const tlw = wxMacFindWindow(win);

    switch ( part )
    {
        case inMenuBar:
            {
                const long res = MenuSelect(ev.where);
                wxMacHandleMenuSelection(res);
                HiliteMenu(0);
            }
            break;

#if !TARGET_API_MAC_CARBON
        case inSysWindow:
            SystemClick(const_cast<EventRecord*>(&ev), win);
            break;
#endif // !TARGET_API_MAC_CARBON

        case inDrag:
            if ( tlw && !tlw->IsEnabled() )
            {
                // This window is disabled because a modal dialog is shown.
                SysBeep(1);
                break;
            }

            {
                Rect limit = wxMacGetScreenBounds();
                limit.top += wxMacGetMenuBarHeight();
                InsetRect(&limit, 4, 4);

                wxRetro68DCImpl::InvalidateCachedPortState();

                DragWindow(win, ev.where, &limit);

                if ( tlw )
                    tlw->HandleUserMove();
            }
            break;

        case inGrow:
            if ( !tlw || !tlw->IsEnabled() )
            {
                SysBeep(1);
                break;
            }

            if ( win != FrontWindow() )
            {
                SelectWindow(win);
                break;
            }

            {
                const wxSize minSize = tlw->GetMinClientSize();
                const wxSize maxSize = tlw->GetMaxClientSize();

                Rect limits;
                limits.left = wxMacClampCoord(minSize.x > 0 ? minSize.x : 64);
                limits.top = wxMacClampCoord(minSize.y > 0 ? minSize.y : 64);
                limits.right = wxMacClampCoord(maxSize.x > 0 ? maxSize.x : 32767);
                limits.bottom = wxMacClampCoord(maxSize.y > 0 ? maxSize.y : 32767);

                wxRetro68DCImpl::InvalidateCachedPortState();

                const long newSize = GrowWindow(win, ev.where, &limits);
                if ( newSize )
                {
                    tlw->SetClientSize(newSize & 0xffff, (newSize >> 16) & 0xffff);
                    tlw->HandleUserResize();
                }
            }
            break;

        case inGoAway:
            wxRetro68DCImpl::InvalidateCachedPortState();

            if ( TrackGoAway(win, ev.where) && tlw )
                tlw->Close();
            break;

        case inZoomIn:
        case inZoomOut:
            wxRetro68DCImpl::InvalidateCachedPortState();

            if ( TrackBox(win, ev.where, part) && tlw )
            {
                wxTopLevelWindow* const frame = wxDynamicCast(tlw, wxTopLevelWindow);
                if ( frame )
                    frame->Maximize(!frame->IsMaximized());
            }
            break;

        case inContent:
            if ( !tlw )
                break;

            if ( !tlw->IsEnabled() )
            {
                SysBeep(1);
                break;
            }

            if ( win != FrontWindow() && tlw->ShouldActivateOnClick() )
            {
                // As is customary under Mac OS, the first click only activates
                // the window and is not passed to it.
                SelectWindow(win);
                break;
            }

            {
                wxWindow* const target = FindWindowAtGlobal(ev.where);
                if ( !target || !target->IsEnabled() )
                    break;

                gs_buttonDown = true;

                // Check for the double click.
                wxEventType type;
                if ( target == gs_lastClickWindow &&
                        ev.when - gs_lastClickTime <= (unsigned long)wxMacGetDblTime() &&
                            abs(ev.where.h - gs_lastClickPos.h) <= 4 &&
                                abs(ev.where.v - gs_lastClickPos.v) <= 4 )
                {
                    type = gs_isRightButton ? wxEVT_RIGHT_DCLICK
                                            : wxEVT_LEFT_DCLICK;

                    // Don't generate double click for the next click.
                    gs_lastClickWindow = nullptr;
                }
                else
                {
                    type = gs_isRightButton ? wxEVT_RIGHT_DOWN
                                            : wxEVT_LEFT_DOWN;

                    gs_lastClickWindow = target;
                    gs_lastClickTime = ev.when;
                    gs_lastClickPos = ev.where;
                }

                const bool processed =
                    SendMouseEvent(target, type, ev.where, ev.modifiers, ev.when);

                // Generate the context menu event for Control-click, as this
                // is the standard way to show the context menus under Mac OS.
                if ( gs_isRightButton && !processed &&
                        type == wxEVT_RIGHT_DOWN &&
                            gs_lastClickWindow == target )
                {
                    wxContextMenuEvent
                        evtCtx(wxEVT_CONTEXT_MENU,
                               target->GetId(),
                               wxPoint(ev.where.h, ev.where.v));
                    evtCtx.SetEventObject(target);
                    target->HandleWindowEvent(evtCtx);
                }
            }
            break;

        default:
            break;
    }
}

void HandleMouseUp(const EventRecord& ev)
{
    if ( !gs_buttonDown )
    {
        // This can happen if the click was used for activating the window or
        // dragging it, ignore it.
        return;
    }

    gs_buttonDown = false;

    wxWindow* target = wxWindow::GetCapture();
    if ( !target )
        target = FindWindowAtGlobal(ev.where);

    if ( target )
    {
        SendMouseEvent(target,
                       gs_isRightButton ? wxEVT_RIGHT_UP : wxEVT_LEFT_UP,
                       ev.where, ev.modifiers, ev.when);
    }
}

void HandleKey(const EventRecord& ev, bool isKeyUp)
{
    if ( !isKeyUp && (ev.modifiers & cmdKey) )
    {
        // Give the native menu bar a chance to handle the shortcut first.
        const long res = MenuKey(ev.message & charCodeMask);
        if ( (res >> 16) & 0xffff )
        {
            wxMacHandleMenuSelection(res);
            HiliteMenu(0);
            return;
        }
    }

    wxNonOwnedWindow* const front = wxMacFindWindow(FrontWindow());

    wxWindow* focus = wxWindow::FindFocus();
    if ( !focus || focus->GetTLW() != front )
        focus = front;

    if ( focus )
        focus->HandleKeyEvent(ev, isKeyUp);
}

} // anonymous namespace

wxWindow *wxRetro68GetWindowUnderMouse()
{
    return gs_windowUnderMouse;
}

void wxRetro68ForgetWindow(wxWindow* win)
{
    if ( gs_windowUnderMouse == win )
        gs_windowUnderMouse = nullptr;

    if ( gs_lastClickWindow == win )
        gs_lastClickWindow = nullptr;
}

void wxMacCheckMouse()
{
    // Get the current mouse position and modifiers state.
    EventRecord ev;
    EventAvail(0, &ev);

    if ( ev.where.h == gs_lastMousePos.h && ev.where.v == gs_lastMousePos.v )
        return;

    gs_lastMousePos = ev.where;
    UpdateMouseRgn(ev.where);

    wxWindow* const under = FindWindowAtGlobal(ev.where);
    ChangeWindowUnderMouse(under, ev.where, ev.modifiers, ev.when);

    wxWindow* target = wxWindow::GetCapture();
    if ( !target )
        target = under;

    if ( target )
        SendMouseEvent(target, wxEVT_MOTION, ev.where, ev.modifiers, ev.when);
}

// ===========================================================================
// events dispatching
// ===========================================================================

void wxMacHandleEvent(EventRecord& ev)
{
    switch ( ev.what )
    {
        case nullEvent:
            wxMacCheckMouse();
            break;

        case mouseDown:
            wxMacCheckMouse();
            HandleMouseDown(ev);
            break;

        case mouseUp:
            wxMacCheckMouse();
            HandleMouseUp(ev);
            break;

        case keyDown:
        case autoKey:
            HandleKey(ev, false);
            break;

        case keyUp:
            HandleKey(ev, true);
            break;

        case updateEvt:
            {
                const WindowRef win = reinterpret_cast<WindowRef>(ev.message);
                wxNonOwnedWindow* const tlw = wxMacFindWindow(win);
                if ( tlw )
                {
                    tlw->HandleUpdateEvent();
                }
                else
                {
                    // We must still call BeginUpdate() and EndUpdate() to
                    // avoid getting more update events for this window.
                    wxMacPortSaver saver;
                    wxMacSetWindowPort(wxMacGetWindowGrafPtr(win));
                    BeginUpdate(win);
                    EndUpdate(win);
                }
            }
            break;

        case activateEvt:
            {
                wxNonOwnedWindow* const tlw =
                    wxMacFindWindow(reinterpret_cast<WindowRef>(ev.message));
                if ( tlw )
                    tlw->HandleActivateEvent((ev.modifiers & activeFlag) != 0);
            }
            break;

        case osEvt:
            switch ( (ev.message >> 24) & 0xff )
            {
                case mouseMovedMessage:
                    wxMacCheckMouse();
                    break;

                case wxMAC_SUSPEND_RESUME_MSG:
                    {
                        const bool resume = (ev.message & wxMAC_RESUME_FLAG) != 0;

                        if ( wxTheApp )
                        {
                            wxTheApp->MacSetInForeground(resume);
                            wxTheApp->SetActive(resume, nullptr);
                        }

                        // We don't get activate events for the front window
                        // when switching applications, so generate them.
                        wxNonOwnedWindow* const front =
                            wxMacFindWindow(FrontWindow());
                        if ( front )
                            front->HandleActivateEvent(resume);

                        if ( resume )
                            wxSTANDARD_CURSOR->MacInstall();
                    }
                    break;
            }
            break;

        case kHighLevelEvent:
            AEProcessAppleEvent(&ev);
            break;

        default:
            break;
    }
}

// ===========================================================================
// background task
// ===========================================================================

namespace
{

wxRetro68BackgroundTask gs_backgroundTask = nullptr;

} // anonymous namespace

void wxRetro68SetBackgroundTask(wxRetro68BackgroundTask task)
{
    gs_backgroundTask = task;
}

void wxRetro68RunBackgroundTask()
{
    if ( gs_backgroundTask )
        gs_backgroundTask();
}

// ===========================================================================
// wxGUIEventLoop implementation
// ===========================================================================

namespace
{

// Convert milliseconds to ticks, rounding up.
long MillisToTicks(long ms)
{
    return (ms * 60 + 999) / 1000;
}

} // anonymous namespace

bool wxGUIEventLoop::Pending() const
{
    EventRecord ev;
    if ( EventAvail(everyEvent, &ev) )
        return true;

    // Expired timers also count as pending events.
    wxUsecClock_t remaining;
    if ( wxTimerScheduler::Get().GetNext(&remaining) && remaining <= 0 )
        return true;

    return false;
}

bool wxGUIEventLoop::Dispatch()
{
    DoDispatchTimeout(-1);

    return !m_shouldExit;
}

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    if ( !DoDispatchTimeout(static_cast<long>(timeout)) )
        return -1;

    return m_shouldExit ? 0 : 1;
}

bool wxGUIEventLoop::DoDispatchTimeout(long timeout)
{
    const unsigned long start = TickCount();

    for ( ;; )
    {
        wxRetro68RunBackgroundTask();

        // Process the expired timers first.
        if ( wxTimerScheduler::Get().NotifyExpired() )
            return true;

        // Compute how long can we sleep: not long at all if we need to run
        // the background task regularly.
        long sleepTicks = gs_backgroundTask ? 1 : 60;

        wxUsecClock_t remaining;
        if ( wxTimerScheduler::Get().GetNext(&remaining) )
        {
            const long ticksTimer = MillisToTicks((remaining / 1000).ToLong());
            if ( ticksTimer < sleepTicks )
                sleepTicks = ticksTimer;
        }

        if ( timeout >= 0 )
        {
            const long elapsed = TickCount() - start;
            const long ticksLeft = MillisToTicks(timeout) - elapsed;
            if ( ticksLeft < sleepTicks )
                sleepTicks = ticksLeft;
        }

        if ( sleepTicks < 0 )
            sleepTicks = 0;

        // Toolbox functions called while waiting for the events (e.g. from the
        // desk accessories or the system) expect the screen to be current.
        wxMacRestoreScreenPort();
        wxRetro68DCImpl::InvalidateCachedPortState();

        // Ensure that we get notified about mouse movements.
        if ( !gs_mouseRgn )
            UpdateMouseRgn(gs_lastMousePos);

        EventRecord ev;
        if ( WaitNextEvent(everyEvent, &ev, sleepTicks, gs_mouseRgn) )
        {
            wxMacHandleEvent(ev);
            return true;
        }

        // Null event: check if the mouse moved, as we may have missed it.
        const Point posOld = gs_lastMousePos;
        wxMacCheckMouse();
        if ( posOld.h != gs_lastMousePos.h || posOld.v != gs_lastMousePos.v )
            return true;

        if ( m_shouldExit || (wxTheApp && wxTheApp->HasPendingEvents()) )
            return true;

        if ( timeout >= 0 &&
                static_cast<long>(TickCount() - start) >= MillisToTicks(timeout) )
            return false;
    }
}

void wxGUIEventLoop::WakeUp()
{
    // Post a dummy application event to wake up WaitNextEvent().
    wxMacPostEvent(app1Evt, 0);
}

void wxGUIEventLoop::DoYieldFor(long eventsToProcess)
{
    // Only process user input events if asked to do it.
    short mask = updateMask | activMask | highLevelEventMask;
    if ( eventsToProcess & wxEVT_CATEGORY_USER_INPUT )
        mask = everyEvent;

    wxRetro68RunBackgroundTask();

    EventRecord ev;
    while ( WaitNextEvent(mask, &ev, 0, nullptr) )
    {
        if ( ev.what == nullEvent )
            break;

        wxMacHandleEvent(ev);
        wxRetro68RunBackgroundTask();
    }

    if ( eventsToProcess & wxEVT_CATEGORY_TIMER )
        wxTimerScheduler::Get().NotifyExpired();

    wxEventLoopBase::DoYieldFor(eventsToProcess);
}
