/////////////////////////////////////////////////////////////////////////////
// Name:        panelg.cpp
// Purpose:     wxPanel
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
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
#include "wx/settings.h"
#endif

#include "wx/generic/panelg.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPanel, wxWindow)

BEGIN_EVENT_TABLE(wxPanel, wxWindow)
  EVT_SYS_COLOUR_CHANGED(wxPanel::OnSysColourChanged)
  EVT_NAVIGATION_KEY(wxPanel::OnNavigationKey)
END_EVENT_TABLE()

#endif

wxPanel::wxPanel()
{
}

bool wxPanel::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
  bool ret = wxWindow::Create(parent, id, pos, size, style, name);

  if ( ret ) {
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

void wxPanel::SetFocus()
{
  SetFocusToNextChild();
}

// Responds to colour changes, and passes event on to children.
void wxPanel::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

void wxPanel::OnNavigationKey(wxNavigationKeyEvent& event)
{
  // don't process these ones here
  if ( event.IsWindowChange() ) {
    event.Skip();
    return;
  }

  // first of all, find the window which currently has the focus
  wxNode *node = GetChildren()->First();
  wxWindow *winFocus = event.GetCurrentFocus();
  
  // @@@ no FindFocus() in wxGTK
  #ifndef __WXGTK__
    if ( winFocus == NULL )
      winFocus = wxWindow::FindFocus();
  #endif
  
  while ( node != NULL ) {
    if ( node->Data() == winFocus )
      break;

    node = node->Next();
  }

  if ( !SetFocusToNextChild(node, event.GetDirection()) )
    event.Skip();
}

// set focus to the next child which accepts it (or first/last if node == NULL)
bool wxPanel::SetFocusToNextChild(wxNode *node, bool bForward)
{
  // @@ using typed list would be better...
  #define WIN(node) ((wxWindow *)(node->Data()))

  bool bFound = FALSE;  // have we found a window we will set focus to?

  wxList *children = GetChildren();
  if ( node == NULL ) {
    // we've never had focus before
    node = bForward ? children->First() : children->Last();
    if ( node == NULL ) {
      // no children
      return FALSE;
    }

    bFound = WIN(node)->AcceptsFocus();
  }
#if 0 // to restore when it will really work (now it's triggered all the time)
  else {
    // just to be sure it's the right one
    wxASSERT( WIN(node)->AcceptsFocus() );
  }
#endif // 0

  // find the next child which accepts focus
  bool bParentWantsIt = TRUE;
  while ( !bFound ) {
    node = bForward ? node->Next() : node->Previous();
    if ( node == NULL ) {
      if ( !bParentWantsIt ) {
        // we've already been here which means that we've done a whole
        // cycle without success - get out from the infinite loop
        return FALSE;
      }

      // ask parent if he doesn't want to advance focus to the next panel
      if ( GetParent() != NULL ) {
        wxNavigationKeyEvent event;
        event.SetDirection(bForward);
        event.SetWindowChange(FALSE);
        event.SetCurrentFocus(this);

        if ( GetParent()->ProcessEvent(event) )
          return TRUE;
      }

      // a sentinel to avoid infinite loops
      bParentWantsIt = FALSE;

      // wrap around
      node = bForward ? children->First() : children->Last();
    }

    bFound = WIN(node)->AcceptsFocus();
  }

  WIN(node)->SetFocus();

  #undef WIN

  return TRUE;
}
