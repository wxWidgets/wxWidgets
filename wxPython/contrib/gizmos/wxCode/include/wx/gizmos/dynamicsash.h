/////////////////////////////////////////////////////////////////////////////
// Name:        dynamicsash.h
// Purpose:     A window which can be dynamically split to an arbitrary depth
//              and later reunified through the user interface
// Author:      Matt Kimball
// Modified by:
// Created:     7/15/2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Matt Kimball
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DYNAMICSASH_H_
#define _WX_DYNAMICSASH_H_

#include "wx/gizmos/gizmos.h"

#if !wxUSE_MDI
#    error "wxUSE_MDI must be defined for gizmos to compile."
#endif /* !wxUSE_MDI */


/*

    wxDynamicSashWindow

    wxDynamicSashWindow widgets manages the way other widgets are viewed.
    When a wxDynamicSashWindow is first shown, it will contain one child
    view, a viewport for that child, and a pair of scrollbars to allow the
    user to navigate the child view area.  Next to each scrollbar is a small
    tab.  By clicking on either tab and dragging to the appropriate spot, a
    user can split the view area into two smaller views separated by a
    draggable sash.  Later, when the user wishes to reunify the two subviews,
    the user simply drags the sash to the side of the window.
    wxDynamicSashWindow will automatically reparent the appropriate child
    view back up the window hierarchy, and the wxDynamicSashWindow will have
    only one child view once again.

    As an application developer, you will simply create a wxDynamicSashWindow
    using either the Create() function or the more complex constructor
    provided below, and then create a view window whose parent is the
    wxDynamicSashWindow.  The child should respond to
    wxDynamicSashSplitEvents -- perhaps with an OnSplit() event handler -- by
    constructing a new view window whose parent is also the
    wxDynamicSashWindow.  That's it!  Now your users can dynamically split
    and reunify the view you provided.

    If you wish to handle the scrollbar events for your view, rather than
    allowing wxDynamicSashWindow to do it for you, things are a bit more
    complex.  (You might want to handle scrollbar events yourself, if,
    for instance, you wish to scroll a subwindow of the view you add to
    your wxDynamicSashWindow object, rather than scrolling the whole view.)
    In this case, you will need to construct your wxDynamicSashWindow without
    the wxDS_MANAGE_SCROLLBARS style and  you will need to use the
    GetHScrollBar() and GetVScrollBar() methods to retrieve the scrollbar
    controls and call SetEventHanler() on them to redirect the scrolling
    events whenever your window is reparented by wxDyanmicSashWindow.
    You will need to set the scrollbars' event handler at three times:

        *  When your view is created
        *  When your view receives a wxDynamicSashSplitEvent
        *  When your view receives a wxDynamicSashUnifyEvent

    See the dynsash_switch sample application for an example which does this.

*/


#include "wx/event.h"
#include "wx/window.h"

class WXDLLIMPEXP_CORE wxScrollBar;

// ----------------------------------------------------------------------------
// dynamic sash styles
// ----------------------------------------------------------------------------

/*
    wxDS_MANAGE_SCROLLBARS is a default style of wxDynamicSashWindow which
    will cause it to respond to scrollbar events for your application by
    automatically scrolling the child view.
*/
#define wxDS_MANAGE_SCROLLBARS  0x0010


/*
  wxDS_DRAG_CORNER style indicates that the views can also be resized by
  dragging the corner piece between the scrollbars, and which is reflected up
  to the frame if necessary.
*/
#define wxDS_DRAG_CORNER        0x0020

/*
    Default style for wxDynamicSashWindow.
 */
#define wxDS_DEFAULT            wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER

// ----------------------------------------------------------------------------
// dynamic sash events
// ----------------------------------------------------------------------------

extern WXDLLIMPEXP_GIZMOS const wxEventType wxEVT_DYNAMIC_SASH_SPLIT;
extern WXDLLIMPEXP_GIZMOS const wxEventType wxEVT_DYNAMIC_SASH_UNIFY;

#define EVT_DYNAMIC_SASH_SPLIT(id, func) \
    wx__DECLARE_EVT1(wxEVT_DYNAMIC_SASH_SPLIT, id, \
                        wxDynamicSashSplitEventHandler(func))

#define EVT_DYNAMIC_SASH_UNIFY(id, func) \
    wx__DECLARE_EVT1(wxEVT_DYNAMIC_SASH_UNIFY, id, \
                        wxDynamicSashUnifyEventHandler(func))


/*
    wxDynamicSashSplitEvents are sent to your view by wxDynamicSashWindow
    whenever your view is being split by the user.  It is your
    responsibility to handle this event by creating a new view window as
    a child of the wxDynamicSashWindow.  wxDynamicSashWindow will
    automatically reparent it to the proper place in its window hierarchy.
*/
class WXDLLIMPEXP_GIZMOS wxDynamicSashSplitEvent : public wxCommandEvent
{
public:
    wxDynamicSashSplitEvent();
    wxDynamicSashSplitEvent(const wxDynamicSashSplitEvent& event)
        : wxCommandEvent(event) { }
    wxDynamicSashSplitEvent(wxObject *target);

    virtual wxEvent* Clone() const { return new wxDynamicSashSplitEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS(wxDynamicSashSplitEvent)
};

/*
    wxDynamicSashUnifyEvents are sent to your view by wxDynamicSashWindow
    whenever the sash which splits your view and its sibling is being
    reunified such that your view is expanding to replace its sibling.
    You needn't do anything with this event if you are allowing
    wxDynamicSashWindow to manage your view's scrollbars, but it is useful
    if you are managing the scrollbars yourself so that you can keep
    the scrollbars' event handlers connected to your view's event handler
    class.
*/
class WXDLLIMPEXP_GIZMOS wxDynamicSashUnifyEvent : public wxCommandEvent
{
public:
    wxDynamicSashUnifyEvent();
    wxDynamicSashUnifyEvent(const wxDynamicSashUnifyEvent& event): wxCommandEvent(event) {}
    wxDynamicSashUnifyEvent(wxObject *target);

    virtual wxEvent* Clone() const { return new wxDynamicSashUnifyEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS(wxDynamicSashUnifyEvent)
};

typedef void (wxEvtHandler::*wxDynamicSashSplitEventFunction)(wxDynamicSashSplitEvent&);
typedef void (wxEvtHandler::*wxDynamicSashUnifyEventFunction)(wxDynamicSashUnifyEvent&);

#define wxDynamicSashSplitEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction) \
        wxStaticCastEvent(wxDynamicSashSplitEventFunction, &func)

#define wxDynamicSashUnifyEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction) \
        wxStaticCastEvent(wxDynamicSashUnifyEventFunction, &func)

#define wx__DECLARE_TREEEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_TREE_ ## evt, id, wxTreeEventHandler(fn))

// ----------------------------------------------------------------------------
// wxDynamicSashWindow itself
// ----------------------------------------------------------------------------

WXDLLIMPEXP_GIZMOS extern const wxChar* wxDynamicSashWindowNameStr;

class WXDLLIMPEXP_GIZMOS wxDynamicSashWindow : public wxWindow
{
public:
    wxDynamicSashWindow();
    wxDynamicSashWindow(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxDS_DEFAULT,
                        const wxString& name = wxDynamicSashWindowNameStr);
    virtual ~wxDynamicSashWindow();

    virtual bool Create(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDS_DEFAULT,
                        const wxString& name = wxDynamicSashWindowNameStr);
    virtual wxScrollBar *GetHScrollBar(const wxWindow *child) const;
    virtual wxScrollBar *GetVScrollBar(const wxWindow *child) const;

    /*  This is overloaded from wxWindowBase.  It's not here for you to
        call directly.  */
    virtual void AddChild(wxWindowBase *child);

private:
    class wxDynamicSashWindowImpl *m_impl;

    DECLARE_DYNAMIC_CLASS(wxDynamicSashWindow)
};

#endif // _WX_DYNAMICSASH_H_

