/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.h
// Purpose:     wxCheckBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKBOX_H_
#define _WX_CHECKBOX_H_

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
  inline wxCheckBox() { }
  inline wxCheckBox(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr);
  virtual void SetValue(bool);
  virtual bool GetValue() const ;
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  virtual void SetLabel(const wxString& label);
  virtual void Command(wxCommandEvent& event);
private:
  void SetSize(int width, int height) {wxWindow::SetSize(width, height);}
};

class WXDLLEXPORT wxBitmapCheckBox: public wxCheckBox
{
  DECLARE_DYNAMIC_CLASS(wxBitmapCheckBox)

 public:
  int checkWidth ;
  int checkHeight ;

  inline wxBitmapCheckBox() { checkWidth = -1; checkHeight = -1; }
  inline wxBitmapCheckBox(wxWindow *parent, wxWindowID id, const wxBitmap *label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id, const wxBitmap *bitmap,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr);
  virtual void SetValue(bool);
  virtual bool GetValue() const ;
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  virtual void SetLabel(const wxBitmap& bitmap);
private:
  void SetSize(int width, int height) {wxWindow::SetSize(width, height);}
  void SetLabel(const wxString& label) {wxCheckBox::SetLabel(label);}
};
#endif
    // _WX_CHECKBOX_H_
