/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valtext.cpp
// Purpose:     wxTextValidator and wxRegexTextValidator classes
// Author:      Julian Smart, Francesco Montorsi et al
// Modified by: Ali Kettab
// Created:     2018-02-26
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#include "wx/valtext.h"

#ifndef WX_PRECOMP
  #include <stdio.h>
  #include "wx/combobox.h"
  #include "wx/log.h"
  #include "wx/msgdlg.h"
  #include "wx/textctrl.h"
  #include "wx/utils.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wx/combo.h"


// ----------------------------------------------------------------------------
// wxTextValidatorBase
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxTextValidatorBase, wxValidator)

wxBEGIN_EVENT_TABLE(wxTextValidatorBase, wxValidator)
    EVT_CHAR(wxTextValidatorBase::OnChar)
wxEND_EVENT_TABLE()

wxTextValidatorBase::wxTextValidatorBase(wxString* str, long style)
{
    m_style = style;
    m_string = str;

    // Notice that we should keep "include char list" always at the end
    // of m_includes as a one string item so that wxFILTER_INCLUDE_LIST
    // can safely be used in combination with wxFILTER_INCLUDE_CHAR_LIST.
    // Same goes for "exclude char list".
    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
        m_includes.Add(wxEmptyString);

    // See notice above...
    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
        m_excludes.Add(wxEmptyString);
}

wxTextValidatorBase::wxTextValidatorBase(const wxTextValidatorBase& val)
    : wxValidator()
{
    Copy(val);
}

bool wxTextValidatorBase::Copy(const wxTextValidatorBase& val)
{
    wxValidator::Copy(val);

    m_style = val.m_style;
    m_string = val.m_string;

    m_includes = val.m_includes;
    m_excludes = val.m_excludes;

    return true;
}

wxTextEntry* wxTextValidatorBase::GetTextEntry()
{
#if wxUSE_TEXTCTRL
    if (wxDynamicCast(m_validatorWindow, wxTextCtrl))
    {
        return (wxTextCtrl*)m_validatorWindow;
    }
#endif

#if wxUSE_COMBOBOX
    if (wxDynamicCast(m_validatorWindow, wxComboBox))
    {
        return (wxComboBox*)m_validatorWindow;
    }
#endif

#if wxUSE_COMBOCTRL
    if (wxDynamicCast(m_validatorWindow, wxComboCtrl))
    {
        return (wxComboCtrl*)m_validatorWindow;
    }
#endif

    wxFAIL_MSG(
        "wxTextValidatorBase can only be used with wxTextCtrl, wxComboBox, "
        "or wxComboCtrl"
    );

    return NULL;
}

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool wxTextValidatorBase::Validate(wxWindow *parent)
{
    // If window is disabled, simply return
    if ( !m_validatorWindow->IsEnabled() )
        return true;

    wxTextEntry * const text = GetTextEntry();
    if ( !text )
        return false;

    const wxString errormsg = DoValidate(text->GetValue());

    if ( !errormsg.empty() )
    {
        m_validatorWindow->SetFocus();
        wxMessageBox(errormsg, _("Validation conflict"),
                     wxOK | wxICON_EXCLAMATION, parent);

        return false;
    }

    return true;
}

// Called to transfer data to the window
bool wxTextValidatorBase::TransferToWindow()
{
    if ( m_string )
    {
        wxTextEntry * const text = GetTextEntry();
        if ( !text )
            return false;

        text->SetValue(*m_string);
    }

    return true;
}

// Called to transfer data to the window
bool wxTextValidatorBase::TransferFromWindow()
{
    if ( m_string )
    {
        wxTextEntry * const text = GetTextEntry();
        if ( !text )
            return false;

        *m_string = text->GetValue();
    }

    return true;
}

wxString wxTextValidatorBase::IsValid(const wxString& val) const
{
    for ( wxString::const_iterator i = val.begin(); i != val.end(); ++i )
    {
        if ( !IsValid(*i) )
            return _("'%s' has invalid characters!");
    }

    return wxEmptyString;
}

// 
void wxTextValidatorBase::SetCharIncludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin();
              i != chars.end(); ++i )
        {
            if ( m_excludes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning("Charactes in the exclude char list won't be included!");
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    // "include char list" is allocated in the constructor, and should
    // always be the last item in m_includes.
    m_includes.Last() = chars;
}

void wxTextValidatorBase::AddCharIncludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin();
              i != chars.end(); ++i )
        {
            if ( m_excludes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning("Charactes in the exclude char list won't be included!");
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    m_includes.Last() += chars;
}

void wxTextValidatorBase::SetCharExcludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin();
              i != chars.end(); ++i )
        {
            if ( m_includes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning("Charactes in the include char list will be excluded!");
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    // "exclude char list" is allocated in the constructor, and should
    // always be the last item in m_excludes.
    m_excludes.Last() = chars;
}

void wxTextValidatorBase::AddCharExcludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin();
              i != chars.end(); ++i )
        {
            if ( m_includes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning("Charactes in the include char list will be excluded!");
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    m_excludes.Last() += chars;
}

void wxTextValidatorBase::DoSetIncludes(const wxArrayString& includes)
{
    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        // Always keep the "include char list" at the end of m_includes
        const wxString chars = m_includes.Last();
        m_includes = includes;
        m_includes.Add(chars);
        // FIXME: this code as is, is not exception safe!
    }
    else
    {
        m_includes = includes;
    }
}

void wxTextValidatorBase::DoSetExcludes(const wxArrayString& excludes)
{
    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        // Always keep the "exclude char list" at the end m_excludes
        const wxString chars = m_excludes.Last();
        m_excludes = excludes;
        m_excludes.Add(chars);
        // FIXME: this code as is, is not exception safe!
    }
    else
    {
        m_excludes = excludes;
    }
}

void wxTextValidatorBase::DoAddInclude(const wxString& include)
{
    if ( include.empty() )
        return;

    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
        // Always keep the "include char list" at the end of m_includes
        m_includes.Insert(include, m_includes.GetCount()-1);
    else
        m_includes.Add(include);
}

void wxTextValidatorBase::DoAddExclude(const wxString& exclude)
{
    if ( exclude.empty() )
        return;

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
        // Always keep the "exclude char list" at the end m_excludes
        m_excludes.Insert(exclude, m_excludes.GetCount()-1);
    else
        m_excludes.Add(exclude);
}

void wxTextValidatorBase::OnChar(wxKeyEvent& event)
{
    // Let the event propagate by default.
    event.Skip();

    if ( !m_validatorWindow )
        return;

#if wxUSE_UNICODE
    // We only filter normal, printable characters.
    int keyCode = event.GetUnicodeKey();
#else // !wxUSE_UNICODE
    int keyCode = event.GetKeyCode();
    if ( keyCode > WXK_START )
        return;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    // we don't filter special keys and delete
    if ( keyCode < WXK_SPACE || keyCode == WXK_DELETE )
        return;

    // Filter out invalid characters
    if ( IsValid(static_cast<wxUniChar>(keyCode)) )
        return;

    if ( !wxValidator::IsSilent() )
        wxBell();

    // eat message
    event.Skip(false);
}

//
wxIMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxTextValidatorBase);

wxTextValidator::wxTextValidator(long style, wxString *val)
    : wxTextValidatorBase(val, style)
{
}

wxTextValidator::wxTextValidator(const wxTextValidator& val)
    : wxTextValidatorBase(val)
{
    Copy(val);
}

bool wxTextValidator::Copy(const wxTextValidator& WXUNUSED(val))
{
    return true;
}

bool wxTextValidator::IsValid(const wxUniChar& c) const
{
    using wxPrivate::wxIsNumeric;

    if ( HasFlag(wxFILTER_SPACE) && wxIsspace(c) )
        return true;

    // FIXME: for now, just preserve the old logic and behaviour!

    if ( HasFlag(wxFILTER_ASCII) && !c.IsAscii() )
        return false;
    if ( HasFlag(wxFILTER_ALPHA) && !wxIsalpha(c) )
        return false;
    if ( HasFlag(wxFILTER_ALPHANUMERIC) && !wxIsalnum(c) )
        return false;
    if ( HasFlag(wxFILTER_DIGITS) && !wxIsdigit(c) )
        return false;
    if ( HasFlag(wxFILTER_NUMERIC) && !wxIsNumeric(c) )
        return false;
    if ( !IsIncluded(c) )
        return false;
    if ( !IsNotExcluded(c) )
        return false;

    return true;
}

wxString wxTextValidator::DoValidate(const wxString& str)
{
    // We can only do some kinds of validation once the input is complete, so
    // check for them here:
    if ( HasFlag(wxFILTER_EMPTY) && str.empty() )
        return _("Required information entry is empty.");
    else if ( HasFlag(wxFILTER_EXCLUDE_LIST) && IsExcluded(str) )
        return wxString::Format(_("'%s' is one of the invalid strings"), str);
    else if ( HasFlag(wxFILTER_INCLUDE_LIST) && IsNotIncluded(str) )
        return wxString::Format(_("'%s' is not one of the valid strings"), str);

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

namespace wxPrivate
{

//template<>
bool WXDLLIMPEXP_CORE
wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST, true>::IsValid(
    const wxUniChar& c, const wxTextValidatorBase* obj)
{
    return obj->IsIncluded(c);
}

//template<>
bool WXDLLIMPEXP_CORE
wxFilterChar<wxFILTER_EXCLUDE_CHAR_LIST, true>::IsValid(
    const wxUniChar& c, const wxTextValidatorBase* obj)
{
    return obj->IsNotExcluded(c);
}

} // namespace wxPrivate

#if wxUSE_REGEX

template<>
bool WXDLLIMPEXP_CORE wxRegexTextValidator<wxFILTER_INCLUDE_CHAR_LIST>::IsValid(const wxUniChar& c) const
{
    return wxPrivate::wxFilterChar<wxFILTER_INCLUDE_CHAR_LIST, true>::IsValid(c, this);
}

#endif // wxUSE_REGEX

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
