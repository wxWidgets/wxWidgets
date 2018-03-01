/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valtext.h
// Purpose:     wxTextValidator and wxRegexTextValidator classes
// Author:      Julian Smart, Francesco Montorsi et al
// Modified by: Ali Kettab
// Created:     2018-02-26
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALTEXT_H_
#define _WX_VALTEXT_H_

#include <wx/defs.h>

#if wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

class WXDLLIMPEXP_FWD_CORE wxTextEntry;
class WXDLLIMPEXP_FWD_CORE wxTextValidatorBase;

#include "wx/validate.h"
#include "wx/regex.h"
#include "wx/sharedptr.h"
#include "wx/intl.h"

// TODO:
//  - 

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
    wxFILTER_SPACE = 0x400
};

namespace wxPrivate
{

static inline bool wxIsNumeric(const wxUniChar& c)
{
    // Allow for "," (French) as well as "." -- in future we should
    // use wxSystemSettings or other to do better localisation
    if ((!wxIsdigit(c)) &&
        (c != '.') && (c != ',') && (c != 'e') &&
        (c != 'E') && (c != '+') && (c != '-'))
        return false;

    return true;
}

template<long flag, bool flagOn>
struct wxFilterChar;

template<long flag>
struct wxFilterChar<flag, false>
{
    static inline bool IsValid(const wxUniChar&)
    {
        return true;
    }
};

template<>
struct wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST, false>
{
    static inline bool IsValid(const wxUniChar&, const wxTextValidatorBase*)
    {
        return false;
    }
};

template<>
struct WXDLLIMPEXP_CORE wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST, true>
{
    static bool IsValid(const wxUniChar&, const wxTextValidatorBase*);
};

template<>
struct wxFilterChar<wxFILTER_EXCLUDE_CHAR_LIST, false>
{
    static inline bool IsValid(const wxUniChar&, const wxTextValidatorBase*)
    {
        return true;
    }
};

template<>
struct WXDLLIMPEXP_CORE wxFilterChar<wxFILTER_EXCLUDE_CHAR_LIST, true>
{
    static bool IsValid(const wxUniChar&, const wxTextValidatorBase*);
};

template<>
struct wxFilterChar<wxFILTER_ASCII, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return c.IsAscii();
    }
};

template<>
struct wxFilterChar<wxFILTER_DIGITS, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return wxIsdigit(c);
    }
};

template<>
struct wxFilterChar<wxFILTER_ALPHA, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return wxIsalpha(c);
    }
};

template<>
struct wxFilterChar<wxFILTER_ALPHANUMERIC, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return wxIsalnum(c);
    }
};

template<>
struct wxFilterChar<wxFILTER_NUMERIC, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return wxIsNumeric(c);
    }
};

template<>
struct wxFilterChar<wxFILTER_SPACE, true>
{
    static inline bool IsValid(const wxUniChar& c)
    {
        return wxIsspace(c);
    }
};

template<>
struct wxFilterChar<wxFILTER_SPACE, false>
{
    static inline bool IsValid(const wxUniChar&)
    {
        return false;
    }
};


// The purpose of AlnumFlag is to beg you to use:
// -----------------------------------------------
// 1. template<wxFILTER_ALPHANUMERIC|...>
//
// instead of these (redundants) constructs:
// ----------------------------
// 2. template<wxFILTER_ALPHA|wxFILTER_DIGITS|...>
// 3. template<wxFILTER_ALPHANUMERIC|wxFILTER_ALPHA|...>
// 4. template<wxFILTER_ALPHANUMERIC|wxFILTER_DIGITS|...>
// 5. template<wxFILTER_ALPHANUMERIC|wxFILTER_ALPHA|wxFILTER_DIGITS|...>
//
template<long Flags>
struct AlnumFlag
{
    enum
    {
        mask1 = wxFILTER_ALPHA|wxFILTER_DIGITS,
        mask2 = wxFILTER_ALPHANUMERIC|wxFILTER_ALPHA,
        mask3 = wxFILTER_ALPHANUMERIC|wxFILTER_DIGITS,
    };

    static const bool normalized = 
        ((Flags & (mask1))!=(mask1)) &&
        ((Flags & (mask2))!=(mask2)) &&
        ((Flags & (mask3))!=(mask3));
};

} // namespace wxPrivate

//
class WXDLLIMPEXP_CORE wxTextValidatorBase: public wxValidator
{
    // Need to call IsCharIncluded().
    friend class wxPrivate::wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST, true>;
    // Need to call IsCharExcluded().
    friend class wxPrivate::wxFilterChar<wxFILTER_EXCLUDE_CHAR_LIST, true>;

public:
    wxTextValidatorBase(wxString* str, long style = wxFILTER_NONE);
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

    void SetCharIncludes(const wxString& chars);
    void AddCharIncludes(const wxString& chars);

    void SetIncludes(const wxArrayString& includes){ DoSetIncludes(includes); }
    void AddInclude(const wxString& include){ DoAddInclude(include); }
    const wxArrayString& GetIncludes() const { return m_includes; }

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
            // We always keep the included characters at the end of m_includes
            return (m_includes.Last().Find(c) != wxNOT_FOUND);

        return false;
    }

    bool IsCharExcluded(const wxUniChar& c) const
    {
        if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
            // We always keep the excluded characters at the end of m_excludes
            return (m_excludes.Last().Find(c) != wxNOT_FOUND);

        return false;
    }

    // Helper used by IsIncluded and IsExcluded
    inline bool IsStrFound(const wxArrayString& arr, 
                           const wxString& str, wxTextValidatorStyle style) const
    {
        const int idx = arr.Index(str);

        if ( HasFlag(style) )
        {
            return idx != wxNOT_FOUND &&
                   idx+1 != (int)arr.GetCount(); // the [inc|exc]lude char list
        }

        return idx != wxNOT_FOUND;        
    }

    bool IsIncluded(const wxString& str) const
    {
        return IsStrFound(m_includes, str, wxFILTER_INCLUDE_CHAR_LIST);
    }

    bool IsExcluded(const wxString& str) const
    {
        return IsStrFound(m_excludes, str, wxFILTER_EXCLUDE_CHAR_LIST);
    }

    virtual void DoSetIncludes(const wxArrayString& includes);
    virtual void DoSetExcludes(const wxArrayString& excludes);
    virtual void DoAddInclude(const wxString& include);
    virtual void DoAddExclude(const wxString& exclude);

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

//
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


#if wxUSE_REGEX

template<long Flags>
class WXDLLIMPEXP_CORE wxRegexTextValidator : public wxTextValidatorBase
{
public:
    // str param not defaulted to NULL to force client code to do so explicitly
    // to avoid silly bugs (e.g. TransferToWindow() and TransferFromWindow()
    // would seem to be doing nothing!).
    explicit wxRegexTextValidator(wxString* str,
                        const wxString& pattern = wxEmptyString,
                        const wxString& intent = wxEmptyString)
        : wxTextValidatorBase(str, Flags), m_regex(new wxRegEx)
    {
        if ( !pattern.empty() )
        {
            wxCHECK_RET(m_regex->Compile(pattern, wxRE_ADVANCED),
                        _("Invalid pattern passed to wxTextValidatorBase() !"));

            m_intent = !intent.empty() ? intent : _("pattern");
        }
    }

    wxRegexTextValidator(wxString* str, wxSharedPtr<wxRegEx> regex, const wxString& intent)
        : wxTextValidatorBase(str, Flags), m_regex(new wxRegEx)
    {
        if ( regex->IsValid() )
        {
            m_regex = regex;
            m_intent = !intent.empty() ? intent : _("pattern");
        }
    }

    ~wxRegexTextValidator()
    {
#if wxDEBUG_LEVEL
        wxCOMPILE_TIME_ASSERT(wxPrivate::AlnumFlag<Flags>::normalized,
                              please_sanitize_the_wxFILTER_ALPHANUMERIC_flag);
#endif // wxDEBUG_LEVEL
    }

    virtual wxObject *Clone() const wxOVERRIDE { return new wxRegexTextValidator(*this); }

    bool Copy(const wxRegexTextValidator& val)
    {
        wxTextValidatorBase::Copy(val);

        m_regex = val.m_regex;
        m_intent = val.m_intent;

        return true;
    }

    virtual wxString DoValidate(const wxString& str) wxOVERRIDE
    {
        if ( HasFlag(wxFILTER_EMPTY) && str.empty() )
            return _("Required information entry is empty.");
        if ( HasFlag(wxFILTER_EXCLUDE_LIST) && IsExcluded(str) )
            return wxString::Format(_("'%s' is one of the invalid strings"), str);

        do
        {
            if ( HasFlag(wxFILTER_INCLUDE_LIST) && IsIncluded(str) )
                break; // str is one of the included strings.
        
            if ( m_regex->IsValid() )
            {
                if ( m_regex->Matches(str) )                
                    break; // str does match regex pattern.
                return wxString::Format(_("'%s' doesn't match %s"), str, m_intent);
            }

            if ( HasFlag(wxFILTER_INCLUDE_LIST) )
                return wxString::Format(_("'%s' is not one of the valid strings"), str);

        //  we haven't set wxFILTER_INCLUDE_LIST nor we have a regex to match against.

        } while ( 0 ); // don't touch the zero!

        // str shouldn't contain invalid chars.
        wxString errormsg = wxTextValidatorBase::IsValid(str);

        if ( !errormsg.empty() )
        {
            // NB: this format string should always contain exactly one '%s'
            wxString buf;
            buf.Printf(errormsg, str);
            return buf;
        }
    
        return wxEmptyString;
    }

private:
// Truth table for wxFILTER_*:
// ---------------------------
// Is set? | SPACE |  | EXC_CHARS |  ASCII  |  DIGITS |  ALPHA  |  ALNUM  |  NUMERIC  | INC_CHARS
//------------------  ---------------------------------------------------------------------------
//   Yes   |   ?   || |     ?     &&   ?    &&   ?    &&    ?   &&   ?    &&    ?     ||    ?
//------------------- ---------------------------------------------------------------------------
//   No    | FALSE || |   TRUE    &&  TRUE  &&  TRUE  &&  TRUE  &&  TRUE  &&   TRUE   ||  FALSE
//------------------- ---------------------------------------------------------------------------
    virtual bool IsValid(const wxUniChar& c) const wxOVERRIDE
    {
wxGCC_WARNING_SUPPRESS(parentheses)

        using wxPrivate::wxFilterChar;

        // chars in the EXCLUDE_CHAR_LIST should be excluded no matter what!
        // unless it is a space in which case we shouldn't cancel the effect
        // of wxFILTER_SPACE anyhow.
        return wxFilterChar<wxFILTER_SPACE,
                            Flags & wxFILTER_SPACE>::IsValid(c) || // yes it is a logical OR.
        (
          wxFilterChar<wxFILTER_EXCLUDE_CHAR_LIST,
                       Flags & wxFILTER_EXCLUDE_CHAR_LIST>::IsValid(c, this) &&
          wxFilterChar<wxFILTER_ASCII,
                       Flags & wxFILTER_ASCII>::IsValid(c) &&
          wxFilterChar<wxFILTER_DIGITS,
                       Flags & wxFILTER_DIGITS>::IsValid(c) &&
          wxFilterChar<wxFILTER_ALPHA,
                       Flags & wxFILTER_ALPHA>::IsValid(c) &&
          wxFilterChar<wxFILTER_ALPHANUMERIC,
                       Flags & wxFILTER_ALPHANUMERIC>::IsValid(c) &&
          wxFilterChar<wxFILTER_NUMERIC,
                       Flags & wxFILTER_NUMERIC>::IsValid(c) || // yes it is a logical OR.
          wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST,
                       Flags & wxFILTER_INCLUDE_CHAR_LIST>::IsValid(c, this)
        );
wxGCC_WARNING_RESTORE()
    }

    // Styles cannot be changed for this class.
    virtual void DoSetStyle(long WXUNUSED(style)) wxOVERRIDE { m_style = Flags; }

private:
    wxSharedPtr<wxRegEx>   m_regex;
    wxString               m_intent;

private:
    wxDECLARE_NO_ASSIGN_CLASS(wxRegexTextValidator);
//    wxDECLARE_DYNAMIC_CLASS(wxRegexTextValidator);
};

// a specialization for wxFILTER_INCLUDE_CHAR_LIST to filter out everything but
// the chars in the include char list.
template<>
bool WXDLLIMPEXP_CORE wxRegexTextValidator<wxFILTER_INCLUDE_CHAR_LIST>::IsValid(const wxUniChar& c) const;

#endif // wxUSE_REGEX

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#endif // _WX_VALTEXT_H_
