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
// Licence:     wxWindows licence
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

#if wxUSE_SASH
IMPLEMENT_CLASS(wxSashLayoutWindow, wxSashWindow)

BEGIN_EVENT_TABLE(wxSashLayoutWindow, wxSashWindow)
    EVT_CALCULATE_LAYOUT(wxSashLayoutWindow::OnCalculateLayout)
    EVT_QUERY_LAYOUT_INFO(wxSashLayoutWindow::OnQueryLayoutInfo)
END_EVENT_TABLE()

bool wxSashLayoutWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
{
    return wxSashWindow::Create(parent, id, pos, size, style, name);
}

void wxSashLayoutWindow::Init()
{
    m_orientation = wxLAYOUT_HORIZONTAL;
    m_alignment = wxLAYOUT_TOP;
}

// This is the function that wxLayoutAlgorithm calls to ascertain the window
// dimensions.
void wxSashLayoutWindow::OnQueryLayoutInfo(wxQueryLayoutInfoEvent& event)
{
  //    int flags = event.GetFlags();
    int requestedLength = event.GetRequestedLength();

    event.SetOrientation(m_orientation);
    event.SetAlignment(m_alignment);

    if (m_orientation == wxLAYOUT_HORIZONTAL)
        event.SetSize(wxSize(requestedLength, m_defaultSize.y));
    else
        event.SetSize(wxSize(m_defaultSize.x, requestedLength));
}

// Called by parent to allow window to take a bit out of the
// client rectangle, and size itself if not in wxLAYOUT_QUERY mode.

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
        wxSize sz = GetSize();
        wxPoint pos = GetPosition();
        SetSize(thisRect.x, thisRect.y, thisRect.width, thisRect.height);

        // Make sure the sash is erased when the window is resized
        if ((pos.x != thisRect.x || pos.y != thisRect.y || sz.x != thisRect.width || sz.y != thisRect.height) &&
            (GetSashVisible(wxSASH_TOP) || GetSashVisible(wxSASH_RIGHT) || GetSashVisible(wxSASH_BOTTOM) || GetSashVisible(wxSASH_LEFT)))
            Refresh(TRUE);

    }

    event.SetRect(clientSize);
}
#endif // wxUSE_SASH

/*
 * wxLayoutAlgorithm
 */

// Lays out windows for an MDI frame. The MDI client area gets what's left
// over.
bool wxLayoutAlgorithm::LayoutMDIFrame(wxMDIParentFrame* frame, wxRect* r)
{
    int cw, ch;
    frame->GetClientSize(& cw, & ch);

    wxRect rect(0, 0, cw, ch);
    if (r)
        rect = * r;

    wxCalculateLayoutEvent event;
    event.SetRect(rect);

    wxNode* node = frame->GetChildren().First();
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

// Layout algorithm for any window. mainWindow gets what's left over.
bool wxLayoutAlgorithm::LayoutWindow(wxWindow* parent, wxWindow* mainWindow)
{
    // Test if the parent is a sash window, and if so,
    // reduce the available space to allow space for any active edges.

    int leftMargin = 0, rightMargin = 0, topMargin = 0, bottomMargin = 0;
#if wxUSE_SASH
    if (parent->IsKindOf(CLASSINFO(wxSashWindow)))
    {
        wxSashWindow* sashWindow = (wxSashWindow*) parent;

        leftMargin = sashWindow->GetExtraBorderSize();
        rightMargin = sashWindow->GetExtraBorderSize();
        topMargin = sashWindow->GetExtraBorderSize();
        bottomMargin = sashWindow->GetExtraBorderSize();

        if (sashWindow->GetSashVisible(wxSASH_LEFT))
            leftMargin += sashWindow->GetDefaultBorderSize();
        if (sashWindow->GetSashVisible(wxSASH_RIGHT))
            rightMargin += sashWindow->GetDefaultBorderSize();
        if (sashWindow->GetSashVisible(wxSASH_TOP))
            topMargin += sashWindow->GetDefaultBorderSize();
        if (sashWindow->GetSashVisible(wxSASH_BOTTOM))
            bottomMargin += sashWindow->GetDefaultBorderSize();
    }
#endif // wxUSE_SASH

    int cw, ch;
    parent->GetClientSize(& cw, & ch);

    wxRect rect(leftMargin, topMargin, cw - leftMargin - rightMargin, ch - topMargin - bottomMargin);

    wxCalculateLayoutEvent event;
    event.SetRect(rect);

    // Find the last layout-aware window, so we can make it fill all remaining
    // space.
    wxWindow* lastAwareWindow = NULL;
    wxNode* node = parent->GetChildren().First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();

        if (win->IsShown())
        {
            wxCalculateLayoutEvent tempEvent(win->GetId());
            tempEvent.SetEventObject(win);
            tempEvent.SetFlags(wxLAYOUT_QUERY);
            tempEvent.SetRect(event.GetRect());
            if (win->GetEventHandler()->ProcessEvent(tempEvent))
                lastAwareWindow = win;
        }

        node = node->Next();
    }

    node = parent->GetChildren().First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();

        // If mainWindow is NULL and we're at the last window,
        // skip this, because we'll simply make it fit the remaining space.
        if ((win != mainWindow) && (mainWindow != NULL || win != lastAwareWindow))
        {
            event.SetId(win->GetId());
            event.SetEventObject(win);
            event.SetFlags(0); // ??

            win->GetEventHandler()->ProcessEvent(event);
        }

        node = node->Next();
    }

    rect = event.GetRect();

    if (mainWindow)
        mainWindow->SetSize(rect.x, rect.y, wxMax(0, rect.width), wxMax(0, rect.height));
    else if (lastAwareWindow)
    {
        // Fit the remaining space
        lastAwareWindow->SetSize(rect.x, rect.y, wxMax(0, rect.width), wxMax(0, rect.height));
    }

    return TRUE;
}

