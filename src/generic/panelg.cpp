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
END_EVENT_TABLE()

#endif

wxPanel::wxPanel(void)
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetDefaultBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxPanel::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  bool ret = wxWindow::Create(parent, id, pos, size, style, name);

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetDefaultBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  return ret;
}

void wxPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// No: if you call the default procedure, it makes
	// the following painting code not work.
//	wxWindow::OnPaint(event);
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
    SetDefaultBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

