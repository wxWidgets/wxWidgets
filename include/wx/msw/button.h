/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __BUTTONH__
#define __BUTTONH__

#ifdef __GNUG__
#pragma interface "button.h"
#endif

#include "wx/control.h"
#include "wx/gdicmn.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

// Pushbutton
class WXDLLEXPORT wxButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxButton)
 public:
  inline wxButton(void) {}
  inline wxButton(wxWindow *parent, const wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

  virtual void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  virtual void SetDefault(void);
  virtual void SetLabel(const wxString& label);
  virtual wxString GetLabel(void) const ;
  virtual void Command(wxCommandEvent& event);
  virtual bool MSWCommand(const WXUINT param, const WXWORD id);
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
};

#endif
    // __BUTTONH__
