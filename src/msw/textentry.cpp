///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/textentry.cpp
// Purpose:     wxTextEntry implementation for wxMSW
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

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/textentry.h"

#include "wx/msw/private.h"

#define GetEditHwnd() ((HWND)(GetEditHWND()))

// ============================================================================
// wxTextEntry implementation
// ============================================================================

void wxTextEntry::WriteText(const wxString& text)
{
    ::SendMessage(GetEditHwnd(), EM_REPLACESEL, 0, (LPARAM)text.wx_str());
}

wxString wxTextEntry::GetValue() const
{
    return wxGetWindowText(GetEditHWND());
}

void wxTextEntry::Remove(long from, long to)
{
    DoSetSelection(from, to, SetSel_NoScroll);
    WriteText(wxString());
}

void wxTextEntry::Copy()
{
    ::SendMessage(GetEditHwnd(), WM_COPY, 0, 0);
}

void wxTextEntry::Cut()
{
    ::SendMessage(GetEditHwnd(), WM_CUT, 0, 0);
}

void wxTextEntry::Paste()
{
    ::SendMessage(GetEditHwnd(), WM_PASTE, 0, 0);
}

void wxTextEntry::Undo()
{
    ::SendMessage(GetEditHwnd(), EM_UNDO, 0, 0);
}

void wxTextEntry::Redo()
{
    // same as Undo, since Undo undoes the undo
    Undo();
    return;
}

bool wxTextEntry::CanUndo() const
{
    return ::SendMessage(GetEditHwnd(), EM_CANUNDO, 0, 0) != 0;
}

bool wxTextEntry::CanRedo() const
{
    // see comment in Redo()
    return CanUndo();
}

void wxTextEntry::SetInsertionPoint(long pos)
{
    // be careful to call DoSetSelection() which is overridden in wxTextCtrl
    // and not just SetSelection() here
    DoSetSelection(pos, pos);
}

long wxTextEntry::GetInsertionPoint() const
{
    long from;
    GetSelection(&from, NULL);
    return from;
}

long wxTextEntry::GetLastPosition() const
{
    return ::SendMessage(GetEditHwnd(), EM_LINELENGTH, 0, 0);
}

void wxTextEntry::DoSetSelection(long from, long to, int WXUNUSED(flags))
{
    // if from and to are both -1, it means (in wxWidgets) that all text should
    // be selected, translate this into Windows convention
    if ( (from == -1) && (to == -1) )
    {
        from = 0;
    }

    ::SendMessage(GetEditHwnd(), EM_SETSEL, from, to);
}

void wxTextEntry::GetSelection(long *from, long *to) const
{
    DWORD dwStart, dwEnd;
    ::SendMessage(GetEditHwnd(), EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);

    if ( from )
        *from = dwStart;
    if ( to )
        *to = dwEnd;
}

bool wxTextEntry::IsEditable() const
{
    return (::GetWindowLong(GetEditHwnd(), GWL_STYLE) & ES_READONLY) != 0;
}

void wxTextEntry::SetEditable(bool editable)
{
    ::SendMessage(GetEditHwnd(), EM_SETREADONLY, !editable, 0);
}

void wxTextEntry::SetMaxLength(unsigned long len)
{
    if ( len >= 0xffff )
    {
        // this will set it to a platform-dependent maximum (much more
        // than 64Kb under NT)
        len = 0;
    }

    ::SendMessage(GetEditHwnd(), EM_LIMITTEXT, len, 0);
}
