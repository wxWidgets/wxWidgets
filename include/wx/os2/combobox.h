/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     wxComboBox class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/choice.h"

#if wxUSE_COMBOBOX

WXDLLEXPORT_DATA(extern const char*) wxComboBoxNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Combobox item
class WXDLLEXPORT wxComboBox: public wxChoice
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
  inline wxComboBox() {}

  inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
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
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxComboBoxNameStr);

    // List functions: see wxChoice

    wxString GetValue() const { return GetLabel(); }
    virtual void SetValue(const wxString& value);

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(int n) { wxChoice::SetSelection(n); }
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    virtual bool OS2Command(WXUINT param, WXWORD id);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
};

#endif // wxUSE_COMBOBOX
#endif
    // _WX_COMBOBOX_H_
