/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

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
  inline wxRadioButton() {}
  inline wxRadioButton(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr);

  virtual void SetValue(bool val);
  virtual bool GetValue() const ;

  	virtual void MacHandleControlClick( ControlHandle control , SInt16 controlpart ); 
  void Command(wxCommandEvent& event);
  wxRadioButton *AddInCycle(wxRadioButton *cycle);
  inline wxRadioButton *NextInCycle() {return m_cycle;}

  protected:
  
  wxRadioButton *m_cycle;  
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
  inline wxBitmapRadioButton() { theButtonBitmap = NULL; }
  inline wxBitmapRadioButton(wxWindow *parent, wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxBitmapRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxBitmapRadioButtonNameStr);

  virtual void SetLabel(const wxBitmap *label);
  virtual void SetValue(bool val) ;
  virtual bool GetValue() const ;
};
#endif

#endif
    // _WX_RADIOBUT_H_
