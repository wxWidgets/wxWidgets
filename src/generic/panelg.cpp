/////////////////////////////////////////////////////////////////////////////
// Name:        panelg.cpp
// Purpose:     wxPanel
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:           wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "panelg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#endif

#include "wx/generic/panelg.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPanel, wxWindow)

BEGIN_EVENT_TABLE(wxPanel, wxWindow)
  EVT_SYS_COLOUR_CHANGED(wxPanel::OnSysColourChanged)
  EVT_SET_FOCUS(wxPanel::OnFocus)
  EVT_NAVIGATION_KEY(wxPanel::OnNavigationKey)
END_EVENT_TABLE()

#endif

wxPanel::wxPanel()
{
    m_lastFocus = 0;
}

bool wxPanel::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    m_lastFocus = 0;

    bool ret = wxWindow::Create(parent, id, pos, size, style, name);

    if ( ret )
    {
#ifndef __WXGTK__
        SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
        SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
#endif
    }

    return ret;
}

void wxPanel::InitDialog(void)
{
    wxInitDialogEvent event(GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

// Responds to colour changes, and passes event on to children.
void wxPanel::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

void wxPanel::OnNavigationKey( wxNavigationKeyEvent& event )
{
    // there is not much to do if we have only one child (or not at all)
    if (GetChildren().GetCount() < 2)
    {
        event.Skip();
        return;
    }

    // don't process these ones here
    if (event.IsWindowChange())
    {
        event.Skip();
        return;
    }

    wxWindow *winFocus = event.GetCurrentFocus();
    if (!winFocus)
        winFocus = wxWindow::FindFocus();

    if (!winFocus)
    {
        event.Skip();
        return;
    }

    wxWindowList::Node *start_node = GetChildren().Find( winFocus );
    if (!start_node)
        start_node = GetChildren().GetFirst();

    wxWindowList::Node *node = event.GetDirection() ? start_node->GetNext()
                                                    : start_node->GetPrevious();

    while (node != start_node)
    {
        if (!node)
        {
            node = event.GetDirection() ? GetChildren().GetFirst()
                                        : GetChildren().GetLast();

            continue;
        }

        wxWindow *child = node->GetData();

        if (child->AcceptsFocus())
        {
            // ok, event processed
            child->SetFocus();
            return;
        }

        node = event.GetDirection() ? node->GetNext() : node->GetPrevious();
    }

    // we cycled through all of our children and none of them wanted to accept
    // focus
    event.Skip();
}

void wxPanel::OnFocus(wxFocusEvent& event)
{
    if (m_lastFocus != 0)
    {
        wxWindow* child = FindWindow(m_lastFocus);
        if (child)
            child->SetFocus();
    }
    else
        event.Skip();
}
