/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valtext.h
// Purpose:     wxTextValidator class
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     29/01/98
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
    wxFILTER_NONE = 0x0,
    wxFILTER_EMPTY = 0x1,
    wxFILTER_ASCII = 0x2,
    wxFILTER_ALPHA = 0x4,
    wxFILTER_ALPHANUMERIC = 0x8,
    wxFILTER_DIGITS = 0x10,
    wxFILTER_NUMERIC = 0x20,
    wxFILTER_INCLUDE_LIST = 0x40,
    wxFILTER_INCLUDE_CHAR_LIST = 0x80,
    wxFILTER_EXCLUDE_LIST = 0x100,
    wxFILTER_EXCLUDE_CHAR_LIST = 0x200,
    wxFILTER_XDIGITS = 0x400,
    wxFILTER_SPACE = 0x800
};

// ----------------------------------------------------------------------------
// wxTextValidator
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextValidator: public wxValidator
{
public:
    wxTextValidator(long style = wxFILTER_NONE, wxString *val = NULL);
    wxTextValidator(const wxTextValidator& val);

    virtual ~wxTextValidator(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const wxOVERRIDE { return new wxTextValidator(*this); }
    bool Copy(const wxTextValidator& val);

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow *parent) wxOVERRIDE;

    // Called to transfer data to the window
    virtual bool TransferToWindow() wxOVERRIDE;

    // Called to transfer data from the window
    virtual bool TransferFromWindow() wxOVERRIDE;

    // Filter keystrokes
    void OnChar(wxKeyEvent& event);

    // ACCESSORS
    inline long GetStyle() const { return m_validatorStyle; }
    void SetStyle(long style);

    wxTextEntry *GetTextEntry();

    // strings & chars inclusions:
    // ---------------------------

    void SetCharIncludes(const wxString& chars);
    void AddCharIncludes(const wxString& chars);

    void SetIncludes(const wxArrayString& includes) { m_includes = includes; }
    void AddInclude(const wxString& include)
    {
        if ( !include.empty() )
            m_includes.Add(include);
    }

    const wxArrayString& GetIncludes() const { return m_includes; }
    wxString GetCharIncludes() const { return m_charIncludes; }

    // strings & chars exclusions:
    // ---------------------------

    void SetCharExcludes(const wxString& chars);
    void AddCharExcludes(const wxString& chars);

    void SetExcludes(const wxArrayString& excludes) { m_excludes = excludes; }
    void AddExclude(const wxString& exclude)
    {
        if ( !exclude.empty() )
            m_excludes.Add(exclude);
    }

    const wxArrayString& GetExcludes() const { return m_excludes; }
    wxString GetCharExcludes() const { return m_charExcludes; }

    bool HasFlag(wxTextValidatorStyle style) const
        { return (m_validatorStyle & style) != 0; }

protected:

    bool IsCharIncluded(const wxUniChar& c) const
    {
        return m_charIncludes.Find(c) != wxNOT_FOUND;
    }

    bool IsCharExcluded(const wxUniChar& c) const
    {
        return m_charExcludes.Find(c) != wxNOT_FOUND;
    }

    bool IsIncluded(const wxString& str) const
    {
        return m_includes.empty() || 
                m_includes.Index(str) != wxNOT_FOUND;
    }

    bool IsExcluded(const wxString& str) const
    {
        return m_excludes.Index(str) != wxNOT_FOUND;
    }

    // returns the error message if the contents of 'str' are invalid
    virtual wxString IsValid(const wxString& str) const;

    // returns false if the character is invalid
    bool IsValidChar(const wxUniChar& c) const;

protected:
    long                 m_validatorStyle;
    wxString*            m_stringValue;
    wxString             m_charIncludes;
    wxString             m_charExcludes;
    wxArrayString        m_includes;
    wxArrayString        m_excludes;

private:
    wxDECLARE_NO_ASSIGN_CLASS(wxTextValidator);
    wxDECLARE_DYNAMIC_CLASS(wxTextValidator);
    wxDECLARE_EVENT_TABLE();
};

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#endif // _WX_VALTEXT_H_
