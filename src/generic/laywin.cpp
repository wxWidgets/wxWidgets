/////////////////////////////////////////////////////////////////////////////
// Name:        laywin.cpp
// Purpose:     Implements a simple layout algorithm, plus
//              wxSashLayoutWindow which is an example of a window with
//              layout-awareness (via event handlers). This is suited to
//              IDE-style window layout.
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "laywin.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/laywin.h"

IMPLEMENT_DYNAMIC_CLASS(wxQueryLayoutInfoEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxCalculateLayoutEvent, wxEvent)

IMPLEMENT_CLASS(wxSashLayoutWindow, wxSashWindow)

BEGIN_EVENT_TABLE(wxSashLayoutWindow, wxSashWindow)
    EVT_CALCULATE_LAYOUT(wxSashLayoutWindow::OnCalculateLayout)
    EVT_QUERY_LAYOUT_INFO(wxSashLayoutWindow::OnQueryLayoutInfo)
END_EVENT_TABLE()

wxSashLayoutWindow::wxSashLayoutWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name):
    wxSashWindow(parent, id, pos, size, style, name)
{
    m_orientation = wxLAYOUT_HORIZONTAL;
    m_alignment = wxLAYOUT_TOP;
}

// These are the functions that wxWin will call to ascertain the window
// dimensions.
void wxSashLayoutWindow::OnQueryLayoutInfo(wxQueryLayoutInfoEvent& event)
{
  //    int flags = event.GetFlags();
    int requestedLength = event.GetRequestedLength();

    // This code won't be in the final thing, it's just so we don't have to give it
    // real windows: mock up some dimensions.

    event.SetOrientation(m_orientation);
    event.SetAlignment(m_alignment);

    if (m_orientation == wxLAYOUT_HORIZONTAL)
        event.SetSize(wxSize(requestedLength, m_defaultSize.y));
    else
        event.SetSize(wxSize(m_defaultSize.x, requestedLength));
}

// Called by parent to allow window to take a bit out of the
// client rectangle, and size itself if not in wxLAYOUT_QUERY mode.
// Will eventually be an event.

void wxSashLayoutWindow::OnCalculateLayout(wxCalculateLayoutEvent& event)
{
    wxRect clientSize(event.GetRect());

    int flags = event.GetFlags();

    if (!IsShown())
        return;

    // Let's assume that all windows stretch the full extent of the window in
    // the direction of that window orientation. This will work for non-docking toolbars,
    // and the status bar. Note that the windows have to have been created in a certain
    // order to work, else you might get a left-aligned window going to the bottom
    // of the window, and the status bar appearing to the right of it. The
    // status bar would have to be created after or before the toolbar(s).

    wxRect thisRect;

    // Try to stretch
    int length = (GetOrientation() == wxLAYOUT_HORIZONTAL) ? clientSize.width : clientSize.height;
    wxLayoutOrientation orient = GetOrientation();

    // We assume that a window that says it's horizontal, wants to be stretched in that
    // direction. Is this distinction too fine? Do we assume that any horizontal
    // window needs to be stretched in that direction? Possibly.
    int whichDimension = (GetOrientation() == wxLAYOUT_HORIZONTAL) ? wxLAYOUT_LENGTH_X : wxLAYOUT_LENGTH_Y;

    wxQueryLayoutInfoEvent infoEvent(GetId());
    infoEvent.SetEventObject(this);
    infoEvent.SetRequestedLength(length);
    infoEvent.SetFlags(orient | whichDimension);

    if (!GetEventHandler()->ProcessEvent(infoEvent))
        return;

    wxSize sz = infoEvent.GetSize();

    if (sz.x == 0 && sz.y == 0) // Assume it's invisible
        return;

    // Now we know the size it wants to be. We wish to decide where to place it, i.e.
    // how it's aligned.
    switch (GetAlignment())
    {
        case wxLAYOUT_TOP:
        {
            thisRect.x = clientSize.x; thisRect.y = clientSize.y;
            thisRect.width = sz.x; thisRect.height = sz.y;
            clientSize.y += thisRect.height;
            clientSize.height -= thisRect.height;
            break;
        }
        case wxLAYOUT_LEFT:
        {
            thisRect.x = clientSize.x; thisRect.y = clientSize.y;
            thisRect.width = sz.x; thisRect.height = sz.y;
            clientSize.x += thisRect.width;
            clientSize.width -= thisRect.width;
            break;
        }
        case wxLAYOUT_RIGHT:
        {
            thisRect.x = clientSize.x + (clientSize.width - sz.x); thisRect.y = clientSize.y;
            thisRect.width = sz.x; thisRect.height = sz.y;
            clientSize.width -= thisRect.width;
            break;
        }
        case wxLAYOUT_BOTTOM:
        {
            thisRect.x = clientSize.x; thisRect.y = clientSize.y + (clientSize.height - sz.y);
            thisRect.width = sz.x; thisRect.height = sz.y;
            clientSize.height -= thisRect.height;
            break;
        }
        case wxLAYOUT_NONE:
        {
	    break;
        }

    }

    if ((flags & wxLAYOUT_QUERY) == 0)
    {
        // If not in query mode, resize the window.
        // TODO: add wxRect& form to wxWindow::SetSize
        SetSize(thisRect.x, thisRect.y, thisRect.width, thisRect.height);
    }

    event.SetRect(clientSize);
}

/*
 * wxLayoutAlgorithm
 */

// Lays out windows for an MDI frame. The MDI client area gets what's left
// over.
bool wxLayoutAlgorithm::LayoutMDIFrame(wxMDIParentFrame* frame)
{
    int cw, ch;
    frame->GetClientSize(& cw, & ch);

    wxRect rect(0, 0, cw, ch);

    wxCalculateLayoutEvent event;
    event.SetRect(rect);

    wxNode* node = frame->GetChildren()->First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();

        event.SetId(win->GetId());
        event.SetEventObject(win);
        event.SetFlags(0); // ??

        win->GetEventHandler()->ProcessEvent(event);

        node = node->Next();
    }

    wxWindow* clientWindow = frame->GetClientWindow();

    rect = event.GetRect();

    clientWindow->SetSize(rect.x, rect.y, rect.width, rect.height);

    return TRUE;
}

// Layout algorithm for normal frame. mainWindow gets what's left over.
bool wxLayoutAlgorithm::LayoutFrame(wxFrame* frame, wxWindow* mainWindow)
{
    int cw, ch;
    frame->GetClientSize(& cw, & ch);

    wxRect rect(0, 0, cw, ch);

    wxCalculateLayoutEvent event;
    event.SetRect(rect);

    wxNode* node = frame->GetChildren()->First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();

        event.SetId(win->GetId());
        event.SetEventObject(win);
        event.SetFlags(0); // ??

        win->GetEventHandler()->ProcessEvent(event);

        node = node->Next();
    }

    rect = event.GetRect();

    mainWindow->SetSize(rect.x, rect.y, rect.width, rect.height);

    return TRUE;
}

