/////////////////////////////////////////////////////////////////////////////
// Name:        panelg.h
// Purpose:     wxPanel: similar to wxWindows but is coloured as for a dialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PANELH_G__
#define __PANELH_G__

#ifdef __GNUG__
#pragma interface "panelg.h"
#endif

#include "wx/window.h"

WXDLLEXPORT_DATA(extern const char*) wxPanelNameStr;

// Dialog boxes
class WXDLLEXPORT wxPanel: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxPanel)
public:

  wxPanel(void);

  // Old-style constructor
  inline wxPanel(wxWindow *parent,
           const int x = -1, const int y= -1, const int width = 500, const int height = 500,
           const long style = wxTAB_TRAVERSAL | wxNO_BORDER,
           const wxString& name = wxPanelNameStr)
  {
      Create(parent, -1, wxPoint(x, y), wxSize(width, height), style, name);
  }

  // Constructor
  inline wxPanel(wxWindow *parent, const wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxTAB_TRAVERSAL | wxNO_BORDER,
           const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, pos, size, style, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxTAB_TRAVERSAL | wxNO_BORDER,
           const wxString& name = wxPanelNameStr);

  void OnPaint(wxPaintEvent& event);

  // Sends an OnInitDialog event, which in turns transfers data to
  // to the dialog via validators.
  virtual void InitDialog(void);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

DECLARE_EVENT_TABLE()
};

#endif
    // __PANELH_G__
