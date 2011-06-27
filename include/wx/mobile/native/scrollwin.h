/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/scrollwin.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_SCROLLWIN_H_
#define _WX_MOBILE_NATIVE_SCROLLWIN_H_

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/scrolwin.h"
#include "wx/timer.h"
#include "wx/scrolbar.h"

#include "wx/mobile/native/panel.h"

class WXDLLEXPORT wxMoScrolledWindow;
class WXDLLEXPORT wxTouchScrollEvent;

/*
 Extra interception for dealing with touch-based scrolling: base class
 */

class wxMoTouchScrollHelperBase: public wxEvtHandler
{
    DECLARE_CLASS(wxMoTouchScrollHelperBase)
    DECLARE_EVENT_TABLE()
public:
    // The current touch status
    enum TouchStatus { TouchStatusNone, TouchStatusHaveTouchDown, TouchStatusScrolling,
        TouchStatusForwardToTargetButDetectScrolling, TouchStatusForwardToTarget } ;
    
    wxMoTouchScrollHelperBase(wxWindow* scrolledWindow);
    ~wxMoTouchScrollHelperBase();
    
    void Init();
    
    virtual bool MovedSignificantly(const wxPoint& pt1, const wxPoint& pt2);
    
    virtual void ShowTransientScrollbars(bool show);
    
    /// Provides the details to be used for the transient scrollbars,
    /// depending on the type of scrolled window.
    /// Returns false if this orientation not supported.
    virtual bool GetScrollBarDetails(int orientation,
                                     int* range, int* thumbPosition, int* thumbSize) = 0;
    
    /// Does the scrolling for the specific scrolled window type.
    virtual bool DoScrolling(const wxPoint& lastPos, const wxPoint& newPos) = 0;
    
    /// Override to do processing on start pos
    virtual void OnStartScrolling(const wxPoint& WXUNUSED(startPos)) {};
    
    void OnTimerNotify(wxTimerEvent& event);
    void OnCaptureChanged(wxMouseCaptureChangedEvent& event);
    void OnIdle(wxIdleEvent& event);
    
    // Implements scrolling when a drag is detected
    void OnTouchScroll(wxTouchScrollEvent& event);
    
    /// Set the delay before assuming touch is for content, in milliseconds.
    void SetContentProcessingDelay(int delay) { m_contentProcessingDelay = delay; }
    
    /// Get the delay before assuming touch is for content, in milliseconds.
    int GetContentProcessingDelay() const { return m_contentProcessingDelay; }
    
    // Process mouse events specially
    virtual bool ProcessEvent(wxEvent& event);
    
    // Process mouse events. If returns false, should continue to process event normally.
    bool DoMouseEvent(wxMouseEvent& event);
    
    /// Cancels scroll overriding, at least in the generic implementation.
    void CancelScrolling();
    
protected:
    wxWindow*           m_scrolledWindow;
    wxPoint             m_startPos;
    wxPoint             m_lastPos;
    wxTimer             m_timer;
    wxMouseEvent        m_originalTouchEvent;
    TouchStatus         m_touchStatus;
    bool                m_inMouseEvent;
    int                 m_contentProcessingDelay;
    wxScrollBar*        m_scrollBarX;
    wxScrollBar*        m_scrollBarY;
    wxWindow*           m_scrollBarXContainer;
    wxWindow*           m_scrollBarYContainer;
};

/*
 A scroll helper for wxMoScrolledWindow
 */

class wxMoTouchScrollHelper: public wxMoTouchScrollHelperBase
{
    DECLARE_CLASS(wxMoTouchScrollHelper)
public:
    wxMoTouchScrollHelper(wxWindow* scrolledWindow): wxMoTouchScrollHelperBase(scrolledWindow) {}
    
    void Init();
    
    /// Returns the details to be used for the transient scrollbars.
    /// Returns false if this orientation not supported.
    virtual bool GetScrollBarDetails(int orientation,
                                     int* range, int* thumbPosition, int* thumbSize);
    
    /// Does the scrolling for the specific scrolled window type.
    virtual bool DoScrolling(const wxPoint& lastPos, const wxPoint& newPos);
    
protected:
};

/**
 @class wxMoScrolledWindow
 
 A container that scrolls its content.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoScrolledWindow : public wxMoPanel,
public wxScrollHelper
{
public:
    /// Default constructor.
    wxMoScrolledWindow();
    
    /// Constructor.
    wxMoScrolledWindow(wxWindow *parent,
                       wxWindowID winid = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxScrolledWindowStyle,
                       const wxString& name = wxPanelNameStr)
    : wxScrollHelper(this), m_touchScrollHelper(this)
    {
        Init();
        
        Create(parent, winid, pos, size, style, name);
    }
    
    virtual ~wxMoScrolledWindow();
    
    void Init();
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxPanelNameStr);
    
    void SetScrollbars(int pixelsPerUnitX,
        int pixelsPerUnitY,
        int noUnitsX,
        int noUnitsY,
        int xPos = 0,
        int yPos = 0,
        bool noRefresh = false);
    
    /// Sets the flag determining whether the horizontal scroll indicator is displayed.
    void SetShowHorizontalScrollIndicator(bool show) { m_showHorizontalScrollIndicator = show; }
    
    /// Gets the flag determining whether the horizontal scroll indicator is displayed.
    bool GetShowHorizontalScrollIndicator() const { return m_showHorizontalScrollIndicator; }
    
    /// Sets the flag determining whether the vertical scroll indicator is displayed.
    void SetShowVerticalScrollIndicator(bool show) { m_showVerticalScrollIndicator = show; }
    
    /// Gets the flag determining whether the vertical scroll indicator is displayed.
    bool GetShowVerticalScrollIndicator() const { return m_showVerticalScrollIndicator; }
    
    /// Set the delay before assuming touch is for content, in milliseconds.
    void SetContentProcessingDelay(int delay) { m_touchScrollHelper.SetContentProcessingDelay(delay); }
    
    /// Get the delay before assuming touch is for content, in milliseconds.
    int GetContentProcessingDelay() const { return m_touchScrollHelper.GetContentProcessingDelay(); }
    
    /// Forward mouse event from another window to this window (useful within simulator
    /// when the scrolled window isn't able to capture events sent to a content window)
    void ForwardMouseEvent(wxWindow* from, wxMouseEvent& event);
    
    /// Cancels scroll overriding, at least in the generic implementation.
    void CancelScrolling() { m_touchScrollHelper.CancelScrolling(); }
    
    WX_FORWARD_TO_SCROLL_HELPER()
    
protected:
    // this is needed for wxEVT_PAINT processing hack described in
    // wxScrollHelperEvtHandler::ProcessEvent()
    void OnPaint(wxPaintEvent& event);
    
    wxMoTouchScrollHelper   m_touchScrollHelper;
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoScrolledWindow)
    DECLARE_EVENT_TABLE()
    
    bool    m_showHorizontalScrollIndicator;
    bool    m_showVerticalScrollIndicator;
};

/**
 @class wxTouchScrollEvent
 
 A scroll event generated when dragging a window.
 This is needed since conventional scrollbar events operate only
 in one orientation at a time, whereas a touch scroll event
 can move it in either direction.
 
 @category{wxMobile}
 */
class WXDLLEXPORT wxTouchScrollEvent: public wxEvent
{
public:
    wxTouchScrollEvent (wxEventType commandType = wxEVT_NULL,
                        const wxPoint& pos = wxDefaultPosition)
    {
        m_eventType = commandType;
        m_pos = pos;
        m_allow = true;
    }
    wxTouchScrollEvent(const wxTouchScrollEvent& event) : wxEvent(event)
    {
        m_pos = event.m_pos;
        m_allow = event.m_allow;
    }
    
    wxPoint GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }
    
    /// Veto the operation
    void Veto() { m_allow = false; }
    
    /// Allow the operation
    void Allow() { m_allow = true; }
    
    /// Is the operation allowed?
    bool IsAllowed() const { return m_allow; }
    
    virtual wxEvent *Clone() const { return new wxTouchScrollEvent(*this); }
    
protected:
    wxPoint m_pos;
    bool    m_allow;
    
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxTouchScrollEvent )
};

typedef void (wxEvtHandler::*wxTouchScrollEventFunction)(wxTouchScrollEvent&);

DECLARE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_DRAG, 450)
DECLARE_EVENT_TYPE(wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES, 451)

#define wxTouchScrollEventHandler(func) \
(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTouchScrollEventFunction, &func)

#define EVT_TOUCH_SCROLL_DRAG(func) wx__DECLARE_EVT0(wxEVT_TOUCH_SCROLL_DRAG, wxTouchScrollEventHandler(func))
#define EVT_TOUCH_SCROLL_CANCEL_TOUCHES(func) wx__DECLARE_EVT0(wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES, wxTouchScrollEventHandler(func))

#endif
    // _WX_MOBILE_NATIVE_SCROLLWIN_H_
