/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __STATTEXTH__
#define __STATTEXTH__

#ifdef __GNUG__
#pragma interface "stattext.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticTextNameStr;

class WXDLLEXPORT wxStaticText: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticText)
 public:
  inline wxStaticText(void) { }

  inline wxStaticText(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticTextNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticTextNameStr);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

  void SetLabel(const wxString&);

  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
};

#endif
    // __STATTEXTH__
