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

   Possible optimization would be to always store the coords in the text in
   triplets (pos, col, line) and update them simultaneously instead of
   recalculating col and line from pos each time it is needed. Currently we
   only do it for the current position but we might also do it for the
   selection start and end.
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
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#include "wx/caret.h"

#include "wx/univ/inphand.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// turn extra wxTextCtrl-specific debugging on/off
#define WXDEBUG_TEXT

// turn wxTextCtrl::Replace() debugging on (very inefficient!)
#define WXDEBUG_TEXT_REPLACE

#ifndef __WXDEBUG__
    #undef WXDEBUG_TEXT
    #undef WXDEBUG_TEXT_REPLACE
#endif

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

    EVT_IDLE(OnIdle)
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

    m_posLast =
    m_curPos =
    m_curCol =
    m_curRow = 0;

    m_scrollRangeX =
    m_scrollRangeY = 0;

    m_updateScrollbarX =
    m_updateScrollbarY = FALSE;

    m_widthMax = -1;

    // init wxScrollHelper
    SetWindow(this);
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

    if ( style & wxTE_MULTILINE )
    {
        // we should always have at least one line in a multiline control
        m_lines.Add(wxEmptyString);

        // we might support it but it's quite useless and other ports don't
        // support it anyhow
        wxASSERT_MSG( !(style & wxTE_PASSWORD),
                      _T("wxTE_PASSWORD can't be used with multiline ctrls") );

        // create vertical scrollbar if necessary (on by default)
        if ( !(style & wxTE_NO_VSCROLL) )
        {
        }

        // and the horizontal one
        if ( style & wxHSCROLL )
        {
        }
    }

    SetValue(value);
    SetBestSize(size);

    CreateCaret();
    InitInsertionPoint();

    // we can't show caret right now as we're not shown yet and so it would
    // result in garbage on the screen - we'll do it after first OnPaint()
    m_hasCaret = FALSE;

    return TRUE;
}

// ----------------------------------------------------------------------------
// set/get the value
// ----------------------------------------------------------------------------

void wxTextCtrl::SetValue(const wxString& value)
{
    if ( IsSingleLine() && (value == GetSLValue()) )
    {
        // nothing changed
        return;
    }

    Replace(0, GetLastPosition(), value);
}

wxString wxTextCtrl::GetValue() const
{
    wxString value;
    if ( IsSingleLine() )
    {
        value = GetSLValue();
    }
    else // multiline
    {
        size_t count = m_lines.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( n )
                value += _T('\n'); // from preceding line

            value += m_lines[n];
        }
    }

    return value;
}

void wxTextCtrl::Clear()
{
    SetValue(_T(""));
}

void wxTextCtrl::Replace(long from, long to, const wxString& text)
{
    long colStart, colEnd,
         lineStart, lineEnd;

    if ( (from > to) ||
         !PositionToXY(from, &colStart, &lineStart) ||
         !PositionToXY(to, &colEnd, &lineEnd) )
    {
        wxFAIL_MSG(_T("invalid range in wxTextCtrl::Replace"));

        return;
    }

#ifdef WXDEBUG_TEXT_REPLACE
    // a straighforward (but very inefficient) way of calculating what the new
    // value should be
    wxString textTotal = GetValue();
    wxString textTotalNew(textTotal, (size_t)from);
    textTotalNew += text;
    if ( (size_t)to < textTotal.length() )
        textTotalNew += textTotal.c_str() + (size_t)to;
#endif // WXDEBUG_TEXT_REPLACE

    // the first attempt at implementing Replace() - it was meant to be more
    // efficient than the current code but it also is much more complicated
    // and, worse, still has a few bugs and so as I'm not even sure any more
    // that it is really more efficient, I'm dropping it in favour of much
    // simpler code below
#if 0
/*
   The algorithm of Replace():

    1. change the line where replacement starts
        a) keep the text in the beginning of it unchanged
        b) replace the middle (if lineEnd == lineStart) or everything to the
           end with the first line of replacement text

    2. delete all lines between lineStart and lineEnd (excluding)

    3. insert the lines of the replacement text

    4. change the line where replacement ends:
        a) remove the part which is in replacement range
        b) insert the last line of replacement text
        c) insert the end of the first line if lineEnd == lineStart
        d) keep the end unchanged

   In the code below the steps 2 and 3 are merged and are done in parallel for
   efficiency reasons (it is better to change lines in place rather than
   remove/insert them from a potentially huge array)
 */

    // break the replacement text into lines
    wxArrayString lines = wxStringTokenize(text, _T("\n"),
                                           wxTOKEN_RET_EMPTY_ALL);
    size_t nReplaceCount = lines.GetCount();

    // first deal with the starting line: (1) take the part before the text
    // being replaced
    wxString lineFirstOrig = GetLineText(lineStart);
    wxString lineFirst(lineFirstOrig, (size_t)colStart);

    // remember the start of the update region for later use
    wxCoord startNewText = GetTextWidth(lineFirst);

    // (2) add the text which replaces this part
    if ( !lines.IsEmpty() )
    {
        lineFirst += lines[0u];
    }

    // (3) and add the text which is left if we replace text only in this line
    wxString lineFirstEnd;
    if ( lineEnd == lineStart )
    {
        wxASSERT_MSG((size_t)colEnd <= lineFirstOrig.length(), _T("logic bug"));

        lineFirstEnd = lineFirstOrig.c_str() + (size_t)colEnd;

        if ( nReplaceCount == 1 )
        {
            // we keep everything on this line
            lineFirst += lineFirstEnd;
        }
        else
        {
            lineEnd++;
        }
    }

    // (4) refresh the part of the line which changed

    // we usually refresh till the end of line except of the most common case
    // when some text is appended to the end of it in which case we refresh
    // just the newly appended text
    wxCoord widthNewText;
    if ( (lineEnd == lineStart) &&
            ((size_t)colStart == lineFirstOrig.length()) )
    {
        // text appended, not replaced, so refresh only the new text
        widthNewText = GetTextWidth(lines[0u]);
    }
    else
    {
        // refresh till the end of line as all its tail changed

        // OPT: should only refresh the part occupied by the text, but OTOH
        //      the line won't be really repainted beyond it anyhow due to the
        //      checks in DoDrawTextInRect(), so is it really worth it?

        widthNewText = 0;
    }

    RefreshPixelRange(lineStart, startNewText, widthNewText);

    // (5) modify the line
    if ( IsSingleLine() )
    {
        SetSLValue(lineFirst);

        // force m_colLastVisible update
        m_colLastVisible = -1;

        // consistency check: when replacing text in a single line control, we
        // shouldn't have more than one line
        wxASSERT_MSG(lines.GetCount() <= 1,
                     _T("can't have more than one line in this wxTextCtrl"));

        // update the current position
        SetInsertionPoint(from + text.length());

        // and the selection (do it after setting the cursor to have correct value
        // for selection anchor)
        ClearSelection();

        // nothing more can happen to us, so bail out
        return;
    }
    else // multiline
    {
        m_lines[lineStart] = lineFirst;
    }

    // now replace all intermediate lines entirely

    bool refreshAllBelow = FALSE;

    // (1) modify all lines which are really repaced
    size_t nReplaceLine = 1;
    for ( long line = lineStart + 1; line < lineEnd; line++ )
    {
        if ( nReplaceLine < nReplaceCount )
        {
            // replace line
            m_lines[line] = lines[nReplaceLine++];
        }
        else // no more replacement text - remove line(s)
        {
            // (2) remove all lines for which there is no more replacement
            //     text (this is slightly more efficient than continuing to
            //     run the loop)

            // adjust the index by the number of lines removed
            lineEnd -= lineEnd - line;

            // and remove them
            while ( line < lineEnd )
            {
                m_lines.RemoveAt(line++);
            }

            // the lines below will scroll up
            refreshAllBelow = TRUE;
        }
    }

    // all the rest is only necessary if there is more than one line of
    // replacement text
    if ( nReplaceCount > 1 )
    {
        // (3) if there are still lines left in the replacement text, insert
        //     them before modifying the last line
        if ( nReplaceLine < nReplaceCount - 1 )
        {
            // the lines below will scroll down
            refreshAllBelow = TRUE;

            while ( nReplaceLine < nReplaceCount - 1 ) // OPT: insert all at once?
            {
                // insert line and adjust for index change by incrementing lineEnd
                m_lines.Insert(lines[nReplaceLine++], lineEnd++);
            }
        }

        // (4) refresh the lines: if we had replaced exactly the same number of
        //     lines that we had before, we can just refresh these lines,
        //     otherwise the lines below will change as well, so we have to
        //     refresh them too
        if ( refreshAllBelow || (lineStart < lineEnd - 1) )
        {
            RefreshLineRange(lineStart + 1, refreshAllBelow ? -1 : lineEnd - 1);
        }

        // now deal with the last line: (1) replace its beginning with the end
        // of the replacement text
        wxString lineLast;
        if ( nReplaceLine < nReplaceCount )
        {
            wxASSERT_MSG(nReplaceLine == nReplaceCount - 1, _T("logic error"));

            lineLast = lines[nReplaceLine];
        }

        // (2) add the text which was at the end of first line if we replaced
        //     its middle with multiline text
        if ( lineEnd == lineStart )
        {
            lineLast += lineFirstEnd;
        }

        // (3) add the tail of the old last line if anything is left
        if ( (size_t)lineEnd < m_lines.GetCount() )
        {
            wxString lineLastOrig = GetLineText(lineEnd);
            if ( (size_t)colEnd < lineLastOrig.length() )
            {
                lineLast += lineLastOrig.c_str() + (size_t)colEnd;
            }

            m_lines[lineEnd] = lineLast;
        }
        else // the number of lines increased, just append the new one
        {
            m_lines.Add(lineLast);
        }

        // (4) always refresh the last line entirely if it hadn't been already
        //     refreshed above
        if ( !refreshAllBelow )
        {
            RefreshPixelRange(lineEnd, 0, 0); // entire line
        }
    }
    //else: only one line of replacement text

#else // 1 (new replacement code)

    if ( IsSingleLine() )
    {
        // replace the part of the text with the new value
        wxString valueNew(m_value, (size_t)from);

        // remember it for later use
        wxCoord startNewText = GetTextWidth(valueNew);

        valueNew += text;
        if ( (size_t)to < m_value.length() )
        {
            valueNew += m_value.c_str() + (size_t)to;
        }

        // OPT: is the following really ok? not sure any more now at 2 am...

        // we usually refresh till the end of line except of the most common case
        // when some text is appended to the end of the string in which case we
        // refresh just it
        wxCoord widthNewText;

        if ( (size_t)from < m_value.length() )
        {
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

        // repaint
        RefreshPixelRange(0, startNewText, widthNewText);
    }
    else // multiline
    {
        /*
           Join all the lines in the replacement range into one string, then
           replace a part of it with the new text and break it into lines again.
        */

        // (1) join lines
        wxString textOrig;
        long line;
        for ( line = lineStart; line <= lineEnd; line++ )
        {
            if ( line > lineStart )
            {
                // from previous line
                textOrig += _T('\n');
            }

            textOrig += m_lines[line];
        }

        // (2) replace text in the combined string
        wxString textNew(textOrig, colStart);

        // these values will be used to refresh the changed area below
        wxCoord widthNewText, startNewText = GetTextWidth(textNew);
        if ( (size_t)colStart == m_lines[lineStart].length() )
        {
            // text appended, refresh just enough to show the new text
            widthNewText = GetTextWidth(text.BeforeFirst(_T('\n')));
        }
        else // text inserted, refresh till the end of line
        {
            widthNewText = 0;
        }

        textNew += text;
        size_t toRel = (size_t)((to - from) + colStart); // adjust for index shift
        if ( toRel < textOrig.length() )
            textNew += textOrig.c_str() + toRel;

        // (3) break it into lines
        wxArrayString lines;
        if ( textNew.empty() )
        {
            // special case: if the replacement string is empty we still want to
            // have one (empty) string in the lines array but wxStringTokenize()
            // won't put anything in it in this case, so do it ourselves
            lines.Add(wxEmptyString);
        }
        else // break into lines normally
        {
           lines = wxStringTokenize(textNew, _T("\n"), wxTOKEN_RET_EMPTY_ALL);
        }

        size_t nReplaceCount = lines.GetCount(),
               nReplaceLine = 0;

        // (4) merge into the array

        size_t countOld = m_lines.GetCount();

        // (4a) replace
        for ( line = lineStart; line <= lineEnd; line++, nReplaceLine++ )
        {
            if ( nReplaceLine < nReplaceCount )
            {
                // we have the replacement line for this one
                m_lines[line] = lines[nReplaceLine];

                UpdateMaxWidth(line);
            }
            else // no more replacement lines
            {
                // (4b) delete all extra lines
                while ( line <= lineEnd )
                {
                    m_lines.RemoveAt(line++);
                }
            }
        }

        // (4c) insert the new lines
        while ( nReplaceLine < nReplaceCount )
        {
            m_lines.Insert(lines[nReplaceLine++], ++lineEnd);

            UpdateMaxWidth(lineEnd);
        }

        // (5) now refresh the changed area
        RefreshPixelRange(lineStart, startNewText, widthNewText);
        if ( m_lines.GetCount() == countOld )
        {
            // number of lines didn't change, refresh the updated lines and the
            // last one
            if ( lineStart < lineEnd )
                RefreshLineRange(lineStart + 1, lineEnd);
        }
        else
        {
            // number of lines did change, we need to refresh everything below
            // the start line
            RefreshLineRange(lineStart + 1,
                             wxMax(m_lines.GetCount(), countOld) - 1);

            // the vert scrollbar might [dis]appear
            m_updateScrollbarY = TRUE;
        }
#endif // 0/1

        // update the (cached) last position
        m_posLast += text.length() - to + from;
    }

    // update the current position: note that we always put the cursor at the
    // end of the replacement text
    SetInsertionPoint(from + text.length());

    // and the selection (do it after setting the cursor to have correct value
    // for selection anchor)
    ClearSelection();

#ifdef WXDEBUG_TEXT_REPLACE
    // optimized code above should give the same result as straightforward
    // computation in the beginning
    wxASSERT_MSG( GetValue() == textTotalNew, _T("error in Replace()") );
#endif // WXDEBUG_TEXT_REPLACE
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
    m_curPos = pos;
    PositionToXY(m_curPos, &m_curCol, &m_curRow);

    ShowPosition(m_curPos);
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
    long pos;
    if ( IsSingleLine() )
    {
        pos = GetSLValue().length();
    }
    else // multiline
    {
#ifdef WXDEBUG_TEXT
        pos = 0;
        size_t nLineCount = m_lines.GetCount();
        for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
        {
            // +1 is because the positions at the end of this line and of the
            // start of the next one are different
            pos += m_lines[nLine].length() + 1;
        }

        if ( pos > 0 )
        {
            // the last position is at the end of the last line, not in the
            // beginning of the next line after it
            pos--;
        }

        // more probable reason of this would be to forget to update m_posLast
        wxASSERT_MSG( pos == m_posLast, _T("bug in GetLastPosition()") );
#endif // WXDEBUG_TEXT

        pos = m_posLast;
    }

    return pos;
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
            sel = GetSLValue().Mid(m_selStart, m_selEnd - m_selStart);
        }
        else // multiline
        {
            long colStart, lineStart,
                 colEnd, lineEnd;
            PositionToXY(m_selStart, &colStart, &lineStart);
            PositionToXY(m_selEnd, &colEnd, &lineEnd);

            // as always, we need to check for the special case when the start
            // and end line are the same
            if ( lineEnd == lineStart )
            {
                sel = m_lines[lineStart].Mid(colStart, colEnd - colStart);
            }
            else // sel on multiple lines
            {
                // take the end of the first line
                sel = m_lines[lineStart].c_str() + colStart;
                sel += _T('\n');

                // all intermediate ones
                for ( long line = lineStart + 1; line < lineEnd; line++ )
                {
                    sel << m_lines[line] << _T('\n');
                }

                // and the start of the last one
                sel += m_lines[lineEnd].Left(colEnd);
            }
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
    // disabled control can never be edited
    return m_isEditable && IsEnabled();
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

/*
    A few remarks about this stuff:

    o   The numbering of the text control columns/rows starts from 0.
    o   Start of first line is position 0, its last position is line.length()
    o   Start of the next line is the last position of the previous line + 1
 */

int wxTextCtrl::GetLineLength(long line) const
{
    if ( IsSingleLine() )
    {
        wxASSERT_MSG( line == 0, _T("invalid GetLineLength() parameter") );

        return GetSLValue().length();
    }
    else // multiline
    {
        wxCHECK_MSG( (size_t)line < m_lines.GetCount(), -1,
                     _T("line index out of range") );

        return m_lines[line].length();
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
        wxCHECK_MSG( (size_t)line < m_lines.GetCount(), _T(""),
                     _T("line index out of range") );

        return m_lines[line];
    }
}

int wxTextCtrl::GetNumberOfLines() const
{
    // there is always 1 line, even if the text is empty
    return IsSingleLine() ? 1 : m_lines.GetCount();
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
        if ( (size_t)y >= m_lines.GetCount() )
        {
            // this position is below the text
            return GetLastPosition();
        }

        long pos = 0;
        for ( size_t nLine = 0; nLine < (size_t)y; nLine++ )
        {
            // +1 is because the positions at the end of this line and of the
            // start of the next one are different
            pos += m_lines[nLine].length() + 1;
        }

        // take into account also the position in line
        if ( (size_t)x > m_lines[y].length() )
        {
            // don't return position in the next line
            x = m_lines[y].length();
        }

        return pos + x;
    }
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( IsSingleLine() )
    {
        if ( (size_t)pos > GetSLValue().length() )
            return FALSE;

        if ( x )
            *x = pos;
        if ( y )
            *y = 0;

        return TRUE;
    }
    else // multiline
    {
        long posCur = 0;
        size_t nLineCount = m_lines.GetCount();
        for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
        {
            // +1 is because the start the start of the next line is one
            // position after the end of this one
            long posNew = posCur + m_lines[nLine].length() + 1;
            if ( posNew > pos )
            {
                // we've found the line, now just calc the column
                if ( x )
                    *x = pos - posCur;

                if ( y )
                    *y = nLine;

#ifdef WXDEBUG_TEXT
                wxASSERT_MSG( XYToPosition(pos - posCur, nLine) == pos,
                              _T("XYToPosition() or PositionToXY() broken") );
#endif // WXDEBUG_TEXT

                return TRUE;
            }
            else // go further down
            {
                posCur = posNew;
            }
        }

        // beyond the last line
        return FALSE;
    }
}

void wxTextCtrl::ShowPosition(long pos)
{
    HideCaret();

    if ( IsSingleLine() )
    {
        ShowHorzPosition(GetCaretPosition(pos));
    }
    else if ( m_scrollRangeX || m_scrollRangeY ) // multiline with scrollbars
    {
        int xStart, yStart;
        GetViewStart(&xStart, &yStart);

        if ( m_scrollRangeY )
        {
            // scroll the position vertically into view: if it is currently
            // above it, make it the first one, otherwise the last one
            if ( m_curRow < yStart )
            {
                Scroll(0, m_curRow);
            }
            else
            {
                int yEnd = yStart + GetClientSize().y / GetCharHeight() - 1;
                if ( yEnd < m_curRow )
                {
                    // scroll down: the current item should appear at the
                    // bottom of the view
                    Scroll(0, m_curRow - (yEnd - yStart));
                }
            }
        }

        if ( m_scrollRangeX )
        {
            // TODO
        }
    }
    //else: multiline but no scrollbars, hence nothing to do

    ShowCaret();
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

    if ( m_curCol == 0 )
    {
        // go to the end of the previous line
        return m_curPos - 1;
    }

    // it shouldn't be possible to learn where the word starts in the password
    // text entry zone
    if ( IsPassword() )
        return 0;

    // start at the previous position
    const wxChar *p0 = GetLineText(m_curRow).c_str();
    const wxChar *p = p0 + m_curCol - 1;

    // find the end of the previous word
    while ( (p > p0) && !IsWordChar(*p) )
        p--;

    // now find the beginning of this word
    while ( (p > p0) && IsWordChar(*p) )
        p--;

    // we might have gone too far
    if ( !IsWordChar(*p) )
        p++;

    return (m_curPos - m_curCol) + p - p0;
}

long wxTextCtrl::GetWordEnd() const
{
    if ( m_curPos == -1 )
        return 0;

    wxString line = GetLineText(m_curRow);
    if ( (size_t)m_curCol == line.length() )
    {
        // if we're on the last position in the line, go to the next one - if
        // it exists
        long pos = m_curPos;
        if ( pos < GetLastPosition() )
            pos++;

        return pos;
    }

    // it shouldn't be possible to learn where the word ends in the password
    // text entry zone
    if ( IsPassword() )
        return GetLastPosition();

    // start at the current position
    const wxChar *p0 = line.c_str();
    const wxChar *p = p0 + m_curCol;

    // find the start of the next word
    while ( *p && !IsWordChar(*p) )
        p++;

    // now find the end of it
    while ( *p && IsWordChar(*p) )
        p++;

    // and find the start of the next word
    while ( *p && !IsWordChar(*p) )
        p++;

    return (m_curPos - m_curCol) + p - p0;
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

        // wxTextFile::Translate() is needed to transform all '\n' into "\r\n"
        wxString text = wxTextFile::Translate(GetTextToShow(GetSelectionText()));
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
        // reverse transformation: '\r\n\" -> '\n'
        WriteText(wxTextFile::Translate(data.GetText(), wxTextFileType_Unix));
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

    int widthMin = wxMin(10*wChar, 100);
    if ( w < widthMin )
        w = widthMin;
    if ( h < hChar )
        h = hChar;

    if ( !IsSingleLine() )
    {
        // let the control have a reasonable number of lines
        int lines = GetNumberOfLines();
        if ( lines < 5 )
            lines = 5;
        else if ( lines > 10 )
            lines = 10;
        h *= 10;
    }

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

    // only scroll this rect when the window is scrolled
    SetTargetRect(m_rectText);

    UpdateLastVisible();
}

void wxTextCtrl::UpdateLastVisible()
{
    // this method is only used for horizontal "scrollbarless" scrolling which
    // is used only with single line controls
    if ( !IsSingleLine() )
        return;

    // OPT: estimate the correct value first, just adjust it later

    wxString text;
    wxCoord w, wOld;

    w =
    wOld = 0;

    m_colLastVisible = m_colStart;

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

    m_updateScrollbarX =
    m_updateScrollbarY = TRUE;

    event.Skip();
}

wxCoord wxTextCtrl::GetTotalWidth() const
{
    wxCoord w;
    CalcUnscrolledPosition(m_rectText.width, 0, &w, NULL);
    return w;
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
        HitTestLine(GetSLValue(), pos, &col);
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
            HitTestLine(GetSLValue(), pos - width, &col);
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
    // for single line controls we only have to deal with m_ofsHorz and it's
    // useless to call base class version as they don't use normal scrolling
    if ( m_ofsHorz )
    {
        // adjust the DC origin if the text is shifted
        wxPoint pt = dc.GetDeviceOrigin();
        dc.SetDeviceOrigin(pt.x - m_ofsHorz, pt.y);
    }
    else
    {
        wxScrollHelper::DoPrepareDC(dc);
    }
}

void wxTextCtrl::UpdateMaxWidth(long line)
{
    wxCoord width;
    GetTextExtent(GetLineText(line), &width, NULL);

    // GetMaxWidth() and not m_widthMax as it might be not calculated yet
    if ( width > GetMaxWidth() )
    {
        m_widthMax = width;

        m_updateScrollbarX = TRUE;
    }
}

wxCoord wxTextCtrl::GetMaxWidth() const
{
    if ( m_widthMax == -1 )
    {
        // recalculate it

        // OPT: should we remember the widths of all the lines?

        wxTextCtrl *self = wxConstCast(this, wxTextCtrl);
        wxClientDC dc(self);
        dc.SetFont(GetFont());

        size_t count = m_lines.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxCoord width;
            dc.GetTextExtent(m_lines[n], &width, NULL);
            if ( width > m_widthMax )
            {
                self->m_widthMax = width;
            }
        }
    }

    return m_widthMax;
}

void wxTextCtrl::UpdateScrollbars()
{
    wxSize size = GetClientSize();

    // is our height enough to show all items?
    int nLines = GetNumberOfLines();
    wxCoord lineHeight = GetCharHeight();
    bool showScrollbarY = nLines*lineHeight > size.y;

    // is our width enough to show the longest line?
    wxCoord charWidth, maxWidth;
    bool showScrollbarX;
    if ( GetWindowStyle() && wxHSCROLL )
    {
        charWidth = GetCharWidth();
        maxWidth = GetMaxWidth();
        showScrollbarX = maxWidth > size.x;
    }
    else // never show the horz scrollbar
    {
        // just to suppress compiler warnings about using uninit vars below
        charWidth = maxWidth = 0;

        showScrollbarX = FALSE;
    }

    // calc the scrollbars ranges
    int scrollRangeX = showScrollbarX
                        ? (maxWidth + 2*charWidth - 1) / charWidth
                        : 0;
    int scrollRangeY = showScrollbarY ? nLines : 0;

    if ( (scrollRangeY != m_scrollRangeY) || (scrollRangeX != m_scrollRangeX) )
    {
        int x, y;
        GetViewStart(&x, &y);
        SetScrollbars(charWidth, lineHeight,
                      scrollRangeX, scrollRangeY,
                      x, y);

        m_scrollRangeX = scrollRangeX;
        m_scrollRangeY = scrollRangeY;
    }
}

void wxTextCtrl::OnIdle(wxIdleEvent& event)
{
    if ( m_updateScrollbarX || m_updateScrollbarY )
    {
        UpdateScrollbars();

        m_updateScrollbarX =
        m_updateScrollbarY = FALSE;
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// refresh
// ----------------------------------------------------------------------------

void wxTextCtrl::RefreshLineRange(long lineFirst, long lineLast)
{
    wxASSERT_MSG( lineFirst <= lineLast, _T("no lines to refresh") );

    wxRect rect;
    // rect.x is already 0
    rect.width = m_rectText.width;
    wxCoord h = GetCharHeight();
    rect.y = lineFirst*h;

    // don't refresh beyond the window boundary
    wxCoord bottom = (lineLast + 1)*h;
    if ( bottom > m_rectText.height )
        bottom = m_rectText.height;

    rect.SetBottom(bottom);

    RefreshTextRect(rect);
}

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

        RefreshColRange(line, posStart, posCount);
    }
}

void wxTextCtrl::RefreshColRange(long line, long start, long count)
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
    rect.x = start;
    rect.y = line*h;

    if ( width == 0 )
    {
        // till the end of line
        rect.width = GetTotalWidth() - rect.x;
    }
    else
    {
        // only part of line
        rect.width = width;
    }

    rect.height = h;

    RefreshTextRect(rect);
}

void wxTextCtrl::RefreshTextRect(wxRect& rect)
{
    if ( IsSingleLine() )
    {
        // account for horz scrolling
        rect.x -= m_ofsHorz;
    }

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
    static int s_countUpdates = -1;
    if ( s_countUpdates != -1 )
    {
        wxWindowDC dc(this);
        dc.SetBrush(*(++s_countUpdates % 2 ? wxRED_BRUSH : wxGREEN_BRUSH));
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

        // for single line controls we may additionally cut off everything
        // which is to the right of the last visible position
        if ( IsSingleLine() )
        {
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
        }

        // extract the part of line we need to redraw
        wxString textLine = GetLineText(line);
        wxString text = textLine.Mid(colStart, colEnd - colStart);

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

bool wxTextCtrl::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return FALSE;

    // recreate it, in fact
    CreateCaret();

    // and refresh everything, of course
    InitInsertionPoint();
    ClearSelection();

    m_widthMax = -1;

    Refresh();

    return TRUE;
}

bool wxTextCtrl::Enable(bool enable)
{
    if ( !wxTextCtrlBase::Enable(enable) )
        return FALSE;

    ShowCaret(enable);
}

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

wxCoord wxTextCtrl::GetCaretPosition(long pos) const
{
    wxString textBeforeCaret(GetLineText(m_curRow),
                             (size_t)(pos == -1 ? m_curCol : pos));

    return GetTextWidth(textBeforeCaret);
}

void wxTextCtrl::ShowCaret(bool show)
{
    wxCaret *caret = GetCaret();
    if ( caret )
    {
        // position it correctly
        caret->Move(m_rectText.x + GetCaretPosition() - m_ofsHorz,
                    m_rectText.y + m_curRow*GetCharHeight());

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
        if ( IsEditable() )
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
        newPos = m_curPos - m_curCol;
    }
    else if ( action == wxACTION_TEXT_END )
    {
        newPos = m_curPos + GetLineLength(m_curRow) - m_curCol;
    }
    else if ( action == wxACTION_TEXT_UP )
    {
        if ( m_curRow > 0 )
            newPos = XYToPosition(m_curCol, m_curRow - 1);
    }
    else if ( action == wxACTION_TEXT_DOWN )
    {
        if ( (size_t)m_curRow < m_lines.GetCount() )
            newPos = XYToPosition(m_curCol, m_curRow + 1);
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
        if ( IsEditable() && !strArg.empty() )
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
        if ( IsEditable() )
            Cut();
    }
    else if ( action == wxACTION_TEXT_PASTE )
    {
        if ( IsEditable() )
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
        wxASSERT_MSG( IsEditable(), _T("non editable control changed?") );

        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());
        InitCommandEvent(event);
        event.SetString(GetValue());
        GetEventHandler()->ProcessEvent(event);

        // as the text changed...
        m_isModified = TRUE;
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
            if ( IsSingleLine() || (GetWindowStyle() & wxTE_PROCESS_ENTER) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, GetId());
                InitCommandEvent(event);
                event.SetString(GetValue());
                GetEventHandler()->ProcessEvent(event);
            }
            else // interpret <Enter> normally: insert new line
            {
                PerformAction(wxACTION_TEXT_INSERT, -1, _T('\n'));
            }
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

        case WXK_UP:
            action << wxACTION_TEXT_UP;
            break;

        case WXK_DOWN:
            action << wxACTION_TEXT_DOWN;
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
