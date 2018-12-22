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
// wxTextValidator
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxValidator)
wxBEGIN_EVENT_TABLE(wxTextValidator, wxValidator)
    EVT_CHAR(wxTextValidator::OnChar)
wxEND_EVENT_TABLE()

wxTextValidator::wxTextValidator(long style, wxString *val)
{
    m_stringValue = val;
    SetStyle(style);
}

wxTextValidator::wxTextValidator(const wxTextValidator& val)
    : wxValidator()
{
    Copy(val);
}

void wxTextValidator::SetStyle(long style)
{
    m_validatorStyle = style;

    // Notice that the presence of either or both 
    // wxFILTER_DIGITS and wxFILTER_XDIGITS with wxFILTER_ALPHA
    // is merely equivalent to wxFILTER_ALPHANUMERIC. 
    // so we should do the substitution for them here.

    if ( HasFlag(wxFILTER_ALPHA) && 
        (HasFlag(wxFILTER_DIGITS) || HasFlag(wxFILTER_XDIGITS)) )
    {
        m_validatorStyle &= ~wxFILTER_ALPHA;
        m_validatorStyle &= ~wxFILTER_DIGITS;
        m_validatorStyle &= ~wxFILTER_XDIGITS;

        m_validatorStyle |= wxFILTER_ALPHANUMERIC;
    }
}

bool wxTextValidator::Copy(const wxTextValidator& val)
{
    wxValidator::Copy(val);

    m_validatorStyle = val.m_validatorStyle;
    m_stringValue    = val.m_stringValue;

    m_charIncludes = val.m_charIncludes;
    m_charExcludes = val.m_charExcludes;
    m_includes     = val.m_includes;
    m_excludes     = val.m_excludes;

    return true;
}

wxTextEntry* wxTextValidator::GetTextEntry()
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
        "wxTextValidator can only be used with wxTextCtrl, wxComboBox, "
        "or wxComboCtrl"
    );

    return NULL;
}

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool wxTextValidator::Validate(wxWindow *parent)
{
    // If window is disabled, simply return
    if ( !m_validatorWindow->IsEnabled() )
        return true;

    wxTextEntry * const text = GetTextEntry();
    if ( !text )
        return false;

    const wxString errormsg = IsValid(text->GetValue());

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
bool wxTextValidator::TransferToWindow()
{
    if ( m_stringValue )
    {
        wxTextEntry * const text = GetTextEntry();
        if ( !text )
            return false;

        text->SetValue(*m_stringValue);
    }

    return true;
}

// Called to transfer data to the window
bool wxTextValidator::TransferFromWindow()
{
    if ( m_stringValue )
    {
        wxTextEntry * const text = GetTextEntry();
        if ( !text )
            return false;

        *m_stringValue = text->GetValue();
    }

    return true;
}

wxString wxTextValidator::IsValid(const wxString& str) const
{
    if ( HasFlag(wxFILTER_EMPTY) && str.empty() )
        return _("Required information entry is empty.");
    else if ( IsExcluded(str) )
        return wxString::Format(_("'%s' is one of the invalid strings"), str);
    else if ( !IsIncluded(str) )
        return wxString::Format(_("'%s' is not one of the valid strings"), str);

    // check the whole string for invalid chars.
    for ( wxString::const_iterator i = str.begin(), end = str.end();
          i != end; ++i )
    {
        if ( !IsValid(*i) )
        {
            return wxString::Format(
                _("'%s' contains invalid character(s)"), str);
        }
    }

    return wxString();
}


void wxTextValidator::SetCharIncludes(const wxString& chars)
{
    for ( wxString::const_iterator i = chars.begin(), end = chars.end();
          i != end; ++i )
    {
        if ( m_charExcludes.Find(*i) != wxNOT_FOUND )
        {
            wxLogWarning(_("A char should either be included or excluded!"));
            return;
        }
    }

    m_charIncludes = chars;
}

void wxTextValidator::AddCharIncludes(const wxString& chars)
{
    for ( wxString::const_iterator i = chars.begin(), end = chars.end();
          i != end; ++i )
    {
        if ( m_charExcludes.Find(*i) != wxNOT_FOUND )
        {
            wxLogWarning(_("A char should either be included or excluded!"));
            return;
        }
    }

    m_charIncludes += chars;
}

void wxTextValidator::SetCharExcludes(const wxString& chars)
{
    for ( wxString::const_iterator i = chars.begin(), end = chars.end();
          i != end; ++i )
    {
        if ( m_charIncludes.Find(*i) != wxNOT_FOUND )
        {
            wxLogWarning(_("A char should either be included or excluded!"));
            return;
        }
    }

    m_charExcludes = chars;
}

void wxTextValidator::AddCharExcludes(const wxString& chars)
{
    for ( wxString::const_iterator i = chars.begin(), end = chars.end();
          i != end; ++i )
    {
        if ( m_charIncludes.Find(*i) != wxNOT_FOUND )
        {
            wxLogWarning(_("A char should either be included or excluded!"));
            return;
        }
    }

    m_charExcludes += chars;
}

void wxTextValidator::OnChar(wxKeyEvent& event)
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

bool wxTextValidator::IsValid(const wxUniChar& c) const
{
    if ( !m_validatorStyle ) // no filtering if HasFlag(wxFILTER_NONE)
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
    
    long flags = m_validatorStyle & mask;

    if ( !flags )
        // accept whatever this character is.
        return true;

    // Turn off the wxFILTER_XXX if the corresponding check failed...

    if ( HasFlag(wxFILTER_ASCII) && !c.IsAscii() )
        flags ^= wxFILTER_ASCII;
    if ( HasFlag(wxFILTER_NUMERIC) && !wxIsNumeric(c) )
        flags ^= wxFILTER_NUMERIC;

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

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
