/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valtext.cpp
// Purpose:     wxTextValidator
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     04/01/98
// Copyright:   (c) Julian Smart
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
  #include "wx/textctrl.h"
  #include "wx/combobox.h"
  #include "wx/log.h"
  #include "wx/utils.h"
  #include "wx/msgdlg.h"
  #include "wx/intl.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wx/combo.h"

// ----------------------------------------------------------------------------
// global helpers
// ----------------------------------------------------------------------------

static bool wxIsNumeric(const wxString& val)
{
    for ( wxString::const_iterator i = val.begin(); i != val.end(); ++i )
    {
        // Allow for "," (French) as well as "." -- in future we should
        // use wxSystemSettings or other to do better localisation
        if ((!wxIsdigit(*i)) &&
            (*i != wxS('.')) && (*i != wxS(',')) && (*i != wxS('e')) &&
            (*i != wxS('E')) && (*i != wxS('+')) && (*i != wxS('-')))
            return false;
    }
    return true;
}

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

    // N.B. the "include char list" should be the last item of m_includes
    //      if wxFILTER_INCLUDE_CHAR_LIST flag is set.
    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
        m_includes.Add(wxEmptyString);

    // N.B. the "exclude char list" should be the last item of m_excludes
    //      if wxFILTER_EXCLUDE_CHAR_LIST flag is set.
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

wxString wxTextValidatorBase::IsValid(const wxString& str) const
{
    for ( wxString::const_iterator i = str.begin(), end = str.end();
          i != end; ++i )
    {
        if ( !IsValid(*i) )
            // N.B. this format string should contain exactly one '%s'
            return _("'%s' contains invalid character(s)!");
    }

    return wxString();
}


void wxTextValidatorBase::SetCharIncludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin(), end = chars.end();
              i != end; ++i )
        {
            if ( m_excludes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning(_("An excluded char won't be included!"));
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    // the "include char list" is allocated in the constructor, and should
    // always be the last item in m_includes.
    m_includes.Last() = chars;
}

void wxTextValidatorBase::AddCharIncludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin(), end = chars.end();
              i != end; ++i )
        {
            if ( m_excludes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning(_("An excluded char won't be included!"));
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
        for ( wxString::const_iterator i = chars.begin(), end = chars.end();
              i != end; ++i )
        {
            if ( m_includes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning(_("An included char will be excluded!"));
                break;
            }
        }
    }

#endif // wxDEBUG_LEVEL

    // the "exclude char list" is allocated in the constructor, and should
    // always be the last item in m_excludes.
    m_excludes.Last() = chars;
}

void wxTextValidatorBase::AddCharExcludes(const wxString& chars)
{
    wxASSERT( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) );

#if wxDEBUG_LEVEL

    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        for ( wxString::const_iterator i = chars.begin(), end = chars.end();
              i != end; ++i )
        {
            if ( m_includes.Last().Find(*i) != wxNOT_FOUND )
            {
                wxLogWarning(_("An included char will be excluded!"));
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
        // Always keep the "exclude char list" at the end of m_excludes
        const wxString chars = m_excludes.Last();
        m_excludes = excludes;
        m_excludes.Add(chars);
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
        // We should insert before the "include char list" which should be
        // kept at the end of m_includes
        m_includes.Insert(include, m_includes.GetCount()-1);
    else
        m_includes.Add(include);
}

void wxTextValidatorBase::DoAddExclude(const wxString& exclude)
{
    if ( exclude.empty() )
        return;

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
        // We should insert before the "exclude char list" which should be
        // kept at the end m_excludes
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

// ----------------------------------------------------------------------------
// wxTextValidator
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxTextValidatorBase);

wxTextValidator::wxTextValidator(wxString *str, long style)
    : wxTextValidatorBase(str, style)
{
}

wxTextValidator::wxTextValidator(long style, wxString *str)
    : wxTextValidatorBase(str, style)
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
    if ( !m_style ) // no filtering if HasFlag(wxFILTER_NONE)
        return true;

    if ( HasFlag(wxFILTER_SPACE) && wxIsspace(c) )
        return true;

    // Notice that if the same char is mistakenly present in both the
    // include char list and exclude char list than the char will be
    // excluded and a warning is given in debug build.

    if ( IsCharExcluded(c) )
        return false;

    if ( IsCharIncluded(c) )
        return true;

    static const long mask = wxFILTER_ASCII|wxFILTER_ALPHA|wxFILTER_NUMERIC|
                      wxFILTER_ALPHANUMERIC|wxFILTER_DIGITS|wxFILTER_XDIGITS;
    
    long flags = m_style & mask;

    if ( !flags )
        // accept whatever this character is.
        return true;

    // Turn off the wxFILTER_XXX if the corresponding check failed...

    if ( HasFlag(wxFILTER_ASCII) && !c.IsAscii() )
        flags ^= wxFILTER_ASCII;
    if ( HasFlag(wxFILTER_NUMERIC) && !wxIsNumeric(c) )
        flags ^= wxFILTER_NUMERIC;

    // wxFILTER_ALPHANUMERIC is the combination of
    // wxFILTER_ALPHA, wxFILTER_DIGITS and wxFILTER_XDIGITS

    if ( HasFlag(wxFILTER_ALPHANUMERIC) && !wxIsalnum(c) )
    {
        flags ^= wxFILTER_ALPHANUMERIC;
    }
    else
    {
        if ( HasFlag(wxFILTER_ALPHA) && !wxIsalpha(c) )
            flags ^= wxFILTER_ALPHA;
        if ( HasFlag(wxFILTER_DIGITS) && !wxIsdigit(c) )
            flags ^= wxFILTER_DIGITS;
        if ( HasFlag(wxFILTER_XDIGITS) && !wxIsxdigit(c) )
            flags ^= wxFILTER_XDIGITS;
    }

    return flags != 0;
}

wxString wxTextValidator::DoValidate(const wxString& str)
{
    // We can only do some kinds of validation once the input is complete, so
    // check for them here:
    if ( HasFlag(wxFILTER_EMPTY) && str.empty() )
        return _("Required information entry is empty.");
    else if ( HasFlag(wxFILTER_EXCLUDE_LIST) && IsExcluded(str) )
        return wxString::Format(_("'%s' is one of the invalid strings"), str);
    else if ( HasFlag(wxFILTER_INCLUDE_LIST) && !IsIncluded(str) )
        return wxString::Format(_("'%s' is not one of the valid strings"), str);

    wxString errormsg = wxTextValidatorBase::IsValid(str);

    if ( !errormsg.empty() )
    {
        // NB: this format string should always contain exactly one '%s'
        wxString buf;
        buf.Printf(errormsg, str);
        return buf;
    }

    return wxString();
}

void wxTextValidator::DoSetStyle(long style)
{
    // Add or remove "include/exclude char list" item when needed

    // wxFILTER_INCLUDE_CHAR_LIST flag:
    // --------------------------------

    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        // want to unset wxFILTER_INCLUDE_CHAR_LIST
        if ( (style & wxFILTER_INCLUDE_CHAR_LIST) == 0 )
            m_includes.RemoveAt(m_includes.GetCount()-1);
    }
    else // wxFILTER_INCLUDE_CHAR_LIST was not set
    {
        // want to set wxFILTER_INCLUDE_CHAR_LIST
        if ( (style & wxFILTER_INCLUDE_CHAR_LIST) != 0 )
            m_includes.Add(wxEmptyString);
    }

    // wxFILTER_EXCLUDE_CHAR_LIST flag:
    // --------------------------------

    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        // want to unset wxFILTER_EXCLUDE_CHAR_LIST
        if ( (style & wxFILTER_EXCLUDE_CHAR_LIST) == 0 )
            m_excludes.RemoveAt(m_excludes.GetCount()-1);
    }
    else // wxFILTER_EXCLUDE_CHAR_LIST was not set
    {
        // want to set wxFILTER_EXCLUDE_CHAR_LIST
        if ( (style & wxFILTER_EXCLUDE_CHAR_LIST) != 0 )
            m_excludes.Add(wxEmptyString);
    }

    wxTextValidatorBase::DoSetStyle(style);
}

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
