/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     wxStaticBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBOX_H_
#define _WX_STATBOX_H_

#ifdef __GNUG__
#pragma interface "statbox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBoxNameStr;

// Group box
class WXDLLEXPORT wxStaticBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticBox)

 public:
  inline wxStaticBox(void) {}
  inline wxStaticBox(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBoxNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBoxNameStr);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};

  void OnEraseBackground(wxEraseEvent& event);

  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
    { wxWindow::SetSize(rect, sizeFlags); }
  void SetSize(const wxSize& size) { wxWindow::SetSize(size); }
  void SetLabel(const wxString& label);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATBOX_H_
