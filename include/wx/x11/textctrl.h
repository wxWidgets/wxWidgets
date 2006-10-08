/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/textctrl.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __X11TEXTCTRLH__
#define __X11TEXTCTRLH__

// Set to 1 to use wxUniv's implementation, 0
// to use wxX11's.
#define wxUSE_UNIV_TEXTCTRL 1

#if wxUSE_UNIV_TEXTCTRL
#include "wx/univ/textctrl.h"
#else

#include "wx/scrolwin.h"
#include "wx/arrstr.h"
#include "wx/datetime.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextCtrl;

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

enum wxSourceUndo
{
    wxSOURCE_UNDO_LINE,
    wxSOURCE_UNDO_ENTER,
    wxSOURCE_UNDO_BACK,
    wxSOURCE_UNDO_INSERT_LINE,
    wxSOURCE_UNDO_DELETE,
    wxSOURCE_UNDO_PASTE
};

class wxSourceUndoStep: public wxObject
{
public:
    wxSourceUndoStep( wxSourceUndo type, int y1, int y2, wxTextCtrl *owner );

    void Undo();

    wxSourceUndo    m_type;
    int             m_y1;
    int             m_y2;
    int             m_cursorX;
    int             m_cursorY;
    wxTextCtrl     *m_owner;
    wxString        m_text;
    wxArrayString   m_lines;
};

class wxSourceLine
{
public:
    wxSourceLine( const wxString &text = wxEmptyString )
    {
        m_text = text;
    }

    wxString       m_text;
};

WX_DECLARE_OBJARRAY(wxSourceLine, wxSourceLineArray);

enum wxSourceLanguage
{
    wxSOURCE_LANG_NONE,
    wxSOURCE_LANG_CPP,
    wxSOURCE_LANG_PERL,
    wxSOURCE_LANG_PYTHON
};

//-----------------------------------------------------------------------------
// wxTextCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextCtrl: public wxTextCtrlBase, public wxScrollHelper
{
public:
    wxTextCtrl() { Init(); }
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString &value = wxEmptyString,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxTextCtrlNameStr);
    virtual ~wxTextCtrl();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &value = wxEmptyString,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxTextCtrlNameStr);

    // required for scrolling with wxScrollHelper
    // ------------------------------------------

    virtual void PrepareDC(wxDC& dc) { DoPrepareDC(dc); }

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value)
        { ChangeValue(value); SendTextUpdatedEvent(); }

    virtual void ChangeValue(const wxString &value);

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    // more readable flag testing methods
    // ----------------------------------

#if 0
    // it seems now in wxTextCtrlBase
    bool IsSingleLine() const { return !(GetWindowStyle() & wxTE_MULTILINE); }
#endif
    bool IsPassword() const { return (GetWindowStyle() & wxTE_PASSWORD) != 0; }
    bool WrapLines() const { return false; }

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

    virtual void SetMaxLength(unsigned long len);

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

    // apply text attribute to the range of text (only works with richedit
    // controls)
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);

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
    virtual void Redo() {}

    virtual bool CanUndo() const    { return (m_undos.GetCount() > 0); }
    virtual bool CanRedo() const    { return false; }

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    virtual bool Enable( bool enable = true );

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);

    bool SetFont(const wxFont& font);
    bool SetForegroundColour(const wxColour& colour);
    bool SetBackgroundColour(const wxColour& colour);

    void SetModified() { m_modified = true; }

    virtual void Freeze();
    virtual void Thaw();

    // textctrl specific scrolling
    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

    // not part of the wxTextCtrl API from now on..

    void SetLanguage( wxSourceLanguage lang = wxSOURCE_LANG_NONE );

    void Delete();
    void DeleteLine();

    void Indent();
    void Unindent();

    bool HasSelection();
    void ClearSelection();

    int GetCursorX()                        { return m_cursorX; }
    int GetCursorY()                        { return m_cursorY; }
    bool IsModified()                       { return m_modified; }
    bool OverwriteMode()                    { return m_overwrite; }

    // implementation from now on...

    int PosToPixel( int line, int pos );
    int PixelToPos( int line, int pixel );

    void SearchForBrackets();

    void DoChar( char c );
    void DoBack();
    void DoDelete();
    void DoReturn();
    void DoDClick();

    wxString GetNextToken( wxString &line, size_t &pos );

    void DrawLinePart( wxDC &dc, int x, int y, const wxString &toDraw, const wxString &origin, const wxColour &colour);
    void DrawLine( wxDC &dc, int x, int y, const wxString &line, int lineNum );
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground( wxEraseEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnSetFocus( wxFocusEvent& event );
    void OnKillFocus( wxFocusEvent& event );

    void OnInternalIdle();
    void RefreshLine( int n );
    void RefreshDown( int n );
    void MoveCursor( int new_x, int new_y, bool shift = false, bool centre = false );
    void MyAdjustScrollbars();

protected:
    // common part of all ctors
    void Init();

    virtual wxSize DoGetBestSize() const;

    friend class wxSourceUndoStep;

    wxSourceLineArray  m_lines;

    wxFont             m_sourceFont;
    wxColour           m_sourceColour;
    wxColour           m_commentColour;
    wxColour           m_stringColour;

    int                m_cursorX;
    int                m_cursorY;

    int                m_selStartX,m_selStartY;
    int                m_selEndX,m_selEndY;

    int                m_lineHeight;
    int                m_charWidth;

    int                m_longestLine;

    bool               m_overwrite;
    bool               m_modified;
    bool               m_editable;
    bool               m_ignoreInput;

    wxArrayString      m_keywords;
    wxColour           m_keywordColour;

    wxArrayString      m_defines;
    wxColour           m_defineColour;

    wxArrayString      m_variables;
    wxColour           m_variableColour;

    wxSourceLanguage   m_lang;

    wxList             m_undos;

    bool               m_capturing;

    int                m_bracketX;
    int                m_bracketY;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTextCtrl);
};

//-----------------------------------------------------------------------------
// this is superfluous here but helps to compile
//-----------------------------------------------------------------------------

// cursor movement and also selection and delete operations
#define wxACTION_TEXT_GOTO          _T("goto")  // to pos in numArg
#define wxACTION_TEXT_FIRST         _T("first") // go to pos 0
#define wxACTION_TEXT_LAST          _T("last")  // go to last pos
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

// undo or redo
#define wxACTION_TEXT_UNDO          _T("undo")
#define wxACTION_TEXT_REDO          _T("redo")

// ----------------------------------------------------------------------------
// wxTextCtrl types
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdTextCtrlInputHandler : public wxStdInputHandler
{
public:
    wxStdTextCtrlInputHandler(wxInputHandler *inphand) : wxStdInputHandler(inphand) {}

    virtual bool HandleKey(wxInputConsumer *consumer,
                           const wxKeyEvent& event,
                           bool pressed) { return false; }
    virtual bool HandleMouse(wxInputConsumer *consumer, const wxMouseEvent& event) { return false; }
    virtual bool HandleMouseMove(wxInputConsumer *consumer, const wxMouseEvent& event) { return false; }
    virtual bool HandleFocus(wxInputConsumer *consumer, const wxFocusEvent& event) { return false; }

protected:
    // get the position of the mouse click
    static wxTextPos HitTest(const wxTextCtrl *text, const wxPoint& pos) { return 0; }

    // capture data
    wxTextCtrl *m_winCapture;
};

#endif
// wxUSE_UNIV_TEXTCTRL

#endif // __X11TEXTCTRLH__

