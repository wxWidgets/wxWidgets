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
   TODO

+  1. update vert scrollbar when any line length changes for WrapLines()
+  2. cursor movement ("Hello,^" -> "^verse!" on Arrow Down)?
     -> maybe save the x position and use it instead of current in handling
        DOWN/UP actions (this would make up/down always return the cursor to
        the same location)?
   3. split file into chunks
+? 4. rewrite Replace() refresh logic to deal with wrapping lines
 */

/*
   Some terminology:

   Everywhere in this file LINE refers to a logical line of text, and ROW to a
   physical line of text on the display. They are the same unless WrapLines()
   is TRUE in which case a single LINE may correspond to multiple ROWs.

   A text position is an unsigned int (which for reasons of compatibility is
   still a long) from 0 to GetLastPosition() inclusive. The positions
   correspond to the gaps between the letters so the position 0 is just
   before the first character and the last position is the one beyond the last
   character. For an empty text control GetLastPosition() returns 0.

   Lines and columns returned/accepted by XYToPosition() and PositionToXY()
   start from 0. The y coordinate is a LINE, not a ROW. Columns correspond to
   the characters, the first column of a line is the first character in it,
   the last one is length(line text). For compatibility, again, lines and
   columns are also longs.

   When translating lines/column coordinates to/from positions, the line and
   column give the character after the given position. Thus, GetLastPosition()
   doesn't have any corresponding column.

   An example of positions and lines/columns for a control without wrapping
   containing the text "Hello, Universe!\nGoodbye"

                               1 1 1 1 1 1 1
   pos:    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
            H e l l o ,   U n i v e r s e !         line 0
   col:     0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1
                                0 1 2 3 4 5

   pos:    1 1 1 2 2 2 2 2
           7 8 9 0 1 2 3 4
            G o o d b y e                           line 1
   col:     0 1 2 3 4 5 6


   The same example for a control with line wrap assuming "Universe" is too
   long to fit on the same line with "Hello,":

   pos:    0 1 2 3 4 5
            H e l l o ,                             line 0 (row 0)
   col:     0 1 2 3 4 5

                   1 1 1 1 1 1 1
   pos:    6 7 8 9 0 1 2 3 4 5 6
              U n i v e r s e !                     line 0 (row 1)
   col:     6 7 8 9 1 1 1 1 1 1
                    0 1 2 3 4 5

   (line 1 == row 2 same as above)

   Note that there is still the same number of columns and positions and that
   there is no (logical) position at the end of the first ROW. This position
   is identified with the preceding one (which is not how Windows does it: it
   identifies it with the next one, i.e. the first position of the next line,
   but much more logical IMHO).
 */

/*
   Search for "OPT" for possible optimizations

   A possible global optimization would be to always store the coords in the
   text in triplets (pos, col, line) and update them simultaneously instead of
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

#include "wx/cmdproc.h"

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

// exchange two positions so that from is always less than or equal to to
static inline void OrderPositions(wxTextPos& from, wxTextPos& to)
{
    if ( from > to )
    {
        wxTextPos tmp = from;
        from = to;
        to = tmp;
    }
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// names of text ctrl commands
#define wxTEXT_COMMAND_INSERT _T("insert")
#define wxTEXT_COMMAND_REMOVE _T("remove")

// ----------------------------------------------------------------------------
// private classes for undo/redo management
// ----------------------------------------------------------------------------

/*
   We use custom versions of wxWindows command processor to implement undo/redo
   as we want to avoid storing the backpointer to wxTextCtrl in wxCommand
   itself: this is a waste of memory as all commands in the given command
   processor always have the same associated wxTextCtrl and so it makes sense
   to store the backpointer there.

   As for the rest of the implementation, it's fairly standard: we have 2
   command classes corresponding to adding and removing text.
 */

// a command corresponding to a wxTextCtrl action
class wxTextCtrlCommand : public wxCommand
{
public:
    wxTextCtrlCommand(const wxString& name) : wxCommand(TRUE, name) { }

    // we don't use these methods as they don't make sense for us as we need a
    // wxTextCtrl to be applied
    virtual bool Do() { wxFAIL_MSG(_T("shouldn't be called")); return FALSE; }
    virtual bool Undo() { wxFAIL_MSG(_T("shouldn't be called")); return FALSE; }

    // instead, our command processor uses these methods
    virtual bool Do(wxTextCtrl *text) = 0;
    virtual bool Undo(wxTextCtrl *text) = 0;
};

// insert text command
class wxTextCtrlInsertCommand : public wxTextCtrlCommand
{
public:
    wxTextCtrlInsertCommand(const wxString& textToInsert)
        : wxTextCtrlCommand(wxTEXT_COMMAND_INSERT), m_text(textToInsert)
    {
        m_from = -1;
    }

    // combine the 2 commands together
    void Append(wxTextCtrlInsertCommand *other);

    virtual bool CanUndo() const;
    virtual bool Do(wxTextCtrl *text);
    virtual bool Undo(wxTextCtrl *text);

private:
    // the text we insert
    wxString m_text;

    // the position where we inserted the text
    wxTextPos m_from;
};

// remove text command
class wxTextCtrlRemoveCommand : public wxTextCtrlCommand
{
public:
    wxTextCtrlRemoveCommand(wxTextPos from, wxTextPos to)
        : wxTextCtrlCommand(wxTEXT_COMMAND_REMOVE)
    {
        m_from = from;
        m_to = to;
    }

    virtual bool CanUndo() const;
    virtual bool Do(wxTextCtrl *text);
    virtual bool Undo(wxTextCtrl *text);

private:
    // the range of text to delete
    wxTextPos m_from,
         m_to;

    // the text which was deleted when this command was Do()ne
    wxString m_textDeleted;
};

// a command processor for a wxTextCtrl
class wxTextCtrlCommandProcessor : public wxCommandProcessor
{
public:
    wxTextCtrlCommandProcessor(wxTextCtrl *text)
    {
        m_compressInserts = FALSE;

        m_text = text;
    }

    // override Store() to compress multiple wxTextCtrlInsertCommand into one
    virtual void Store(wxCommand *command);

    // stop compressing insert commands when this is called
    void StopCompressing() { m_compressInserts = FALSE; }

    // accessors
    wxTextCtrl *GetTextCtrl() const { return m_text; }
    bool IsCompressing() const { return m_compressInserts; }

protected:
    virtual bool DoCommand(wxCommand& cmd)
        { return ((wxTextCtrlCommand &)cmd).Do(m_text); }
    virtual bool UndoCommand(wxCommand& cmd)
        { return ((wxTextCtrlCommand &)cmd).Undo(m_text); }

    // check if this command is a wxTextCtrlInsertCommand and return it casted
    // to the right type if it is or NULL otherwise
    wxTextCtrlInsertCommand *IsInsertCommand(wxCommand *cmd);

private:
    // the control we're associated with
    wxTextCtrl *m_text;

    // if the flag is TRUE we're compressing subsequent insert commands into
    // one so that the entire typing could be undone in one call to Undo()
    bool m_compressInserts;
};

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
    m_lineLongest = 0;

    // init wxScrollHelper
    SetWindow(this);

    // init the undo manager
    m_cmdProcessor = new wxTextCtrlCommandProcessor(this);
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
    if ( style & wxTE_MULTILINE )
    {
        // for compatibility with wxMSW we create the controls with vertical
        // scrollbar always shown unless they have wxTE_RICH style (because
        // Windows text controls always has vert scrollbar but richedit one
        // doesn't)
        if ( !(style & wxTE_RICH) )
        {
            style |= wxALWAYS_SHOW_SB;
        }

        // TODO: support wxTE_NO_VSCROLL (?)
    }
    else
    {
        // this doesn't make sense for single line controls
        style &= ~wxHSCROLL;
    }

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
    }

    SetValue(value);
    SetBestSize(size);

    m_isEditable = !(style & wxTE_READONLY);

    CreateCaret();
    InitInsertionPoint();

    // we can't show caret right now as we're not shown yet and so it would
    // result in garbage on the screen - we'll do it after first OnPaint()
    m_hasCaret = FALSE;

    return TRUE;
}

wxTextCtrl::~wxTextCtrl()
{
    delete m_cmdProcessor;
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

void wxTextCtrl::Replace(wxTextPos from, wxTextPos to, const wxString& text)
{
    wxTextCoord colStart, colEnd,
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
    //OPT: special case for replacements inside single line?
    else // multiline
    {
        /*
           Join all the lines in the replacement range into one string, then
           replace a part of it with the new text and break it into lines again.
        */

        // (0) remember the number of rows before we started
        wxTextCoord rowsOld = GetNumberOfRowsBefore(GetNumberOfLines());

        // (1) join lines
        wxString textOrig;
        wxTextCoord line;
        for ( line = lineStart; line <= lineEnd; line++ )
        {
            if ( line > lineStart )
            {
                // from the previous line
                textOrig += _T('\n');
            }

            textOrig += m_lines[line];
        }

        // we need to append the '\n' for the last line unless there is no
        // following line
        size_t countOld = m_lines.GetCount();

        // (2) replace text in the combined string

        // (2a) leave the part before replaced area unchanged
        wxString textNew(textOrig, colStart);

        // these values will be used to refresh the changed area below
        wxCoord widthNewText,
                startNewText = GetTextWidth(textNew);
        if ( (size_t)colStart == m_lines[lineStart].length() )
        {
            // text appended, refresh just enough to show the new text
            widthNewText = GetTextWidth(text.BeforeFirst(_T('\n')));
        }
        else // text inserted, refresh till the end of line
        {
            widthNewText = 0;
        }

        // (2b) insert new text
        textNew += text;

        // (2c) and append the end of the old text

        // adjust for index shift: to is relative to colStart, not 0
        size_t toRel = (size_t)((to - from) + colStart);
        if ( toRel < textOrig.length() )
        {
            textNew += textOrig.c_str() + toRel;
        }

        // (3) break it into lines

        // (3a) all empty tokens should be counted as replacing with "foo" and
        //      with "foo\n" should have different effects
        wxArrayString lines = wxStringTokenize(textNew, _T("\n"),
                                               wxTOKEN_RET_EMPTY_ALL);

        if ( lines.IsEmpty() )
        {
            lines.Add(wxEmptyString);
        }

        // (3b) special case: if we replace everything till the end we need to
        //      keep an empty line or the lines would disappear completely
        //      (this also takes care of never leaving m_lines empty)
        if ( ((size_t)lineEnd == countOld - 1) && lines.IsEmpty() )
        {
            lines.Add(wxEmptyString);
        }

        size_t nReplaceCount = lines.GetCount(),
               nReplaceLine = 0;

        // (4) merge into the array

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
                // (4b) delete all extra lines (note that we need to delete
                //      them backwards because indices shift while we do it)
                bool deletedLongestLine = FALSE;
                for ( wxTextCoord lineDel = lineEnd; lineDel >= line; lineDel-- )
                {
                    if ( lineDel == m_lineLongest )
                    {
                        // we will need to recalc the max line width
                        deletedLongestLine = TRUE;
                    }

                    m_lines.RemoveAt(lineDel);
                }

                if ( deletedLongestLine )
                {
                    RecalcMaxWidth();
                }

                // update line to exit the loop
                line = lineEnd + 1;
            }
        }

        // (4c) insert the new lines
        while ( nReplaceLine < nReplaceCount )
        {
            m_lines.Insert(lines[nReplaceLine++], ++lineEnd);

            UpdateMaxWidth(lineEnd);
        }

        // (5) now refresh the changed area

        // we may optimize refresh if the number of rows didn't change - but if
        // it did we have to refresh everything below the part we chanegd as
        // well as it might have moved
        wxTextCoord rowsNew = GetNumberOfRowsBefore(GetNumberOfLines());
        if ( rowsNew == rowsOld )
        {
            // refresh the line we changed
            RefreshPixelRange(lineStart, startNewText, widthNewText);

            // number of rows didn't change, refresh the updated rows and the
            // last one
            if ( lineStart < lineEnd )
                RefreshLineRange(lineStart + 1, lineEnd);
        }
        else
        {
            if ( !WrapLines() )
            {
                // refresh only part of the first line
                RefreshPixelRange(lineStart, startNewText, widthNewText);

                // and refresh the rest below
                lineStart++;
            }
            //else: refresh all lines we changed (OPT?)

            RefreshLineRange(lineStart, m_lines.GetCount() - 1);

            // refresh text rect left below
            if ( rowsNew < rowsOld )
            {
                wxCoord h = GetCharHeight();
                wxRect rect;
                rect.x = 0;
                rect.width = m_rectText.width;
                rect.y = rowsNew*h;
                rect.height = (rowsOld - rowsNew)*h;
                RefreshTextRect(rect);
            }

            // the vert scrollbar might [dis]appear
            m_updateScrollbarY = TRUE;
        }

        // update the (cached) last position
        m_posLast += text.length() - to + from;
    }

#ifdef WXDEBUG_TEXT_REPLACE
    // optimized code above should give the same result as straightforward
    // computation in the beginning
    wxASSERT_MSG( GetValue() == textTotalNew, _T("error in Replace()") );
#endif // WXDEBUG_TEXT_REPLACE

    // update the current position: note that we always put the cursor at the
    // end of the replacement text
    DoSetInsertionPoint(from + text.length());

    // and the selection: this is complicated by the fact that selection coords
    // must be first updated to reflect change in text coords, i.e. if we had
    // selection from 17 to 19 and we just removed this range, we don't have to
    // refresh anything, so we can't just use ClearSelection() here
    if ( HasSelection() )
    {
        // refresh the parst of the selection outside the changed text (which
        // we already refreshed)
        if ( m_selStart < from )
            RefreshTextRange(m_selStart, from);
        if ( to < m_selEnd )
            RefreshTextRange(to, m_selEnd);

        m_selStart =
        m_selEnd = -1;
    }

    // now call it to do the rest (not related to refreshing)
    ClearSelection();
}

void wxTextCtrl::Remove(wxTextPos from, wxTextPos to)
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

void wxTextCtrl::SetInsertionPoint(wxTextPos pos)
{
    wxCHECK_RET( pos >= 0 && pos <= GetLastPosition(),
                 _T("insertion point position out of range") );

    // don't do anything if it didn't change
    if ( pos != m_curPos )
    {
        DoSetInsertionPoint(pos);
    }

    ClearSelection();
}

void wxTextCtrl::InitInsertionPoint()
{
    // so far always put it in the beginning
    DoSetInsertionPoint(0);

    // this will also set the selection anchor correctly
    ClearSelection();
}

void wxTextCtrl::DoSetInsertionPoint(wxTextPos pos)
{
    wxASSERT_MSG( pos >= 0 && pos <= GetLastPosition(),
                 _T("DoSetInsertionPoint() can only be called with valid pos") );

    m_curPos = pos;
    PositionToXY(m_curPos, &m_curCol, &m_curRow);

    ShowPosition(m_curPos);
}

void wxTextCtrl::SetInsertionPointEnd()
{
    SetInsertionPoint(GetLastPosition());
}

wxTextPos wxTextCtrl::GetInsertionPoint() const
{
    return m_curPos;
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    wxTextPos pos;
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

void wxTextCtrl::GetSelection(wxTextPos* from, wxTextPos* to) const
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
            wxTextCoord colStart, lineStart,
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
                for ( wxTextCoord line = lineStart + 1; line < lineEnd; line++ )
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

void wxTextCtrl::SetSelection(wxTextPos from, wxTextPos to)
{
    if ( from == -1 || to == from )
    {
        ClearSelection();
    }
    else // valid sel range
    {
        if ( from >= to )
        {
            wxTextPos tmp = from;
            from = to;
            to = tmp;
        }

        wxCHECK_RET( to <= GetLastPosition(),
                     _T("invalid range in wxTextCtrl::SetSelection") );

        if ( from != m_selStart || to != m_selEnd )
        {
            // we need to use temp vars as RefreshTextRange() may call DoDraw()
            // directly and so m_selStart/End must be reset by then
            wxTextPos selStartOld = m_selStart,
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
        wxTextPos selStart = m_selStart,
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

bool wxTextCtrl::GetSelectedPartOfLine(wxTextCoord line,
                                       wxTextPos *start, wxTextPos *end) const
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

    wxTextCoord lineStart, colStart;
    PositionToXY(m_selStart, &colStart, &lineStart);
    if ( lineStart > line )
    {
        // this line is entirely above the selection
        return FALSE;
    }

    wxTextCoord lineEnd, colEnd;
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

        // the caret (dis)appears
        CreateCaret();

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

int wxTextCtrl::GetLineLength(wxTextCoord line) const
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

wxString wxTextCtrl::GetLineText(wxTextCoord line) const
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

wxTextPos wxTextCtrl::XYToPosition(wxTextCoord x, wxTextCoord y) const
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

        wxTextPos pos = 0;
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

bool wxTextCtrl::PositionToXY(wxTextPos pos,
                              wxTextCoord *x, wxTextCoord *y) const
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
        wxTextPos posCur = 0;
        size_t nLineCount = m_lines.GetCount();
        for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
        {
            // +1 is because the start the start of the next line is one
            // position after the end of this one
            wxTextPos posNew = posCur + m_lines[nLine].length() + 1;
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

wxTextCoord wxTextCtrl::GetRowsPerLine(wxTextCoord line) const
{
    // we can't just divide the line length by the width of the window as there
    // are "unuse" parts of rows at the end, so we really need to use
    // GetPartOfWrappedLine() here
    wxString lineText = GetLineText(line);

    wxTextCoord rows = 0;
    size_t rowLen,
           colRowStart = 0;
    for ( ;; )
    {
        rowLen = GetPartOfWrappedLine(lineText.c_str() + colRowStart);
        if ( !rowLen )
            break;

        colRowStart += rowLen;
        rows++;
    }

    // empty line still takes one row
    return rows == 0 ? 1 : rows;
}

wxTextCoord wxTextCtrl::GetNumberOfRowsBefore(wxTextCoord lineMax) const
{
    if ( !WrapLines() )
        return lineMax;

    size_t nRows = 0;
    for ( wxTextCoord n = 0; n < lineMax; n++ )
    {
        nRows += GetRowsPerLine(n);
    }

    return nRows;
}

bool wxTextCtrl::PositionToLogicalXY(wxTextPos pos,
                                     wxCoord *xOut,
                                     wxCoord *yOut) const
{
    wxTextCoord col, line;

    // optimization for special (but common) case when we already have the col
    // and line
    if ( pos == m_curPos )
    {
        col = m_curCol;
        line = m_curRow;
    }
    else // must really calculate col/line from pos
    {
        if ( !PositionToXY(pos, &col, &line) )
            return FALSE;
    }

    int hLine = GetCharHeight();
    wxCoord x, y;
    wxString textLine = GetLineText(line);
    if ( IsSingleLine() || !WrapLines() )
    {
        x = GetTextWidth(textLine.Left(col));
        y = line*hLine;
    }
    else // difficult case: multline control with line wrap
    {
        // first find the start of this line
        y = 0;
        for ( wxTextCoord lineCur = 0; lineCur < line; lineCur++ )
        {
            y += GetRowsPerLine(lineCur)*hLine;
        }

        // now find in which row of this line the position lies
        wxTextCoord colRowStart = 0;
        for ( ;; )
        {
            int rowLen = GetPartOfWrappedLine(textLine.c_str() + colRowStart);

            // the test is quite interesting because the last position in any
            // but the last row of the line is invalid: you can't put the
            // cursor there, for example, as it's really the same position as
            // the first position of the next row - but the last position of
            // the last row is perfectly fine as it's different from the
            // starting position from the next line
            //
            // NB: this is compatible with Windows behaviour except that
            //     Windows has a bug: you can position the cursor in such
            //     invalid position by clicking the mouse there even if you
            //     can't reach this position from keyboard!
            wxTextCoord colNextRow = colRowStart + rowLen;
            if ( (col < colNextRow) ||
                    (col == colNextRow &&
                     (size_t)colNextRow == textLine.length()) )
            {
                // we found the right row
                break;
            }

            // go further down
            colRowStart = colNextRow;
            y += hLine;
        }

        // x is the width of the text before this position in this row
        x = GetTextWidth(textLine.Mid(colRowStart, col - colRowStart));
    }

    if ( xOut )
        *xOut = x;
    if ( yOut )
        *yOut = y;

    return TRUE;
}

bool wxTextCtrl::PositionToDeviceXY(wxTextPos pos,
                                    wxCoord *xOut,
                                    wxCoord *yOut) const
{
    wxCoord x, y;
    if ( !PositionToLogicalXY(pos, &x, &y) )
        return FALSE;

    // finally translate the logical text rect coords into physical client
    // coords
    CalcScrolledPosition(m_rectText.x + x - m_ofsHorz,
                         m_rectText.y + y,
                         xOut,
                         yOut);

    return TRUE;
}

wxPoint wxTextCtrl::GetCaretPosition() const
{
    wxCoord xCaret, yCaret;
    if ( !PositionToDeviceXY(m_curPos, &xCaret, &yCaret) )
    {
        wxFAIL_MSG( _T("Caret can't be beyond the text!") );
    }

    return wxPoint(xCaret, yCaret);
}

// pos may be -1 to show the current position
void wxTextCtrl::ShowPosition(wxTextPos pos)
{
    HideCaret();

    if ( IsSingleLine() )
    {
        ShowHorzPosition(GetTextWidth(GetSLValue().Left(pos)));
    }
    else if ( m_scrollRangeX || m_scrollRangeY ) // multiline with scrollbars
    {
        int xStart, yStart;
        GetViewStart(&xStart, &yStart);

        if ( pos == -1 )
            pos = m_curPos;

        wxCoord x, y;
        PositionToLogicalXY(pos, &x, &y);

        wxRect rectText = GetRealTextArea();

        // scroll the position vertically into view: if it is currently above
        // it, make it the first one, otherwise the last one
        if ( m_scrollRangeY )
        {
            y /= GetCharHeight();

            if ( y < yStart )
            {
                Scroll(0, y);
            }
            else // we are currently in or below the view area
            {
                // find the last row currently shown
                wxTextCoord yEnd;

                if ( WrapLines() )
                {
                    // to find the last row we need to use the generic HitTest
                    wxTextCoord col;

                    // OPT this is a bit silly: we undo this in HitTest(), so
                    //     it would be better to factor out the common
                    //     functionality into a separate function (OTOH it
                    //     won't probably save us that much)
                    wxPoint pt(0, rectText.height - 1);
                    pt += GetClientAreaOrigin();
                    pt += m_rectText.GetPosition();
                    HitTest(pt, &col, &yEnd);

                    // find the row inside the line
                    wxString text = GetLineText(yEnd);
                    yEnd = GetNumberOfRowsBefore(yEnd);
                    wxTextCoord colRowStart = GetPartOfWrappedLine(text);
                    while ( colRowStart < col )
                    {
                        yEnd++;
                        colRowStart += GetPartOfWrappedLine(text.c_str() +
                                                            colRowStart);
                    }
                }
                else
                {
                    // finding the last line is easy if each line has exactly
                    // one row
                    yEnd = yStart + rectText.height / GetCharHeight() - 1;
                }

                if ( yEnd < y )
                {
                    // scroll down: the current item should appear at the
                    // bottom of the view
                    Scroll(0, y - (yEnd - yStart));
                }
            }
        }

        // scroll the position horizontally into view
        if ( m_scrollRangeX )
        {
            // unlike for the rows, xStart doesn't correspond to the starting
            // column as they all have different widths, so we need to
            // translate everything to pixels

            // we want the text between x and x2 be entirely inside the view
            // (i.e. the current character)

            // make xStart the first visible pixel (and not position)
            int wChar = GetCharWidth();
            xStart *= GetCharWidth();

            if ( x < xStart )
            {
                Scroll(x / wChar, y);
            }
            else // maybe we're beyond the right border of the view?
            {
                wxTextCoord col, row;
                if ( PositionToXY(pos, &col, &row) )
                {
                    wxString lineText = GetLineText(row);
                    wxCoord x2 = x + GetTextWidth(lineText[(size_t)col]);
                    if ( x2 > xStart + rectText.width )
                    {
                        Scroll(x2 / wChar, row);
                    }
                }
            }
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

wxTextPos wxTextCtrl::GetWordStart() const
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

wxTextPos wxTextCtrl::GetWordEnd() const
{
    if ( m_curPos == -1 )
        return 0;

    wxString line = GetLineText(m_curRow);
    if ( (size_t)m_curCol == line.length() )
    {
        // if we're on the last position in the line, go to the next one - if
        // it exists
        wxTextPos pos = m_curPos;
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
    (void)DoCut();
}

bool wxTextCtrl::DoCut()
{
    if ( !HasSelection() )
        return FALSE;

    Copy();

    RemoveSelection();

    return TRUE;
}

void wxTextCtrl::Paste()
{
    (void)DoPaste();
}

bool wxTextCtrl::DoPaste()
{
#if wxUSE_CLIPBOARD
    wxClipboardLocker clipLock;

    wxTextDataObject data;
    if ( wxTheClipboard->IsSupported(data.GetFormat())
            && wxTheClipboard->GetData(data) )
    {
        // reverse transformation: '\r\n\" -> '\n'
        wxString text = wxTextFile::Translate(data.GetText(),
                                              wxTextFileType_Unix);
        if ( !text.empty() )
        {
            WriteText(text);

            return TRUE;
        }
    }
#endif // wxUSE_CLIPBOARD

    return FALSE;
}

// ----------------------------------------------------------------------------
// Undo and redo
// ----------------------------------------------------------------------------

wxTextCtrlInsertCommand *
wxTextCtrlCommandProcessor::IsInsertCommand(wxCommand *command)
{
    return (wxTextCtrlInsertCommand *)
            (command && (command->GetName() == wxTEXT_COMMAND_INSERT)
                ? command : NULL);
}

void wxTextCtrlCommandProcessor::Store(wxCommand *command)
{
    wxTextCtrlInsertCommand *cmdIns = IsInsertCommand(command);
    if ( cmdIns )
    {
        if ( IsCompressing() )
        {
            wxTextCtrlInsertCommand *
                cmdInsLast = IsInsertCommand(GetCurrentCommand());

            // it is possible that we don't have any last command at all if,
            // for example, it was undone since the last Store(), so deal with
            // this case too
            if ( cmdInsLast )
            {
                cmdInsLast->Append(cmdIns);

                delete cmdIns;

                // don't need to call the base class version
                return;
            }
        }

        // append the following insert commands to this one
        m_compressInserts = TRUE;

        // let the base class version will do the job normally
    }
    else // not an insert command
    {
        // stop compressing insert commands - this won't work with the last
        // command not being an insert one anyhow
        StopCompressing();

        // let the base class version will do the job normally
    }

    wxCommandProcessor::Store(command);
}

void wxTextCtrlInsertCommand::Append(wxTextCtrlInsertCommand *other)
{
    m_text += other->m_text;
}

bool wxTextCtrlInsertCommand::CanUndo() const
{
    return m_from != -1;
}

bool wxTextCtrlInsertCommand::Do(wxTextCtrl *text)
{
    // the text is going to be inserted at the current position, remember where
    // exactly it is
    m_from = text->GetInsertionPoint();

    // and now do insert it
    text->WriteText(m_text);

    return TRUE;
}

bool wxTextCtrlInsertCommand::Undo(wxTextCtrl *text)
{
    wxCHECK_MSG( CanUndo(), FALSE, _T("impossible to undo insert cmd") );

    // remove the text from where we inserted it
    text->Remove(m_from, m_from + m_text.length());

    return TRUE;
}

bool wxTextCtrlRemoveCommand::CanUndo() const
{
    // if we were executed, we should have the text we removed
    return !m_textDeleted.empty();
}

bool wxTextCtrlRemoveCommand::Do(wxTextCtrl *text)
{
    text->SetSelection(m_from, m_to);
    m_textDeleted = text->GetSelectionText();
    text->RemoveSelection();

    return TRUE;
}

bool wxTextCtrlRemoveCommand::Undo(wxTextCtrl *text)
{
    text->SetInsertionPoint(m_from);
    text->WriteText(m_textDeleted);

    return TRUE;
}

void wxTextCtrl::Undo()
{
    // the caller must check it
    wxASSERT_MSG( CanUndo(), _T("can't call Undo() if !CanUndo()") );

    m_cmdProcessor->Undo();
}

void wxTextCtrl::Redo()
{
    // the caller must check it
    wxASSERT_MSG( CanRedo(), _T("can't call Undo() if !CanUndo()") );

    m_cmdProcessor->Redo();
}

bool wxTextCtrl::CanUndo() const
{
    return IsEditable() && m_cmdProcessor->CanUndo();
}

bool wxTextCtrl::CanRedo() const
{
    return IsEditable() && m_cmdProcessor->CanRedo();
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

    int widthMin = wxMax(10*wChar, 100);
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
    SetTargetRect(GetRealTextArea());

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

wxRect wxTextCtrl::GetRealTextArea() const
{
    // the real text area always holds an entire number of lines, so the only
    // difference with the text area is a narrow strip along the bottom border
    wxRect rectText = m_rectText;
    int hLine = GetCharHeight();
    rectText.height = (m_rectText.height / hLine) * hLine;
    return rectText;
}

size_t wxTextCtrl::GetPartOfWrappedLine(const wxChar* text,
                                        wxCoord *widthReal) const
{
    wxTextCtrl *self = wxConstCast(this, wxTextCtrl);
    wxClientDC dc(self);
    dc.SetFont(GetFont());
    self->DoPrepareDC(dc);

    wxCoord widthMax = m_rectText.width;

    // the text which we can keep in this ROW
    wxString str;
    wxCoord w, wOld;
    for ( wOld = w = 0; *text && (w <= widthMax); )
    {
        wOld = w;
        str += *text++;
        dc.GetTextExtent(str, &w, NULL);
    }

    if ( w > widthMax )
    {
        // if we wrapped, the last letter was one too much
        if ( str.length() > 1 )
        {
            // remove it
            str.erase(str.length() - 1, 1);
        }
        else // but always keep at least one letter in each row
        {
            // the real width then is the last value of w and not teh one
            // before last
            wOld = w;
        }
    }
    else // we didn't wrap
    {
        wOld = w;
    }

    if ( widthReal )
    {
        *widthReal = wOld;
    }

    return str.length();
}

wxTextCtrlHitTestResult wxTextCtrl::HitTestLine(const wxString& line,
                                                wxCoord x,
                                                wxTextCoord *colOut) const
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
        col = line.length() - 1;

        res = wxTE_HT_BEYOND;
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

        // matchDir is the direction in which we should move to reach the
        // character containing the given position
        enum
        {
            Match_Left  = -1,
            Match_None  = 0,
            Match_Right = 1
        } matchDir = Match_None;
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
                if ( matchDir == Match_Right )
                {
                    // we were going to the right and, finally, moved beyond
                    // the original position - stop on the previous one
                    col--;

                    break;
                }

                if ( matchDir == Match_None )
                {
                    // we just started iterating, now we know that we should
                    // move to the left
                    matchDir = Match_Left;
                }
                //else: we are still to the right of the target, continue
            }
            else // width < x
            {
                // invert the logic above
                if ( matchDir == Match_Left )
                {
                    // with the exception that we don't need to backtrack here
                    break;
                }

                if ( matchDir == Match_None )
                {
                    // go to the right
                    matchDir = Match_Right;
                }
            }

            // this is not supposed to happen
            wxASSERT_MSG( matchDir, _T("logic error in wxTextCtrl::HitTest") );

            if ( matchDir == Match_Right )
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
                                            wxTextCoord *colOut,
                                            wxTextCoord *rowOut) const
{
    return HitTest2(pos.y, pos.x, 0, rowOut, colOut, NULL, NULL);
}

wxTextCtrlHitTestResult wxTextCtrl::HitTestLogical(const wxPoint& pos,
                                                   wxTextCoord *colOut,
                                                   wxTextCoord *rowOut) const
{
    return HitTest2(pos.y, pos.x, 0, rowOut, colOut, NULL, NULL, FALSE);
}

wxTextCtrlHitTestResult wxTextCtrl::HitTest2(wxCoord y0,
                                             wxCoord x10,
                                             wxCoord x20,
                                             wxTextCoord *rowOut,
                                             wxTextCoord *colStart,
                                             wxTextCoord *colEnd,
                                             wxTextCoord *colRowStartOut,
                                             bool deviceCoords) const
{
    // is the point in the text area or to the right or below it?
    wxTextCtrlHitTestResult res = wxTE_HT_ON_TEXT;

    // translate the window coords x0 and y0 into the client coords in the text
    // area by adjusting for both the client and text area offsets (unless this
    // was already done)
    int x1, y;
    if ( deviceCoords )
    {
        wxPoint pt = GetClientAreaOrigin() + m_rectText.GetPosition();
        CalcUnscrolledPosition(x10 - pt.x, y0 - pt.y, &x1, &y);
    }
    else
    {
        y = y0;
        x1 = x10;
    }

    // calculate the row (it is really a LINE, not a ROW)
    wxTextCoord row;

    // these vars are used only for WrapLines() case
    wxTextCoord colRowStart = 0;
    size_t rowLen = 0;

    if ( colRowStartOut )
        *colRowStartOut = 0;

    int hLine = GetCharHeight();
    wxTextCoord rowLast = GetNumberOfLines() - 1;
    if ( IsSingleLine() || !WrapLines() )
    {
        // in this case row calculation is simple as all lines have the
        // same height
        row = y / hLine;
        if ( row > rowLast )
        {
            // clicking below the text is the same as clicking on the last
            // line
            row = rowLast;

            res = wxTE_HT_BELOW;
        }
        else if ( row < 0 )
        {
            // and clicking before it is the same as clicking on the first
            // one
            row = 0;

            res = wxTE_HT_BEFORE;
        }
    }
    else // multline control with line wrap
    {
        // one line can take several rows in this case, so we need to
        // rescan the text

        // OPT: as for horz hit testing we might approximate the result
        //      with y / hLine first and then scan from there, this is
        //      probably much more efficient when there is a lot of text

        bool found = FALSE; // have we found the row containing this point?

        // go down LINE and by ROW inside each line until we reach y or exhaust
        // all text
        wxString textLine;
        int yCur = 0;
        for ( row = 0; row <= rowLast; row++ )
        {
            textLine = GetLineText(row);
            size_t nRowInLine,
                   nRowsPerLine = GetRowsPerLine(row);
            for ( nRowInLine = 0;
                  !found && (nRowInLine < nRowsPerLine);
                  nRowInLine++ )
            {
                rowLen = GetPartOfWrappedLine(textLine.c_str() + colRowStart);

                yCur += hLine;
                if ( yCur > y )
                {
                    // point is in this row, so don't change colRowStart
                    found = TRUE;
                }
                else
                {
                    // the start of the next row
                    colRowStart += rowLen;
                }
            }

            // we shouldn't increase LINE in the outer for loop if we found
            // match inside this one, this is why we break here instead of just
            // putting "& !found" in for condition
            if ( found )
            {
                break;
            }

            // we didn't find the correct row yet so reset colRowStart and
            // rowLen: they don't have any meaning without a valid row
            colRowStart = 0;
            rowLen = 0;
        }

        if ( row > rowLast )
        {
            // we are beyond the last line
            res = wxTE_HT_BELOW;

            row = rowLast;
        }
        else if ( !rowLen )
        {
            // we must get the length of the first row of the next line
            rowLen = GetPartOfWrappedLine(GetLineText(row));
        }
    }

    if ( res == wxTE_HT_ON_TEXT )
    {
        // now find the position in the line
        wxString lineText = GetLineText(row),
                 rowText;

        if ( colRowStart || rowLen )
        {
            // look in this row only, not in whole line
            rowText = lineText.Mid(colRowStart, rowLen);
        }
        else
        {
            // just take the whole string
            rowText = lineText;
        }

        if ( colStart )
        {
            res = HitTestLine(GetTextToShow(rowText), x1, colStart);

            if ( colRowStart )
            {
                if ( colRowStartOut )
                {
                    // give them the column offset in this ROW in pixels
                    *colRowStartOut = colRowStart;
                }

                // take into account that the ROW doesn't start in the
                // beginning of the LINE
                *colStart += colRowStart;
            }

            if ( colEnd )
            {
                // the hit test result we return is for x1, so throw out
                // the result for x2 here
                int x2 = x1 + x20 - x10;
                (void)HitTestLine(GetTextToShow(rowText), x2, colEnd);

                *colEnd += colRowStart;
            }
        }
    }
    else // before/after vertical text span
    {
        if ( colStart )
        {
            // fill the column with the first/last position in the
            // corresponding line
            if ( res == wxTE_HT_BEFORE )
                *colStart = 0;
            else // res == wxTE_HT_BELOW
                *colStart = GetLineText(GetNumberOfLines() - 1).length();
        }
    }

    if ( rowOut )
    {
        // give them the row in text coords (as is)
        *rowOut = row;
    }

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
        wxTextCoord col;
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
            wxTextCoord col;
            HitTestLine(GetSLValue(), pos - width, &col);
            ScrollText(col + 1);
        }
    }
}

// scroll the window horizontally so that the first visible character becomes
// the one at this position
void wxTextCtrl::ScrollText(wxTextCoord col)
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

void wxTextCtrl::UpdateMaxWidth(wxTextCoord line)
{
    // check if the max width changes after this line was modified
    wxCoord widthMaxOld = m_widthMax,
            width;
    GetTextExtent(GetLineText(line), &width, NULL);

    if ( line == m_lineLongest )
    {
        // this line was the longest one, is it still?
        if ( width > m_widthMax )
        {
            m_widthMax = width;
        }
        else if ( width < m_widthMax )
        {
            // we need to find the new longest line
            RecalcMaxWidth();
        }
        //else: its length didn't change, nothing to do
    }
    else // it wasn't the longest line, but maybe it became it?
    {
        // GetMaxWidth() and not m_widthMax as it might be not calculated yet
        if ( width > GetMaxWidth() )
        {
            m_widthMax = width;
            m_lineLongest = line;
        }
    }

    m_updateScrollbarX = m_widthMax != widthMaxOld;
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

        self->m_widthMax = 0;

        size_t count = m_lines.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxCoord width;
            dc.GetTextExtent(m_lines[n], &width, NULL);
            if ( width > m_widthMax )
            {
                // remember the width and the line which has it
                self->m_widthMax = width;
                self->m_lineLongest = n;
            }
        }
    }

    wxASSERT_MSG( m_widthMax != -1, _T("should have at least 1 line") );

    return m_widthMax;
}

void wxTextCtrl::UpdateScrollbars()
{
    wxSize size = GetRealTextArea().GetSize();

    // is our height enough to show all items?
    wxTextCoord nRows = GetNumberOfRowsBefore(GetNumberOfLines());
    wxCoord lineHeight = GetCharHeight();
    bool showScrollbarY = nRows*lineHeight > size.y;

    // is our width enough to show the longest line?
    wxCoord charWidth, maxWidth;
    bool showScrollbarX;
    if ( !WrapLines() )
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
    int scrollRangeY = showScrollbarY ? nRows : 0;

    if ( (scrollRangeY != m_scrollRangeY) || (scrollRangeX != m_scrollRangeX) )
    {
        int x, y;
        GetViewStart(&x, &y);
        SetScrollbars(charWidth, lineHeight,
                      scrollRangeX, scrollRangeY,
                      x, y);

        m_scrollRangeX = scrollRangeX;
        m_scrollRangeY = scrollRangeY;

        // bring the current position in view
        ShowPosition(-1);
    }
}

void wxTextCtrl::OnIdle(wxIdleEvent& event)
{
    // notice that single line text control never has scrollbars
    if ( !IsSingleLine() && (m_updateScrollbarX || m_updateScrollbarY) )
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

void wxTextCtrl::RefreshLineRange(wxTextCoord lineFirst, wxTextCoord lineLast)
{
    wxASSERT_MSG( lineFirst <= lineLast, _T("no lines to refresh") );

    wxRect rect;
    // rect.x is already 0
    rect.width = m_rectText.width;
    wxCoord h = GetCharHeight();
    rect.y = GetNumberOfRowsBefore(lineFirst)*h;

    wxCoord bottom = GetNumberOfRowsBefore(lineLast + 1)*h;
    rect.SetBottom(bottom);

    RefreshTextRect(rect);
}

void wxTextCtrl::RefreshTextRange(wxTextPos start, wxTextPos end)
{
    wxCHECK_RET( start != -1 && end != -1,
                 _T("invalid RefreshTextRange() arguments") );

    // accept arguments in any order as it is more conenient for the caller
    OrderPositions(start, end);

    // this is acceptable but we don't do anything in this case
    if ( start == end )
        return;

    wxTextPos colStart, lineStart;
    if ( !PositionToXY(start, &colStart, &lineStart) )
    {
        // the range is entirely beyond the end of the text, nothing to do
        return;
    }

    wxTextCoord colEnd, lineEnd;
    if ( !PositionToXY(end, &colEnd, &lineEnd) )
    {
        // the range spans beyond the end of text, refresh to the end
        colEnd = -1;
        lineEnd = GetNumberOfLines() - 1;
    }

    // refresh all lines one by one
    for ( wxTextCoord line = lineStart; line <= lineEnd; line++ )
    {
        // refresh the first line from the start of the range to the end, the
        // intermediate ones entirely and the last one from the beginning to
        // the end of the range
        wxTextPos posStart = line == lineStart ? colStart : 0;
        size_t posCount;
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

        if ( posCount )
            RefreshColRange(line, posStart, posCount);
    }
}

void wxTextCtrl::RefreshColRange(wxTextCoord line,
                                 wxTextPos start,
                                 size_t count)
{
    wxString text = GetLineText(line);

    wxASSERT_MSG( (size_t)start <= text.length() && count,
                  _T("invalid RefreshColRange() parameter") );

    RefreshPixelRange(line,
                      GetTextWidth(text.Left((size_t)start)),
                      GetTextWidth(text.Mid((size_t)start, (size_t)count)));
}

// this method accepts "logical" coords in the sense that they are coordinates
// in a logical line but it can span several rows if we wrap lines and
// RefreshPixelRange() will then refresh several rows
void wxTextCtrl::RefreshPixelRange(wxTextCoord line,
                                   wxCoord start,
                                   wxCoord width)
{
    // we will use line text only in line wrap case
    wxString text;
    if ( WrapLines() )
    {
        text = GetLineText(line);
    }

    // special case: width == 0 means to refresh till the end of line
    if ( width == 0 )
    {
        // FIXME we refresh till the end of the current line here, but we can't
        //       tell if the line didn't have fewer or more rows before - it is
        //       the callers responsability to not call us in this way if it is
        //       the case
        if ( WrapLines() )
        {
            width = wxMax(GetTextWidth(text), GetTotalWidth());
        }
        else
        {
            // refresh till the end of visible line
            width = GetTotalWidth();
        }

        width -= start;
    }

    wxCoord h = GetCharHeight();
    wxRect rect;
    rect.x = start;
    rect.y = GetNumberOfRowsBefore(line)*h;
    rect.height = h;

    if ( WrapLines() )
    {
        // (1) skip all rows which we don't touch at all
        wxCoord wLine;
        size_t ofs = GetPartOfWrappedLine(text.c_str(), &wLine);
        while ( wLine && (rect.x >= wLine) )
        {
            rect.x -= wLine;
            rect.y += h;

            ofs += GetPartOfWrappedLine(text.c_str() + ofs, &wLine);
        }

        // (2) now refresh all lines except the last one: note that the first
        //     line is refreshed from the given start to the end, all the next
        //     ones - entirely
        while ( wLine && (rect.x + width > wLine) )
        {
            rect.width = GetTotalWidth() - rect.x;
            RefreshTextRect(rect);
            width -= wLine - rect.x;
            rect.x = 0;
            rect.y += h;

            ofs += GetPartOfWrappedLine(text.c_str() + ofs, &wLine);
        }

        // (3) the code below will refresh the last line
    }

    rect.width = width;

    RefreshTextRect(rect);
}

void wxTextCtrl::RefreshTextRect(const wxRect& rectClient)
{
    wxRect rect = rectClient;
    if ( IsSingleLine() )
    {
        // account for horz scrolling
        rect.x -= m_ofsHorz;
    }
    else // multiline
    {
        CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
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
                              wxTextPos selStart, wxTextPos selEnd)
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

    // calculate the range lineStart..lineEnd of lines to redraw
    wxTextCoord lineStart, lineEnd;
    if ( IsSingleLine() )
    {
        lineStart =
        lineEnd = 0;
    }
    else // multiline
    {
        wxPoint pt = rectUpdate.GetPosition();
        (void)HitTest(pt, NULL, &lineStart);

        pt.y += rectUpdate.height;
        (void)HitTest(pt, NULL, &lineEnd);
    }

    // prepare for drawing
    wxCoord hLine = GetCharHeight();

    // these vars will be used for hit testing of the current row
    wxCoord y = rectUpdate.y;
    const wxCoord x1 = rectUpdate.x;
    const wxCoord x2 = rectUpdate.x + rectUpdate.width;

    wxRect rectText;
    rectText.height = hLine;
    wxCoord yClient = y - GetClientAreaOrigin().y;
    if ( IsSingleLine() )
    {
        rectText.y = yClient;
    }
    else // multiline, adjust for scrolling
    {
        CalcUnscrolledPosition(0, yClient, NULL, &rectText.y);
    }

    // do draw the invalidated parts of each line: note that we iterate here
    // over ROWs, not over LINEs
    for ( wxTextCoord line = lineStart;
          y < rectUpdate.y + rectUpdate.height;
          y += hLine,
          rectText.y += hLine )
    {
        // calculate the update rect in text positions for this line
        wxTextCoord colStart, colEnd, colRowStart;
        wxTextCtrlHitTestResult ht = HitTest2(y, x1, x2,
                                              &line, &colStart, &colEnd,
                                              &colRowStart);

        if ( (ht == wxTE_HT_BEYOND) || (ht == wxTE_HT_BELOW) )
        {
            wxASSERT_MSG( line <= lineEnd, _T("how did we get that far?") );

            if ( line == lineEnd )
            {
                // we redrew everything
                break;
            }

            // the update rect is beyond the end of line, no need to redraw
            // anything on this line - but continue with the remaining ones
            continue;
        }

        // don't show the columns which are scrolled out to the left
        if ( colStart < m_colStart )
            colStart = m_colStart;

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
            {
                colEnd = m_colLastVisible;

                // we don't draw the last character because it may be shown
                // only partially in single line mode (in multi line we can't
                // avoid showing parts of characters anyhow)
                if ( colEnd > colStart )
                    colEnd--;
            }
        }

        // extract the part of line we need to redraw
        wxString textLine = GetTextToShow(GetLineText(line));
        wxString text = textLine.Mid(colStart, colEnd - colStart + 1);

        // now deal with the selection: only do something if at least part of
        // the line is selected and if this part is (at least partly) in the
        // current row
        wxTextPos selStart, selEnd;
        if ( GetSelectedPartOfLine(line, &selStart, &selEnd) &&
             (selStart <= colEnd) && (selEnd >= colRowStart) )
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

        // calculate the text coords on screen
        wxCoord ofsStart = GetTextWidth(
                                    textLine.Mid(colRowStart,
                                                 colStart - colRowStart));
        rectText.x = m_rectText.x + ofsStart;
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
    // the update region is in window coords and text area is in the client
    // ones, so it must be shifted before computing intersection
    wxRegion rgnUpdate = GetUpdateRegion();
    wxRect rectTextArea = GetRealTextArea();
    wxPoint pt = GetClientAreaOrigin();
    wxRect rectTextAreaAdjusted = rectTextArea;
    rectTextAreaAdjusted.x += pt.x;
    rectTextAreaAdjusted.y += pt.y;
    rgnUpdate.Intersect(rectTextAreaAdjusted);

    // even though the drawing is already clipped to the update region, we must
    // explicitly clip it to the rect we will use as otherwise parts of letters
    // might be drawn outside of it (if even a small part of a charater is
    // inside, HitTest() will return its column and DrawText() can't draw only
    // the part of the character, of course)
    dc.SetClippingRegion(rectTextArea);

    // adjust for scrolling
    DoPrepareDC(dc);

    // and now refresh the invalidated parts of the window
    wxRegionIterator iter(rgnUpdate);
    for ( ; iter.HaveRects(); iter++ )
    {
        wxRect r = iter.GetRect();

        // this is a workaround for wxGTK::wxRegion bug
#ifdef __WXGTK__
        if ( !r.width || !r.height )
        {
            // ignore invalid rect
            continue;
        }
#endif // __WXGTK__

        DoDrawTextInRect(dc, r);
    }

    // show caret first time only: we must show it after drawing the text or
    // the display can be corrupted when it's hidden
    if ( !m_hasCaret && GetCaret() )
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

    // update geometry parameters
    UpdateTextRect();
    UpdateScrollbars();
    RecalcMaxWidth();

    Refresh();

    return TRUE;
}

bool wxTextCtrl::Enable(bool enable)
{
    if ( !wxTextCtrlBase::Enable(enable) )
        return FALSE;

    ShowCaret(enable);

    return TRUE;
}

void wxTextCtrl::CreateCaret()
{
    wxCaret *caret;

    if ( IsEditable() )
    {
        // FIXME use renderer
        caret = new wxCaret(this, 1, GetCharHeight());
#ifndef __WXMSW__
        caret->SetBlinkTime(0);
#endif // __WXMSW__
    }
    else
    {
        // read only controls don't have the caret
        caret = (wxCaret *)NULL;
    }

    // SetCaret() will delete the old caret if any
    SetCaret(caret);
}

void wxTextCtrl::ShowCaret(bool show)
{
    wxCaret *caret = GetCaret();
    if ( caret )
    {
        // (re)position caret correctly
        caret->Move(GetCaretPosition());

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
    // has the text changed as result of this action?
    bool textChanged = FALSE;

    // the command this action corresponds to or NULL if this action doesn't
    // change text at all or can't be undone
    wxTextCtrlCommand *command = (wxTextCtrlCommand *)NULL;

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
    static const wxTextPos INVALID_POS_VALUE = -2;

    wxTextPos newPos = INVALID_POS_VALUE;

    if ( action == wxACTION_TEXT_HOME )
    {
        newPos = m_curPos - m_curCol;
    }
    else if ( action == wxACTION_TEXT_END )
    {
        newPos = m_curPos + GetLineLength(m_curRow) - m_curCol;
    }
    else if ( (action == wxACTION_TEXT_GOTO) ||
              (action == wxACTION_TEXT_FIRST) ||
              (action == wxACTION_TEXT_LAST) )
    {
        if ( action == wxACTION_TEXT_FIRST )
            numArg = 0;
        else if ( action == wxACTION_TEXT_LAST )
            numArg = GetLastPosition();
        //else: numArg already contains the position

        newPos = numArg;
    }
    else if ( action == wxACTION_TEXT_UP )
    {
        // move the cursor up by one ROW not by one LINE: this means that we
        // should really use HitTest() and not just go to the same position in
        // the previous line
        wxPoint pt = GetCaretPosition() - m_rectText.GetPosition();
        CalcUnscrolledPosition(pt.x, pt.y, &pt.x, &pt.y);
        pt.y -= GetCharHeight();

        wxTextCoord col, row;
        if ( HitTestLogical(pt, &col, &row) != wxTE_HT_BEFORE )
        {
            newPos = XYToPosition(col, row);
        }
    }
    else if ( action == wxACTION_TEXT_DOWN )
    {
        // see comments for wxACTION_TEXT_UP
        wxPoint pt = GetCaretPosition() - m_rectText.GetPosition();
        CalcUnscrolledPosition(pt.x, pt.y, &pt.x, &pt.y);
        pt.y += GetCharHeight();

        wxTextCoord col, row;
        if ( HitTestLogical(pt, &col, &row) != wxTE_HT_BELOW )
        {
            // note that wxTE_HT_BEYOND is ok: it happens when we go down from
            // a longer line to a shorter one, for example (OTOH wxTE_HT_BEFORE
            // can never happen)
            newPos = XYToPosition(col, row);
        }
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
            // inserting text can be undone
            command = new wxTextCtrlInsertCommand(strArg);

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
    else if ( action == wxACTION_TEXT_UNDO )
    {
        if ( CanUndo() )
            Undo();
    }
    else if ( action == wxACTION_TEXT_REDO )
    {
        if ( CanRedo() )
            Redo();
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

        wxTextPos posLast = GetLastPosition();
        if ( newPos > posLast )
            newPos = posLast;

        if ( del )
        {
            // if we have the selection, remove just it
            wxTextPos from, to;
            if ( HasSelection() )
            {
                from = m_selStart;
                to = m_selEnd;
            }
            else
            {
                // otherwise delete everything between current position and
                // the new one
                if ( m_curPos != newPos )
                {
                    from = m_curPos;
                    to = newPos;
                }
                else // nothing to delete
                {
                    // prevent test below from working
                    from = INVALID_POS_VALUE;

                    // and this is just to silent the compiler warning
                    to = 0;
                }
            }

            if ( from != INVALID_POS_VALUE )
            {
                command = new wxTextCtrlRemoveCommand(from, to);
            }
        }
        else // cursor movement command
        {
            // just go there
            DoSetInsertionPoint(newPos);

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

    if ( command )
    {
        // execute and remember it to be able to undo it later
        m_cmdProcessor->Submit(command);

        // undoable commands always change text
        textChanged = TRUE;
    }
    else // no undoable command
    {
        // m_cmdProcessor->StopCompressing()
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
wxTextPos wxStdTextCtrlInputHandler::HitTest(const wxTextCtrl *text,
                                             const wxPoint& pt)
{
    wxTextCoord col, row;
    wxTextCtrlHitTestResult ht = text->HitTest(pt, &col, &row);

    wxTextPos pos = text->XYToPosition(col, row);

    // if the point is after the last column we must adjust the position to be
    // the last position in the line (unless it is already the last)
    if ( (ht == wxTE_HT_BEYOND) && (pos < text->GetLastPosition()) )
    {
        pos++;
    }

    return pos;
}

bool wxStdTextCtrlInputHandler::HandleKey(wxControl *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    // we're only interested in key presses
    if ( !pressed )
        return FALSE;

    int keycode = event.GetKeyCode();

    wxControlAction action;
    wxString str;
    bool ctrlDown = event.ControlDown(),
         shiftDown = event.ShiftDown();
    if ( shiftDown )
    {
        action = wxACTION_TEXT_PREFIX_SEL;
    }

    // the only key combination with Alt we recognize is Alt-Bksp for undo, so
    // treat it first separately
    if ( event.AltDown() )
    {
        if ( keycode == WXK_BACK && !ctrlDown && !shiftDown )
            action = wxACTION_TEXT_UNDO;
    }
    else switch ( keycode )
    {
        // cursor movement
        case WXK_HOME:
            action << (ctrlDown ? wxACTION_TEXT_FIRST
                                : wxACTION_TEXT_HOME);
            break;

        case WXK_END:
            action << (ctrlDown ? wxACTION_TEXT_LAST
                                : wxACTION_TEXT_END);
            break;

        case WXK_UP:
            if ( !ctrlDown )
                action << wxACTION_TEXT_UP;
            break;

        case WXK_DOWN:
            if ( !ctrlDown )
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
            if ( !ctrlDown )
                action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_RIGHT;
            break;

        case WXK_BACK:
            if ( !ctrlDown )
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
                    case 'A':
                        action = wxACTION_TEXT_REDO;
                        break;

                    case 'C':
                        action = wxACTION_TEXT_COPY;
                        break;

                    case 'V':
                        action = wxACTION_TEXT_PASTE;
                        break;

                    case 'X':
                        action = wxACTION_TEXT_CUT;
                        break;

                    case 'Z':
                        action = wxACTION_TEXT_UNDO;
                        break;
                }
            }
    }

    if ( (action != wxACTION_NONE) && (action != wxACTION_TEXT_PREFIX_SEL) )
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

        wxTextPos pos = HitTest(text, event.GetPosition());
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
        wxTextPos pos = HitTest(text, event.GetPosition());
        if ( pos != -1 )
        {
            text->PerformAction(wxACTION_TEXT_EXTEND_SEL, pos);
        }
    }

    return wxStdInputHandler::HandleMouseMove(control, event);
}

bool wxStdTextCtrlInputHandler::HandleFocus(wxControl *control,
                                            const wxFocusEvent& event)
{
    if ( event.GetEventType() == wxEVT_KILL_FOCUS )
    {
        wxStaticCast(control, wxTextCtrl)->ClearSelection();

        // don't refresh
        return FALSE;
    }

    return wxStdInputHandler::HandleFocus(control, event);
}

#endif // wxUSE_TEXTCTRL
