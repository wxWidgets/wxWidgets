/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.h
// Purpose:     wxCheckBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHECKBOXH__
#define __CHECKBOXH__

#ifdef __GNUG__
#pragma interface "checkbox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxCheckBoxNameStr;

// Checkbox item (single checkbox)
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxCheckBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxCheckBox)

 public:
  inline wxCheckBox(void) { }
  inline wxCheckBox(wxWindow *parent, const wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr);
  virtual void SetValue(const bool);
  virtual bool GetValue(void) const ;
  virtual void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  virtual bool MSWCommand(const WXUINT param, const WXWORD id);
  virtual void SetLabel(const wxString& label);
  virtual void Command(wxCommandEvent& event);
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
};

class WXDLLEXPORT wxBitmapCheckBox: public wxCheckBox
{
  DECLARE_DYNAMIC_CLASS(wxBitmapCheckBox)

 public:
  int checkWidth ;
  int checkHeight ;

  inline wxBitmapCheckBox(void) { checkWidth = -1; checkHeight = -1; }
  inline wxBitmapCheckBox(wxWindow *parent, const wxWindowID id, const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id, const wxBitmap *bitmap,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr);
  virtual void SetValue(const bool);
  virtual bool GetValue(void) const ;
  virtual void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  virtual void SetLabel(const wxBitmap *bitmap);
};
#endif
    // __CHECKBOXH__
