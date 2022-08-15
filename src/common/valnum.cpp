/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valnum.cpp
// Purpose:     Numeric validator classes implementation
// Author:      Vadim Zeitlin based on the submission of Fulvio Senore
// Created:     2010-11-06
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_VALIDATORS && wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
    #include "wx/combobox.h"
#endif

#include "wx/valnum.h"
#include "wx/numformatter.h"

#include <math.h>

// ============================================================================
// wxNumValidatorBase implementation
// ============================================================================

wxBEGIN_EVENT_TABLE(wxNumValidatorBase, wxValidator)
    EVT_CHAR(wxNumValidatorBase::OnChar)
    EVT_KILL_FOCUS(wxNumValidatorBase::OnKillFocus)
wxEND_EVENT_TABLE()

int wxNumValidatorBase::GetFormatFlags() const
{
    int flags = wxNumberFormatter::Style_None;
    if ( m_style & wxNUM_VAL_THOUSANDS_SEPARATOR )
        flags |= wxNumberFormatter::Style_WithThousandsSep;
    if ( m_style & wxNUM_VAL_NO_TRAILING_ZEROES )
        flags |= wxNumberFormatter::Style_NoTrailingZeroes;

    return flags;
}

void wxNumValidatorBase::SetWindow(wxWindow *win)
{
    wxValidator::SetWindow(win);

#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return;
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
    if ( wxDynamicCast(m_validatorWindow, wxComboBox) )
        return;
#endif // wxUSE_COMBOBOX

    wxFAIL_MSG("Can only be used with wxTextCtrl or wxComboBox");
}

wxTextEntry *wxNumValidatorBase::GetTextEntry() const
{
#if wxUSE_TEXTCTRL
    if ( wxTextCtrl *text = wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return text;
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
    if ( wxComboBox *combo = wxDynamicCast(m_validatorWindow, wxComboBox) )
        return combo;
#endif // wxUSE_COMBOBOX

    return NULL;
}

void
wxNumValidatorBase::GetCurrentValueAndInsertionPoint(wxString& val,
                                                             int& pos) const
{
    wxTextEntry * const control = GetTextEntry();
    if ( !control )
        return;

    val = control->GetValue();
    pos = control->GetInsertionPoint();

    long selFrom, selTo;
    control->GetSelection(&selFrom, &selTo);

    const long selLen = selTo - selFrom;
    if ( selLen )
    {
        // Remove selected text because pressing a key would make it disappear.
        val.erase(selFrom, selLen);

        // And adjust the insertion point to have correct position in the new
        // string.
        if ( pos > selFrom )
        {
            if ( pos >= selTo )
                pos -= selLen;
            else
                pos = selFrom;
        }
    }
}

bool wxNumValidatorBase::IsMinusOk(const wxString& val, int pos) const
{
    // We need to know if we accept negative numbers at all.
    if ( !CanBeNegative() )
        return false;

    // Minus is only ever accepted in the beginning of the string.
    if ( pos != 0 )
        return false;

    // And then only if there is no existing minus sign there.
    if ( !val.empty() && val[0] == '-' )
        return false;

    // Notice that entering '-' can make our value invalid, for example if
    // we're limited to -5..15 range and the current value is 12, then the
    // new value would be (invalid) -12. We consider it better to let the
    // user do this because perhaps he is going to press Delete key next to
    // make it -2 and forcing him to delete 1 first would be unnatural.
    //
    // TODO: It would be nice to indicate that the current control contents
    //       is invalid (if it's indeed going to be the case) once
    //       wxValidator supports doing this non-intrusively.
    return true;
}

void wxNumValidatorBase::OnChar(wxKeyEvent& event)
{
    // By default we just validate this key so don't prevent the normal
    // handling from taking place.
    event.Skip();

    if ( !m_validatorWindow )
        return;

#if wxUSE_UNICODE
    const int ch = event.GetUnicodeKey();
    if ( ch == WXK_NONE )
    {
        // It's a character without any Unicode equivalent at all, e.g. cursor
        // arrow or function key, we never filter those.
        return;
    }
#else // !wxUSE_UNICODE
    const int ch = event.GetKeyCode();
    if ( ch > WXK_DELETE )
    {
        // Not a character either.
        return;
    }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    if ( ch < WXK_SPACE || ch == WXK_DELETE )
    {
        // Allow ASCII control characters and Delete.
        return;
    }

    // Check if this character is allowed in the current state.
    wxString val;
    int pos;
    GetCurrentValueAndInsertionPoint(val, pos);

    // Minus is a special case because we can deal with it directly here, for
    // all the rest call the derived class virtual function.
    const bool ok = ch == '-' ? IsMinusOk(val, pos) : IsCharOk(val, pos, ch);
    if ( !ok )
    {
        if ( !wxValidator::IsSilent() )
            wxBell();

        // Do not skip the event in this case, stop handling it here.
        event.Skip(false);
    }
}

void wxNumValidatorBase::OnKillFocus(wxFocusEvent& event)
{
    event.Skip();

    wxTextEntry * const control = GetTextEntry();
    if ( !control )
        return;

    const wxString& valueNorm = NormalizeString(control->GetValue());
    if ( control->GetValue() == valueNorm )
    {
        // Don't do anything at all if the value doesn't really change, even if
        // the control optimizes away the calls to ChangeValue() which don't
        // actually change it, it's easier to skip all the complications below
        // if we don't need to do anything.
        return;
    }

    // When we change the control value below, its "modified" status is reset
    // so we need to explicitly keep it marked as modified if it was so in the
    // first place.
    //
    // Notice that only wxTextCtrl (and not wxTextEntry) has
    // IsModified()/MarkDirty() methods hence the need for dynamic cast.
    wxTextCtrl * const text = wxDynamicCast(m_validatorWindow, wxTextCtrl);
    const bool wasModified = text ? text->IsModified() : false;

    control->ChangeValue(valueNorm);

    if ( wasModified )
        text->MarkDirty();
}

// ============================================================================
// wxIntegerValidatorBase implementation
// ============================================================================

wxString wxIntegerValidatorBase::ToString(LongestValueType value) const
{
    if ( CanBeNegative() )
    {
        return wxNumberFormatter::ToString(value, GetFormatFlags());
    }
    else
    {
        ULongestValueType uvalue = static_cast<ULongestValueType>(value);
        return wxNumberFormatter::ToString(uvalue, GetFormatFlags());
    }
}

bool
wxIntegerValidatorBase::FromString(const wxString& s,
                                   LongestValueType *value) const
{
    if ( CanBeNegative() )
    {
        return wxNumberFormatter::FromString(s, value);
    }
    else
    {
        // Parse as unsigned to ensure we don't accept minus sign here.
        ULongestValueType uvalue;
        if ( !wxNumberFormatter::FromString(s, &uvalue) )
            return false;

        // This cast is lossless.
        *value = static_cast<LongestValueType>(uvalue);

        return true;
    }
}

bool
wxIntegerValidatorBase::IsCharOk(const wxString& val, int pos, wxChar ch) const
{
    // We only accept digits here (remember that '-' is taken care of by the
    // base class already).
    if ( ch < '0' || ch > '9' )
        return false;

    // And the value after insertion needs to be in the defined range.
    LongestValueType value;
    if ( !FromString(GetValueAfterInsertingChar(val, pos, ch), &value) )
        return false;

    return IsInRange(value);
}

// ============================================================================
// wxFloatingPointValidatorBase implementation
// ============================================================================

wxString wxFloatingPointValidatorBase::ToString(LongestValueType value) const
{
    // When using factor > 1 we're going to show more digits than are
    // significant in the real value, so decrease precision to account for it.
    int precision = m_precision;
    if ( precision && m_factor > 1 )
    {
        precision -= static_cast<int>(log10(static_cast<double>(m_factor)));
        if ( precision < 0 )
            precision = 0;
    }

    return wxNumberFormatter::ToString(value*m_factor,
                                       precision,
                                       GetFormatFlags());
}

bool
wxFloatingPointValidatorBase::FromString(const wxString& s,
                                         LongestValueType *value) const
{
    if ( !wxNumberFormatter::FromString(s, value) )
        return false;

    *value /= m_factor;

    return true;
}

bool
wxFloatingPointValidatorBase::IsCharOk(const wxString& val,
                                       int pos,
                                       wxChar ch) const
{
    const wxChar separator = wxNumberFormatter::GetDecimalSeparator();
    if ( ch == separator )
    {
        if ( val.find(separator) != wxString::npos )
        {
            // There is already a decimal separator, can't insert another one.
            return false;
        }

        // Prepending a separator before the minus sign isn't allowed.
        if ( pos == 0 && !val.empty() && val[0] == '-' )
            return false;

        // Otherwise always accept it, adding a decimal separator doesn't
        // change the number value and, in particular, can't make it invalid.
        // OTOH the checks below might not pass because strings like "." or
        // "-." are not valid numbers so parsing them would fail, hence we need
        // to treat it specially here.
        return true;
    }

    // Must be a digit then.
    if ( ch < '0' || ch > '9' )
        return false;

    // Check whether the value we'd obtain if we accepted this key is correct.
    const wxString newval(GetValueAfterInsertingChar(val, pos, ch));

    LongestValueType value;
    if ( !FromString(newval, &value) )
        return false;

    // Also check that it doesn't have too many decimal digits.
    const size_t posSep = newval.find(separator);
    if ( posSep != wxString::npos && newval.length() - posSep - 1 > m_precision )
        return false;

    // Finally check whether it is in the range.
    return IsInRange(value);
}

#endif // wxUSE_VALIDATORS && wxUSE_TEXTCTRL
