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
public:
  wxPanel();

  // Old-style constructor (no default values for coordinates to avoid ambiguity
  // with the new one)
  inline wxPanel(wxWindow *parent,
                 int x, int y, int width, int height,
                 long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                 const wxString& name = wxPanelNameStr)
  {
      Create(parent, -1, wxPoint(x, y), wxSize(width, height), style, name);
  }

  // Constructor
  inline wxPanel(wxWindow *parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                 const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER,
              const wxString& name = wxPanelNameStr);

  // Sends an OnInitDialog event, which in turns transfers data to
  // to the dialog via validators.
  virtual void InitDialog();

  // overriden base class virtuals
  virtual void SetFocus();

  // callbacks
    // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);
    // Process a keyboard navigation message (Tab traversal)
  void OnNavigationKey(wxNavigationKeyEvent& event);

protected:
  bool SetFocusToNextChild(wxNode *nodeCurrent = (wxNode *) NULL, bool bForward = TRUE);

DECLARE_DYNAMIC_CLASS(wxPanel)
DECLARE_EVENT_TABLE()
};

#endif
    // __PANELH_G__
