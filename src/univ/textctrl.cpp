/////////////////////////////////////////////////////////////////////////////
// Name:        univ/textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtextctrl.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/validate.h"
    #include "wx/textctrl.h"
#endif

#include "wx/caret.h"

#include "wx/univ/inphand.h"
#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxTextCtrl::Init()
{
    m_selStart =
    m_selEnd = -1;

    m_isModified = FALSE;
    m_isEditable = TRUE;

    m_caret = (wxCaret *)NULL;
}

bool wxTextCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style,
                            validator, name) )
    {
        return FALSE;
    }

    wxCaret *caret = new wxCaret(this, 1, GetCharHeight());
    SetCaret(caret);
    caret->Show();

    SetCursor(wxCURSOR_IBEAM);

    SetValue(value);
    SetBestSize(size);

    return TRUE;
}

// ----------------------------------------------------------------------------
// set/get the value
// ----------------------------------------------------------------------------

void wxTextCtrl::SetValue(const wxString& value)
{
    m_value = value;

    if ( IsSingleLine() )
    {
        SetInsertionPointEnd();
    }
    else
    {
        SetInsertionPoint(0);
    }
}

wxString wxTextCtrl::GetValue() const
{
    return m_value;
}

void wxTextCtrl::Clear()
{
    SetValue(_T(""));
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
    wxFAIL_MSG(_T("not implemented"));

    // update current position
}

void wxTextCtrl::Remove(long from, long to)
{
    Replace(from, to, _T(""));
}

void wxTextCtrl::WriteText(const wxString& text)
{
    Replace(m_curPos, m_curPos, text);
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

// ----------------------------------------------------------------------------
// current position
// ----------------------------------------------------------------------------

void wxTextCtrl::SetInsertionPoint(long pos)
{
    HideCaret();

    m_curPos = pos;

    ShowCaret();
}

void wxTextCtrl::SetInsertionPointEnd()
{
    SetInsertionPoint(GetLastPosition());
}

long wxTextCtrl::GetInsertionPoint() const
{
    return m_curPos;
}

long wxTextCtrl::GetLastPosition() const
{
    if ( IsSingleLine() )
    {
        return m_value.length();
    }
    else
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return -1;
    }
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    if ( from )
        *from = m_selStart;
    if ( to )
        *to = m_selEnd;
}

void wxTextCtrl::SetSelection(long from, long to)
{
    if ( from != m_selStart || to != m_selEnd )
    {
        m_selStart = from;
        m_selEnd = to;

        // TODO: update display
    }
}

// ----------------------------------------------------------------------------
// flags
// ----------------------------------------------------------------------------

bool wxTextCtrl::IsModified() const
{
    return m_isModified;
}

bool wxTextCtrl::IsEditable() const
{
    return m_isEditable;
}

void wxTextCtrl::DiscardEdits()
{
    m_isModified = FALSE;
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable != m_isEditable )
    {
        m_isEditable = editable;

        Refresh();
    }
}

// ----------------------------------------------------------------------------
// col/lines <-> position correspondence
// ----------------------------------------------------------------------------

int wxTextCtrl::GetLineLength(long line) const
{
    if ( IsSingleLine() )
    {
        wxASSERT_MSG( line == 0, _T("invalid GetLineLength() parameter") );

        return m_value.length();
    }
    else // multiline
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return 0;
    }
}

wxString wxTextCtrl::GetLineText(long line) const
{
    if ( IsSingleLine() )
    {
        wxASSERT_MSG( line == 0, _T("invalid GetLineLength() parameter") );

        return m_value;
    }
    else // multiline
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return _T("");
    }
}

int wxTextCtrl::GetNumberOfLines() const
{
    if ( IsSingleLine() )
    {
        return 1;
    }
    else // multiline
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return 0;
    }
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    if ( IsSingleLine() )
    {
        wxASSERT_MSG( y == 0, _T("invalid XYToPosition() parameter") );

        return x;
    }
    else // multiline
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return -1;
    }
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( IsSingleLine() )
    {
        if ( x )
            *x = m_curPos;
        if ( y )
            *y = 0;

        return TRUE;
    }
    else // multiline
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));

        return FALSE;
    }
}

void wxTextCtrl::ShowPosition(long pos)
{
    wxFAIL_MSG(_T("not implemented"));
}

// ----------------------------------------------------------------------------
// clipboard stuff
// ----------------------------------------------------------------------------

void wxTextCtrl::Copy()
{
    wxFAIL_MSG(_T("not implemented"));
}

void wxTextCtrl::Cut()
{
    wxFAIL_MSG(_T("not implemented"));
}

void wxTextCtrl::Paste()
{
    wxFAIL_MSG(_T("not implemented"));
}


void wxTextCtrl::Undo()
{
    wxFAIL_MSG(_T("not implemented"));
}

void wxTextCtrl::Redo()
{
    wxFAIL_MSG(_T("not implemented"));
}


bool wxTextCtrl::CanUndo() const
{
    return FALSE;
}

bool wxTextCtrl::CanRedo() const
{
    return FALSE;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxTextCtrl::DoGetBestClientSize() const
{
    wxCoord w, h;
    GetTextExtent(GetLineText(0), &w, &h);

    int wChar = GetCharWidth(),
        hChar = GetCharHeight();

    if ( w < wChar )
        w = 8*wChar;
    if ( h < hChar )
        h = hChar;

    w += 2*wChar;

    return wxSize(w, h);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxTextCtrl::DoDraw(wxControlRenderer *renderer)
{
    if ( IsSingleLine() )
    {
        // just redraw everything
        renderer->GetDC().DrawText(m_value, 0, 0);
    }
    else
    {
        // TODO
    }
}

void wxTextCtrl::ShowCaret(bool show)
{
    wxCaret *caret = GetCaret();
    if ( caret )
    {
        caret->Show(show);

        if ( caret->IsVisible() )
        {
            // position it correctly
            wxString textBeforeCaret(m_value, (size_t)m_curPos);
            wxCoord x;
            GetTextExtent(textBeforeCaret, &x, NULL);

            caret->Move(x + 1, 0);
        }
    }
}

// ----------------------------------------------------------------------------
// input
// ----------------------------------------------------------------------------

wxString wxTextCtrl::GetInputHandlerType() const
{
    return wxINP_HANDLER_TEXTCTRL;
}

bool wxTextCtrl::PerformAction(const wxControlAction& action,
                               long numArg,
                               const wxString& strArg)
{
    return wxControl::PerformAction(action, numArg, strArg);
}

#endif // wxUSE_TEXTCTRL
