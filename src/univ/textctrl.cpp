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

/*
   Search for "OPT" for possible optimizations
 */

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
    #include "wx/log.h"

    #include "wx/dcclient.h"
    #include "wx/validate.h"
    #include "wx/textctrl.h"
#endif

#include "wx/clipbrd.h"

#include "wx/caret.h"

#include "wx/univ/inphand.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// turn extra wxTextCtrl-specific debugging on/off
#define WXDEBUG_TEXT

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline void OrderPositions(long& from, long& to)
{
    if ( from > to )
    {
        long tmp = from;
        from = to;
        to = tmp;
    }
}

// ============================================================================
// implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(OnChar)
    EVT_SIZE(OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxTextCtrl::Init()
{
    m_selAnchor =
    m_selStart =
    m_selEnd = -1;

    m_isModified = FALSE;
    m_isEditable = TRUE;

    m_colStart = 0;
    m_ofsHorz = 0;

    m_colLastVisible = -1;
    m_posLastVisible = -1;

    m_curPos =
    m_curRow =
    m_curLine = 0;
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

    SetCursor(wxCURSOR_IBEAM);

    SetValue(value);
    SetBestSize(size);

    CreateCaret();
    InitInsertionPoint();

    // we can't show caret right now as we're not shown yet and so it would
    // result in garbage on the screen - we'll do it after first OnPaint()
    m_hasCaret = FALSE;

    return TRUE;
}

bool wxTextCtrl::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return FALSE;

    // recreate it, in fact
    CreateCaret();

    // and refresh everything, of course
    InitInsertionPoint();
    ClearSelection();
    Refresh();

    return TRUE;
}

// ----------------------------------------------------------------------------
// set/get the value
// ----------------------------------------------------------------------------

void wxTextCtrl::SetValue(const wxString& value)
{
    if ( m_value == value )
        return;

    Replace(0, GetLastPosition(), value);
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
    wxCHECK_RET( from >= 0 && to >= 0 && from <= to && to <= GetLastPosition(),
                 _T("invalid range in wxTextCtrl::Replace") );

    // replace the part of the text with the new value
    wxString valueNew(m_value, (size_t)from);

    // remember it for later use
    wxCoord startNewText = GetTextWidth(valueNew);

    // if we really replace something, refresh till the end of line as all
    // remaining text in it is affected, but if we just added some text to the
    // end of line, we only need to refresh the area occupied by this text
    // refresh to the end of the line
    wxCoord widthNewText;

    valueNew += text;
    if ( (size_t)to < m_value.length() )
    {
        valueNew += m_value.c_str() + (size_t)to;

        // refresh till the end of line
        widthNewText = 0;
    }
    else // text appended, not replaced
    {
        // refresh only the new text
        widthNewText = GetTextWidth(text);
    }

    m_value = valueNew;

    // force m_colLastVisible update
    m_colLastVisible = -1;

    // update the current position
    SetInsertionPoint(from + text.length());

    // and the selection (do it after setting the cursor to have correct value
    // for selection anchor)
    ClearSelection();

    // repaint
    RefreshPixelRange(0, startNewText, widthNewText);

    // TODO: and the affected parts of the next line(s)
}

void wxTextCtrl::Remove(long from, long to)
{
    // Replace() only works with correctly ordered arguments, so exchange them
    // if necessary
    OrderPositions(from, to);

    Replace(from, to, _T(""));
}

void wxTextCtrl::WriteText(const wxString& text)
{
    // replace the selection with the new text
    RemoveSelection();

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
                 _T("insertion point position out of range") );

    // don't do anything if it didn't change
    if ( pos != m_curPos )
    {
        DoSetInsertionPoint(pos);
    }
}

void wxTextCtrl::InitInsertionPoint()
{
    // so far always put it in the beginning
    DoSetInsertionPoint(0);
}

void wxTextCtrl::DoSetInsertionPoint(long pos)
{
    HideCaret();

    m_curPos = pos;

    if ( IsSingleLine() )
    {
        m_curLine = 0;
        m_curRow = m_curPos;

        // the current position should always be shown, scroll the window
        // for this if necessary
        ShowHorzPosition(GetCaretPosition());
    }
    else // multi line
    {
        wxFAIL_MSG(_T("unimplemented for multi line"));
    }

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

wxString wxTextCtrl::GetSelectionText() const
{
    wxString sel;

    if ( HasSelection() )
    {
        if ( IsSingleLine() )
        {
            sel = wxString(m_value.c_str() + m_selStart,
                           m_value.c_str() + m_selEnd);
        }
        else
        {
            wxFAIL_MSG(_T("unimplemented for multi line"));
        }
    }

    return sel;
}

void wxTextCtrl::SetSelection(long from, long to)
{
    if ( from == -1 || to == from )
    {
        ClearSelection();
    }
    else // valid sel range
    {
        if ( from >= to )
        {
            long tmp = from;
            from = to;
            to = tmp;
        }

        wxCHECK_RET( to <= GetLastPosition(),
                     _T("invalid range in wxTextCtrl::SetSelection") );

        if ( from != m_selStart || to != m_selEnd )
        {
            // we need to use temp vars as RefreshTextRange() may call DoDraw()
            // directly and so m_selStart/End must be reset by then
            long selStartOld = m_selStart,
                 selEndOld = m_selEnd;

            m_selStart = from;
            m_selEnd = to;

            wxLogTrace(_T("text"), _T("Selection range is %ld-%ld"),
                       m_selStart, m_selEnd);

            // refresh only the part of text which became (un)selected if
            // possible
            if ( selStartOld == m_selStart )
            {
                RefreshTextRange(selEndOld, m_selEnd);
            }
            else if ( selEndOld == m_selEnd )
            {
                RefreshTextRange(m_selStart, selStartOld);
            }
            else
            {
                // OPT: could check for other cases too but it is probably not
                //      worth it as the two above are the most common ones
                if ( selStartOld != -1 )
                    RefreshTextRange(selStartOld, selEndOld);
                if ( m_selStart != -1 )
                    RefreshTextRange(m_selStart, m_selEnd);
            }
        }
        //else: nothing to do
    }
}

void wxTextCtrl::ClearSelection()
{
    if ( HasSelection() )
    {
        // we need to use temp vars as RefreshTextRange() may call DoDraw()
        // directly (see above as well)
        long selStart = m_selStart,
             selEnd = m_selEnd;

        // no selection any more
        m_selStart =
        m_selEnd = -1;

        // refresh the old selection
        RefreshTextRange(selStart, selEnd);
    }

    // the anchor should be moved even if there was no selection previously
    m_selAnchor = m_curPos;
}

void wxTextCtrl::RemoveSelection()
{
    if ( !HasSelection() )
        return;

    Remove(m_selStart, m_selEnd);
}

bool wxTextCtrl::GetSelectedPartOfLine(long line, int *start, int *end) const
{
    if ( start )
        *start = -1;
    if ( end )
        *end = -1;

    if ( !HasSelection() )
    {
        // no selection at all, hence no selection in this line
        return FALSE;
    }

    long lineStart, colStart;
    PositionToXY(m_selStart, &colStart, &lineStart);
    if ( lineStart > line )
    {
        // this line is entirely above the selection
        return FALSE;
    }

    long lineEnd, colEnd;
    PositionToXY(m_selEnd, &colEnd, &lineEnd);
    if ( lineEnd < line )
    {
        // this line is entirely below the selection
        return FALSE;
    }

    if ( line == lineStart )
    {
        if ( start )
            *start = colStart;
        if ( end )
            *end = lineEnd == lineStart ? colEnd : GetLineLength(line);
    }
    else if ( line == lineEnd )
    {
        if ( start )
            *start = lineEnd == lineStart ? colStart : 0;
        if ( end )
            *end = colEnd;
    }
    else // the line is entirely inside the selection
    {
        if ( start )
            *start = 0;
        if ( end )
            *end = GetLineLength(line);
    }

    return TRUE;
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

        // the appearance of the control might have changed
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
    // note that this method should accept any values of x and y and return -1
    // if they are out of range
    if ( IsSingleLine() )
    {
        return x > GetLastPosition() || y > 0 ? -1 : x;
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
        if ( pos > GetLastPosition() )
            return FALSE;

        if ( x )
            *x = pos;
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

    // it shouldn't be possible to learn where the word starts in the password
    // text entry zone
    if ( IsPassword() )
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

    // it shouldn't be possible to learn where the word ends in the password
    // text entry zone
    if ( IsPassword() )
        return GetLastPosition();

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
#if wxUSE_CLIPBOARD
    if ( HasSelection() )
    {
        wxClipboardLocker clipLock;

        wxString text = GetTextToShow(GetSelectionText());
        wxTextDataObject *data = new wxTextDataObject(text);
        wxTheClipboard->SetData(data);
    }
#endif // wxUSE_CLIPBOARD
}

void wxTextCtrl::Cut()
{
    if ( HasSelection() )
    {
        Copy();

        RemoveSelection();
    }
}

void wxTextCtrl::Paste()
{
#if wxUSE_CLIPBOARD
    wxClipboardLocker clipLock;

    wxTextDataObject data;
    if ( wxTheClipboard->IsSupported(data.GetFormat())
            && wxTheClipboard->GetData(data) )
    {
        WriteText(data.GetText());
    }
#endif // wxUSE_CLIPBOARD
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
    GetTextExtent(GetTextToShow(GetLineText(0)), &w, &h);

    int wChar = GetCharWidth(),
        hChar = GetCharHeight();

    if ( w < wChar )
        w = 8*wChar;
    if ( h < hChar )
        h = hChar;

    wxRect rectText;
    rectText.width = w;
    rectText.height = h;
    wxRect rectTotal = GetRenderer()->GetTextTotalArea(this, rectText);
    return wxSize(rectTotal.width, rectTotal.height);
}

void wxTextCtrl::UpdateTextRect()
{
    m_rectText = GetRenderer()->
                    GetTextClientArea(this,
                                      wxRect(wxPoint(0, 0), GetClientSize()));

    UpdateLastVisible();
}

void wxTextCtrl::UpdateLastVisible()
{
    // OPT: estimate the correct value first, just adjust it later

    wxString text;
    wxCoord w, wOld;

    w =
    wOld = 0;

    m_colLastVisible = m_colStart - 1;

    const wxChar *pc = m_value.c_str() + (size_t)m_colStart;
    for ( ; *pc; pc++ )
    {
        text += *pc;
        wOld = w;
        w = GetTextWidth(text);
        if ( w > m_rectText.width )
        {
            // this char is too much
            break;
        }

        m_colLastVisible++;
    }

    m_posLastVisible = wOld;

    wxLogTrace(_T("text"), _T("Last visible column/position is %d/%ld"),
               m_colLastVisible, m_posLastVisible);
}

void wxTextCtrl::OnSize(wxSizeEvent& event)
{
    UpdateTextRect();

    event.Skip();
}

wxCoord wxTextCtrl::GetTextWidth(const wxString& text) const
{
    wxCoord w;
    GetTextExtent(GetTextToShow(text), &w, NULL);
    return w;
}

wxTextCtrlHitTestResult wxTextCtrl::HitTestLine(const wxString& line,
                                                wxCoord x,
                                                long *colOut) const
{
    wxTextCtrlHitTestResult res = wxTE_HT_ON_TEXT;

    int col;
    wxTextCtrl *self = wxConstCast(this, wxTextCtrl);
    wxClientDC dc(self);
    dc.SetFont(GetFont());
    self->DoPrepareDC(dc);

    wxCoord width;
    dc.GetTextExtent(line, &width, NULL);
    if ( x >= width )
    {
        // clicking beyond the end of line is equivalent to clicking at
        // the end of it
        col = line.length();

        res = wxTE_HT_AFTER;
    }
    else if ( x < 0 )
    {
        col = 0;

        res = wxTE_HT_BEFORE;
    }
    else // we're inside the line
    {
        // now calculate the column: first, approximate it with fixed-width
        // value and then calculate the correct value iteratively: note that
        // we use the first character of the line instead of (average)
        // GetCharWidth(): it is common to have lines of dashes, for example,
        // and this should give us much better approximation in such case
        dc.GetTextExtent(line[0], &width, NULL);
        col = x / width;
        if ( col < 0 )
        {
            col = 0;
        }
        else if ( (size_t)col > line.length() )
        {
            col = line.length();
        }

        // matchDir is -1 if we must move left, +1 to move right and 0 when
        // we're exactly on the character we need
        int matchDir = 0;
        for ( ;; )
        {
            // check that we didn't go beyond the line boundary
            if ( col < 0 )
            {
                col = 0;
                break;
            }
            if ( (size_t)col > line.length() )
            {
                col = line.length();
                break;
            }

            wxString strBefore(line, (size_t)col);
            dc.GetTextExtent(strBefore, &width, NULL);
            if ( width > x )
            {
                if ( matchDir == 1 )
                {
                    // we were going to the right and, finally, moved beyond
                    // the original position - stop on the previous one
                    col--;

                    break;
                }

                if ( matchDir == 0 )
                {
                    // we just started iterating, now we know that we should
                    // move to the left
                    matchDir = -1;
                }
                //else: we are still to the right of the target, continue
            }
            else // width < x
            {
                // invert the logic above
                if ( matchDir == -1 )
                {
                    // with the exception that we don't need to backtrack here
                    break;
                }

                if ( matchDir == 0 )
                {
                    // go to the right
                    matchDir = 1;
                }
            }

            // this is not supposed to happen
            wxASSERT_MSG( matchDir, _T("logic error in wxTextCtrl::HitTest") );

            if ( matchDir == 1 )
                col++;
            else
                col--;
        }
    }

    // check that we calculated it correctly
#ifdef WXDEBUG_TEXT
    if ( res == wxTE_HT_ON_TEXT )
    {
        wxCoord width1;
        wxString text = line.Left(col);
        dc.GetTextExtent(text, &width1, NULL);
        if ( (size_t)col < line.length() )
        {
            wxCoord width2;

            text += line[col];
            dc.GetTextExtent(text, &width2, NULL);

            wxASSERT_MSG( (width1 <= x) && (x < width2),
                          _T("incorrect HitTestLine() result") );
        }
        else // we return last char
        {
            wxASSERT_MSG( x >= width1, _T("incorrect HitTestLine() result") );
        }
    }
#endif // WXDEBUG_TEXT

    if ( colOut )
        *colOut = col;

    return res;
}

wxTextCtrlHitTestResult wxTextCtrl::HitTest(const wxPoint& pos,
                                            long *colOut, long *rowOut) const
{
    // is the point in the text area or to the right or below it?
    wxTextCtrlHitTestResult res = wxTE_HT_ON_TEXT;

    // we accept the window coords and adjust for both the client and text area
    // offsets ourselves
    int x, y;
    wxPoint pt = GetClientAreaOrigin();
    pt += m_rectText.GetPosition();
    CalcUnscrolledPosition(pos.x - pt.x, pos.y - pt.y, &x, &y);

    // row calculation is simple as we assume that all lines have the same
    // height
    int row = y / GetCharHeight();
    int rowMax = GetNumberOfLines() - 1;
    if ( row > rowMax )
    {
        // clicking below the text is the same as clicking on the last line
        row = rowMax;

        res = wxTE_HT_AFTER;
    }
    else if ( row < 0 )
    {
        row = 0;

        res = wxTE_HT_BEFORE;
    }

    // now find the position in the line
    wxTextCtrlHitTestResult res2 =
        HitTestLine(GetTextToShow(GetLineText(row)), x, colOut);
    if ( res == wxTE_HT_ON_TEXT )
    {
        res = res2;
    }

    if ( rowOut )
        *rowOut = row;

    return res;
}

// ----------------------------------------------------------------------------
// scrolling
// ----------------------------------------------------------------------------

/*
   wxTextCtrl has not one but two scrolling mechanisms: one is semi-automatic
   scrolling in both horizontal and vertical direction implemented using
   wxScrollHelper and the second one is manual scrolling implemented using
   m_ofsHorz and used by the single line controls without scroll bar.

   The first version (the standard one) always scrolls by fixed amount which is
   fine for vertical scrolling as all lines have the same height but is rather
   ugly for horizontal scrolling if proportional font is used. This is why we
   manually update and use m_ofsHorz which contains the length of the string
   which is hidden beyond the left borde. An important property of text
   controls using this kind of scrolling is that an entire number of characters
   is always shown and that parts of characters never appear on display -
   neither in the leftmost nor rightmost positions.

   Once again, for multi line controls m_ofsHorz is always 0 and scrolling is
   done as usual for wxScrollWindow.
 */

void wxTextCtrl::ShowHorzPosition(wxCoord pos)
{
    // pos is the logical position to show

    // m_ofsHorz is the fisrt logical position shown
    if ( pos < m_ofsHorz )
    {
        // scroll backwards
        long col;
        HitTestLine(GetValue(), pos, &col);
        ScrollText(col);
    }
    else
    {
        wxCoord width = m_rectText.width;
        if ( !width )
        {
            // if we are called from the ctor, m_rectText is not initialized
            // yet, so do it now
            UpdateTextRect();
            width = m_rectText.width;
        }

        // m_ofsHorz + width is the last logical position shown
        if ( pos > m_ofsHorz + width)
        {
            // scroll forward
            long col;
            HitTestLine(GetValue(), pos - width, &col);
            ScrollText(col + 1);
        }
    }
}

// scroll the window horizontally so that the first visible character becomes
// the one at this position
void wxTextCtrl::ScrollText(long col)
{
    wxASSERT_MSG( IsSingleLine(),
                  _T("ScrollText() is for single line controls only") );

    // never scroll beyond the left border
    if ( col < 0 )
        col = 0;

    // OPT: could only get the extent of the part of the string between col
    //      and m_colStart
    wxCoord ofsHorz = GetTextWidth(GetLineText(0).Left(col));

    if ( ofsHorz != m_ofsHorz )
    {
        // what is currently shown?
        if ( m_colLastVisible == -1 )
        {
            UpdateLastVisible();
        }

        // NB1: to scroll to the right, offset must be negative, hence the
        //      order of operands
        int dx = m_ofsHorz - ofsHorz;

        // NB2: we call Refresh() below which results in a call to
        //      DoDraw(), so we must update m_ofsHorz before calling it
        m_ofsHorz = ofsHorz;
        m_colStart = col;

        // scroll only the rectangle inside which there is the text
        if ( dx > 0 )
        {
            // scrolling to the right: we need to recalc the last visible
            // position beore scrolling in order to make it appear exactly at
            // the right edge of the text area after scrolling
            UpdateLastVisible();
        }
        else
        {
            // when scrolling to the left, we need to scroll the old rectangle
            // occupied by the text - then the area where there was no text
            // before will be refreshed and redrawn

            // but we still need to force updating after scrolling
            m_colLastVisible = -1;
        }

        wxRect rect = m_rectText;
        rect.width = m_posLastVisible;

        rect = ScrollNoRefresh(dx, 0, &rect);

        /*
           we need to manually refresh the part which ScrollWindow() doesn't
           refresh: indeed, if we had this:

                                   ********o

           where '*' is text and 'o' is blank area at the end (too small to
           hold the next char) then after scrolling by 2 positions to the left
           we're going to have

                                   ******RRo

           where 'R' is the area refreshed by ScrollWindow() - but we still
           need to refresh the 'o' at the end as it may be now big enough to
           hold the new character shifted into view.

           when we are scrolling to the right, we need to update this rect as
           well because it might have contained something before but doesn't
           contain anything any more
         */

        // we can combine both rectangles into one when scrolling to the left,
        // but we need two separate Refreshes() otherwise
        if ( dx > 0 )
        {
            // refresh the uncovered part on the left
            Refresh(TRUE, &rect);

            // and now the area on the right
            rect.x = m_rectText.x + m_posLastVisible;
            rect.width = m_rectText.width - m_posLastVisible;
        }
        else
        {
            // just extend the rect covering the uncovered area to the edge of
            // the text rect
            rect.width += m_rectText.width - m_posLastVisible;
        }

        Refresh(TRUE, &rect);
    }
}

void wxTextCtrl::CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const
{
    wxScrollHelper::CalcUnscrolledPosition(x + m_ofsHorz, y, xx, yy);
}

void wxTextCtrl::DoPrepareDC(wxDC& dc)
{
    // adjust the DC origin if the text is shifted
    if ( m_ofsHorz )
    {
        wxPoint pt = dc.GetDeviceOrigin();
        dc.SetDeviceOrigin(pt.x - m_ofsHorz, pt.y);
    }
}

// ----------------------------------------------------------------------------
// refresh
// ----------------------------------------------------------------------------

void wxTextCtrl::RefreshTextRange(long start, long end)
{
    wxCHECK_RET( start != -1 && end != -1,
                 _T("invalid RefreshTextRange() arguments") );

    // accept arguments in any order as it is more conenient for the caller
    OrderPositions(start, end);

    long colStart, lineStart;
    if ( !PositionToXY(start, &colStart, &lineStart) )
    {
        // the range is entirely beyond the end of the text, nothing to do
        return;
    }

    long colEnd, lineEnd;
    if ( !PositionToXY(end, &colEnd, &lineEnd) )
    {
        // the range spans beyond the end of text, refresh to the end
        colEnd = -1;
        lineEnd = GetNumberOfLines() - 1;
    }

    // refresh all lines one by one
    for ( long line = lineStart; line <= lineEnd; line++ )
    {
        // refresh the first line from the start of the range to the end, the
        // intermediate ones entirely and the last one from the beginning to
        // the end of the range
        long posStart = line == lineStart ? colStart : 0;
        long posCount;
        if ( (line != lineEnd) || (colEnd == -1) )
        {
            // intermediate line or the last one but we need to refresh it
            // until the end anyhow - do it
            posCount = wxSTRING_MAXLEN;
        }
        else // last line
        {
            // refresh just the positions in between the start and the end one
            posCount = colEnd - posStart;
        }

        RefreshLineRange(line, posStart, posCount);
    }
}

void wxTextCtrl::RefreshLineRange(long line, long start, long count)
{
    wxString text = GetLineText(line);

    RefreshPixelRange(line,
                      GetTextWidth(text.Left((size_t)start)),
                      GetTextWidth(text.Mid((size_t)start, (size_t)count)));
}

void wxTextCtrl::RefreshPixelRange(long line, wxCoord start, wxCoord width)
{
    wxCoord h = GetCharHeight();
    wxRect rect;
    rect.x = start - m_ofsHorz;
    rect.y = line*h;

    if ( width == 0 )
    {
        // till the end of line
        rect.width = m_rectText.width - rect.x;
    }
    else
    {
        // only part of line
        rect.width = width;
    }

    rect.height = h;

    // account for the text area offset
    rect.Offset(m_rectText.GetPosition());

    wxLogTrace(_T("text"), _T("Refreshing (%d, %d)-(%d, %d)"),
               rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);

    Refresh(TRUE, &rect);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

/*
   Several remarks about wxTextCtrl redraw logic:

   1. only the regions which must be updated are redrawn, this means that we
      never Refresh() the entire window but use RefreshPixelRange() and
      ScrollWindow() which only refresh small parts of it and iterate over the
      update region in our DoDraw()

   2. the text displayed on the screen is obtained using GetTextToShow(): it
      should be used for all drawing/measuring
 */

wxString wxTextCtrl::GetTextToShow(const wxString& text) const
{
    wxString textShown;
    if ( IsPassword() )
        textShown = wxString(_T('*'), text.length());
    else
        textShown = text;

    return textShown;
}

void wxTextCtrl::DrawTextLine(wxDC& dc, const wxRect& rect,
                              const wxString& text,
                              long selStart, long selEnd)
{
    if ( selStart == -1 )
    {
        // just draw it as is
        dc.DrawText(text, rect.x, rect.y);
    }
    else // we have selection
    {
        wxCoord width,
                x = rect.x;

        // draw the part before selection
        wxString s(text, (size_t)selStart);
        if ( !s.empty() )
        {
            dc.DrawText(s, x, rect.y);

            dc.GetTextExtent(s, &width, NULL);
            x += width;
        }

        // draw the selection itself
        s = wxString(text.c_str() + selStart, text.c_str() + selEnd);
        if ( !s.empty() )
        {
            wxColour colFg = dc.GetTextForeground();
            dc.SetTextForeground(wxTHEME_COLOUR(HIGHLIGHT_TEXT));
            dc.SetTextBackground(wxTHEME_COLOUR(HIGHLIGHT));
            dc.SetBackgroundMode(wxSOLID);

            dc.DrawText(s, x, rect.y);
            dc.GetTextExtent(s, &width, NULL);
            x += width;

            dc.SetBackgroundMode(wxTRANSPARENT);
            dc.SetTextForeground(colFg);
        }

        // draw the final part
        s = text.c_str() + selEnd;
        if ( !s.empty() )
        {
            dc.DrawText(s, x, rect.y);
        }
    }
}

void wxTextCtrl::DoDrawTextInRect(wxDC& dc, const wxRect& rectUpdate)
{
    // debugging trick to see the update rect visually
#ifdef WXDEBUG_TEXT
    if ( 0 )
    {
        wxWindowDC dc(this);
        static int s_count = 0;
        dc.SetBrush(*(++s_count % 2 ? wxRED_BRUSH : wxGREEN_BRUSH));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rectUpdate);
    }
#endif // WXDEBUG_TEXT

    // calculate the range of lines to refresh
    wxPoint pt1 = rectUpdate.GetPosition();
    long colStart, lineStart;
    (void)HitTest(pt1, &colStart, &lineStart);

    wxPoint pt2 = pt1;
    pt2.x += rectUpdate.width;
    pt2.y += rectUpdate.height;
    long colEnd, lineEnd;
    (void)HitTest(pt2, &colEnd, &lineEnd);

    // pt1 and pt2 will run along the left and right update rect borders
    // respectively from top to bottom (NB: they're in device coords)
    pt2.y = pt1.y;

    // prepare for drawing
    wxRect rectText;
    rectText.height = GetCharHeight();
    rectText.y = m_rectText.y + lineStart*rectText.height;

    // do draw the invalidated parts of each line
    for ( long line = lineStart;
          line <= lineEnd;
          line++,
          rectText.y += rectText.height,
          pt1.y += rectText.height,
          pt2.y += rectText.height )
    {
        // calculate the update rect in text positions for this line
        if ( HitTest(pt1, &colStart, NULL) == wxTE_HT_AFTER )
        {
            // the update rect is beyond the end of line, no need to redraw
            // anything
            continue;
        }

        // don't show the columns which are scrolled out to the left
        if ( colStart < m_colStart )
            colStart = m_colStart;

        (void)HitTest(pt2, &colEnd, NULL);

        // colEnd may be less than colStart if colStart was changed by the
        // assignment above
        if ( colEnd < colStart )
            colEnd = colStart;

        // don't draw the chars beyond the rightmost one
        if ( m_colLastVisible == -1 )
        {
            // recalculate this rightmost column
            UpdateLastVisible();
        }

        if ( colStart > m_colLastVisible )
        {
            // don't bother redrawing something that is beyond the last
            // visible position
            continue;
        }

        if ( colEnd > m_colLastVisible )
            colEnd = m_colLastVisible;

        // extract the part of line we need to redraw
        wxString textLine = GetLineText(line);
        wxString text = textLine.Mid(colStart, colEnd - colStart + 1);

        // now deal with the selection
        int selStart, selEnd;
        GetSelectedPartOfLine(line, &selStart, &selEnd);

        if ( selStart != -1 )
        {
            // these values are relative to the start of the line while the
            // string passed to DrawTextLine() is only part of it, so adjust
            // the selection range accordingly
            selStart -= colStart;
            selEnd -= colStart;

            if ( selStart < 0 )
                selStart = 0;

            if ( (size_t)selEnd >= text.length() )
                selEnd = text.length();
        }

        // calculate the logical text coords
        rectText.x = m_rectText.x + GetTextWidth(textLine.Left(colStart));
        rectText.width = GetTextWidth(text);

        // do draw the text
        DrawTextLine(dc, rectText, text, selStart, selEnd);
        wxLogTrace(_T("text"), _T("Line %ld: positions %ld-%ld redrawn."),
                   line, colStart, colEnd);
    }
}

void wxTextCtrl::DoDraw(wxControlRenderer *renderer)
{
    // hide the caret while we're redrawing the window and show it after we are
    // done with it
    wxCaretSuspend cs(this);

    // prepare the DC
    wxDC& dc = renderer->GetDC();
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());

    // get the intersection of the update region with the text area: note that
    // the update region is in window coord and text area is in the client
    // ones, so it must be shifted before computing intesection
    wxRegion rgnUpdate = GetUpdateRegion();
    wxRect rectTextArea = m_rectText;
    wxPoint pt = GetClientAreaOrigin();
    rectTextArea.x += pt.x;
    rectTextArea.y += pt.y;
    rgnUpdate.Intersect(rectTextArea);

#if 0
    // even though the drawing is already clipped to the update region, we must
    // explicitly clip it to the rect we will use as otherwise parts of letters
    // might be drawn outside of it (if even a small part of a charater is
    // inside, HitTest() will return its column and DrawText() can't draw only
    // the part of the character, of course)
    dc.SetClippingRegion(m_rectText);
#endif // 0

    // adjust for scrolling
    DoPrepareDC(dc);

    // and now refresh thei nvalidated parts of the window
    wxRegionIterator iter(rgnUpdate);
    for ( ; iter.HaveRects(); iter++ )
    {
        wxRect r = iter.GetRect();
        if ( !r.width || !r.height )
        {
            // this happens under wxGTK
            continue;
        }

        DoDrawTextInRect(dc, r);
    }

    // show caret first time only
    if ( !m_hasCaret )
    {
        GetCaret()->Show();

        m_hasCaret = TRUE;
    }

}

// ----------------------------------------------------------------------------
// caret
// ----------------------------------------------------------------------------

void wxTextCtrl::CreateCaret()
{
    // FIXME use renderer
    wxCaret *caret = new wxCaret(this, 1, GetCharHeight());
#ifndef __WXMSW__
    caret->SetBlinkTime(0);
#endif // __WXMSW__

    // SetCaret() will delete the old caret if any
    SetCaret(caret);
}

wxCoord wxTextCtrl::GetCaretPosition() const
{
    wxString textBeforeCaret(GetLineText(m_curLine), (size_t)m_curRow);

    return GetTextWidth(textBeforeCaret);
}

void wxTextCtrl::ShowCaret(bool show)
{
    wxCaret *caret = GetCaret();
    if ( caret )
    {
        // position it correctly
        caret->Move(m_rectText.x + GetCaretPosition() - m_ofsHorz,
                    m_rectText.y + m_curLine*GetCharHeight());

        // and show it there
        caret->Show(show);
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
    bool del = FALSE,
         sel = FALSE;
    if ( actionOrig.StartsWith(wxACTION_TEXT_PREFIX_DEL, &action) )
    {
        del = TRUE;
    }
    else if ( actionOrig.StartsWith(wxACTION_TEXT_PREFIX_SEL, &action) )
    {
        sel = TRUE;
    }
    else // not selection nor delete action
    {
        action = actionOrig;
    }

    // set newPos to -2 as it can't become equal to it in the assignments below
    // (but it can become -1)
    static const long INVALID_POS_VALUE = -2;

    long newPos = INVALID_POS_VALUE;

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
    else if ( action == wxACTION_TEXT_SEL_WORD )
    {
        SetSelection(GetWordStart(), GetWordEnd());
    }
    else if ( action == wxACTION_TEXT_ANCHOR_SEL )
    {
        newPos = numArg;
    }
    else if ( action == wxACTION_TEXT_EXTEND_SEL )
    {
        SetSelection(m_selAnchor, numArg);
    }
    else if ( action == wxACTION_TEXT_COPY )
    {
        Copy();
    }
    else if ( action == wxACTION_TEXT_CUT )
    {
        Cut();
    }
    else if ( action == wxACTION_TEXT_PASTE )
    {
        Paste();
    }
    else
    {
        return wxControl::PerformAction(action, numArg, strArg);
    }

    if ( newPos != INVALID_POS_VALUE )
    {
        // bring the new position into the range
        if ( newPos < 0 )
            newPos = 0;

        long posLast = GetLastPosition();
        if ( newPos > posLast )
            newPos = posLast;

        if ( del )
        {
            // if we have the selection, remove just it
            if ( HasSelection() )
            {
                RemoveSelection();
            }
            else
            {
                // otherwise delete everything between current position and
                // the new one
                if ( m_curPos != newPos )
                {
                    Remove(m_curPos, newPos);

                    textChanged = TRUE;
                }
            }
        }
        else // cursor movement command
        {
            // just go there
            SetInsertionPoint(newPos);

            if ( sel )
            {
                SetSelection(m_selAnchor, m_curPos);
            }
            else // simple movement
            {
                // clear the existing selection
                ClearSelection();
            }
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
        if ( keycode == WXK_RETURN )
        {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, GetId());
            InitCommandEvent(event);
            event.SetString(GetValue());
            GetEventHandler()->ProcessEvent(event);
        }
        else if ( keycode < 255 &&
                  keycode != WXK_DELETE &&
                  keycode != WXK_BACK )
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
    m_winCapture = (wxTextCtrl *)NULL;
}

/* static */
long wxStdTextCtrlInputHandler::HitTest(const wxTextCtrl *text,
                                        const wxPoint& pos)
{
    long col, row;
    (void)text->HitTest(pos, &col, &row);

    return text->XYToPosition(col, row);
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

        // something else
        default:
            // reset the action as it could be already set to one of the
            // prefixes
            action = wxACTION_NONE;

            if ( ctrlDown )
            {
                switch ( keycode )
                {
                    case 'C':
                        action = wxACTION_TEXT_COPY;
                        break;

                    case 'V':
                        action = wxACTION_TEXT_PASTE;
                        break;

                    case 'X':
                        action = wxACTION_TEXT_CUT;
                        break;
                }
            }
    }

    if ( action != wxACTION_NONE )
    {
        control->PerformAction(action, -1, str);

        return TRUE;
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdTextCtrlInputHandler::HandleMouse(wxControl *control,
                                            const wxMouseEvent& event)
{
    if ( event.LeftDown() )
    {
        wxASSERT_MSG( !m_winCapture, _T("left button going down twice?") );

        wxTextCtrl *text = wxStaticCast(control, wxTextCtrl);

        m_winCapture = text;
        m_winCapture->CaptureMouse();

        text->HideCaret();

        long pos = HitTest(text, event.GetPosition());
        if ( pos != -1 )
        {
            text->PerformAction(wxACTION_TEXT_ANCHOR_SEL, pos);
        }
    }
    else if ( event.LeftDClick() )
    {
        // select the word the cursor is on
        control->PerformAction(wxACTION_TEXT_SEL_WORD);
    }
    else if ( event.LeftUp() )
    {
        if ( m_winCapture )
        {
            m_winCapture->ShowCaret();

            m_winCapture->ReleaseMouse();
            m_winCapture = (wxTextCtrl *)NULL;
        }
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdTextCtrlInputHandler::HandleMouseMove(wxControl *control,
                                                const wxMouseEvent& event)
{
    if ( m_winCapture )
    {
        // track it
        wxTextCtrl *text = wxStaticCast(m_winCapture, wxTextCtrl);
        long pos = HitTest(text, event.GetPosition());
        if ( pos != -1 )
        {
            text->PerformAction(wxACTION_TEXT_EXTEND_SEL, pos);
        }
    }

    return wxStdInputHandler::HandleMouseMove(control, event);
}

#endif // wxUSE_TEXTCTRL
