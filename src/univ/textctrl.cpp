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
    EVT_CHAR(OnChar)
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

    // FIXME use renderer
    wxCaret *caret = new wxCaret(this, 2, GetCharHeight());
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
    if ( m_value == value )
        return;

    m_value = value;

    if ( IsSingleLine() )
    {
        SetInsertionPointEnd();
    }
    else
    {
        SetInsertionPoint(0);
    }

    Refresh();
}

wxString wxTextCtrl::GetValue() const
{
    return m_value;
}

void wxTextCtrl::Clear()
{
    SetValue(_T(""));
}

void wxTextCtrl::Replace(long from, long to, const wxString& text)
{
    wxCHECK_RET( from >= 0 && to >= 0 && from <= to,
                 _T("invalid range in wxTextCtrl::Replace") );

    // replace the part of the text with the new value
    wxString valueNew(m_value, (size_t)from);
    valueNew += text;
    if ( (unsigned long)to < m_value.length() )
    {
        valueNew += m_value.c_str() + (size_t)to;
    }
    m_value = valueNew;

    // update current position
    SetInsertionPoint(from + text.length());

    // FIXME shouldn't refresh everything of course
    Refresh();
}

void wxTextCtrl::Remove(long from, long to)
{
    if ( from > to )
    {
        // Replace() only works with correctly ordered arguments, so exchange
        // them
        long tmp = from;
        from = to;
        to = tmp;
    }

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
    wxCHECK_RET( pos >= 0 && pos <= GetLastPosition(),
                 _T("insertion poitn position out of range") );

    if ( pos != m_curPos )
    {
        HideCaret();

        m_curPos = pos;

        if ( IsSingleLine() )
        {
            m_curLine = 0;
            m_curRow = m_curPos;
        }
        else // multi line
        {
            wxFAIL_MSG(_T("unimplemented for multi line"));
        }

        ShowCaret();
    }
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
// word stuff
// ----------------------------------------------------------------------------

/*
    TODO: we could have (easy to do) vi-like options for word movement, i.e.
          distinguish between inlusive/exclusive words and between words and
          WORDS (in vim sense) and also, finally, make the set of characters
          which make up a word configurable - currently we use the exclusive
          WORDS only (coincidentally, this is what Windows edit control does)

          For future references, here is what vim help says:

          A word consists of a sequence of letters, digits and underscores, or
          a sequence of other non-blank characters, separated with white space
          (spaces, tabs, <EOL>).  This can be changed with the 'iskeyword'
          option.

          A WORD consists of a sequence of non-blank characters, separated with
          white space.  An empty line is also considered to be a word and a
          WORD.
 */

static inline bool IsWordChar(wxChar ch)
{
    return !wxIsspace(ch);
}

long wxTextCtrl::GetWordStart() const
{
    if ( m_curPos == -1 || m_curPos == 0 )
        return 0;

    // start at the previous position
    const wxChar *p0 = m_value.c_str();
    const wxChar *p = p0 + m_curPos - 1;

    // find the end of the previous word
    while ( (p > p0) && !IsWordChar(*p) )
        p--;

    // now find the beginning of this word
    while ( (p > p0) && IsWordChar(*p) )
        p--;

    // we might have gone too far
    if ( !IsWordChar(*p) )
        p++;

    return p - p0;
}

long wxTextCtrl::GetWordEnd() const
{
    if ( m_curPos == -1 )
        return 0;

    // start at the current position
    const wxChar *p0 = m_value.c_str();
    const wxChar *p = p0 + m_curPos;

    // find the start of the next word
    while ( *p && !IsWordChar(*p) )
        p++;

    // now find the end of it
    while ( *p && IsWordChar(*p) )
        p++;

    // and find the start of the next word
    while ( *p && !IsWordChar(*p) )
        p++;

    return p - p0;
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
    h += hChar / 2;

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
        renderer->DrawTextLine(m_value);
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

            // FIXME: use renderer
            caret->Move(x + 3, 3);
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

bool wxTextCtrl::PerformAction(const wxControlAction& actionOrig,
                               long numArg,
                               const wxString& strArg)
{
    bool textChanged = FALSE;

    wxString action;
    bool del = FALSE;
    if ( actionOrig.StartsWith(wxACTION_TEXT_PREFIX_DEL, &action) )
    {
        del = TRUE;
    }
    else // not selection nor delete action
    {
        action = actionOrig;
    }

    long newPos = -1;
    if ( action == wxACTION_TEXT_HOME )
    {
        newPos = m_curPos - m_curRow;
    }
    else if ( action == wxACTION_TEXT_END )
    {
        newPos = m_curPos + GetLineLength(m_curLine) - m_curRow;
    }
    else if ( action == wxACTION_TEXT_LEFT )
    {
        newPos = m_curPos - 1;
    }
    else if ( action == wxACTION_TEXT_WORD_LEFT )
    {
        newPos = GetWordStart();
    }
    else if ( action == wxACTION_TEXT_RIGHT )
    {
        newPos = m_curPos + 1;
    }
    else if ( action == wxACTION_TEXT_WORD_RIGHT )
    {
        newPos = GetWordEnd();
    }
    else if ( action == wxACTION_TEXT_INSERT )
    {
        if ( !strArg.empty() )
        {
            WriteText(strArg);

            textChanged = TRUE;
        }
    }
    else
    {
        return wxControl::PerformAction(action, numArg, strArg);
    }

    if ( newPos != -1 )
    {
        // bring the new position into the range
        if ( newPos < 0 )
            newPos = 0;

        long posLast = GetLastPosition();
        if ( newPos > posLast )
            newPos = posLast;

        if ( del )
        {
            // delete everything between current opsition and the new one
            if ( m_curPos != newPos )
            {
                Remove(m_curPos, newPos);

                textChanged = TRUE;
            }
        }
        else
        {
            // just go there
            SetInsertionPoint(newPos);
        }
    }

    if ( textChanged )
    {
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());
        InitCommandEvent(event);
        event.SetString(GetValue());
        GetEventHandler()->ProcessEvent(event);
    }

    return TRUE;
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    // only process the key events from "simple keys" here
    if ( !event.HasModifiers() )
    {
        int keycode = event.GetKeyCode();
        if ( keycode != WXK_DELETE && keycode != WXK_BACK )
        {
            PerformAction(wxACTION_TEXT_INSERT, -1, (wxChar)keycode);

            // skip event.Skip() below
            return;
        }
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxStdTextCtrlInputHandler
// ----------------------------------------------------------------------------

wxStdTextCtrlInputHandler::wxStdTextCtrlInputHandler(wxInputHandler *inphand)
                         : wxStdInputHandler(inphand)
{
}

bool wxStdTextCtrlInputHandler::HandleKey(wxControl *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    if ( !pressed )
        return FALSE;

    wxControlAction action;
    wxString str;
    bool ctrlDown = event.ControlDown();
    if ( event.ShiftDown() )
    {
        action = wxACTION_TEXT_PREFIX_SEL;
    }

    int keycode = event.GetKeyCode();
    switch ( keycode )
    {
        // cursor movement
        case WXK_HOME:
            action << wxACTION_TEXT_HOME;
            break;

        case WXK_END:
            action << wxACTION_TEXT_END;
            break;

        case WXK_LEFT:
            action << (ctrlDown ? wxACTION_TEXT_WORD_LEFT
                                : wxACTION_TEXT_LEFT);
            break;

        case WXK_RIGHT:
            action << (ctrlDown ? wxACTION_TEXT_WORD_RIGHT
                                : wxACTION_TEXT_RIGHT);
            break;

        // delete
        case WXK_DELETE:
            action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_RIGHT;
            break;

        case WXK_BACK:
            action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_LEFT;
            break;
    }

    if ( !!action )
    {
        control->PerformAction(action, -1, str);

        return TRUE;
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdTextCtrlInputHandler::HandleMouse(wxControl *control,
                                            const wxMouseEvent& event)
{
    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdTextCtrlInputHandler::HandleMouseMove(wxControl *control,
                                                const wxMouseEvent& event)
{
    return wxStdInputHandler::HandleMouseMove(control, event);
}

#endif // wxUSE_TEXTCTRL
