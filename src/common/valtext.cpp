/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valtext.cpp
// Purpose:     wxTextValidator
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     04/01/98
// RCS-ID:      $Id$
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
  #include "wx/utils.h"
  #include "wx/msgdlg.h"
  #include "wx/intl.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------
// global helpers
// ----------------------------------------------------------------------------

static bool wxIsNumeric(const wxString& val)
{
    for ( wxString::const_iterator i = val.begin(); i != val.end(); ++i )
    {
        // Allow for "," (French) as well as "." -- in future we should
        // use wxSystemSettings or other to do better localisation
        if ((!wxIsdigit(*i)) && (*i != wxT('.')) && (*i != wxT(',')) && (*i != wxT('e')) &&
            (*i != wxT('E')) && (*i != wxT('+')) && (*i != wxT('-')))
            return false;
    }
    return true;
}

// ----------------------------------------------------------------------------
// wxTextValidator
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxValidator)
BEGIN_EVENT_TABLE(wxTextValidator, wxValidator)
    EVT_CHAR(wxTextValidator::OnChar)
END_EVENT_TABLE()


#if WXWIN_COMPATIBILITY_2_8
wxTextValidator::wxTextValidator(long style, wxString *val)
{
    m_validatorStyle = (wxTextValidatorStyle)style;
    m_stringValue = val;
}

void wxTextValidator::SetStyle(long style)
{
    SetStyle((wxTextValidatorStyle)style);
}
#endif

wxTextValidator::wxTextValidator(wxTextValidatorStyle style, wxString *val)
{
    m_validatorStyle = style;
    m_stringValue = val;
}

wxTextValidator::wxTextValidator(const wxTextValidator& val)
    : wxValidator()
{
    Copy(val);
}

bool wxTextValidator::Copy(const wxTextValidator& val)
{
    wxValidator::Copy(val);

    m_validatorStyle = val.m_validatorStyle;
    m_stringValue = val.m_stringValue;

    m_includes    = val.m_includes;
    m_excludes    = val.m_excludes;

    return true;
}

wxTextEntry *wxTextValidator::GetTextEntry()
{
#if wxUSE_TEXTCTRL
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    {
        return (wxTextCtrl*)m_validatorWindow;
    }
#endif

#if wxUSE_COMBOBOX
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)))
    {
        return (wxComboBox*)m_validatorWindow;
    }
#endif

    wxFAIL_MSG(
        _T("wxTextValidator can only be used with wxTextCtrl or wxComboBox")
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

    wxString val(text->GetValue());

    // NB: this format string should always contain exactly one '%s'
    wxString errormsg;
    if (!IsValid(val, &errormsg))
    {
        wxASSERT(!errormsg.empty());

        m_validatorWindow->SetFocus();

        wxString buf;
        buf.Printf(errormsg, val.c_str());

        wxMessageBox(buf, _("Validation conflict"),
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

bool wxTextValidator::IsValid(const wxString& val, wxString* pErr) const
{
    wxString errormsg;
    switch (m_validatorStyle)
    {
    case wxFILTER_NONE:
        // nothing to do...
        break;

    case wxFILTER_INCLUDE_LIST:
        if ( m_includes.Index(val) == wxNOT_FOUND )
            errormsg = _("'%s' is invalid");
        break;

    case wxFILTER_EXCLUDE_LIST:
        if ( m_excludes.Index(val) != wxNOT_FOUND )
            errormsg = _("'%s' is invalid");
        break;

    case wxFILTER_ASCII:
        if ( !val.IsAscii() )
            errormsg = _("'%s' should only contain ASCII characters.");
        break;

    case wxFILTER_ALPHA:
        if ( !wxStringCheck<wxIsalpha>(val) )
            errormsg = _("'%s' should only contain alphabetic characters.");
        break;

    case wxFILTER_ALPHANUMERIC:
        if ( !wxStringCheck<wxIsalnum>(val) )
            errormsg = _("'%s' should only contain alphabetic or numeric characters.");
        break;

    case wxFILTER_SIMPLE_NUMBER:
        if ( !wxStringCheck<wxIsdigit>(val) )
            errormsg = _("'%s' should be numeric.");
        break;

    case wxFILTER_NUMERIC:
        if ( !wxIsNumeric(val) )
            errormsg = _("'%s' should be numeric.");
        break;

    case wxFILTER_INCLUDE_CHAR_LIST:
        if ( !ContainsOnlyIncludedCharacters(val) )
            errormsg = _("'%s' is invalid");
        break;

    case wxFILTER_EXCLUDE_CHAR_LIST:
        if ( ContainsExcludedCharacters(val) )
            errormsg = _("'%s' is invalid");
        break;

    default:
        wxFAIL_MSG("invalid text validator style");
    }

    if (pErr)
        *pErr = errormsg;

    return errormsg.empty();
}

bool wxTextValidator::ContainsOnlyIncludedCharacters(const wxString& val) const
{
    for (size_t i = 0; i < val.length(); i++)
        if (m_includes.Index((wxString) val[i]) == wxNOT_FOUND)
            // one character of 'val' is NOT present in m_includes...
            return false;

    // all characters of 'val' are present in m_includes
    return true;
}

bool wxTextValidator::ContainsExcludedCharacters(const wxString& val) const
{
    for (size_t i = 0; i < val.length(); i++)
        if (m_excludes.Index((wxString) val[i]) != wxNOT_FOUND)
            // one character of 'val' is present in m_excludes...
            return false;

    // all characters of 'val' are NOT present in m_excludes
    return true;
}

void wxTextValidator::OnChar(wxKeyEvent& event)
{
    if (!m_validatorWindow)
    {
        event.Skip();
        return;
    }

    int keyCode = event.GetKeyCode();

    // we don't filter special keys and delete
    if (keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode >= WXK_START)
    {
        event.Skip();
        return;
    }

    wxString str((wxUniChar)keyCode, 1);
    if (!IsValid(str, NULL))
    {
        if ( !wxValidator::IsSilent() )
            wxBell();

        // eat message
        return;
    }
    else
        event.Skip();
}


#endif
  // wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
