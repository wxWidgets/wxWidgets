/////////////////////////////////////////////////////////////////////////////
// Name:        panelg.cpp
// Purpose:     wxPanel
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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

IMPLEMENT_DYNAMIC_CLASS(wxPanel, wxWindow)

BEGIN_EVENT_TABLE(wxPanel, wxWindow)
  EVT_SYS_COLOUR_CHANGED(wxPanel::OnSysColourChanged)
  EVT_SET_FOCUS(wxPanel::OnFocus)
  EVT_NAVIGATION_KEY(wxPanel::OnNavigationKey)
  EVT_SIZE(wxPanel::OnSize)
END_EVENT_TABLE()


void wxPanel::Init()
{
    m_winLastFocused = (wxWindow *)NULL;
    m_btnDefault = (wxButton *)NULL;
}

bool wxPanel::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    bool ret = wxWindow::Create(parent, id, pos, size, style, name);

    if ( ret )
    {
        SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
        SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
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

    // Did the event emitter tell us where the last focus was?
    // wxGTK does this in wxWindow, but wxMSW does not. It is
    // also done in wxPanel if the event is propagated up.
        wxWindow *winFocus = event.GetCurrentFocus();

    // Do we know where the focus was ourselves, then?
    if (!winFocus)
        winFocus = m_winLastFocused;

    if (!winFocus)
        winFocus = wxWindow::FindFocus();

    if (!winFocus)
    {
        event.Skip();
        return;
    }

    wxWindowList::Node *start_node = GetChildren().Find( winFocus );
    if ( !start_node )
        start_node = GetChildren().Find( m_winLastFocused );
    if ( !start_node )
        start_node = GetChildren().GetFirst();

    wxWindowList::Node *node = event.GetDirection() ? start_node->GetNext()
                                                    : start_node->GetPrevious();

    while ( node != start_node )
    {
        // Have we come to the last or first item on the panel?
        if ( !node )
        {
            // Check if our (may be grand) parent is another panel: if this is
            // the case, they will know what to do with this navigation key and
            // so give them the chance to process it instead of looping inside
            // this panel (normally, the focus will go to the next/previous
            // item after this panel in the parent panel).
            wxWindow *focussed_child_of_parent = this;
            for ( wxWindow *parent = GetParent(); parent; parent = parent->GetParent() )
            {
                // we don't want to tab into a different dialog or frame
                if ( focussed_child_of_parent->IsTopLevel() )
                    break;

                // is the parent a panel?
                wxPanel *panel = wxDynamicCast(parent, wxPanel);
                if (panel)
                {
                    event.SetCurrentFocus( focussed_child_of_parent );
                    if (parent->GetEventHandler()->ProcessEvent( event ))
                        return;
                }

                focussed_child_of_parent = parent;
            }

            // no, we are not inside another panel so process this ourself
            node = event.GetDirection() ? GetChildren().GetFirst()
                                        : GetChildren().GetLast();

            continue;
        }

        wxWindow *child = node->GetData();

        if ( child->AcceptsFocus() )
        {
            m_winLastFocused = child;  // should be redundant, but it is not
            child->SetFocus();
            return;
        }

        node = event.GetDirection() ? node->GetNext() : node->GetPrevious();
    }

    // we cycled through all of our children and none of them wanted to accept
    // focus
    event.Skip();
}


void wxPanel::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
        Layout();
#endif
}

void wxPanel::SetFocus()
{
    wxLogTrace(_T("focus"), _T("SetFocus on wxPanel 0x%08x."), GetHandle());

    // If the panel gets the focus *by way of getting it set directly*
    // we move the focus to the first window that can get it.

    // VZ: no, we set the focus to the last window too. I don't understand why
    //     should we make this distinction: if an app wants to set focus to
    //     some precise control, it may always do it directly, but if we don't
    //     use m_winLastFocused here, the focus won't be set correctly after a
    //     notebook page change nor after frame activation under MSW (it calls
    //     SetFocus too)
    //
    //     If you still want to have old behaviour for wxGTK, edit the
    //     following line
#if 0 // def __WXGTK__
    m_winLastFocused = (wxWindow *)NULL;
#endif // 0

    if ( !SetFocusToChild() )
    {
        wxWindow::SetFocus();
    }
}

void wxPanel::OnFocus(wxFocusEvent& event)
{
    wxLogTrace(_T("focus"), _T("OnFocus on wxPanel 0x%08x."), GetHandle());

    // If the panel gets the focus *by way of getting clicked on*
    // we move the focus to either the last window that had the 
    // focus or the first one that can get it.
    (void)SetFocusToChild();

    event.Skip();
}

bool wxPanel::SetFocusToChild()
{
    if ( m_winLastFocused )
    {
        // It might happen that the window got reparented or no longer accepts
        // the focus.
        if ( (m_winLastFocused->GetParent() == this) &&
             m_winLastFocused->AcceptsFocus() )
        {
            wxLogTrace(_T("focus"),
                       _T("SetFocusToChild() => last child (0x%08x)."),
                       m_winLastFocused->GetHandle());

            m_winLastFocused->SetFocus();
            return TRUE;
        }
        else
        {
            // it doesn't count as such any more
            m_winLastFocused = (wxWindow *)NULL;
        }
    }

    // set the focus to the first child who wants it
    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();
        if ( child->AcceptsFocus() )
        {
            wxLogTrace(_T("focus"),
                       _T("SetFocusToChild() => first child (0x%08x)."),
                       child->GetHandle());

            m_winLastFocused = child;  // should be redundant, but it is not
            child->SetFocus();
            return TRUE;
        }

        node = node->GetNext();
    }

    return FALSE;
}
