/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxRadioButtonNameStr;

class WXDLLEXPORT wxRadioButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioButton)
 protected:
 public:
  inline wxRadioButton() {}
  inline wxRadioButton(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxRadioButtonNameStr);

  virtual void SetLabel(const wxString& label);
  virtual void SetValue(bool val);
  virtual bool GetValue() const ;

  bool OS2Command(WXUINT param, WXWORD id);
  void Command(wxCommandEvent& event);
};

// Not implemented
class WXDLLEXPORT wxBitmap ;

WXDLLEXPORT_DATA(extern const char*) wxBitmapRadioButtonNameStr;

class WXDLLEXPORT wxBitmapRadioButton: public wxRadioButton
{
  DECLARE_DYNAMIC_CLASS(wxBitmapRadioButton)
 protected:
  wxBitmap *theButtonBitmap;
 public:
  inline wxBitmapRadioButton() { theButtonBitmap = NULL; }
  inline wxBitmapRadioButton(wxWindow *parent, wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxBitmapRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxBitmapRadioButtonNameStr);

  virtual void SetLabel(const wxBitmap *label);
  virtual void SetValue(bool val) ;
  virtual bool GetValue() const ;
private:
  // virtual function hiding suppression, do not use
  virtual void SetLabel(const wxString& label)
  { wxRadioButton::SetLabel(label); };
};

#endif
    // _WX_RADIOBUT_H_
