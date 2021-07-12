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

#include "wx/clipbrd.h"
#include "wx/combo.h"
#include "wx/regex.h"

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
// wxTextEntryValidator implementation
// ----------------------------------------------------------------------------
/*static*/
bool wxTextEntryValidator::ms_skipTextEvent = true;

void wxTextEntryValidator::SetWindow(wxWindow *win)
{
    wxValidator::SetWindow(win);

    if ( GetTextEntry() != NULL )
    {
        Bind(wxEVT_TEXT, &wxTextEntryValidator::OnText, this);
        Bind(wxEVT_TEXT_PASTE, &wxTextEntryValidator::OnPasteText, this);
        Bind(wxEVT_VALIDATE_ERROR, &wxTextEntryValidator::OnValidate, this);

        if ( ShouldValidateOnFocusLost() )
        {
            Bind(wxEVT_KILL_FOCUS, &wxTextEntryValidator::OnKillFocus, this);
        }
    }
    else
    {
        wxFAIL_MSG(
            "wxTextEntryValidator can only be used with wxTextCtrl, wxComboBox "
            "or wxComboCtrl"
        );
    }
}

wxTextEntry *wxTextEntryValidator::GetTextEntry() const
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

    return NULL;
}

void wxTextEntryValidator::OnText(wxCommandEvent& event)
{
    ClearValidationStatus();

    if ( IsInteractive() )
    {
        DoValidate(NULL, wxVALIDATOR_NO_POPUP);
    }

    event.Skip(ms_skipTextEvent);
}

void wxTextEntryValidator::OnPasteText(wxClipboardTextEvent& event)
{
    event.Skip();

#if wxUSE_CLIPBOARD
    wxTextEntry * const entry = GetTextEntry();
    if ( !entry )
        return;

    wxClipboardLocker clipLock;
    if ( !clipLock )
        return;

    wxTextDataObject data;

    if ( wxTheClipboard->IsSupported(data.GetFormat())
            && wxTheClipboard->GetData(data) )
    {
        wxString text = entry->GetValue();

        long from, to;
        entry->GetSelection(&from, &to);

        if ( from != to )
        {
            // Replace selection with the pasted text.
            text.replace(from, (to - from), data.GetText());
        }
        else
        {
            text.insert(entry->GetInsertionPoint(), data.GetText());
        }

        if ( !IsValid(text).empty() )
        {
            // Don't accept the pasted text.
            event.Skip(false);
        }
    }
#endif // wxUSE_CLIPBOARD
}

void wxTextEntryValidator::OnValidate(wxValidationStatusEvent& event)
{
    if ( !event.CanPopup() )
        return;

    const wxString& errormsg = event.GetErrorMessage();

    m_validatorWindow->SetFocus();
    wxMessageBox(errormsg, _("Validation conflict"),
                 wxOK | wxICON_EXCLAMATION, NULL);
}

void wxTextEntryValidator::OnKillFocus(wxFocusEvent& event)
{
    event.Skip();
    DoValidate(NULL, wxVALIDATOR_NO_POPUP);
}

// ----------------------------------------------------------------------------
// wxTextValidator
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxValidator);
wxBEGIN_EVENT_TABLE(wxTextValidator, wxValidator)
    EVT_CHAR(wxTextValidator::OnChar)
wxEND_EVENT_TABLE()

wxTextValidator::wxTextValidator(long style, wxString *val)
{
    m_stringValue = val;
    SetStyle(style);
}

wxTextValidator::wxTextValidator(const wxTextValidator& val)
    : wxTextEntryValidator()
{
    Copy(val);
}

void wxTextValidator::SetStyle(long style)
{
    m_validatorStyle = style;
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

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool wxTextValidator::Validate(wxWindow *WXUNUSED(parent))
{
    // If window is disabled, simply return
    if ( !m_validatorWindow->IsEnabled() )
        return true;

    wxTextEntry * const text = GetTextEntry();
    if ( !text )
        return false;

    const wxString& errormsg = IsValid(text->GetValue());

    if ( !errormsg.empty() )
    {
        SendErrorEvent(errormsg);
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
        if ( !IsValidChar(*i) )
        {
            return wxString::Format(
                _("'%s' contains invalid character(s)"), str);
        }
    }

    return wxString();
}


void wxTextValidator::SetCharIncludes(const wxString& chars)
{
    m_charIncludes.clear();

    AddCharIncludes(chars);
}

void wxTextValidator::AddCharIncludes(const wxString& chars)
{
    m_charIncludes += chars;
}

void wxTextValidator::SetCharExcludes(const wxString& chars)
{
    m_charExcludes.clear();

    AddCharExcludes(chars);
}

void wxTextValidator::AddCharExcludes(const wxString& chars)
{
    m_charExcludes += chars;
}

void wxTextValidator::SetIncludes(const wxArrayString& includes)
{
    // preserve compatibily with versions prior 3.1.3 which used m_includes
    // to store the list of char includes.
    if ( HasFlag(wxFILTER_INCLUDE_CHAR_LIST) )
    {
        for ( wxArrayString::const_iterator i = includes.begin(),
              end = includes.end(); i != end; ++i )
        {
            AddCharIncludes(*i);
        }

        return;
    }

    m_includes = includes;
}

void wxTextValidator::AddInclude(const wxString& include)
{
    m_includes.push_back(include);
}

void wxTextValidator::SetExcludes(const wxArrayString& excludes)
{
    // preserve compatibily with versions prior 3.1.3 which used m_excludes
    // to store the list of char excludes.
    if ( HasFlag(wxFILTER_EXCLUDE_CHAR_LIST) )
    {
        for ( wxArrayString::const_iterator i = excludes.begin(),
              end = excludes.end(); i != end; ++i )
        {
            AddCharExcludes(*i);
        }

        return;
    }

    m_excludes = excludes;
}

void wxTextValidator::AddExclude(const wxString& exclude)
{
    m_excludes.push_back(exclude);
}

void wxTextValidator::OnChar(wxKeyEvent& event)
{
    // Let the event propagate by default.
    event.Skip();

    if (!m_validatorWindow)
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
    if (keyCode < WXK_SPACE || keyCode == WXK_DELETE)
        return;

    // Filter out invalid characters
    if ( IsValidChar(static_cast<wxUniChar>(keyCode)) )
        return;

    if ( !wxValidator::IsSilent() )
        wxBell();

    // eat message
    event.Skip(false);
}

bool wxTextValidator::IsValidChar(const wxUniChar& c) const
{
    if ( !m_validatorStyle ) // no filtering if HasFlag(wxFILTER_NONE)
        return true;

    if ( IsCharExcluded(c) ) // disallow any char in the m_charExcludes.
        return false;

    if ( IsCharIncluded(c) ) // allow any char in the m_charIncludes.
        return true;

    if ( !HasFlag(wxFILTER_CC) )
    {
        // Validity is entirely determined by the exclude/include char lists
        // and this character is in neither, so consider that it is valid if
        // and only if we accept anything.
        return !HasFlag(wxFILTER_INCLUDE_CHAR_LIST);
    }

    if ( HasFlag(wxFILTER_SPACE) && wxIsspace(c) )
        return true;
    if ( HasFlag(wxFILTER_ASCII) && c.IsAscii() )
        return true;
    if ( HasFlag(wxFILTER_NUMERIC) && wxIsNumeric(c) )
        return true;
    if ( HasFlag(wxFILTER_ALPHANUMERIC) && wxIsalnum(c) )
        return true;
    if ( HasFlag(wxFILTER_ALPHA) && wxIsalpha(c) )
        return true;
    if ( HasFlag(wxFILTER_DIGITS) && wxIsdigit(c) )
        return true;
    if ( HasFlag(wxFILTER_XDIGITS) && wxIsxdigit(c) )
        return true;

    // If we are here, this means that the char c does not belong to any of the
    // character classes checked above (e.g. emoji chars) so just return false.

    return false;
}

// kept for compatibility reasons.
bool wxTextValidator::ContainsOnlyIncludedCharacters(const wxString& str) const
{
    for ( wxString::const_iterator i = str.begin(), end = str.end();
          i != end; ++i )
    {
        if ( !IsCharIncluded(*i) )
            return false;
    }

    return true;
}

// kept for compatibility reasons.
bool wxTextValidator::ContainsExcludedCharacters(const wxString& str) const
{
    for ( wxString::const_iterator i = str.begin(), end = str.end();
          i != end; ++i )
    {
        if ( IsCharExcluded(*i) )
            return true;
    }

    return false;
}

#if wxUSE_REGEX
// ----------------------------------------------------------------------------
// wxRegexTextValidator implementation
// ----------------------------------------------------------------------------
wxIMPLEMENT_DYNAMIC_CLASS(wxRegexTextValidator, wxTextValidator);

wxRegexTextValidator::wxRegexTextValidator(long style, wxString* str)
    : wxTextValidator(style, str)
{
}

wxRegexTextValidator::wxRegexTextValidator(const wxString& pattern,
                                           const wxString& intent,
                                           long style, wxString* str)
    : wxTextValidator(style, str)
{
    SetRegEx(pattern, intent);
}

void wxRegexTextValidator::SetRegEx(const wxString& pattern, const wxString& intent)
{
    m_intent = intent;
    m_regex.reset(new wxRegEx);

    // Use wxRE_ADVANCED flag if available
#ifdef wxHAS_REGEX_ADVANCED
    const int flag = wxRE_ADVANCED;
#else
    const int flag = wxRE_DEFAULT;
#endif

    // No need to check for a (possible) compilation error here
    // as the object will be checked for validity when used anyhow.
    m_regex->Compile(pattern, flag);
}

void wxRegexTextValidator::SetRegEx(wxSharedPtr<wxRegEx> regex, const wxString& intent)
{
    m_regex = regex;
    m_intent = intent;
}

wxObject* wxRegexTextValidator::Clone() const
{
    return new wxRegexTextValidator(*this);
}

wxString wxRegexTextValidator::IsValid(const wxString& str) const
{
    wxASSERT_MSG( (m_regex && m_regex->IsValid()),
        "wxRegexTextValidator not properly initialized!" );

    wxString errmsg = wxTextValidator::IsValid(str);

    if ( errmsg.empty() && !m_regex->Matches(str) )
    {
        errmsg = wxString::Format(_("'%s' is not a valid %s"), str, m_intent);
    }

    return errmsg;
}

#endif // wxUSE_REGEX

#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
