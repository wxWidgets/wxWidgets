/////////////////////////////////////////////////////////////////////////////
// Name:        stc.i
// Purpose:     Wrappers for the wxStyledTextCtrl.
//
// Author:      Robin Dunn
//
// Created:     12-Oct-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module stc_


%{
#include "helpers.h"
#include "wx/stc/stc.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i

// Get all our defs from the real header file.  This is a trial...
%include stc.h


//----------------------------------------------------------------------
#ifdef THE_OLD_WAY


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


enum {
    wxSTC_LEX_STYLE_MAX,
    wxSTC_STYLE_DEFAULT,
    wxSTC_STYLE_LINENUMBER,
    wxSTC_STYLE_BRACELIGHT,
    wxSTC_STYLE_BRACEBAD,
    wxSTC_STYLE_CONTROLCHAR,
    wxSTC_STYLE_MAX,
    wxSTC_STYLE_MASK,

    wxSTC_MARKER_MAX,
    wxSTC_MARK_CIRCLE,
    wxSTC_MARK_ROUNDRECT,
    wxSTC_MARK_ARROW,
    wxSTC_MARK_SMALLRECT,
    wxSTC_MARK_SHORTARROW,
    wxSTC_MARK_EMPTY,
    wxSTC_MARK_ARROWDOWN,
    wxSTC_MARK_MINUS,
    wxSTC_MARK_PLUS,
    wxSTC_MARKNUM_FOLDER,
    wxSTC_MARKNUM_FOLDEROPEN,
    wxSTC_MASK_FOLDERS,

    wxSTC_INDIC_PLAIN,
    wxSTC_INDIC_SQUIGGLE,
    wxSTC_INDIC_TT,
    wxSTC_INDIC0_MASK,
    wxSTC_INDIC1_MASK,
    wxSTC_INDIC2_MASK,
    wxSTC_INDICS_MASK,

    wxSTC_FOLDLEVELBASE,
    wxSTC_FOLDLEVELWHITEFLAG,
    wxSTC_FOLDLEVELHEADERFLAG,
    wxSTC_FOLDLEVELNUMBERMASK,

};

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


enum {
    wxSTC_CARET_SLOP,
    WXSTC_CARET_CENTER,
    wxSTC_CARET_STRICT,

    wxSTC_MARGIN_SYMBOL,
    wxSTC_MARGIN_NUMBER,
};


class  ScintillaWX;                      // forward declare
class  WordList;
struct SCNotification;


extern const wxChar* wxSTCNameStr;

//----------------------------------------------------------------------

class wxStyledTextCtrl : public wxControl {
public:

    wxStyledTextCtrl(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0,
                     const char* name = wxSTCNameStr);
    //~wxStyledTextCtrl();



    // Text retrieval and modification
    wxString GetText();
    bool     SetText(const wxString& text);
    wxString GetLine(int line);
    void     ReplaceSelection(const wxString& text);
    void     SetReadOnly(bool readOnly);
    bool     GetReadOnly();
    wxString GetTextRange(int startPos, int endPos);
    wxString GetStyledTextRange(int startPos, int endPos);
    //void     GetTextRange(int startPos, int endPos, char* buff);
    //void     GetStyledTextRange(int startPos, int endPos, char* buff);
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
    void     GetSelection(int* OUTPUT, int* OUTPUT);
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
    wxString GetCurrentLineText(/* TODO?? int* linePos=NULL*/);
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
    int      GetLastChild(int line);
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


};

//----------------------------------------------------------------------

class wxStyledTextEvent : public wxCommandEvent {
public:
    wxStyledTextEvent(wxEventType commandType, int id);
    ~wxStyledTextEvent() {}

    void SetPosition(int pos);
    void SetKey(int k);
    void SetModifiers(int m);
    void SetModificationType(int t);
    void SetText(const char* t);
    void SetLength(int len);
    void SetLinesAdded(int num);
    void SetLine(int val);
    void SetFoldLevelNow(int val);
    void SetFoldLevelPrev(int val);
    void SetMargin(int val);
    void SetMessage(int val);
    void SetWParam(int val);
    void SetLParam(int val);

    int  GetPosition() const;
    int  GetKey()  const;
    int  GetModifiers() const;
    int  GetModificationType() const;
    wxString GetText() const;
    int  GetLength() const;
    int  GetLinesAdded() const;
    int  GetLine() const;
    int  GetFoldLevelNow() const;
    int  GetFoldLevelPrev() const;
    int  GetMargin() const;
    int  GetMessage() const;
    int  GetWParam() const;
    int  GetLParam() const;

    bool GetShift() const;
    bool GetControl() const;
    bool GetAlt() const;

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



// End of THE_OLD_WAY
#endif



%pragma(python) code = "
def EVT_STC_CHANGE(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHANGE, fn)

def EVT_STC_STYLENEEDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_STYLENEEDED, fn)

def EVT_STC_CHARADDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHARADDED, fn)

def EVT_STC_UPDATEUI(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_UPDATEUI, fn)

def EVT_STC_SAVEPOINTREACHED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTREACHED, fn)

def EVT_STC_SAVEPOINTLEFT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTLEFT, fn)

def EVT_STC_ROMODIFYATTEMPT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_ROMODIFYATTEMPT, fn)

def EVT_STC_DOUBLECLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_DOUBLECLICK, fn)

def EVT_STC_MODIFIED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MODIFIED, fn)

def EVT_STC_CMDKEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CMDKEY, fn)

def EVT_STC_UNKNOWNCMDKEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_UNKNOWNCMDKEY, fn)

"

//----------------------------------------------------------------------
//----------------------------------------------------------------------


