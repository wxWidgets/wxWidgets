/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#ifdef __GNUG__
#pragma interface "combobox.h"
#endif

#include "wx/choice.h"

#if USE_COMBOBOX

WXDLLEXPORT_DATA(extern const char*) wxComboBoxNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Combobox item
class WXDLLEXPORT wxComboBox: public wxChoice
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
  inline wxComboBox(void) {}

  inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
  {
    Create(parent, id, value, pos, size, n, choices, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr);

  // List functions: see wxChoice
  
  // Text field functions
  virtual wxString GetValue(void) const ;
  virtual void SetValue(const wxString& value);

  // Clipboard operations
  virtual void Copy(void);
  virtual void Cut(void);
  virtual void Paste(void);
  virtual void SetInsertionPoint(long pos);
  virtual void SetInsertionPointEnd(void);
  virtual long GetInsertionPoint(void) const ;
  virtual long GetLastPosition(void) const ;
  virtual void Replace(long from, long to, const wxString& value);
  virtual void Remove(long from, long to);
  virtual void SetSelection(int n)
  {
    wxChoice::SetSelection(n);
  }
  virtual void SetSelection(long from, long to);
  virtual void SetEditable(bool editable);

  virtual bool MSWCommand(WXUINT param, WXWORD id);
};

#endif // USE_COMBOBOX
#endif
    // _WX_COMBOBOX_H_
