/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TEXTCTRL_H_
#define _WX_UNIV_TEXTCTRL_H_

#ifdef __GNUG__
    #pragma interface "univtextctrl.h"
#endif

class WXDLLEXPORT wxCaret;

#include "wx/scrolwin.h"    // for wxScrollHelper

// ----------------------------------------------------------------------------
// wxTextCtrl actions
// ----------------------------------------------------------------------------

// cursor movement and also selection and delete operations
#define wxACTION_TEXT_HOME          _T("home")
#define wxACTION_TEXT_END           _T("end")
#define wxACTION_TEXT_LEFT          _T("left")
#define wxACTION_TEXT_RIGHT         _T("right")
#define wxACTION_TEXT_UP            _T("up")
#define wxACTION_TEXT_DOWN          _T("down")
#define wxACTION_TEXT_WORD_LEFT     _T("wordleft")
#define wxACTION_TEXT_WORD_RIGHT    _T("wordright")
#define wxACTION_TEXT_PAGE_UP       _T("pageup")
#define wxACTION_TEXT_PAGE_DOWN     _T("pagedown")

// clipboard operations
#define wxACTION_TEXT_COPY          _T("copy")
#define wxACTION_TEXT_CUT           _T("cut")
#define wxACTION_TEXT_PASTE         _T("paste")

// insert text at the cursor position: the text is in strArg of PerformAction
#define wxACTION_TEXT_INSERT        _T("insert")

// if the action starts with either of these prefixes and the rest of the
// string is one of the movement commands, it means to select/delete text from
// the current cursor position to the new one
#define wxACTION_TEXT_PREFIX_SEL    _T("sel")
#define wxACTION_TEXT_PREFIX_DEL    _T("del")

// mouse selection
#define wxACTION_TEXT_ANCHOR_SEL    _T("anchorsel")
#define wxACTION_TEXT_EXTEND_SEL    _T("extendsel")
#define wxACTION_TEXT_SEL_WORD      _T("wordsel")
#define wxACTION_TEXT_SEL_LINE      _T("linesel")

// ----------------------------------------------------------------------------
// wxTextCtrl::HitTest return values
// ----------------------------------------------------------------------------

enum wxTextCtrlHitTestResult
{
    wxTE_HT_BEFORE = -1,
    wxTE_HT_ON_TEXT,
    wxTE_HT_AFTER
};

// ----------------------------------------------------------------------------
// wxTextCtrl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextCtrl : public wxTextCtrlBase, public wxScrollHelper
{
public:
    // creation
    // --------

    wxTextCtrl() { Init(); }

    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr)
    {
        Init();

        Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const;

    // operations
    // ----------

    // editing
    virtual void Clear();
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);

    // clears the dirty flag
    virtual void DiscardEdits();

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;

    virtual void ShowPosition(long pos);

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // wxUniv-specific methods
    // -----------------------

    // caret stuff
    virtual void ShowCaret(bool show = TRUE);
    void HideCaret() { ShowCaret(FALSE); }
    void CreateCaret(); // for the current font size
    wxCoord GetCaretPosition() const; // in pixels

    // helpers for cursor movement
    long GetWordStart() const;
    long GetWordEnd() const;

    // selection helpers
    bool HasSelection() const
        { return m_selStart != -1 && m_selEnd > m_selStart; }
    void ClearSelection();
    void RemoveSelection();
    wxString GetSelectionText() const;

    // find the character at this position, return 0 if the character is
    // really there, -1 if the point is before the beginning of the text/line
    // and the returned character is the first one to follow it or +1 if it the
    // position is beyond the end of line/text and the returned character is
    // the last one
    //
    // NB: pt is in device coords (not adjusted for the client area origin nor
    //     for the scrolling)
    wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                    long *col, long *row) const;

    // find the character at this position in the given line, return value as
    // for HitTest()
    //
    // NB: x is the logical coord (client and unscrolled)
    wxTextCtrlHitTestResult HitTestLine(const wxString& line,
                                        wxCoord x,
                                        long *colOut) const;

    // bring the given position into view
    void ShowHorzPosition(wxCoord pos);

    // scroll the window horizontally so that the first character shown is in
    // position pos
    void ScrollText(long col);

    // adjust the DC for horz text control scrolling too
    virtual void DoPrepareDC(wxDC& dc);

    // implementation only from now on
    // -------------------------------

    // override this to take into account our scrollbar-less scrolling
    virtual void CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const;

    // set the right colours
    virtual bool IsContainerWindow() const { return TRUE; }
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_SUNKEN; }

    // perform an action
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);

    // override this to recreate the caret here
    virtual bool SetFont(const wxFont &font);

protected:
    // draw the text
    void DrawTextLine(wxDC& dc, const wxRect& rect,
                      const wxString& text,
                      long selStart, long selEnd);

    void DoDrawTextInRect(wxDC& dc, const wxRect& rectUpdate);

    virtual void DoDraw(wxControlRenderer *renderer);

    // calc the size from the text extent
    virtual wxSize DoGetBestClientSize() const;

    // input support
    virtual wxString GetInputHandlerType() const;

    // common part of all ctors
    void Init();

    // more readable flag testing methods
    bool IsSingleLine() const { return !(GetWindowStyle() & wxTE_MULTILINE); }
    bool IsPassword() const { return (GetWindowStyle() & wxTE_PASSWORD) != 0; }

    // get the extent (width) of the text
    wxCoord GetTextWidth(const wxString& text) const;

    // refresh the text in the given range (in logical coords) of this line, if
    // width is 0, refresh to the end of line
    void RefreshPixelRange(long line, wxCoord start, wxCoord width);

    // refresh the text in the given range (in text coords) in this line
    void RefreshLineRange(long line, long start, long count);

    // refresh the text in the given range which can span multiple lines
    // (this method accepts arguments in any order)
    void RefreshTextRange(long start, long end);

    // get the text to show: either the text itself or the text replaced with
    // starts for wxTE_PASSWORD control
    wxString GetTextToShow(const wxString& text) const;

    // get the start and end of the selection for this line: if the line is
    // outside the selection, both will be -1 and FALSE will be returned
    bool GetSelectedPartOfLine(long line, int *start, int *end) const;

    // update the text rect: the zone inside our client rect (its coords are
    // client coords) which contains the text
    void UpdateTextRect();

    // calculate the last visible position
    void UpdateLastVisible();

    // move caret to the given position unconditionally
    // (SetInsertionPoint() does nothing if the position didn't change)
    void DoSetInsertionPoint(long pos);

    // set the caret to its initial (default) position
    void InitInsertionPoint();

    // event handlers
    void OnChar(wxKeyEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    // the value (may be only part of it for the multiline controls)
    wxString m_value;

    // the initially specified control size
    wxSize m_sizeInitial;

    // current position
    long m_curPos,
         m_curLine,
         m_curRow;

    // selection
    long m_selAnchor,
         m_selStart,
         m_selEnd;

    // flags
    bool m_isModified:1,
         m_isEditable:1,
         m_hasCaret:1;

    // the rectangle (in client coordinates) to draw text inside
    wxRect m_rectText;

    // this section is for the controls without horz scrollbar only

    // the position of the first visible pixel and the first visible column
    wxCoord m_ofsHorz;
    long m_colStart;

    // and the last ones (m_posLastVisible is the width but m_colLastVisible
    // is an absolute value)
    wxCoord m_posLastVisible;
    long m_colLastVisible;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTextCtrl)
};

#endif // _WX_UNIV_TEXTCTRL_H_

