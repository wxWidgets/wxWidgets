/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __RADIOBUTH__
#define __RADIOBUTH__

#ifdef __GNUG__
#pragma interface "radiobut.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxRadioButtonNameStr;

class WXDLLEXPORT wxRadioButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioButton)
 protected:
 public:
  inline wxRadioButton(void) {}
  inline wxRadioButton(wxWindow *parent, const wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr);

  virtual void SetLabel(const wxString& label);
  virtual void SetValue(const bool val);
  virtual bool GetValue(void) const ;

  void Command(wxCommandEvent& event);
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
};

// Not implemented
#if 0
class WXDLLEXPORT wxBitmap ;

WXDLLEXPORT_DATA(extern const char*) wxBitmapRadioButtonNameStr;

class WXDLLEXPORT wxBitmapRadioButton: public wxRadioButton
{
  DECLARE_DYNAMIC_CLASS(wxBitmapRadioButton)
 protected:
  wxBitmap *theButtonBitmap;
 public:
  inline wxBitmapRadioButton(void) { theButtonBitmap = NULL; }
  inline wxBitmapRadioButton(wxWindow *parent, const wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxBitmapRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxBitmapRadioButtonNameStr);

  virtual void SetLabel(const wxBitmap *label);
  virtual void SetValue(const bool val) ;
  virtual bool GetValue(void) const ;
};
#endif

#endif
    // __RADIOBUTH__
