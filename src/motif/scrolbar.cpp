/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "scrolbar.h"
#endif

#include "wx/scrolbar.h"

#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>

#include <wx/motif/private.h>

static void wxScrollBarCallback(Widget widget, XtPointer clientData,
                        XmScaleCallbackStruct *cbs);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)
#endif

// Scrollbar
bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    if (!parent)
        return FALSE;
    parent->AddChild(this);
    SetName(name);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    SetValidator(validator);
    
    m_windowStyle = style;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (width == -1)
    {
      if (style & wxHORIZONTAL)
        width = 140;
      else
        width = 12;
    }
    if (height == -1)
    {
      if (style & wxVERTICAL)
        height = 140;
      else
        height = 12;
    }

    Widget parentWidget = (Widget) parent->GetClientWidget();
    int direction = (style & wxHORIZONTAL) ? XmHORIZONTAL: XmVERTICAL;

    Widget scrollBarWidget = XtVaCreateManagedWidget("scrollBarWidget",
                  xmScrollBarWidgetClass,  parentWidget,
                  XmNorientation,      direction,
                  NULL);

    m_mainWidget = (Widget) scrollBarWidget;

    // This will duplicate other events
    //    XtAddCallback(scrollBarWidget, XmNvalueChangedCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNdragCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNdecrementCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNincrementCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNpageDecrementCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNpageIncrementCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNtoTopCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNtoBottomCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, x, y, width, height);
    ChangeBackgroundColour();

    return TRUE;
}

wxScrollBar::~wxScrollBar()
{
}

void wxScrollBar::SetThumbPosition(int pos)
{
	if (m_mainWidget)
	{
		XtVaSetValues ((Widget) m_mainWidget,
				XmNvalue, pos,
				NULL);
	}
}

int wxScrollBar::GetThumbPosition() const
{
	if (m_mainWidget)
	{
        int pos;
        XtVaGetValues((Widget) m_mainWidget,
            XmNvalue, &pos, NULL);
        return pos;
    }
    else
        return 0;
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh)
{
    m_viewSize = pageSize;
    m_pageSize = thumbSize;
    m_objectSize = range;

    if (range == 0)
      range = 1;
    if (thumbSize == 0)
      thumbSize = 1;

    XtVaSetValues((Widget) m_mainWidget,
         XmNvalue, position,
         XmNminimum, 0,
         XmNmaximum, range,
         XmNsliderSize, thumbSize,
         XmNpageIncrement, pageSize,
         NULL);
}

void wxScrollBar::Command(wxCommandEvent& event)
{
    SetThumbPosition(event.m_commandInt);
    ProcessCommand(event);
}

void wxScrollBar::ChangeFont(bool keepOriginalSize)
{
    // TODO
    // Do anything for a scrollbar? A font will never be seen.
}

void wxScrollBar::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxScrollBar::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

static void wxScrollBarCallback(Widget widget, XtPointer clientData,
                        XmScaleCallbackStruct *cbs)
{
    wxScrollBar *scrollBar = (wxScrollBar *)clientData;

    wxEventType eventType = wxEVT_NULL;
    switch (cbs->reason)
    {
        case XmCR_INCREMENT:
        {
            eventType = wxEVT_SCROLL_LINEDOWN;
            break;
        }
        case XmCR_DECREMENT:
        {
            eventType = wxEVT_SCROLL_LINEUP;
            break;
        }
        case XmCR_DRAG:
        {
            eventType = wxEVT_SCROLL_THUMBTRACK;
            break;
        }
        case XmCR_VALUE_CHANGED:
        {
            // TODO: Should this be intercepted too, or will it cause
            // duplicate events?
            eventType = wxEVT_SCROLL_THUMBTRACK;
            break;
        }
        case XmCR_PAGE_INCREMENT:
        {
            eventType = wxEVT_SCROLL_PAGEDOWN;
            break;
        }
        case XmCR_PAGE_DECREMENT:
        {
            eventType = wxEVT_SCROLL_PAGEUP;
            break;
        }
        case XmCR_TO_TOP:
        {
            eventType = wxEVT_SCROLL_TOP;
            break;
        }
        case XmCR_TO_BOTTOM:
        {
            eventType = wxEVT_SCROLL_BOTTOM;
            break;
        }
        default:
        {
            // Should never get here
            wxFAIL_MSG("Unknown scroll event.");
            break;
        }
    }

    wxScrollEvent event(eventType, scrollBar->GetId());
    event.SetEventObject(scrollBar);
    event.SetPosition(cbs->value);
    scrollBar->GetEventHandler()->ProcessEvent(event);
/*
    if (!scrollBar->inSetValue)
      scrollBar->ProcessCommand(event);
*/
}

