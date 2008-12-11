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
    #include "wx/window.h"
    #include "wx/dataobj.h"
#endif //WX_PRECOMP

#include "wx/textentry.h"
#include "wx/clipbrd.h"

// ============================================================================
// wxTextEntryBase implementation
// ============================================================================

wxString wxTextEntryBase::GetRange(long from, long to) const
{
    wxString sel;
    wxString value = GetValue();

    if ( from < to && (long)value.length() >= to )
    {
        sel = value.substr(from, to - from);
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

    SetInsertionPoint(0);
}

void wxTextEntryBase::Replace(long from, long to, const wxString& value)
{
    {
        EventsSuppressor noevents(this);
        Remove(from, to);
    }

    SetInsertionPoint(from);
    WriteText(value);
}

bool wxTextEntryBase::HasSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return from < to;
}

void wxTextEntryBase::RemoveSelection()
{
    long from, to;
    GetSelection(& from, & to);
    if (from != -1 && to != -1)
        Remove(from, to);
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
    if ( IsEditable() )
    {
#if wxUSE_CLIPBOARD
        // check if there is any text on the clipboard
        if ( wxTheClipboard->IsSupported(wxDF_TEXT)
#if wxUSE_UNICODE
                || wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
#endif // wxUSE_UNICODE
           )
        {
            return true;
        }
#endif // wxUSE_CLIPBOARD
    }

    return false;
}

#endif // wxUSE_TEXTCTRL || wxUSE_COMBOBOX
