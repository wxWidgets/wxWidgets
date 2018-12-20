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
    wxFILTER_DIGITS = 0x8,
    wxFILTER_XDIGITS = 0x10,
    wxFILTER_ALPHANUMERIC = wxFILTER_ALPHA|wxFILTER_DIGITS|wxFILTER_XDIGITS,
    wxFILTER_NUMERIC = 0x20,
    wxFILTER_INCLUDE_LIST = 0x40,
    wxFILTER_INCLUDE_CHAR_LIST = 0x80,
    wxFILTER_EXCLUDE_LIST = 0x100,
    wxFILTER_EXCLUDE_CHAR_LIST = 0x200,
    wxFILTER_SPACE = 0x400
};

// ----------------------------------------------------------------------------
// wxTextValidatorBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextValidatorBase: public wxValidator
{
public:
    explicit wxTextValidatorBase(wxString* str, long style = wxFILTER_NONE);

    wxTextValidatorBase(const wxTextValidatorBase& val);

    virtual ~wxTextValidatorBase(){}

    bool Copy(const wxTextValidatorBase& val);

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
    inline long GetStyle() const { return m_style; }
    void SetStyle(long style){ DoSetStyle(style); }

    wxTextEntry *GetTextEntry();

    // strings & chars inclusions:
    // ---------------------------

    void SetCharIncludes(const wxString& chars);
    void AddCharIncludes(const wxString& chars);

    void SetIncludes(const wxArrayString& includes){ DoSetIncludes(includes); }
    void AddInclude(const wxString& include){ DoAddInclude(include); }
    const wxArrayString& GetIncludes() const { return m_includes; }

    // strings & chars exclusions:
    // ---------------------------

    void SetCharExcludes(const wxString& chars);
    void AddCharExcludes(const wxString& chars);

    void SetExcludes(const wxArrayString& excludes){ DoSetExcludes(excludes); }
    void AddExclude(const wxString& exclude){ DoAddExclude(exclude); }
    const wxArrayString& GetExcludes() const { return m_excludes; }

    bool HasFlag(wxTextValidatorStyle style) const
        { return (m_style & style) != 0; }

protected:

    bool IsCharIncluded(const wxUniChar& c) const
    {
        if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
            // The string of included chars is always kept at
            // the end of m_includes member.
            return (m_includes.Last().Find(c) != wxNOT_FOUND);

        return false;
    }

    bool IsCharExcluded(const wxUniChar& c) const
    {
        if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
            // The string of excluded chars is always kept at
            // the end of m_excludes member.
            return (m_excludes.Last().Find(c) != wxNOT_FOUND);

        return false;
    }

    // Helper used by IsIncluded() and IsExcluded() methods
    inline bool IsStringFound(const wxArrayString& arr,
                              const wxString& str,
                              const bool excludeLastString) const
    {
        const int idx = arr.Index(str);

        const bool found = idx != wxNOT_FOUND;

        if ( excludeLastString )
        {
            return found &&
                   (int)arr.GetCount() > idx+1; // the [inc|exc]lude char list
        }

        return found;
    }

    bool IsIncluded(const wxString& str) const
    {
        return IsStringFound(m_includes, str, HasFlag(wxFILTER_INCLUDE_CHAR_LIST));
    }

    bool IsExcluded(const wxString& str) const
    {
        return IsStringFound(m_excludes, str, HasFlag(wxFILTER_EXCLUDE_CHAR_LIST));
    }

    virtual void DoSetIncludes(const wxArrayString& strs);
    virtual void DoSetExcludes(const wxArrayString& strs);

    virtual void DoAddInclude(const wxString& str);
    virtual void DoAddExclude(const wxString& str);

    // returns the error message if the contents of 'str' are invalid
    wxString IsValid(const wxString& str) const;

    // returns false if the character is invalid
    virtual bool IsValid(const wxUniChar& c) const = 0;

    // Called by Validate() to do the actual validation
    virtual wxString DoValidate(const wxString& str) = 0;

    // For each derived class set m_style properly
    virtual void DoSetStyle(long style){ m_style = style; }

protected:
    long      m_style;
    wxString* m_string;

    wxArrayString m_includes;
    wxArrayString m_excludes;

private:
    wxDECLARE_NO_ASSIGN_CLASS(wxTextValidatorBase);
    wxDECLARE_ABSTRACT_CLASS(wxTextValidatorBase);
    wxDECLARE_EVENT_TABLE();
};

class WXDLLIMPEXP_CORE wxTextValidator: public wxTextValidatorBase
{
public:
    explicit wxTextValidator(wxString *str, long style = wxFILTER_NONE);
    wxTextValidator(long style = wxFILTER_NONE, wxString *str = NULL);

    wxTextValidator(const wxTextValidator& val);

    virtual ~wxTextValidator(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const wxOVERRIDE { return new wxTextValidator(*this); }
    bool Copy(const wxTextValidator& val);

protected:
    // returns false if the character is invalid
    virtual bool IsValid(const wxUniChar& c) const wxOVERRIDE;

    // Called by Validate() to do the actual validation
    virtual wxString DoValidate(const wxString& str) wxOVERRIDE;

    virtual void DoSetStyle(long style) wxOVERRIDE;

private:
    wxDECLARE_NO_ASSIGN_CLASS(wxTextValidator);
    wxDECLARE_DYNAMIC_CLASS(wxTextValidator);
};

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#endif // _WX_VALTEXT_H_
