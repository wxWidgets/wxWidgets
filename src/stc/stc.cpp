////////////////////////////////////////////////////////////////////////////
// Name:        stc.cpp
// Purpose:     A wxWindows implementation of Scintilla.  This class is the
//              one meant to be used directly by wx applications.  It does not
//              derive directly from the Scintilla classes, but instead
//              delegates most things to the real Scintilla class.
//              This allows the use of Scintilla without polluting the
//              namespace with all the classes and identifiers from Scintilla.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <ctype.h>

#include "wx/stc/stc.h"
#include "ScintillaWX.h"

#include <wx/tokenzr.h>

// The following code forces a reference to all of the Scintilla lexers.
// If we don't do something like this, then the linker tends to "optimize"
// them away. (eric@sourcegear.com)

int wxForceScintillaLexers(void)
{
  extern LexerModule lmCPP;
  extern LexerModule lmHTML;
  extern LexerModule lmXML;
  extern LexerModule lmProps;
  extern LexerModule lmErrorList;
  extern LexerModule lmMake;
  extern LexerModule lmBatch;
  extern LexerModule lmPerl;
  extern LexerModule lmPython;
  extern LexerModule lmSQL;
  extern LexerModule lmVB;

  if (
      &lmCPP
      && &lmHTML
      && &lmXML
      && &lmProps
      && &lmErrorList
      && &lmMake
      && &lmBatch
      && &lmPerl
      && &lmPython
      && &lmSQL
      && &lmVB
      )
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

//----------------------------------------------------------------------

const wxChar* wxSTCNameStr = "stcwindow";

BEGIN_EVENT_TABLE(wxStyledTextCtrl, wxControl)
    EVT_PAINT                   (wxStyledTextCtrl::OnPaint)
    EVT_SCROLLWIN               (wxStyledTextCtrl::OnScrollWin)
    EVT_SIZE                    (wxStyledTextCtrl::OnSize)
    EVT_LEFT_DOWN               (wxStyledTextCtrl::OnMouseLeftDown)
    EVT_MOTION                  (wxStyledTextCtrl::OnMouseMove)
    EVT_LEFT_UP                 (wxStyledTextCtrl::OnMouseLeftUp)
    EVT_RIGHT_UP                (wxStyledTextCtrl::OnMouseRightUp)
    EVT_CHAR                    (wxStyledTextCtrl::OnChar)
    EVT_KEY_DOWN                (wxStyledTextCtrl::OnKeyDown)
    EVT_KILL_FOCUS              (wxStyledTextCtrl::OnLoseFocus)
    EVT_SET_FOCUS               (wxStyledTextCtrl::OnGainFocus)
    EVT_SYS_COLOUR_CHANGED      (wxStyledTextCtrl::OnSysColourChanged)
    EVT_ERASE_BACKGROUND        (wxStyledTextCtrl::OnEraseBackground)
    EVT_MENU_RANGE              (-1, -1, wxStyledTextCtrl::OnMenu)
END_EVENT_TABLE()


IMPLEMENT_CLASS(wxStyledTextCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxStyledTextEvent, wxCommandEvent)

//----------------------------------------------------------------------
// Constructor and Destructor

wxStyledTextCtrl::wxStyledTextCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name) :
    wxControl(parent, id, pos, size,
              style | wxVSCROLL | wxHSCROLL | wxWANTS_CHARS,
              wxDefaultValidator, name)
{
    m_swx = new ScintillaWX(this);
    // m_keywords = new WordList;
    m_stopWatch.Start();
    m_readOnly = false;
    m_undoType = wxSTC_UndoCollectAutoStart;
}


wxStyledTextCtrl::~wxStyledTextCtrl() {
    delete m_swx;
    // delete m_keywords;
}


//----------------------------------------------------------------------

long wxStyledTextCtrl::SendMsg(int msg, long wp, long lp) {

    return m_swx->WndProc(msg, wp, lp);
}


//----------------------------------------------------------------------
// Text retrieval and modification

wxString wxStyledTextCtrl::GetText() {
    wxString text;
    int   len  = GetTextLength();
    char* buff = text.GetWriteBuf(len);

    SendMsg(WM_GETTEXT, len, (long)buff);
    text.UngetWriteBuf();
    return text;
}


bool wxStyledTextCtrl::SetText(const wxString& text) {
    return SendMsg(WM_SETTEXT, 0, (long)text.c_str()) != 0;
}


wxString wxStyledTextCtrl::GetLine(int line) {
    wxString text;
    int   len  = GetLineLength(line);
    char* buff = text.GetWriteBuf(len+1);

    *((WORD*)buff) = len;
    SendMsg(EM_GETLINE, line, (long)buff);
    buff[len] = 0;
    text.UngetWriteBuf();
    return text;
}


void wxStyledTextCtrl::ReplaceSelection(const wxString& text) {
    SendMsg(EM_REPLACESEL, 0, (long)text.c_str());
}


void wxStyledTextCtrl::SetReadOnly(bool readOnly) {
    SendMsg(EM_SETREADONLY, (long)readOnly);
    m_readOnly = readOnly;
}


bool wxStyledTextCtrl::GetReadOnly() {
    // TODO: need support in Scintilla to do this right,
    //       until then we'll track it ourselves
    return m_readOnly;
}


void wxStyledTextCtrl::GetTextRange(int startPos, int endPos, char* buff) {
    TEXTRANGE tr;
    tr.lpstrText = buff;
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    SendMsg(EM_GETTEXTRANGE, 0, (long)&tr);
}


wxString wxStyledTextCtrl::GetTextRange(int startPos, int endPos) {
    wxString  text;
    int       len  = endPos - startPos;
    char*     buff = text.GetWriteBuf(len);
    GetTextRange(startPos, endPos, buff);
    text.UngetWriteBuf();
    return text;
}


void wxStyledTextCtrl::GetStyledTextRange(int startPos, int endPos, char* buff) {
    TEXTRANGE tr;
    tr.lpstrText = buff;
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    SendMsg(SCI_GETSTYLEDTEXT, 0, (long)&tr);
}


wxString wxStyledTextCtrl::GetStyledTextRange(int startPos, int endPos) {
    wxString  text;
    int       len  = endPos - startPos;
    char*     buff = text.GetWriteBuf(len*2);
    GetStyledTextRange(startPos, endPos, buff);
    text.UngetWriteBuf(len*2);
    return text;
}


void wxStyledTextCtrl::AddText(const wxString& text) {
    SendMsg(SCI_ADDTEXT, text.Len(), (long)text.c_str());
}


void wxStyledTextCtrl::AddStyledText(const wxString& text) {
    SendMsg(SCI_ADDSTYLEDTEXT, text.Len(), (long)text.c_str());
}


void wxStyledTextCtrl::InsertText(int pos, const wxString& text) {
    SendMsg(SCI_INSERTTEXT, pos, (long)text.c_str());
}


void wxStyledTextCtrl::ClearAll() {
    SendMsg(SCI_CLEARALL);
}


char wxStyledTextCtrl::GetCharAt(int pos) {
    return SendMsg(SCI_GETCHARAT, pos);
}


char wxStyledTextCtrl::GetStyleAt(int pos) {
    return SendMsg(SCI_GETSTYLEAT, pos);
}


void wxStyledTextCtrl::SetStyleBits(int bits) {
    SendMsg(SCI_SETSTYLEBITS, bits);
}


int  wxStyledTextCtrl::GetStyleBits() {
    return SendMsg(SCI_GETSTYLEBITS);
}


//----------------------------------------------------------------------
// Clipboard


void wxStyledTextCtrl::Cut() {
    SendMsg(WM_CUT);
}


void wxStyledTextCtrl::Copy() {
    SendMsg(WM_COPY);
}


void wxStyledTextCtrl::Paste() {
    SendMsg(WM_PASTE);
}


bool wxStyledTextCtrl::CanPaste() {
    return SendMsg(EM_CANPASTE) != 0;
}


void wxStyledTextCtrl::ClearClipbrd() {
    SendMsg(WM_CLEAR);
}



//----------------------------------------------------------------------
// Undo and Redo

void wxStyledTextCtrl::Undo() {
    SendMsg(WM_UNDO);
}


bool wxStyledTextCtrl::CanUndo() {
    return SendMsg(EM_CANUNDO) != 0;
}


void wxStyledTextCtrl::EmptyUndoBuffer() {
    SendMsg(EM_EMPTYUNDOBUFFER);
}


void wxStyledTextCtrl::Redo() {
    SendMsg(SCI_REDO);
}


bool wxStyledTextCtrl::CanRedo() {
    return SendMsg(SCI_CANREDO) != 0;
}


void wxStyledTextCtrl::SetUndoCollection(wxSTC_UndoType type) {
    SendMsg(SCI_SETUNDOCOLLECTION, type);
    m_undoType = type;
}


wxSTC_UndoType wxStyledTextCtrl::GetUndoCollection() {
    // TODO: need support in Scintilla to do this right,
    //       until then we'll track it ourselves
    return m_undoType;
}


void wxStyledTextCtrl::BeginUndoAction() {
    SendMsg(SCI_BEGINUNDOACTION);
}


void wxStyledTextCtrl::EndUndoAction() {
    SendMsg(SCI_ENDUNDOACTION);
}




//----------------------------------------------------------------------
// Selection and information


void wxStyledTextCtrl::GetSelection(int* startPos, int* endPos) {
    SendMsg(EM_GETSEL, (long)startPos, (long)endPos);
}


void wxStyledTextCtrl::SetSelection(int  startPos, int  endPos) {
    SendMsg(EM_SETSEL, startPos, endPos);
}


wxString wxStyledTextCtrl::GetSelectedText() {
    wxString text;
    int   start;
    int   end;

    GetSelection(&start, &end);
    int   len  = end - start;
    char* buff = text.GetWriteBuf(len);

    SendMsg(EM_GETSELTEXT, 0, (long)buff);
    text.UngetWriteBuf();
    return text;
}


void wxStyledTextCtrl::HideSelection(bool hide) {
    SendMsg(EM_HIDESELECTION, hide);
}


bool wxStyledTextCtrl::GetHideSelection() {
    return m_swx->GetHideSelection();
}


int wxStyledTextCtrl::GetTextLength() {
    return SendMsg(WM_GETTEXTLENGTH);
}


int wxStyledTextCtrl::GetFirstVisibleLine() {
    return SendMsg(EM_GETFIRSTVISIBLELINE);
}


int wxStyledTextCtrl::GetLineCount() {
    return SendMsg(EM_GETLINECOUNT);
}


bool wxStyledTextCtrl::GetModified() {
    return SendMsg(EM_GETMODIFY) != 0;
}


wxRect wxStyledTextCtrl::GetRect() {
    PRectangle pr;
    SendMsg(EM_GETRECT, 0, (long)&pr);

    wxRect rect = wxRectFromPRectangle(pr);
    return rect;
}


int wxStyledTextCtrl::GetLineFromPos(int pos) {
    return SendMsg(EM_LINEFROMCHAR, pos);
}


int wxStyledTextCtrl::GetLineStartPos(int line) {
    return SendMsg(EM_LINEINDEX, line);
}


int wxStyledTextCtrl::GetLineLengthAtPos(int pos) {
    return SendMsg(EM_LINELENGTH, pos);
}


int wxStyledTextCtrl::GetLineLength(int line) {
    return SendMsg(SCI_LINELENGTH, line);
}


int wxStyledTextCtrl::GetCurrentLine() {
    int line = GetLineFromPos(GetCurrentPos());
    return line;
}


wxString wxStyledTextCtrl::GetCurrentLineText(int* linePos) {
    wxString text;
    int   len  = GetLineLength(GetCurrentLine());
    char* buff = text.GetWriteBuf(len+1);

    int pos = SendMsg(SCI_GETCURLINE, len, (long)buff);
    text.UngetWriteBuf();

    if (linePos)
        *linePos = pos;

    return text;
}


int wxStyledTextCtrl::PositionFromPoint(wxPoint pt) {
    Point spt(pt.x, pt.y);
    long rv = SendMsg(EM_CHARFROMPOS, 0, (long)&spt);
    return LOWORD(rv);
}


int wxStyledTextCtrl::LineFromPoint(wxPoint pt) {
    Point spt(pt.x, pt.y);
    long rv = SendMsg(EM_CHARFROMPOS, 0, (long)&spt);
    return HIWORD(rv);
}


wxPoint wxStyledTextCtrl::PointFromPosition(int pos) {
    Point pt;
    SendMsg(EM_POSFROMCHAR, (long)&pt, pos);
    return wxPoint(pt.x, pt.y);
}


int wxStyledTextCtrl::GetCurrentPos() {
    return SendMsg(SCI_GETCURRENTPOS);
}


int wxStyledTextCtrl::GetAnchor() {
    return SendMsg(SCI_GETANCHOR);
}


void wxStyledTextCtrl::SelectAll() {
    SendMsg(SCI_SELECTALL);
}


void wxStyledTextCtrl::SetCurrentPosition(int pos) {
    SendMsg(SCI_GOTOPOS, pos);
}


void wxStyledTextCtrl::SetAnchor(int pos) {
    SendMsg(SCI_SETANCHOR, pos);
}


void wxStyledTextCtrl::GotoPos(int pos) {
    SendMsg(SCI_GOTOPOS, pos);
}


void wxStyledTextCtrl::GotoLine(int line) {
    SendMsg(SCI_GOTOLINE, line);
}


void wxStyledTextCtrl::ChangePosition(int delta, bool extendSelection) {
    // TODO:  Is documented but doesn't seem to be implemented
    //SendMsg(SCI_CHANGEPOSITION, delta, extendSelection);
}


void wxStyledTextCtrl::PageMove(int cmdKey, bool extendSelection) {
    // TODO:  Is documented but doesn't seem to be implemented
    //SendMsg(SCI_PAGEMOVE, cmdKey, extendSelection);
}


void wxStyledTextCtrl::ScrollBy(int columnDelta, int lineDelta) {
    SendMsg(EM_LINESCROLL, columnDelta, lineDelta);
}

void wxStyledTextCtrl::ScrollToLine(int line) {
    m_swx->DoScrollToLine(line);
}


void wxStyledTextCtrl::ScrollToColumn(int column) {
    m_swx->DoScrollToColumn(column);
}


void wxStyledTextCtrl::EnsureCaretVisible() {
    SendMsg(EM_SCROLLCARET);
}


void wxStyledTextCtrl::SetCaretPolicy(int policy, int slop) {
    SendMsg(SCI_SETCARETPOLICY, policy, slop);
}


int wxStyledTextCtrl::GetSelectionType() {
    return SendMsg(EM_SELECTIONTYPE);
}


int wxStyledTextCtrl::GetLinesOnScreen() {
    return SendMsg(SCI_LINESONSCREEN);
}


bool wxStyledTextCtrl::IsSelectionRectangle() {
    return SendMsg(SCI_SELECTIONISRECTANGLE) != 0;
}


void wxStyledTextCtrl::SetUseHorizontalScrollBar(bool use) {
    SendMsg(SCI_SETHSCROLLBAR, use);
}


bool wxStyledTextCtrl::GetUseHorizontalScrollBar() {
    return SendMsg(SCI_GETHSCROLLBAR) != 0;
}





//----------------------------------------------------------------------
// Searching

int wxStyledTextCtrl::FindText(int minPos, int maxPos,
                                     const wxString& text,
                                     bool caseSensitive, bool wholeWord) {
    FINDTEXTEX  ft;
    int         flags = 0;

    flags |= caseSensitive ? FR_MATCHCASE : 0;
    flags |= wholeWord     ? FR_WHOLEWORD : 0;
    ft.chrg.cpMin = minPos;
    ft.chrg.cpMax = maxPos;
    ft.lpstrText = (char*)text.c_str();

    return SendMsg(EM_FINDTEXT, flags, (long)&ft);
}


void wxStyledTextCtrl::SearchAnchor() {
    SendMsg(SCI_SEARCHANCHOR);
}


int wxStyledTextCtrl::SearchNext(const wxString& text, bool caseSensitive, bool wholeWord) {
    int flags = 0;
    flags |= caseSensitive ? FR_MATCHCASE : 0;
    flags |= wholeWord     ? FR_WHOLEWORD : 0;

    return SendMsg(SCI_SEARCHNEXT, flags, (long)text.c_str());
}


int wxStyledTextCtrl::SearchPrev(const wxString& text, bool caseSensitive, bool wholeWord) {
    int flags = 0;
    flags |= caseSensitive ? FR_MATCHCASE : 0;
    flags |= wholeWord     ? FR_WHOLEWORD : 0;

    return SendMsg(SCI_SEARCHPREV, flags, (long)text.c_str());
}

//----------------------------------------------------------------------
// Visible whitespace


bool wxStyledTextCtrl::GetViewWhitespace() {
    return SendMsg(SCI_GETVIEWWS) != 0;
}


void wxStyledTextCtrl::SetViewWhitespace(bool visible) {
    SendMsg(SCI_SETVIEWWS, visible);
}



//----------------------------------------------------------------------
// Line endings

wxSTC_EOL wxStyledTextCtrl::GetEOLMode() {
    return (wxSTC_EOL)SendMsg(SCI_GETEOLMODE);
}


void wxStyledTextCtrl::SetEOLMode(wxSTC_EOL mode) {
    SendMsg(SCI_SETEOLMODE, mode);
}


bool wxStyledTextCtrl::GetViewEOL() {
    return SendMsg(SCI_GETVIEWEOL) != 0;
}


void wxStyledTextCtrl::SetViewEOL(bool visible) {
    SendMsg(SCI_SETVIEWEOL, visible);
}

void wxStyledTextCtrl::ConvertEOL(wxSTC_EOL mode) {
    SendMsg(SCI_CONVERTEOLS, mode);
}

//----------------------------------------------------------------------
// Styling

int wxStyledTextCtrl::GetEndStyled() {
    return SendMsg(SCI_GETENDSTYLED);
}


void wxStyledTextCtrl::StartStyling(int pos, int mask) {
    SendMsg(SCI_STARTSTYLING, pos, mask);
}


void wxStyledTextCtrl::SetStyleFor(int length, int style) {
    SendMsg(SCI_SETSTYLING, length, style);
}


void wxStyledTextCtrl::SetStyleBytes(int length, char* styleBytes) {
    SendMsg(SCI_SETSTYLINGEX, length, (long)styleBytes);
}


void wxStyledTextCtrl::SetLineState(int line, int value) {
    SendMsg(SCI_SETLINESTATE, line, value);
}


int  wxStyledTextCtrl::GetLineState(int line) {
    return SendMsg(SCI_GETLINESTATE, line);
}


//----------------------------------------------------------------------
// Style Definition


static long wxColourAsLong(const wxColour& co) {
    return (((long)co.Blue()  << 16) |
            ((long)co.Green() <<  8) |
            ((long)co.Red()));
}

static wxColour wxColourFromLong(long c) {
    wxColour clr;
    clr.Set(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    return clr;
}


static wxColour wxColourFromSpec(const wxString& spec) {
    // spec should be #RRGGBB
    char* junk;
    int red   = strtol(spec.Mid(1,2), &junk, 16);
    int green = strtol(spec.Mid(3,2), &junk, 16);
    int blue  = strtol(spec.Mid(5,2), &junk, 16);
    return wxColour(red, green, blue);
}


void wxStyledTextCtrl::StyleClearAll() {
    SendMsg(SCI_STYLECLEARALL);
}


void wxStyledTextCtrl::StyleResetDefault() {
    SendMsg(SCI_STYLERESETDEFAULT);
}



// Extract style settings from a spec-string which is composed of one or
// more of the following comma separated elements:
//
//      bold                    turns on bold
//      italic                  turns on italics
//      fore:#RRGGBB            sets the foreground colour
//      back:#RRGGBB            sets the background colour
//      face:[facename]         sets the font face name to use
//      size:[num]              sets the font size in points
//      eol                     turns on eol filling
//

void wxStyledTextCtrl::StyleSetSpec(int styleNum, const wxString& spec) {

    wxStringTokenizer tkz(spec, ",");
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();

        wxString option = token.BeforeFirst(':');
        wxString val = token.AfterFirst(':');

        if (option == "bold")
            StyleSetBold(styleNum, true);

        else if (option == "italic")
            StyleSetItalic(styleNum, true);

        else if (option == "eol")
            StyleSetEOLFilled(styleNum, true);

        else if (option == "size") {
            long points;
            if (val.ToLong(&points))
                StyleSetSize(styleNum, points);
        }

        else if (option == "face")
            StyleSetFaceName(styleNum, val);

        else if (option == "fore")
            StyleSetForeground(styleNum, wxColourFromSpec(val));

        else if (option == "back")
            StyleSetBackground(styleNum, wxColourFromSpec(val));
    }
}


void wxStyledTextCtrl::StyleSetForeground(int styleNum, const wxColour& colour) {
    SendMsg(SCI_STYLESETFORE, styleNum, wxColourAsLong(colour));
}


void wxStyledTextCtrl::StyleSetBackground(int styleNum, const wxColour& colour) {
    SendMsg(SCI_STYLESETBACK, styleNum, wxColourAsLong(colour));
}


void wxStyledTextCtrl::StyleSetFont(int styleNum, wxFont& font) {
    int      size     = font.GetPointSize();
    wxString faceName = font.GetFaceName();
    bool     bold     = font.GetWeight() == wxBOLD;
    bool     italic   = font.GetStyle() != wxNORMAL;

    StyleSetFontAttr(styleNum, size, faceName, bold, italic);
}


void wxStyledTextCtrl::StyleSetFontAttr(int styleNum, int size,
                                        const wxString& faceName,
                                        bool bold, bool italic) {
    StyleSetSize(styleNum, size);
    StyleSetFaceName(styleNum, faceName);
    StyleSetBold(styleNum, bold);
    StyleSetItalic(styleNum, italic);
}


void wxStyledTextCtrl::StyleSetBold(int styleNum, bool bold) {
    SendMsg(SCI_STYLESETBOLD, styleNum, bold);
}


void wxStyledTextCtrl::StyleSetItalic(int styleNum, bool italic) {
    SendMsg(SCI_STYLESETITALIC, styleNum, italic);
}


void wxStyledTextCtrl::StyleSetFaceName(int styleNum, const wxString& faceName) {
    SendMsg(SCI_STYLESETFONT, styleNum, (long)faceName.c_str());
}


void wxStyledTextCtrl::StyleSetSize(int styleNum, int pointSize) {
    SendMsg(SCI_STYLESETSIZE, styleNum, pointSize);
}


void wxStyledTextCtrl::StyleSetEOLFilled(int styleNum, bool fillEOL) {
    SendMsg(SCI_STYLESETEOLFILLED, styleNum, fillEOL);
}


//----------------------------------------------------------------------
// Margins in the edit area

int wxStyledTextCtrl::GetLeftMargin() {
    return LOWORD(SendMsg(EM_GETMARGINS));
}


int wxStyledTextCtrl::GetRightMargin() {
    return HIWORD(SendMsg(EM_GETMARGINS));
}


void wxStyledTextCtrl::SetMargins(int left, int right) {
    int flag = 0;
    int val = 0;

    if (right != -1) {
        flag |= EC_RIGHTMARGIN;
        val = right << 16;
    }
    if (left != -1) {
        flag |= EC_LEFTMARGIN;
        val |= (left & 0xffff);
    }

    SendMsg(EM_SETMARGINS, flag, val);
}


//----------------------------------------------------------------------
// Margins for selection, markers, etc.

void wxStyledTextCtrl::SetMarginType(int margin, int type) {
    SendMsg(SCI_SETMARGINTYPEN, margin, type);
}


int  wxStyledTextCtrl::GetMarginType(int margin) {
    return SendMsg(SCI_GETMARGINTYPEN, margin);
}


void wxStyledTextCtrl::SetMarginWidth(int margin, int pixelWidth) {
    SendMsg(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}


int  wxStyledTextCtrl::GetMarginWidth(int margin) {
    return SendMsg(SCI_GETMARGINWIDTHN, margin);
}


void wxStyledTextCtrl::SetMarginMask(int margin, int mask) {
    SendMsg(SCI_SETMARGINMASKN, margin, mask);
}


int  wxStyledTextCtrl::GetMarginMask(int margin) {
    return SendMsg(SCI_GETMARGINMASKN, margin);
}


void wxStyledTextCtrl::SetMarginSensitive(int margin, bool sensitive) {
    SendMsg(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}


bool wxStyledTextCtrl::GetMarginSensitive(int margin) {
    return SendMsg(SCI_GETMARGINSENSITIVEN, margin) != 0;
}




//----------------------------------------------------------------------
// Selection and Caret styles


void wxStyledTextCtrl::SetSelectionForeground(const wxColour& colour) {
    SendMsg(SCI_SETSELFORE, 0, wxColourAsLong(colour));
}


void wxStyledTextCtrl::SetSelectionBackground(const wxColour& colour) {
    SendMsg(SCI_SETSELBACK, 0, wxColourAsLong(colour));
}


void wxStyledTextCtrl::SetCaretForeground(const wxColour& colour) {
    SendMsg(SCI_SETCARETFORE, 0, wxColourAsLong(colour));
}


int wxStyledTextCtrl::GetCaretPeriod() {
    return SendMsg(SCI_GETCARETPERIOD);
}


void wxStyledTextCtrl::SetCaretPeriod(int milliseconds) {
    SendMsg(SCI_SETCARETPERIOD, milliseconds);
}



//----------------------------------------------------------------------
// Other settings


void wxStyledTextCtrl::SetBufferedDraw(bool isBuffered) {
    SendMsg(SCI_SETBUFFEREDDRAW, isBuffered);
}


void wxStyledTextCtrl::SetTabWidth(int numChars) {
    SendMsg(SCI_SETTABWIDTH, numChars);
}


void wxStyledTextCtrl::SetIndent(int numChars) {
    SendMsg(SCI_SETINDENT, numChars);
}


void wxStyledTextCtrl::SetUseTabs(bool usetabs) {
    SendMsg(SCI_SETUSETABS, usetabs);
}


void wxStyledTextCtrl::SetLineIndentation(int line, int indentation) {
    SendMsg(SCI_SETLINEINDENTATION, line, indentation);
}


int wxStyledTextCtrl:: GetLineIndentation(int line) {
    return SendMsg(SCI_GETLINEINDENTATION, line);
}


int  wxStyledTextCtrl::GetLineIndentationPos(int line) {
    return SendMsg(SCI_GETLINEINDENTPOSITION, line);
}


void wxStyledTextCtrl::SetWordChars(const wxString& wordChars) {
    SendMsg(SCI_SETTABWIDTH, 0, (long)wordChars.c_str());
}


void wxStyledTextCtrl::SetUsePop(bool usepopup) {
    SendMsg(SCI_USEPOPUP, usepopup);
}


//----------------------------------------------------------------------
// Brace highlighting


void wxStyledTextCtrl::BraceHighlight(int pos1, int pos2) {
    SendMsg(SCI_BRACEHIGHLIGHT, pos1, pos2);
}


void wxStyledTextCtrl::BraceBadlight(int pos) {
    SendMsg(SCI_BRACEBADLIGHT, pos);
}


int wxStyledTextCtrl::BraceMatch(int pos, int maxReStyle) {
    return SendMsg(SCI_BRACEMATCH, pos, maxReStyle);
}



//----------------------------------------------------------------------
// Markers

void wxStyledTextCtrl::MarkerDefine(int markerNumber, int markerSymbol,
                                          const wxColour& foreground,
                                          const wxColour& background) {
    MarkerSetType(markerNumber, markerSymbol);
    MarkerSetForeground(markerNumber, foreground);
    MarkerSetBackground(markerNumber, background);
}


void wxStyledTextCtrl::MarkerSetType(int markerNumber, int markerSymbol) {
    SendMsg(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}


void wxStyledTextCtrl::MarkerSetForeground(int markerNumber, const wxColour& colour) {
    SendMsg(SCI_MARKERSETFORE, markerNumber, wxColourAsLong(colour));
}


void wxStyledTextCtrl::MarkerSetBackground(int markerNumber, const wxColour& colour) {
    SendMsg(SCI_MARKERSETBACK, markerNumber, wxColourAsLong(colour));
}


int wxStyledTextCtrl::MarkerAdd(int line, int markerNumber) {
    return SendMsg(SCI_MARKERADD, line, markerNumber);
}


void wxStyledTextCtrl::MarkerDelete(int line, int markerNumber) {
    SendMsg(SCI_MARKERDELETE, line, markerNumber);
}


void wxStyledTextCtrl::MarkerDeleteAll(int markerNumber) {
    SendMsg(SCI_MARKERDELETEALL, markerNumber);
}


int wxStyledTextCtrl::MarkerGet(int line) {
    return SendMsg(SCI_MARKERGET);
}


int wxStyledTextCtrl::MarkerGetNextLine(int lineStart, int markerMask) {
    return SendMsg(SCI_MARKERNEXT, lineStart, markerMask);
}


int wxStyledTextCtrl::MarkerGetPrevLine(int lineStart, int markerMask) {
//    return SendMsg(SCI_MARKERPREV, lineStart, markerMask);
    return 0;
}


int wxStyledTextCtrl::MarkerLineFromHandle(int handle) {
    return SendMsg(SCI_MARKERLINEFROMHANDLE, handle);
}


void wxStyledTextCtrl::MarkerDeleteHandle(int handle) {
    SendMsg(SCI_MARKERDELETEHANDLE, handle);
}



//----------------------------------------------------------------------
// Indicators


void wxStyledTextCtrl::IndicatorSetStyle(int indicNum, int indicStyle) {
    SendMsg(SCI_INDICSETSTYLE, indicNum, indicStyle);
}


int wxStyledTextCtrl::IndicatorGetStyle(int indicNum) {
    return SendMsg(SCI_INDICGETSTYLE, indicNum);
}


void wxStyledTextCtrl::IndicatorSetColour(int indicNum, const wxColour& colour) {
    SendMsg(SCI_INDICSETFORE, indicNum, wxColourAsLong(colour));
}



//----------------------------------------------------------------------
// Auto completion


void wxStyledTextCtrl::AutoCompShow(const wxString& listOfWords) {
    SendMsg(SCI_AUTOCSHOW, 0, (long)listOfWords.c_str());
}


void wxStyledTextCtrl::AutoCompCancel() {
    SendMsg(SCI_AUTOCCANCEL);
}


bool wxStyledTextCtrl::AutoCompActive() {
    return SendMsg(SCI_AUTOCACTIVE) != 0;
}


int wxStyledTextCtrl::AutoCompPosAtStart() {
    return SendMsg(SCI_AUTOCPOSSTART);
}


void wxStyledTextCtrl::AutoCompComplete() {
    SendMsg(SCI_AUTOCCOMPLETE);
}


void wxStyledTextCtrl::AutoCompStopChars(const wxString& stopChars) {
    SendMsg(SCI_AUTOCSHOW, 0, (long)stopChars.c_str());
}


void wxStyledTextCtrl::AutoCompSetSeparator(char separator) {
    SendMsg(SCI_AUTOCSETSEPARATOR, separator);
}


char wxStyledTextCtrl::AutoCompGetSeparator() {
    return SendMsg(SCI_AUTOCGETSEPARATOR);
}


void wxStyledTextCtrl::AutoCompSelect(const wxString& stringtoselect) {
    SendMsg(SCI_AUTOCSELECT, (long)stringtoselect.c_str());
}


//----------------------------------------------------------------------
// Call tips

void wxStyledTextCtrl::CallTipShow(int pos, const wxString& text) {
    SendMsg(SCI_CALLTIPSHOW, pos, (long)text.c_str());
}


void wxStyledTextCtrl::CallTipCancel() {
    SendMsg(SCI_CALLTIPCANCEL);
}


bool wxStyledTextCtrl::CallTipActive() {
    return SendMsg(SCI_CALLTIPACTIVE) != 0;
}


int wxStyledTextCtrl::CallTipPosAtStart() {
    return SendMsg(SCI_CALLTIPPOSSTART);
}


void wxStyledTextCtrl::CallTipSetHighlight(int start, int end) {
    SendMsg(SCI_CALLTIPSETHLT, start, end);
}


void wxStyledTextCtrl::CallTipSetBackground(const wxColour& colour) {
    SendMsg(SCI_CALLTIPSETBACK, wxColourAsLong(colour));
}


//----------------------------------------------------------------------
// Key bindings

void wxStyledTextCtrl::CmdKeyAssign(int key, int modifiers, int cmd) {
    SendMsg(SCI_ASSIGNCMDKEY, MAKELONG(key, modifiers), cmd);
}


void wxStyledTextCtrl::CmdKeyClear(int key, int modifiers) {
    SendMsg(SCI_CLEARCMDKEY, MAKELONG(key, modifiers));
}


void wxStyledTextCtrl::CmdKeyClearAll() {
    SendMsg(SCI_CLEARALLCMDKEYS);
}


void wxStyledTextCtrl::CmdKeyExecute(int cmd) {
    SendMsg(cmd);
}



//----------------------------------------------------------------------
// Print formatting

int
wxStyledTextCtrl::FormatRange(bool   doDraw,
                                    int    startPos,
                                    int    endPos,
                                    wxDC*  draw,
                                    wxDC*  target,  // Why does it use two? Can they be the same?
                                    wxRect renderRect,
                                    wxRect pageRect) {
    FORMATRANGE fr;

    fr.hdc = draw;
    fr.hdcTarget = target;
    fr.rc.top = renderRect.GetTop();
    fr.rc.left = renderRect.GetLeft();
    fr.rc.right = renderRect.GetRight();
    fr.rc.bottom = renderRect.GetBottom();
    fr.rcPage.top = pageRect.GetTop();
    fr.rcPage.left = pageRect.GetLeft();
    fr.rcPage.right = pageRect.GetRight();
    fr.rcPage.bottom = pageRect.GetBottom();
    fr.chrg.cpMin = startPos;
    fr.chrg.cpMax = endPos;

    return SendMsg(EM_FORMATRANGE, doDraw, (long)&fr);
}


//----------------------------------------------------------------------
// Document Sharing

void* wxStyledTextCtrl::GetDocument() {
    return (void*)SendMsg(SCI_GETDOCPOINTER);
}


void wxStyledTextCtrl::SetDocument(void* document) {
    SendMsg(SCI_SETDOCPOINTER, 0, (long)document);
}


//----------------------------------------------------------------------
// Folding

int  wxStyledTextCtrl::VisibleFromDocLine(int docLine) {
    return SendMsg(SCI_VISIBLEFROMDOCLINE, docLine);
}


int  wxStyledTextCtrl::DocLineFromVisible(int displayLine) {
    return SendMsg(SCI_DOCLINEFROMVISIBLE, displayLine);
}


int  wxStyledTextCtrl::SetFoldLevel(int line, int level) {
    return SendMsg(SCI_SETFOLDLEVEL, line, level);
}


int  wxStyledTextCtrl::GetFoldLevel(int line) {
    return SendMsg(SCI_GETFOLDLEVEL,  line);
}


int  wxStyledTextCtrl::GetLastChild(int line, int level) {
    return SendMsg(SCI_GETLASTCHILD,  line, level);
}


int  wxStyledTextCtrl::GetFoldParent(int line) {
    return SendMsg(SCI_GETFOLDPARENT,  line);
}


void wxStyledTextCtrl::ShowLines(int lineStart, int lineEnd) {
    SendMsg(SCI_SHOWLINES, lineStart, lineEnd);
}


void wxStyledTextCtrl::HideLines(int lineStart, int lineEnd) {
    SendMsg(SCI_HIDELINES, lineStart, lineEnd);
}


bool wxStyledTextCtrl::GetLineVisible(int line) {
    return SendMsg(SCI_GETLINEVISIBLE, line) != 0;
}


void wxStyledTextCtrl::SetFoldExpanded(int line, bool expanded) {
    SendMsg(SCI_SETFOLDEXPANDED, line, expanded);
}


bool wxStyledTextCtrl::GetFoldExpanded(int line) {
    return SendMsg(SCI_GETFOLDEXPANDED, line) != 0;
}


void wxStyledTextCtrl::ToggleFold(int line) {
    SendMsg(SCI_TOGGLEFOLD, line);
}


void wxStyledTextCtrl::EnsureVisible(int line) {
    SendMsg(SCI_ENSUREVISIBLE, line);
}


void wxStyledTextCtrl::SetFoldFlags(int flags) {
    SendMsg(SCI_SETFOLDFLAGS, flags);
}


//----------------------------------------------------------------------
// Long Lines

int wxStyledTextCtrl::GetEdgeColumn() {
    return SendMsg(SCI_GETEDGECOLUMN);
}

void wxStyledTextCtrl::SetEdgeColumn(int column) {
    SendMsg(SCI_SETEDGECOLUMN, column);
}

wxSTC_EDGE wxStyledTextCtrl::GetEdgeMode() {
    return (wxSTC_EDGE) SendMsg(SCI_GETEDGEMODE);
}

void wxStyledTextCtrl::SetEdgeMode(wxSTC_EDGE mode){
    SendMsg(SCI_SETEDGEMODE, mode);
}

wxColour wxStyledTextCtrl::GetEdgeColour() {
    long c = SendMsg(SCI_GETEDGECOLOUR);
    return wxColourFromLong(c);
}

void wxStyledTextCtrl::SetEdgeColour(const wxColour& colour) {
    SendMsg(SCI_SETEDGECOLOUR, wxColourAsLong(colour));
}


//----------------------------------------------------------------------
// Lexer

void     wxStyledTextCtrl::SetLexer(wxSTC_LEX lexer) {
    SendMsg(SCI_SETLEXER, lexer);
}


wxSTC_LEX wxStyledTextCtrl::GetLexer() {
    return (wxSTC_LEX)SendMsg(SCI_GETLEXER);
}


void     wxStyledTextCtrl::Colourise(int start, int end) {
    SendMsg(SCI_COLOURISE, start, end);
}


void     wxStyledTextCtrl::SetProperty(const wxString& key, const wxString& value) {
    SendMsg(SCI_SETPROPERTY, (long)key.c_str(), (long)value.c_str());
}


void     wxStyledTextCtrl::SetKeywords(int keywordSet, const wxString& keywordList) {
    SendMsg(SCI_SETKEYWORDS, keywordSet, (long)keywordList.c_str());
}



//----------------------------------------------------------------------
// Event handlers

void wxStyledTextCtrl::OnPaint(wxPaintEvent& evt) {
    wxPaintDC dc(this);
    wxRegion  region = GetUpdateRegion();

    m_swx->DoPaint(&dc, region.GetBox());
}

void wxStyledTextCtrl::OnScrollWin(wxScrollWinEvent& evt) {
    if (evt.GetOrientation() == wxHORIZONTAL)
        m_swx->DoHScroll(evt.GetEventType(), evt.GetPosition());
    else
        m_swx->DoVScroll(evt.GetEventType(), evt.GetPosition());
}

void wxStyledTextCtrl::OnSize(wxSizeEvent& evt) {
    wxSize sz = GetClientSize();
    m_swx->DoSize(sz.x, sz.y);
}

void wxStyledTextCtrl::OnMouseLeftDown(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoButtonDown(Point(pt.x, pt.y), m_stopWatch.Time(),
                      evt.ShiftDown(), evt.ControlDown(), evt.AltDown());
}

void wxStyledTextCtrl::OnMouseMove(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoButtonMove(Point(pt.x, pt.y));
}

void wxStyledTextCtrl::OnMouseLeftUp(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoButtonUp(Point(pt.x, pt.y), m_stopWatch.Time(),
                      evt.ControlDown());
}


void wxStyledTextCtrl::OnMouseRightUp(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoContextMenu(Point(pt.x, pt.y));
}

void wxStyledTextCtrl::OnChar(wxKeyEvent& evt) {
    long key = evt.KeyCode();
    if ((key > WXK_ESCAPE) &&
        (key != WXK_DELETE) && (key < 255) &&
        !evt.ControlDown() && !evt.AltDown()) {

        m_swx->DoAddChar(key);
    }
    else {
        evt.Skip();
    }
}

void wxStyledTextCtrl::OnKeyDown(wxKeyEvent& evt) {
    long key = evt.KeyCode();
    key = toupper(key);
    int processed = m_swx->DoKeyDown(key, evt.ShiftDown(),
                                     evt.ControlDown(), evt.AltDown());
    if (! processed)
        evt.Skip();
}

void wxStyledTextCtrl::OnLoseFocus(wxFocusEvent& evt) {
    m_swx->DoLoseFocus();
}

void wxStyledTextCtrl::OnGainFocus(wxFocusEvent& evt) {
    m_swx->DoGainFocus();
}

void wxStyledTextCtrl::OnSysColourChanged(wxSysColourChangedEvent& evt) {
    m_swx->DoSysColourChange();
}

void wxStyledTextCtrl::OnEraseBackground(wxEraseEvent& evt) {
    // do nothing to help avoid flashing
}



void wxStyledTextCtrl::OnMenu(wxCommandEvent& evt) {
    m_swx->DoCommand(evt.GetId());
}


//----------------------------------------------------------------------
// Turn notifications from Scintilla into events


void wxStyledTextCtrl::NotifyChange() {
    wxStyledTextEvent evt(wxEVT_STC_CHANGE, GetId());
    GetEventHandler()->ProcessEvent(evt);
}

void wxStyledTextCtrl::NotifyParent(SCNotification* _scn) {
    SCNotification& scn = *_scn;
    int eventType = 0;
    switch (scn.nmhdr.code) {
    case SCN_STYLENEEDED:
        eventType = wxEVT_STC_STYLENEEDED;
        break;
    case SCN_CHARADDED:
        eventType = wxEVT_STC_CHARADDED;
        break;
    case SCN_UPDATEUI:
        eventType = wxEVT_STC_UPDATEUI;
        break;
    case SCN_SAVEPOINTREACHED:
        eventType = wxEVT_STC_SAVEPOINTREACHED;
        break;
    case SCN_SAVEPOINTLEFT:
        eventType = wxEVT_STC_SAVEPOINTLEFT;
        break;
    case SCN_MODIFYATTEMPTRO:
        eventType = wxEVT_STC_ROMODIFYATTEMPT;
        break;
    case SCN_DOUBLECLICK:
        eventType = wxEVT_STC_DOUBLECLICK;
        break;
    case SCN_MODIFIED:
        eventType = wxEVT_STC_MODIFIED;
        break;
    case SCN_KEY:
        eventType = wxEVT_STC_KEY;
        break;
    case SCN_MACRORECORD:
        eventType = wxEVT_STC_MACRORECORD;
        break;
    case SCN_MARGINCLICK:
        eventType = wxEVT_STC_MARGINCLICK;
        break;
    case SCN_NEEDSHOWN:
        eventType = wxEVT_STC_NEEDSHOWN;
        break;
    }
    if (eventType) {
        wxStyledTextEvent evt(eventType, GetId());
        evt.SetPosition(scn.position);
        evt.SetKey(scn.ch);
        evt.SetModifiers(scn.modifiers);
        if (eventType == wxEVT_STC_MODIFIED) {
            evt.SetModificationType(scn.modificationType);
            evt.SetText(scn.text);
            evt.SetLength(scn.length);
            evt.SetLinesAdded(scn.linesAdded);
            evt.SetLine(scn.line);
            evt.SetFoldLevelNow(scn.foldLevelNow);
            evt.SetFoldLevelPrev(scn.foldLevelPrev);
        }
        if (eventType == wxEVT_STC_MARGINCLICK)
            evt.SetMargin(scn.margin);
        if (eventType == wxEVT_STC_MACRORECORD) {
            evt.SetMessage(scn.message);
            evt.SetWParam(scn.wParam);
            evt.SetLParam(scn.lParam);
        }

        GetEventHandler()->ProcessEvent(evt);
    }
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

wxStyledTextEvent::wxStyledTextEvent(wxEventType commandType, int id)
    : wxCommandEvent(commandType, id)
{
    m_position = 0;
    m_key = 0;
    m_modifiers = 0;
    m_modificationType = 0;
    m_length = 0;
    m_linesAdded = 0;
    m_line = 0;
    m_foldLevelNow = 0;
    m_foldLevelPrev = 0;
    m_margin = 0;
    m_message = 0;
    m_wParam = 0;
    m_lParam = 0;


}

bool wxStyledTextEvent::GetShift() const { return (m_modifiers & SCI_SHIFT) != 0; }
bool wxStyledTextEvent::GetControl() const { return (m_modifiers & SCI_CTRL) != 0; }
bool wxStyledTextEvent::GetAlt() const { return (m_modifiers & SCI_ALT) != 0; }

void wxStyledTextEvent::CopyObject(wxObject& obj) const {
    wxCommandEvent::CopyObject(obj);

    wxStyledTextEvent* o = (wxStyledTextEvent*)&obj;
    o->m_position =      m_position;
    o->m_key =           m_key;
    o->m_modifiers =     m_modifiers;
    o->m_modificationType = m_modificationType;
    o->m_text =          m_text;
    o->m_length =        m_length;
    o->m_linesAdded =    m_linesAdded;
    o->m_line =          m_line;
    o->m_foldLevelNow =  m_foldLevelNow;
    o->m_foldLevelPrev = m_foldLevelPrev;

    o->m_margin =        m_margin;

    o->m_message =       m_message;
    o->m_wParam =        m_wParam;
    o->m_lParam =        m_lParam;



}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

