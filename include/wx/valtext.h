/////////////////////////////////////////////////////////////////////////////
// Name:        valtext.h
// Purpose:     wxTextValidator class
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALTEXTH__
#define _WX_VALTEXTH__

#ifdef __GNUG__
#pragma interface "valtext.h"
#endif

#include "wx/validate.h"

#define wxFILTER_NONE           0x0000
#define wxFILTER_ASCII          0x0001
#define wxFILTER_ALPHA          0x0002
#define wxFILTER_ALPHANUMERIC   0x0004
#define wxFILTER_NUMERIC        0x0008
#define wxFILTER_INCLUDE_LIST   0x0010
#define wxFILTER_EXCLUDE_LIST   0x0020

class WXDLLEXPORT wxTextValidator: public wxValidator
{
DECLARE_DYNAMIC_CLASS(wxTextValidator)
public:
  wxTextValidator(long style = wxFILTER_NONE, wxString *val = (wxString *) NULL);
  wxTextValidator(const wxTextValidator& val);

  ~wxTextValidator();

  // Make a clone of this validator (or return NULL) - currently necessary
  // if you're passing a reference to a validator.
  // Another possibility is to always pass a pointer to a new validator
  // (so the calling code can use a copy constructor of the relevant class).
  virtual wxObject *Clone(void) const { return new wxTextValidator(*this); }
  bool Copy(const wxTextValidator& val);

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *parent);

  // Called to transfer data to the window
  virtual bool TransferToWindow(void);

  // Called to transfer data to the window
  virtual bool TransferFromWindow(void);

  // ACCESSORS
  inline long GetStyle(void) const { return m_validatorStyle; }
  inline void SetStyle(long style) { m_validatorStyle = style; }

  void SetIncludeList(const wxStringList& list);
  inline wxStringList& GetIncludeList(void) { return m_includeList; }

  void SetExcludeList(const wxStringList& list);
  inline wxStringList& GetExcludeList(void) { return m_excludeList; }

  // Filter keystrokes
  void OnChar(wxKeyEvent& event);

DECLARE_EVENT_TABLE()

protected:
    long            m_validatorStyle;
    wxString *      m_stringValue;
    wxStringList    m_includeList;
    wxStringList    m_excludeList;
};

#endif
