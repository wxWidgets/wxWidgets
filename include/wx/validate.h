/////////////////////////////////////////////////////////////////////////////
// Name:        validate.h
// Purpose:     wxValidator class
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALIDATEH__
#define _WX_VALIDATEH__

#ifdef __GNUG__
#pragma interface "validate.h"
#endif

#include "wx/event.h"

class WXDLLEXPORT wxWindow;

/*
 A validator has up to three purposes:

 1) To validate the data in the window that's associated
    with the validator.
 2) To transfer data to and from the window.
 3) To filter input, using its role as a wxEvtHandler
    to intercept e.g. OnChar.

 Note that wxValidator and derived classes use reference counting.
 */

class WXDLLEXPORT wxValidator: public wxEvtHandler
{
DECLARE_DYNAMIC_CLASS(wxValidator)
public:
  wxValidator(void);
  ~wxValidator();

  // Make a clone of this validator (or return NULL) - currently necessary
  // if you're passing a reference to a validator.
  // Another possibility is to always pass a pointer to a new validator
  // (so the calling code can use a copy constructor of the relevant class).
  virtual wxValidator *Clone(void) const { return NULL; }
  inline bool Copy(const wxValidator& val) { m_validatorWindow = val.m_validatorWindow; return TRUE; }

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return FALSE; };

  // Called to transfer data to the window
  virtual bool TransferToWindow(void) { return FALSE; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow(void) { return FALSE; };

  // ACCESSORS
  inline wxWindow *GetWindow(void) const { return m_validatorWindow; }
  inline void SetWindow(wxWindow *win) { m_validatorWindow = win; }

protected:
    wxWindow *m_validatorWindow;
};

WXDLLEXPORT_DATA(extern const wxValidator) wxDefaultValidator;

#endif
    // _WX_VALIDATEH__
