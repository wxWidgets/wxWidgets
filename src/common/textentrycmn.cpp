///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textentrycmn.cpp
// Purpose:     wxTextEntryBase implementation
// Author:      Vadim Zeitlin
// Created:     2007-09-26
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTCTRL || wxUSE_COMBOBOX

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/textentry.h"

// ============================================================================
// wxTextEntryBase implementation
// ============================================================================

wxString wxTextEntryBase::GetRange(long from, long to) const
{
    wxString sel;
    if ( from < to )
    {
        sel = GetValue().substr(from, to - from);
    }

    return sel;
}

void wxTextEntryBase::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextEntryBase::DoSetValue(const wxString& value, int flags)
{
    EventsSuppressor noeventsIf(this, !(flags & SetValue_SendEvent));

    SelectAll();
    WriteText(value);
}

void wxTextEntryBase::Replace(long from, long to, const wxString& value)
{
    {
        EventsSuppressor noevents(this);
        Remove(from, to);
    }

    WriteText(value);
}

bool wxTextEntryBase::HasSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return from < to;
}

wxString wxTextEntryBase::GetStringSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return GetRange(from, to);
}

bool wxTextEntryBase::CanCopy() const
{
    return HasSelection();
}

bool wxTextEntryBase::CanCut() const
{
    return CanCopy() && IsEditable();
}

bool wxTextEntryBase::CanPaste() const
{
    return IsEditable();
}

#endif // wxUSE_TEXTCTRL || wxUSE_COMBOBOX
