/////////////////////////////////////////////////////////////////////////////
// Name:        valtext.h
// Purpose:     wxTextValidator class
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALTEXT_H_
#define _WX_VALTEXT_H_

#include "wx/defs.h"

#if wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

class WXDLLIMPEXP_FWD_CORE wxTextEntry;

#include "wx/validate.h"

enum wxTextValidatorStyle
{
    wxFILTER_NONE,
    wxFILTER_ASCII,
    wxFILTER_ALPHA,
    wxFILTER_ALPHANUMERIC,
    wxFILTER_NUMERIC,
    wxFILTER_INCLUDE_LIST,
    wxFILTER_EXCLUDE_LIST,
    wxFILTER_INCLUDE_CHAR_LIST,
    wxFILTER_EXCLUDE_CHAR_LIST
};

class WXDLLIMPEXP_CORE wxTextValidator: public wxValidator
{
public:
    wxTextValidator(wxTextValidatorStyle style = wxFILTER_NONE, wxString *val = NULL);
#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED_CONSTRUCTOR( wxTextValidator(long style, wxString *val) );
#endif
    wxTextValidator(const wxTextValidator& val);

    virtual ~wxTextValidator(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const { return new wxTextValidator(*this); }
    bool Copy(const wxTextValidator& val);

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow *parent);

    // Called to transfer data to the window
    virtual bool TransferToWindow();

    // Called to transfer data from the window
    virtual bool TransferFromWindow();

    // ACCESSORS
    inline wxTextValidatorStyle GetStyle() const { return m_validatorStyle; }
    inline void SetStyle(wxTextValidatorStyle style) { m_validatorStyle = style; }
#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( void SetStyle(long style) );
#endif

    wxTextEntry *GetTextEntry();

    void SetIncludes(const wxArrayString& includes) { m_includes = includes; }
    inline wxArrayString& GetIncludes() { return m_includes; }

    void SetExcludes(const wxArrayString& excludes) { m_excludes = excludes; }
    inline wxArrayString& GetExcludes() { return m_excludes; }

    bool IsInCharIncludes(const wxString& val);
    bool IsNotInCharExcludes(const wxString& val);

    // Filter keystrokes
    void OnChar(wxKeyEvent& event);

protected:
    wxTextValidatorStyle m_validatorStyle;
    wxString *           m_stringValue;
    wxArrayString        m_includes;
    wxArrayString        m_excludes;

private:
    DECLARE_DYNAMIC_CLASS(wxTextValidator)
    DECLARE_EVENT_TABLE()
// Cannot use
//  DECLARE_NO_COPY_CLASS(wxTextValidator)
// because copy constructor is explicitly declared above;
// but no copy assignment operator is defined, so declare
// it private to prevent the compiler from defining it:
    wxTextValidator& operator=(const wxTextValidator&);
};

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#endif // _WX_VALTEXT_H_
