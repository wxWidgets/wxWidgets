////////////////////////////////////////////////////////////////////////////
// Name:        stc.h
// Purpose:     A wxWindows implementation of Scintilla.  This class is the
//              one meant to be used directly by wx applications.  It does not
//              derive directly from the Scintilla classes, and in fact there
//              is no mention of Scintilla classes at all in this header.
//              This class delegates all method calls and events to the
//              Scintilla objects and so forth.  This allows the use of
//              Scintilla without polluting the namespace with all the
//              classes and itentifiers from Scintilla.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __stc_h__
#define __stc_h__


#include <wx/wx.h>


//----------------------------------------------------------------------
// constants and stuff

enum wxSTC_UndoType {
    wxSTC_UndoCollectNone,
    wxSTC_UndoCollectAutoStart
};


enum wxSTC_EOL {
    wxSTC_EOL_CRLF,
    wxSTC_EOL_CR,
    wxSTC_EOL_LF
};

enum wxSTC_EDGE {
    wxSTC_EDGE_NONE,
    wxSTC_EDGE_LINE,
    wxSTC_EDGE_BACKGROUND
};



const int wxSTC_LEX_STYLE_MAX     = 31;
const int wxSTC_STYLE_DEFAULT     = 32;
const int wxSTC_STYLE_LINENUMBER  = 33;
const int wxSTC_STYLE_BRACELIGHT  = 34;
const int wxSTC_STYLE_BRACEBAD    = 35;
const int wxSTC_STYLE_CONTROLCHAR = 36;
const int wxSTC_STYLE_MAX         = 127;
const int wxSTC_STYLE_MASK        = 31;

const int wxSTC_MARKER_MAX        = 31;
const int wxSTC_MARK_CIRCLE       = 0;
const int wxSTC_MARK_ROUNDRECT    = 1;
const int wxSTC_MARK_ARROW        = 2;
const int wxSTC_MARK_SMALLRECT    = 3;
const int wxSTC_MARK_SHORTARROW   = 4;
const int wxSTC_MARK_EMPTY        = 5;
const int wxSTC_MARK_ARROWDOWN    = 6;
const int wxSTC_MARK_MINUS        = 7;
const int wxSTC_MARK_PLUS         = 8;

const int wxSTC_MARKNUM_FOLDER    = 30;
const int wxSTC_MARKNUM_FOLDEROPEN= 31;
const int wxSTC_MASK_FOLDERS      = ((1 << wxSTC_MARKNUM_FOLDER) | (1 << wxSTC_MARKNUM_FOLDEROPEN));

const int wxSTC_INDIC_MAX         = 7;
const int wxSTC_INDIC_PLAIN       = 0;
const int wxSTC_INDIC_SQUIGGLE    = 1;
const int wxSTC_INDIC_TT          = 2;
const int wxSTC_INDIC0_MASK       = 32;
const int wxSTC_INDIC1_MASK       = 64;
const int wxSTC_INDIC2_MASK       = 128;
const int wxSTC_INDICS_MASK       = (wxSTC_INDIC0_MASK | wxSTC_INDIC1_MASK | wxSTC_INDIC2_MASK);


const int wxSTC_FOLDLEVELBASE       = 0x0400;
const int wxSTC_FOLDLEVELWHITEFLAG  = 0x1000;
const int wxSTC_FOLDLEVELHEADERFLAG = 0x2000;
const int wxSTC_FOLDLEVELNUMBERMASK = 0x0FFF;


// key commands
enum {
    wxSTC_CMD_LINEDOWN = 2300,
    wxSTC_CMD_LINEDOWNEXTEND,
    wxSTC_CMD_LINEUP,
    wxSTC_CMD_LINEUPEXTEND,
    wxSTC_CMD_CHARLEFT,
    wxSTC_CMD_CHARLEFTEXTEND,
    wxSTC_CMD_CHARRIGHT,
    wxSTC_CMD_CHARRIGHTEXTEND,
    wxSTC_CMD_WORDLEFT,
    wxSTC_CMD_WORDLEFTEXTEND,
    wxSTC_CMD_WORDRIGHT,
    wxSTC_CMD_WORDRIGHTEXTEND,
    wxSTC_CMD_HOME,
    wxSTC_CMD_HOMEEXTEND,
    wxSTC_CMD_LINEEND,
    wxSTC_CMD_LINEENDEXTEND,
    wxSTC_CMD_DOCUMENTSTART,
    wxSTC_CMD_DOCUMENTSTARTEXTEND,
    wxSTC_CMD_DOCUMENTEND,
    wxSTC_CMD_DOCUMENTENDEXTEND,
    wxSTC_CMD_PAGEUP,
    wxSTC_CMD_PAGEUPEXTEND,
    wxSTC_CMD_PAGEDOWN,
    wxSTC_CMD_PAGEDOWNEXTEND,
    wxSTC_CMD_EDITTOGGLEOVERTYPE,
    wxSTC_CMD_CANCEL,
    wxSTC_CMD_DELETEBACK,
    wxSTC_CMD_TAB,
    wxSTC_CMD_BACKTAB,
    wxSTC_CMD_NEWLINE,
    wxSTC_CMD_FORMFEED,
    wxSTC_CMD_VCHOME,
    wxSTC_CMD_VCHOMEEXTEND,
    wxSTC_CMD_ZOOMIN,
    wxSTC_CMD_ZOOMOUT,
    wxSTC_CMD_DELWORDLEFT,
    wxSTC_CMD_DELWORDRIGHT,
    wxSTC_CMD_LINECUT,
    wxSTC_CMD_LINEDELETE,
    wxSTC_CMD_LINETRANSPOSE,
    wxSTC_CMD_LOWERCASE,
    wxSTC_CMD_UPPERCASE
};


enum wxSTC_LEX {
    wxSTC_LEX_CONTAINER=0,
    wxSTC_LEX_NULL,
    wxSTC_LEX_PYTHON,
    wxSTC_LEX_CPP,
    wxSTC_LEX_HTML,
    wxSTC_LEX_XML,
    wxSTC_LEX_PERL,
    wxSTC_LEX_SQL,
    wxSTC_LEX_VB,
    wxSTC_LEX_PROPERTIES,
    wxSTC_LEX_ERRORLIST,
    wxSTC_LEX_MAKEFILE,
    wxSTC_LEX_BATCH,
};



const int wxSTC_CARET_SLOP    = 0x01;
const int WXSTC_CARET_CENTER  = 0x02;
const int wxSTC_CARET_STRICT  = 0x04;

const int wxSTC_MARGIN_SYMBOL = 0;
const int wxSTC_MARGIN_NUMBER = 1;


class  ScintillaWX;                      // forward declare
class  WordList;
struct SCNotification;


extern const wxChar* wxSTCNameStr;

//----------------------------------------------------------------------

class wxStyledTextCtrl : public wxControl {
public:

#ifdef SWIG
    wxStyledTextCtrl(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0,
                     const char* name = wxSTCNameStr);
#else
    wxStyledTextCtrl(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0,
                     const wxString& name = wxSTCNameStr);
#endif


#ifndef SWIG
    ~wxStyledTextCtrl();
#endif


    // Text retrieval and modification
    wxString GetText();
    bool     SetText(const wxString& text);
    wxString GetLine(int line);
    void     ReplaceSelection(const wxString& text);
    void     SetReadOnly(bool readOnly);
    bool     GetReadOnly();
    wxString GetTextRange(int startPos, int endPos);
    wxString GetStyledTextRange(int startPos, int endPos);
#ifndef SWIG
    void     GetTextRange(int startPos, int endPos, char* buff);
    void     GetStyledTextRange(int startPos, int endPos, char* buff);
#endif
    void     AddText(const wxString& text);
    void     AddStyledText(const wxString& text);
    void     InsertText(int pos, const wxString& text);
    void     ClearAll();
    char     GetCharAt(int pos);
    char     GetStyleAt(int pos);
    void     SetStyleBits(int bits);
    int      GetStyleBits();


    // Clipboard
    void     Cut();
    void     Copy();
    void     Paste();
    bool     CanPaste();
    void     ClearClipbrd();  // avoiding name conflict with virtual in wxWindow


    // Undo and Redo
    void     Undo();
    bool     CanUndo();
    void     EmptyUndoBuffer();
    void     Redo();
    bool     CanRedo();
    void     SetUndoCollection(wxSTC_UndoType type);
    wxSTC_UndoType GetUndoCollection();
    void     BeginUndoAction();
    void     EndUndoAction();


    // Selection and information
#ifdef SWIG
    void     GetSelection(int* OUTPUT, int* OUTPUT);
#else
    void     GetSelection(int* startPos, int* endPos);
#endif
    void     SetSelection(int  startPos, int  endPos);
    wxString GetSelectedText();
    void     HideSelection(bool hide);
    bool     GetHideSelection();

    int      GetTextLength();
    int      GetFirstVisibleLine();
    bool     GetModified();
    int      GetLineCount();
    wxRect   GetRect();
    int      GetLineFromPos(int pos);
    int      GetLineStartPos(int line);
    int      GetLineLengthAtPos(int pos);
    int      GetLineLength(int line);
    wxString GetCurrentLineText();
    int      GetCurrentLine();
    int      PositionFromPoint(wxPoint pt);
    int      LineFromPoint(wxPoint pt);
    wxPoint  PointFromPosition(int pos);
    int      GetCurrentPos();
    int      GetAnchor();
    void     SelectAll();
    void     SetCurrentPosition(int pos);
    void     SetAnchor(int pos);
    void     GotoPos(int pos);
    void     GotoLine(int line);
    void     ChangePosition(int delta, bool extendSelection);
    void     PageMove(int cmdKey, bool extendSelection);

    void     ScrollBy(int columnDelta, int lineDelta);
    void     ScrollToLine(int line);
    void     ScrollToColumn(int column);
    void     EnsureCaretVisible();
    void     SetCaretPolicy(int policy, int slop=0);
    int      GetSelectionType();



    // Searching
    int      FindText(int minPos, int maxPos, const wxString& text,
                      bool caseSensitive, bool wholeWord);
    void     SearchAnchor();
    int      SearchNext(const wxString& text, bool caseSensitive, bool wholeWord);
    int      SearchPrev(const wxString& text, bool caseSensitive, bool wholeWord);


    // Visible whitespace
    bool     GetViewWhitespace();
    void     SetViewWhitespace(bool visible);


    // Line endings
    wxSTC_EOL GetEOLMode();
    void     SetEOLMode(wxSTC_EOL mode);
    bool     GetViewEOL();
    void     SetViewEOL(bool visible);
    void     ConvertEOL(wxSTC_EOL mode);


    // Styling
    int      GetEndStyled();
    void     StartStyling(int pos, int mask);
    void     SetStyleFor(int length, int style);
    void     SetStyleBytes(int length, char* styleBytes);


    // Style Definition
    void     StyleClearAll();
    void     StyleResetDefault();
    void     StyleSetSpec(int styleNum, const wxString& spec);
    void     StyleSetForeground(int styleNum, const wxColour& colour);
    void     StyleSetBackground(int styleNum, const wxColour& colour);
    void     StyleSetFont(int styleNum, wxFont& font);
    void     StyleSetFontAttr(int styleNum, int size, const wxString& faceName, bool bold, bool italic);
    void     StyleSetBold(int styleNum, bool bold);
    void     StyleSetItalic(int styleNum, bool italic);
    void     StyleSetFaceName(int styleNum, const wxString& faceName);
    void     StyleSetSize(int styleNum, int pointSize);
    void     StyleSetEOLFilled(int styleNum, bool fillEOL);


    // Margins in the edit area
    int      GetLeftMargin();
    int      GetRightMargin();
    void     SetMargins(int left, int right);


    // Margins for selection, markers, etc.
    void SetMarginType(int margin, int type);
    int  GetMarginType(int margin);
    void SetMarginWidth(int margin, int pixelWidth);
    int  GetMarginWidth(int margin);
    void SetMarginMask(int margin, int mask);
    int  GetMarginMask(int margin);
    void SetMarginSensitive(int margin, bool sensitive);
    bool GetMarginSensitive(int margin);


    // Selection and Caret styles
    void     SetSelectionForeground(const wxColour& colour);
    void     SetSelectionBackground(const wxColour& colour);
    void     SetCaretForeground(const wxColour& colour);
    int      GetCaretPeriod();
    void     SetCaretPeriod(int milliseconds);


    // Other settings
    void     SetBufferedDraw(bool isBuffered);
    void     SetTabWidth(int numChars);
    void     SetWordChars(const wxString& wordChars);


    // Brace highlighting
    void     BraceHighlight(int pos1, int pos2);
    void     BraceBadlight(int pos);
    int      BraceMatch(int pos, int maxReStyle=0);


    // Markers
    void     MarkerDefine(int markerNumber, int markerSymbol,
                          const wxColour& foreground,
                          const wxColour& background);
    void     MarkerSetType(int markerNumber, int markerSymbol);
    void     MarkerSetForeground(int markerNumber, const wxColour& colour);
    void     MarkerSetBackground(int markerNumber, const wxColour& colour);
    int      MarkerAdd(int line, int markerNumber);
    void     MarkerDelete(int line, int markerNumber);
    void     MarkerDeleteAll(int markerNumber);
    int      MarkerGet(int line);
    int      MarkerGetNextLine(int lineStart, int markerMask);
    int      MarkerGetPrevLine(int lineStart, int markerMask);
    int      MarkerLineFromHandle(int handle);
    void     MarkerDeleteHandle(int handle);


    // Indicators
    void     IndicatorSetStyle(int indicNum, int indicStyle);
    int      IndicatorGetStyle(int indicNum);
    void     IndicatorSetColour(int indicNum, const wxColour& colour);


    // Auto completion
    void     AutoCompShow(const wxString& listOfWords);
    void     AutoCompCancel();
    bool     AutoCompActive();
    int      AutoCompPosAtStart();
    void     AutoCompComplete();
    void     AutoCompStopChars(const wxString& stopChars);


    // Call tips
    void     CallTipShow(int pos, const wxString& text);
    void     CallTipCancel();
    bool     CallTipActive();
    int      CallTipPosAtStart();
    void     CallTipSetHighlight(int start, int end);
    void     CallTipSetBackground(const wxColour& colour);


    // Key bindings
    void     CmdKeyAssign(int key, int modifiers, int cmd);
    void     CmdKeyClear(int key, int modifiers);
    void     CmdKeyClearAll();
    void     CmdKeyExecute(int cmd);


    // Print formatting
    int      FormatRange(bool   doDraw,
                         int    startPos,
                         int    endPos,
                         wxDC*  draw,
                         wxDC*  target,  // Why does it use two? Can they be the same?
                         wxRect renderRect,
                         wxRect pageRect);


    // Document Sharing (multiple views)
    void*    GetDocument();
    void     SetDocument(void* document);
    // TODO: create a wx wrapper for Scintilla's document class


    // Folding
    int      VisibleFromDocLine(int docLine);
    int      DocLineFromVisible(int displayLine);
    int      SetFoldLevel(int line, int level);
    int      GetFoldLevel(int line);
    int      GetLastChild(int line, int level);
    int      GetFoldParent(int line);
    void     ShowLines(int lineStart, int lineEnd);
    void     HideLines(int lineStart, int lineEnd);
    bool     GetLineVisible(int line);
    void     SetFoldExpanded(int line, bool expanded);
    bool     GetFoldExpanded(int line);
    void     ToggleFold(int line);
    void     EnsureVisible(int line);
    void     SetFoldFlags(int flags);


    // Long Lines
    int      GetEdgeColumn();
    void     SetEdgeColumn(int column);
    wxSTC_EDGE GetEdgeMode();
    void     SetEdgeMode(wxSTC_EDGE mode);
    wxColour GetEdgeColour();
    void     SetEdgeColour(const wxColour& colour);


    // Lexer
    void     SetLexer(wxSTC_LEX lexer);
    wxSTC_LEX GetLexer();
    void     Colourise(int start, int end);
    void     SetProperty(const wxString& key, const wxString& value);
    void     SetKeywords(int keywordSet, const wxString& keywordList);


#ifndef SWIG
private:
    // Event handlers
    void OnPaint(wxPaintEvent& evt);
    void OnScrollWin(wxScrollWinEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnMouseLeftDown(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnMouseLeftUp(wxMouseEvent& evt);
    void OnMouseRightUp(wxMouseEvent& evt);
    void OnChar(wxKeyEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);
    void OnLoseFocus(wxFocusEvent& evt);
    void OnGainFocus(wxFocusEvent& evt);
    void OnSysColourChanged(wxSysColourChangedEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnMenu(wxCommandEvent& evt);


    // Turn notifications from Scintilla into events
    void NotifyChange();
    void NotifyParent(SCNotification* scn);

    long SendMsg(int msg, long wp=0, long lp=0);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxStyledTextCtrl)

    ScintillaWX*        m_swx;
    wxStopWatch         m_stopWatch;
    bool                m_readOnly;
    wxSTC_UndoType      m_undoType;


    friend class ScintillaWX;
    friend class Platform;
#endif
};

//----------------------------------------------------------------------

class wxStyledTextEvent : public wxCommandEvent {
public:
    wxStyledTextEvent(wxEventType commandType=0, int id=0);
    ~wxStyledTextEvent() {}

    void SetPosition(int pos)        { m_position = pos; }
    void SetKey(int k)               { m_key = k; }
    void SetModifiers(int m)         { m_modifiers = m; }
    void SetModificationType(int t)  { m_modificationType = t; }
    void SetText(const char* t)      { m_text = t; }
    void SetLength(int len)          { m_length = len; }
    void SetLinesAdded(int num)      { m_linesAdded = num; }
    void SetLine(int val)            { m_line = val; }
    void SetFoldLevelNow(int val)    { m_foldLevelNow = val; }
    void SetFoldLevelPrev(int val)   { m_foldLevelPrev = val; }
    void SetMargin(int val)          { m_margin = val; }
    void SetMessage(int val)         { m_message = val; }
    void SetWParam(int val)          { m_wParam = val; }
    void SetLParam(int val)          { m_lParam = val; }

    int  GetPosition() const         { return m_position; }
    int  GetKey()  const             { return m_key; }
    int  GetModifiers() const        { return m_modifiers; }
    int  GetModificationType() const { return m_modificationType; }
    wxString GetText() const         { return m_text; }
    int  GetLength() const           { return m_length; }
    int  GetLinesAdded() const       { return m_linesAdded; }
    int  GetLine() const             { return m_line; }
    int  GetFoldLevelNow() const     { return m_foldLevelNow; }
    int  GetFoldLevelPrev() const    { return m_foldLevelPrev; }
    int  GetMargin() const           { return m_margin; }
    int  GetMessage() const          { return m_message; }
    int  GetWParam() const           { return m_wParam; }
    int  GetLParam() const           { return m_lParam; }

    bool GetShift() const;
    bool GetControl() const;
    bool GetAlt() const;

    void CopyObject(wxObject& obj) const;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS(wxStyledTextEvent)

    int  m_position;
    int  m_key;
    int  m_modifiers;

    int  m_modificationType;    // wxEVT_STC_MODIFIED
    wxString m_text;
    int  m_length;
    int  m_linesAdded;
    int  m_line;
    int  m_foldLevelNow;
    int  m_foldLevelPrev;

    int  m_margin;              // wxEVT_STC_MARGINCLICK

    int  m_message;             // wxEVT_STC_MACRORECORD
    int  m_wParam;
    int  m_lParam;
#endif
};



enum {
    wxEVT_STC_CHANGE = 1650,
    wxEVT_STC_STYLENEEDED,
    wxEVT_STC_CHARADDED,
    wxEVT_STC_UPDATEUI,
    wxEVT_STC_SAVEPOINTREACHED,
    wxEVT_STC_SAVEPOINTLEFT,
    wxEVT_STC_ROMODIFYATTEMPT,
    wxEVT_STC_DOUBLECLICK,
    wxEVT_STC_MODIFIED,
    wxEVT_STC_KEY,
    wxEVT_STC_MACRORECORD,
    wxEVT_STC_MARGINCLICK,
    wxEVT_STC_NEEDSHOWN
};

#ifndef SWIG
typedef void (wxEvtHandler::*wxStyledTextEventFunction)(wxStyledTextEvent&);

#define EVT_STC_CHANGE(id, fn) { wxEVT_STC_CHANGE, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_STYLENEEDED(id, fn) { wxEVT_STC_STYLENEEDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_CHARADDED(id, fn) { wxEVT_STC_CHARADDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_UPDATEUI(id, fn) { wxEVT_STC_UPDATEUI, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_SAVEPOINTREACHED(id, fn) { wxEVT_STC_SAVEPOINTREACHED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_SAVEPOINTLEFT(id, fn) { wxEVT_STC_SAVEPOINTLEFT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_ROMODIFYATTEMPT(id, fn) { wxEVT_STC_ROMODIFYATTEMPT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_DOUBLECLICK(id, fn) { wxEVT_STC_DOUBLECLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_MODIFIED(id, fn) { wxEVT_STC_MODIFIED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_CMDKEY(id, fn) { wxEVT_STC_CMDKEY, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_MACRORECORD(id, fn) { wxEVT_STC_MACRORECORD, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_MARGINCLICK(id, fn) { wxEVT_STC_MARGINCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },
#define EVT_STC_NEEDSHOWN(id, fn) { wxEVT_STC_NEEDSHOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction) & fn, (wxObject *) NULL },

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
#endif


