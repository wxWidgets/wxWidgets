/////////////////////////////////////////////////////////////////////////////
// Name:        valgen.h
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by:
// Created:     Jan 22 1999
// RCS-ID:
// Copyright:   (c) 1999 Kevin Smith
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALGENH__
#define _WX_VALGENH__

#ifdef __GNUG__
#pragma interface "valgen.h"
#endif

#include "wx/validate.h"

class WXDLLEXPORT wxGenericValidator: public wxValidator
{
public:
  wxGenericValidator(bool* val);
  wxGenericValidator(int* val);
  wxGenericValidator(wxString* val);
  wxGenericValidator(wxArrayInt* val);
  wxGenericValidator(const wxGenericValidator& copyFrom);

  ~wxGenericValidator();

  // Make a clone of this validator (or return NULL) - currently necessary
  // if you're passing a reference to a validator.
  // Another possibility is to always pass a pointer to a new validator
  // (so the calling code can use a copy constructor of the relevant class).
  virtual wxObject *Clone() const { return new wxGenericValidator(*this); }
  bool Copy(const wxGenericValidator& val);

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow * WXUNUSED(parent)) { return TRUE; }

  // Called to transfer data to the window
  virtual bool TransferToWindow();

  // Called to transfer data to the window
  virtual bool TransferFromWindow();

protected:
  void Initialize();

  bool*       m_pBool;
  int*        m_pInt;
  wxString*   m_pString;
  wxArrayInt* m_pArrayInt;
};

#endif    // _WX_VALGENH__
