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
    EVT_LISTBOX_DCLICK          (-1, wxStyledTextCtrl::OnListBox)
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
    m_stopWatch.Start();
}


wxStyledTextCtrl::~wxStyledTextCtrl() {
    delete m_swx;
}


//----------------------------------------------------------------------

long wxStyledTextCtrl::SendMsg(int msg, long wp, long lp) {

    return m_swx->WndProc(msg, wp, lp);
}


#define MAKELONG(a, b) ((a) | ((b) << 16))


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


//----------------------------------------------------------------------
// BEGIN generated section.  The following code is automatically generated
//       by gen_iface.py from the contents of Scintilla.iface.  Do not edit
//       this file.  Edit stc.cpp.in or gen_iface.py instead and regenerate.


// Add text to the document
void wxStyledTextCtrl::AddText(const wxString& text) {
                    SendMsg(2001, text.Len(), (long)text.c_str());
}

// Add array of cells to document
void wxStyledTextCtrl::AddStyledText(const wxString& text) {
                          SendMsg(2002, text.Len(), (long)text.c_str());
}

// Insert string at a position
void wxStyledTextCtrl::InsertText(int pos, const wxString& text) {
    SendMsg(2003, pos, (long)text.c_str());
}

// Delete all text in the document
void wxStyledTextCtrl::ClearAll() {
    SendMsg(2004, 0, 0);
}

// Set all style bytes to 0, remove all folding information
void wxStyledTextCtrl::ClearDocumentStyle() {
    SendMsg(2005, 0, 0);
}

// The number of characters in the document
int wxStyledTextCtrl::GetLength() {
    return SendMsg(2006, 0, 0);
}

// Returns the character byte at the position
int wxStyledTextCtrl::GetCharAt(int pos) {
    return SendMsg(2007, pos, 0);
}

// Returns the position of the caret
int wxStyledTextCtrl::GetCurrentPos() {
    return SendMsg(2008, 0, 0);
}

// Returns the position of the opposite end of the selection to the caret
int wxStyledTextCtrl::GetAnchor() {
    return SendMsg(2009, 0, 0);
}

// Returns the style byte at the position
int wxStyledTextCtrl::GetStyleAt(int pos) {
    return SendMsg(2010, pos, 0);
}

// Redoes the next action on the undo history
void wxStyledTextCtrl::Redo() {
    SendMsg(2011, 0, 0);
}

// Choose between collecting actions into the undo
// history and discarding them.
void wxStyledTextCtrl::SetUndoCollection(bool collectUndo) {
    SendMsg(2012, collectUndo, 0);
}

// Select all the text in the document.
void wxStyledTextCtrl::SelectAll() {
    SendMsg(2013, 0, 0);
}

// Remember the current position in the undo history as the position
// at which the document was saved.
void wxStyledTextCtrl::SetSavePoint() {
    SendMsg(2014, 0, 0);
}

// Retrieve a buffer of cells.
wxString wxStyledTextCtrl::GetStyledText(int startPos, int endPos) {
                          wxString text;
                          int len = endPos - startPos;
                          TextRange tr;
                          tr.lpstrText = text.GetWriteBuf(len*2+1);
                          tr.chrg.cpMin = startPos;
                          tr.chrg.cpMax = endPos;
                          SendMsg(2015, 0, (long)&tr);
                          text.UngetWriteBuf(len*2);
                          return text;
}

// Are there any redoable actions in the undo history.
bool wxStyledTextCtrl::CanRedo() {
    return SendMsg(2016, 0, 0) != 0;
}

// Retrieve the line number at which a particular marker is located
int wxStyledTextCtrl::MarkerLineFromHandle(int handle) {
    return SendMsg(2017, handle, 0);
}

// Delete a marker.
void wxStyledTextCtrl::MarkerDeleteHandle(int handle) {
    SendMsg(2018, handle, 0);
}

// Is undo history being collected?
bool wxStyledTextCtrl::GetUndoCollection() {
    return SendMsg(2019, 0, 0) != 0;
}

// Are white space characters currently visible?
// Returns one of SCWS_* constants.
int wxStyledTextCtrl::GetViewWhiteSpace() {
    return SendMsg(2020, 0, 0);
}

// Make white space characters invisible, always visible or visible outside indentation.
void wxStyledTextCtrl::SetViewWhiteSpace(int viewWS) {
    SendMsg(2021, viewWS, 0);
}

// Find the position from a point within the window.
int wxStyledTextCtrl::PositionFromPoint(wxPoint pt) {
                              return SendMsg(2022, pt.x, pt.y);
}

// Set caret to start of a line and ensure it is visible.
void wxStyledTextCtrl::GotoLine(int line) {
    SendMsg(2024, line, 0);
}

// Set caret to a position and ensure it is visible.
void wxStyledTextCtrl::GotoPos(int pos) {
    SendMsg(2025, pos, 0);
}

// Set the selection anchor to a position. The anchor is the opposite
// end of the selection from the caret.
void wxStyledTextCtrl::SetAnchor(int posAnchor) {
    SendMsg(2026, posAnchor, 0);
}

// Retrieve the text of the line containing the caret.
// Returns the index of the caret on the line.
wxString wxStyledTextCtrl::GetCurLine(int* linePos) {
                       wxString text;
                       int len = LineLength(GetCurrentLine());
                       char* buf = text.GetWriteBuf(len+1);

                       int pos = SendMsg(2027, len, (long)buf);
                       text.UngetWriteBuf();
                       if (linePos)  *linePos = pos;

                       return text;
}

// Retrieve the position of the last correctly styled character.
int wxStyledTextCtrl::GetEndStyled() {
    return SendMsg(2028, 0, 0);
}

// Convert all line endings in the document to use the current mode.
void wxStyledTextCtrl::ConvertEOLs() {
    SendMsg(2029, 0, 0);
}

// Retrieve the current end of line mode - one of CRLF, CR, or LF.
int wxStyledTextCtrl::GetEOLMode() {
    return SendMsg(2030, 0, 0);
}

// Set the current end of line mode.
void wxStyledTextCtrl::SetEOLMode(int eolMode) {
    SendMsg(2031, eolMode, 0);
}

// Set the current styling position to pos and the styling mask to mask.
// The styling mask can be used to protect some bits in each styling byte from
// modification.
void wxStyledTextCtrl::StartStyling(int pos, int mask) {
    SendMsg(2032, pos, mask);
}

// Change style from current styling position for length characters to a style
// and move the current styling position to after this newly styled segment.
void wxStyledTextCtrl::SetStyling(int length, int style) {
    SendMsg(2033, length, style);
}

// Is drawing done first into a buffer or direct to the screen.
bool wxStyledTextCtrl::GetBufferedDraw() {
    return SendMsg(2034, 0, 0) != 0;
}

// If drawing is buffered then each line of text is drawn into a bitmap buffer
// before drawing it to the screen to avoid flicker.
void wxStyledTextCtrl::SetBufferedDraw(bool buffered) {
    SendMsg(2035, buffered, 0);
}

// Change the visible size of a tab to be a multiple of the width of a space
// character.
void wxStyledTextCtrl::SetTabWidth(int tabWidth) {
    SendMsg(2036, tabWidth, 0);
}

// Retrieve the visible size of a tab.
int wxStyledTextCtrl::GetTabWidth() {
    return SendMsg(2121, 0, 0);
}

// Set the code page used to interpret the bytes of the document as characters.
// The SC_CP_UTF8 value can be used to enter Unicode mode.
void wxStyledTextCtrl::SetCodePage(int codePage) {
    SendMsg(2037, codePage, 0);
}

// Set the symbol used for a particular marker number,
// and optionally the for and background colours.
void wxStyledTextCtrl::MarkerDefine(int markerNumber, int markerSymbol,
                            const wxColour& foreground,
                            const wxColour& background) {

                            SendMsg(2040, markerNumber, markerSymbol);
                            if (foreground.Ok())
                                MarkerSetForeground(markerNumber, foreground);
                            if (background.Ok())
                                MarkerSetBackground(markerNumber, background);
}

// Set the foreground colour used for a particular marker number.
void wxStyledTextCtrl::MarkerSetForeground(int markerNumber, const wxColour& fore) {
    SendMsg(2041, markerNumber, wxColourAsLong(fore));
}

// Set the background colour used for a particular marker number.
void wxStyledTextCtrl::MarkerSetBackground(int markerNumber, const wxColour& back) {
    SendMsg(2042, markerNumber, wxColourAsLong(back));
}

// Add a marker to a line.
void wxStyledTextCtrl::MarkerAdd(int line, int markerNumber) {
    SendMsg(2043, line, markerNumber);
}

// Delete a marker from a line
void wxStyledTextCtrl::MarkerDelete(int line, int markerNumber) {
    SendMsg(2044, line, markerNumber);
}

// Delete all markers with a particular number from all lines
void wxStyledTextCtrl::MarkerDeleteAll(int markerNumber) {
    SendMsg(2045, markerNumber, 0);
}

// Get a bit mask of all the markers set on a line.
int wxStyledTextCtrl::MarkerGet(int line) {
    return SendMsg(2046, line, 0);
}

// Find the next line after lineStart that includes a marker in mask.
int wxStyledTextCtrl::MarkerNext(int lineStart, int markerMask) {
    return SendMsg(2047, lineStart, markerMask);
}

// Find the previous line before lineStart that includes a marker in mask.
int wxStyledTextCtrl::MarkerPrevious(int lineStart, int markerMask) {
    return SendMsg(2048, lineStart, markerMask);
}

// Set a margin to be either numeric or symbolic.
void wxStyledTextCtrl::SetMarginType(int margin, int marginType) {
    SendMsg(2240, margin, marginType);
}

// Retrieve the type of a margin.
int wxStyledTextCtrl::GetMarginType(int margin) {
    return SendMsg(2241, margin, 0);
}

// Set the width of a margin to a width expressed in pixels.
void wxStyledTextCtrl::SetMarginWidth(int margin, int pixelWidth) {
    SendMsg(2242, margin, pixelWidth);
}

// Retrieve the width of a margin in pixels.
int wxStyledTextCtrl::GetMarginWidth(int margin) {
    return SendMsg(2243, margin, 0);
}

// Set a mask that determines which markers are displayed in a margin.
void wxStyledTextCtrl::SetMarginMask(int margin, int mask) {
    SendMsg(2244, margin, mask);
}

// Retrieve the marker mask of a margin.
int wxStyledTextCtrl::GetMarginMask(int margin) {
    return SendMsg(2245, margin, 0);
}

// Make a margin sensitive or insensitive to mouse clicks.
void wxStyledTextCtrl::SetMarginSensitive(int margin, bool sensitive) {
    SendMsg(2246, margin, sensitive);
}

// Retrieve the mouse click sensitivity of a margin.
bool wxStyledTextCtrl::GetMarginSensitive(int margin) {
    return SendMsg(2247, margin, 0) != 0;
}

// Clear all the styles and make equivalent to the global default style.
void wxStyledTextCtrl::StyleClearAll() {
    SendMsg(2050, 0, 0);
}

// Set the foreground colour of a style.
void wxStyledTextCtrl::StyleSetForeground(int style, const wxColour& fore) {
    SendMsg(2051, style, wxColourAsLong(fore));
}

// Set the background colour of a style.
void wxStyledTextCtrl::StyleSetBackground(int style, const wxColour& back) {
    SendMsg(2052, style, wxColourAsLong(back));
}

// Set a style to be bold or not.
void wxStyledTextCtrl::StyleSetBold(int style, bool bold) {
    SendMsg(2053, style, bold);
}

// Set a style to be italic or not.
void wxStyledTextCtrl::StyleSetItalic(int style, bool italic) {
    SendMsg(2054, style, italic);
}

// Set the size of characters of a style.
void wxStyledTextCtrl::StyleSetSize(int style, int sizePoints) {
    SendMsg(2055, style, sizePoints);
}

// Set the font of a style.
void wxStyledTextCtrl::StyleSetFaceName(int style, const wxString& fontName) {
    SendMsg(2056, style, (long)fontName.c_str());
}

// Set a style to have its end of line filled or not.
void wxStyledTextCtrl::StyleSetEOLFilled(int style, bool filled) {
    SendMsg(2057, style, filled);
}

// Reset the default style to its state at startup
void wxStyledTextCtrl::StyleResetDefault() {
    SendMsg(2058, 0, 0);
}

// Set a style to be underlined or not.
void wxStyledTextCtrl::StyleSetUnderline(int style, bool underline) {
    SendMsg(2059, style, underline);
}

// Set the foreground colour of the selection and whether to use this setting.
void wxStyledTextCtrl::SetSelForeground(bool useSetting, const wxColour& fore) {
    SendMsg(2067, useSetting, wxColourAsLong(fore));
}

// Set the background colour of the selection and whether to use this setting.
void wxStyledTextCtrl::SetSelBackground(bool useSetting, const wxColour& back) {
    SendMsg(2068, useSetting, wxColourAsLong(back));
}

// Set the foreground colour of the caret.
void wxStyledTextCtrl::SetCaretForeground(const wxColour& fore) {
    SendMsg(2069, wxColourAsLong(fore), 0);
}

// When key+modifier combination km is pressed perform msg.
void wxStyledTextCtrl::CmdKeyAssign(int key, int modifiers, int cmd) {
                          SendMsg(2070, MAKELONG(key, modifiers), cmd);
}

// When key+modifier combination km do nothing.
void wxStyledTextCtrl::CmdKeyClear(int key, int modifiers) {
                          SendMsg(2071, MAKELONG(key, modifiers));
}

// Drop all key mappings.
void wxStyledTextCtrl::CmdKeyClearAll() {
    SendMsg(2072, 0, 0);
}

// Set the styles for a segment of the document.
void wxStyledTextCtrl::SetStyleBytes(int length, char* styleBytes) {
                          SendMsg(2073, length, (long)styleBytes);
}

// Set a style to be visible or not.
void wxStyledTextCtrl::StyleSetVisible(int style, bool visible) {
    SendMsg(2074, style, visible);
}

// Get the time in milliseconds that the caret is on and off.
int wxStyledTextCtrl::GetCaretPeriod() {
    return SendMsg(2075, 0, 0);
}

// Get the time in milliseconds that the caret is on and off. 0 = steady on.
void wxStyledTextCtrl::SetCaretPeriod(int periodMilliseconds) {
    SendMsg(2076, periodMilliseconds, 0);
}

// Set the set of characters making up words for when moving or selecting
// by word.
void wxStyledTextCtrl::SetWordChars(const wxString& characters) {
    SendMsg(2077, 0, (long)characters.c_str());
}

// Start a sequence of actions that is undone and redone as a unit.
// May be nested.
void wxStyledTextCtrl::BeginUndoAction() {
    SendMsg(2078, 0, 0);
}

// End a sequence of actions that is undone and redone as a unit.
void wxStyledTextCtrl::EndUndoAction() {
    SendMsg(2079, 0, 0);
}

// Set an indicator to plain, squiggle or TT.
void wxStyledTextCtrl::IndicatorSetStyle(int indic, int style) {
    SendMsg(2080, indic, style);
}

// Retrieve the style of an indicator.
int wxStyledTextCtrl::IndicatorGetStyle(int indic) {
    return SendMsg(2081, indic, 0);
}

// Set the foreground colour of an indicator.
void wxStyledTextCtrl::IndicatorSetForeground(int indic, const wxColour& fore) {
    SendMsg(2082, indic, wxColourAsLong(fore));
}

// Retrieve the foreground colour of an indicator.
wxColour wxStyledTextCtrl::IndicatorGetForeground(int indic) {
    long c = SendMsg(2083, indic, 0);
    return wxColourFromLong(c);
}

// Divide each styling byte into lexical class bits (default:5) and indicator
// bits (default:3). If a lexer requires more than 32 lexical states, then this
// is used to expand the possible states.
void wxStyledTextCtrl::SetStyleBits(int bits) {
    SendMsg(2090, bits, 0);
}

// Retrieve number of bits in style bytes used to hold the lexical state.
int wxStyledTextCtrl::GetStyleBits() {
    return SendMsg(2091, 0, 0);
}

// Used to hold extra styling information for each line.
void wxStyledTextCtrl::SetLineState(int line, int state) {
    SendMsg(2092, line, state);
}

// Retrieve the extra styling information for a line.
int wxStyledTextCtrl::GetLineState(int line) {
    return SendMsg(2093, line, 0);
}

// Retrieve the last line number that has line state.
int wxStyledTextCtrl::GetMaxLineState() {
    return SendMsg(2094, 0, 0);
}

// Display a auto-completion list.
// The lenEntered parameter indicates how many characters before
// the caret should be used to provide context.
void wxStyledTextCtrl::AutoCompShow(int lenEntered, const wxString& itemList) {
    SendMsg(2100, lenEntered, (long)itemList.c_str());
}

// Remove the auto-completion list from the screen.
void wxStyledTextCtrl::AutoCompCancel() {
    SendMsg(2101, 0, 0);
}

// Is there an auto-completion list visible?
bool wxStyledTextCtrl::AutoCompActive() {
    return SendMsg(2102, 0, 0) != 0;
}

// Retrieve the position of the caret when the auto-completion list was
// displayed.
int wxStyledTextCtrl::AutoCompPosStart() {
    return SendMsg(2103, 0, 0);
}

// User has selected an item so remove the list and insert the selection.
void wxStyledTextCtrl::AutoCompComplete() {
    SendMsg(2104, 0, 0);
}

// Define a set of character that when typed cancel the auto-completion list.
void wxStyledTextCtrl::AutoCompStops(const wxString& characterSet) {
    SendMsg(2105, 0, (long)characterSet.c_str());
}

// Change the separator character in the string setting up an auto-completion
// list. Default is space but can be changed if items contain space.
void wxStyledTextCtrl::AutoCompSetSeparator(int separatorCharacter) {
    SendMsg(2106, separatorCharacter, 0);
}

// Retrieve the auto-completion list separator character.
int wxStyledTextCtrl::AutoCompGetSeparator() {
    return SendMsg(2107, 0, 0);
}

// Select the item in the auto-completion list that starts with a string.
void wxStyledTextCtrl::AutoCompSelect(const wxString& text) {
    SendMsg(2108, 0, (long)text.c_str());
}

// Should the auto-completion list be cancelled if the user backspaces to a
// position before where the box was created.
void wxStyledTextCtrl::AutoCompSetCancelAtStart(bool cancel) {
    SendMsg(2110, cancel, 0);
}

// Retrieve whether auto-completion cancelled by backspacing before start.
bool wxStyledTextCtrl::AutoCompGetCancelAtStart() {
    return SendMsg(2111, 0, 0) != 0;
}

// Define a set of character that when typed fills up the selected word.
void wxStyledTextCtrl::AutoCompSetFillUps(const wxString& characterSet) {
    SendMsg(2112, 0, (long)characterSet.c_str());
}

// Should a single item auto-completion list automatically choose the item.
void wxStyledTextCtrl::AutoCompSetChooseSingle(bool chooseSingle) {
    SendMsg(2113, chooseSingle, 0);
}

// Retrieve whether a single item auto-completion list automatically choose the item.
bool wxStyledTextCtrl::AutoCompGetChooseSingle() {
    return SendMsg(2114, 0, 0) != 0;
}

// Set whether case is significant when performing auto-completion searches.
void wxStyledTextCtrl::AutoCompSetIgnoreCase(bool ignoreCase) {
    SendMsg(2115, ignoreCase, 0);
}

// Retrieve state of ignore case flag.
bool wxStyledTextCtrl::AutoCompGetIgnoreCase() {
    return SendMsg(2116, 0, 0) != 0;
}

// Set the number of spaces used for one level of indentation.
void wxStyledTextCtrl::SetIndent(int indentSize) {
    SendMsg(2122, indentSize, 0);
}

// Retrieve indentation size.
int wxStyledTextCtrl::GetIndent() {
    return SendMsg(2123, 0, 0);
}

// Indentation will only use space characters if useTabs is false, otherwise
// it will use a combination of tabs and spaces.
void wxStyledTextCtrl::SetUseTabs(bool useTabs) {
    SendMsg(2124, useTabs, 0);
}

// Retrieve whether tabs will be used in indentation.
bool wxStyledTextCtrl::GetUseTabs() {
    return SendMsg(2125, 0, 0) != 0;
}

// Change the indentation of a line to a number of columns.
void wxStyledTextCtrl::SetLineIndentation(int line, int indentSize) {
    SendMsg(2126, line, indentSize);
}

// Retrieve the number of columns that a line is indented.
int wxStyledTextCtrl::GetLineIndentation(int line) {
    return SendMsg(2127, line, 0);
}

// Retrieve the position before the first non indentation character on a line.
int wxStyledTextCtrl::GetLineIndentPosition(int line) {
    return SendMsg(2128, line, 0);
}

// Retrieve the column number of a position, taking tab width into account.
int wxStyledTextCtrl::GetColumn(int pos) {
    return SendMsg(2129, pos, 0);
}

// Show or hide the horizontal scroll bar.
void wxStyledTextCtrl::SetUseHorizontalScrollBar(bool show) {
    SendMsg(2130, show, 0);
}

// Is the horizontal scroll bar visible?
bool wxStyledTextCtrl::GetUseHorizontalScrollBar() {
    return SendMsg(2131, 0, 0) != 0;
}

// Show or hide indentation guides.
void wxStyledTextCtrl::SetIndentationGuides(bool show) {
    SendMsg(2132, show, 0);
}

// Are the indentation guides visible?
bool wxStyledTextCtrl::GetIndentationGuides() {
    return SendMsg(2133, 0, 0) != 0;
}

// Set the highlighted indentation guide column.
// 0 = no highlighted guide.
void wxStyledTextCtrl::SetHighlightGuide(int column) {
    SendMsg(2134, column, 0);
}

// Get the highlighted indentation guide column.
int wxStyledTextCtrl::GetHighlightGuide() {
    return SendMsg(2135, 0, 0);
}

// Get the position after the last visible characters on a line.
int wxStyledTextCtrl::GetLineEndPosition(int line) {
    return SendMsg(2136, line, 0);
}

// Get the code page used to interpret the bytes of the document as characters.
int wxStyledTextCtrl::GetCodePage() {
    return SendMsg(2137, 0, 0);
}

// Get the foreground colour of the caret.
wxColour wxStyledTextCtrl::GetCaretForeground() {
    long c = SendMsg(2138, 0, 0);
    return wxColourFromLong(c);
}

// In read-only mode?
bool wxStyledTextCtrl::GetReadOnly() {
    return SendMsg(2140, 0, 0) != 0;
}

// Sets the position of the caret.
void wxStyledTextCtrl::SetCurrentPos(int pos) {
    SendMsg(2141, pos, 0);
}

// Sets the position that starts the selection - this becomes the anchor.
void wxStyledTextCtrl::SetSelectionStart(int pos) {
    SendMsg(2142, pos, 0);
}

// Returns the position at the start of the selection.
int wxStyledTextCtrl::GetSelectionStart() {
    return SendMsg(2143, 0, 0);
}

// Sets the position that ends the selection - this becomes the currentPosition.
void wxStyledTextCtrl::SetSelectionEnd(int pos) {
    SendMsg(2144, pos, 0);
}

// Returns the position at the end of the selection.
int wxStyledTextCtrl::GetSelectionEnd() {
    return SendMsg(2145, 0, 0);
}

// Sets the print magnification added to the point size of each style for printing.
void wxStyledTextCtrl::SetPrintMagnification(int magnification) {
    SendMsg(2146, magnification, 0);
}

// Returns the print magnification.
int wxStyledTextCtrl::GetPrintMagnification() {
    return SendMsg(2147, 0, 0);
}

// Modify colours when printing for clearer printed text.
void wxStyledTextCtrl::SetPrintColourMode(int mode) {
    SendMsg(2148, mode, 0);
}

// Returns the print colour mode.
int wxStyledTextCtrl::GetPrintColourMode() {
    return SendMsg(2149, 0, 0);
}

// Find some text in the document.
int wxStyledTextCtrl::FindText(int minPos, int maxPos,
                               const wxString& text,
                               bool caseSensitive, bool wholeWord) {
                     TextToFind  ft;
                     int         flags = 0;

                     flags |= caseSensitive ? SCFIND_MATCHCASE : 0;
                     flags |= wholeWord     ? SCFIND_WHOLEWORD : 0;
                     ft.chrg.cpMin = minPos;
                     ft.chrg.cpMax = maxPos;
                     ft.lpstrText = (char*)text.c_str();

                     return SendMsg(2150, flags, (long)&ft);
}

// On Windows will draw the document into a display context such as a printer.
 int wxStyledTextCtrl::FormatRange(bool   doDraw,
                                int    startPos,
                                int    endPos,
                                wxDC*  draw,
                                wxDC*  target,  // Why does it use two? Can they be the same?
                                wxRect renderRect,
                                wxRect pageRect) {
                            RangeToFormat fr;

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

                            return SendMsg(2151, doDraw, (long)&fr);
}

// Retrieve the line at the top of the display.
int wxStyledTextCtrl::GetFirstVisibleLine() {
    return SendMsg(2152, 0, 0);
}

// Retrieve the contents of a line.
wxString wxStyledTextCtrl::GetLine(int line) {
                       wxString text;
                       int len = LineLength(line);
                       char* buf = text.GetWriteBuf(len+1);

                       int pos = SendMsg(2153, line, (long)buf);
                       text.UngetWriteBuf();

                       return text;
}

// Returns the number of lines in the document. There is always at least one.
int wxStyledTextCtrl::GetLineCount() {
    return SendMsg(2154, 0, 0);
}

// Sets the size in pixels of the left margin.
void wxStyledTextCtrl::SetMarginLeft(int width) {
    SendMsg(2155, 0, width);
}

// Returns the size in pixels of the left margin.
int wxStyledTextCtrl::GetMarginLeft() {
    return SendMsg(2156, 0, 0);
}

// Sets the size in pixels of the right margin.
void wxStyledTextCtrl::SetMarginRight(int width) {
    SendMsg(2157, 0, width);
}

// Returns the size in pixels of the right margin.
int wxStyledTextCtrl::GetMarginRight() {
    return SendMsg(2158, 0, 0);
}

// Is the document different from when it was last saved?
bool wxStyledTextCtrl::GetModify() {
    return SendMsg(2159, 0, 0) != 0;
}

// Select a range of text.
void wxStyledTextCtrl::SetSelection(int start, int end) {
    SendMsg(2160, start, end);
}

// Retrieve the selected text.
wxString wxStyledTextCtrl::GetSelectedText() {
                            wxString text;
                            int   start;
                            int   end;

                            GetSelection(&start, &end);
                            int   len  = end - start;
                            char* buff = text.GetWriteBuf(len+1);

                            SendMsg(2161, 0, (long)buff);
                            text.UngetWriteBuf();
                            return text;
}

// Retrieve a range of text.
wxString wxStyledTextCtrl::GetTextRange(int startPos, int endPos) {
                            wxString text;
                            int   len  = endPos - startPos;
                            char* buff = text.GetWriteBuf(len+1);
                            TextRange tr;
                            tr.lpstrText = buff;
                            tr.chrg.cpMin = startPos;
                            tr.chrg.cpMax = endPos;

                            SendMsg(2162, 0, (long)&tr);
                            text.UngetWriteBuf();
                            return text;
}

// Draw the selection in normal style or with selection highlighted.
void wxStyledTextCtrl::HideSelection(bool normal) {
    SendMsg(2163, normal, 0);
}

// Retrieve the line containing a position.
int wxStyledTextCtrl::LineFromPosition(int pos) {
    return SendMsg(2166, pos, 0);
}

// Retrieve the position at the start of a line.
int wxStyledTextCtrl::PositionFromLine(int line) {
    return SendMsg(2167, line, 0);
}

// Scroll horizontally and vertically.
void wxStyledTextCtrl::LineScroll(int columns, int lines) {
    SendMsg(2168, columns, lines);
}

// Ensure the caret is visible.
void wxStyledTextCtrl::EnsureCaretVisible() {
    SendMsg(2169, 0, 0);
}

// Replace the selected text with the argument text.
void wxStyledTextCtrl::ReplaceSelection(const wxString& text) {
    SendMsg(2170, 0, (long)text.c_str());
}

// Set to read only or read write.
void wxStyledTextCtrl::SetReadOnly(bool readOnly) {
    SendMsg(2171, readOnly, 0);
}

// Will a paste succeed?
bool wxStyledTextCtrl::CanPaste() {
    return SendMsg(2173, 0, 0) != 0;
}

// Are there any undoable actions in the undo history.
bool wxStyledTextCtrl::CanUndo() {
    return SendMsg(2174, 0, 0) != 0;
}

// Delete the undo history.
void wxStyledTextCtrl::EmptyUndoBuffer() {
    SendMsg(2175, 0, 0);
}

// Undo one action in the undo history.
void wxStyledTextCtrl::Undo() {
    SendMsg(2176, 0, 0);
}

// Cut the selection to the clipboard.
void wxStyledTextCtrl::Cut() {
    SendMsg(2177, 0, 0);
}

// Copy the selection to the clipboard.
void wxStyledTextCtrl::Copy() {
    SendMsg(2178, 0, 0);
}

// Paste the contents of the clipboard into the document replacing the selection.
void wxStyledTextCtrl::Paste() {
    SendMsg(2179, 0, 0);
}

// Clear the selection.
void wxStyledTextCtrl::Clear() {
    SendMsg(2180, 0, 0);
}

// Replace the contents of the document with the argument text.
void wxStyledTextCtrl::SetText(const wxString& text) {
    SendMsg(2181, 0, (long)text.c_str());
}

// Retrieve all the text in the document.
wxString wxStyledTextCtrl::GetText() {
                        wxString text;
                        int   len  = GetTextLength();
                        char* buff = text.GetWriteBuf(len+1);

                        SendMsg(2182, len, (long)buff);
                        buff[len] = 0;
                        text.UngetWriteBuf();
                        return text;
}

// Retrieve the number of characters in the document.
int wxStyledTextCtrl::GetTextLength() {
    return SendMsg(2183, 0, 0);
}

// Set to overtype (true) or insert mode
void wxStyledTextCtrl::SetOvertype(bool overtype) {
    SendMsg(2186, overtype, 0);
}

// Returns true if overtype mode is active otherwise false is returned.
bool wxStyledTextCtrl::GetOvertype() {
    return SendMsg(2187, 0, 0) != 0;
}

// Show a call tip containing a definition near position pos.
void wxStyledTextCtrl::CallTipShow(int pos, const wxString& definition) {
    SendMsg(2200, pos, (long)definition.c_str());
}

// Remove the call tip from the screen.
void wxStyledTextCtrl::CallTipCancel() {
    SendMsg(2201, 0, 0);
}

// Is there an active call tip?
bool wxStyledTextCtrl::CallTipActive() {
    return SendMsg(2202, 0, 0) != 0;
}

// Retrieve the position where the caret was before displaying the call tip.
int wxStyledTextCtrl::CallTipPosAtStart() {
    return SendMsg(2203, 0, 0);
}

// Highlight a segment of the definition.
void wxStyledTextCtrl::CallTipSetHighlight(int start, int end) {
    SendMsg(2204, start, end);
}

// Set the background colour for the call tip.
void wxStyledTextCtrl::CallTipSetBackground(const wxColour& back) {
    SendMsg(2205, wxColourAsLong(back), 0);
}

// Find the display line of a document line taking hidden lines into account.
int wxStyledTextCtrl::VisibleFromDocLine(int line) {
    return SendMsg(2220, line, 0);
}

// Find the document line of a display line taking hidden lines into account.
int wxStyledTextCtrl::DocLineFromVisible(int lineDisplay) {
    return SendMsg(2221, lineDisplay, 0);
}

// Set the fold level of a line.
// This encodes an integer level along with flags indicating whether the
// line is a header and whether it is effectively white space.
void wxStyledTextCtrl::SetFoldLevel(int line, int level) {
    SendMsg(2222, line, level);
}

// Retrieve the fold level of a line.
int wxStyledTextCtrl::GetFoldLevel(int line) {
    return SendMsg(2223, line, 0);
}

// Find the last child line of a header line.
int wxStyledTextCtrl::GetLastChild(int line, int level) {
    return SendMsg(2224, line, level);
}

// Find the parent line of a child line.
int wxStyledTextCtrl::GetFoldParent(int line) {
    return SendMsg(2225, line, 0);
}

// Make a range of lines visible.
void wxStyledTextCtrl::ShowLines(int lineStart, int lineEnd) {
    SendMsg(2226, lineStart, lineEnd);
}

// Make a range of lines invisible.
void wxStyledTextCtrl::HideLines(int lineStart, int lineEnd) {
    SendMsg(2227, lineStart, lineEnd);
}

// Is a line visible?
bool wxStyledTextCtrl::GetLineVisible(int line) {
    return SendMsg(2228, line, 0) != 0;
}

// Show the children of a header line.
void wxStyledTextCtrl::SetFoldExpanded(int line, bool expanded) {
    SendMsg(2229, line, expanded);
}

// Is a header line expanded?
bool wxStyledTextCtrl::GetFoldExpanded(int line) {
    return SendMsg(2230, line, 0) != 0;
}

// Switch a header line between expanded and contracted.
void wxStyledTextCtrl::ToggleFold(int line) {
    SendMsg(2231, line, 0);
}

// Ensure a particular line is visible by expanding any header line hiding it.
void wxStyledTextCtrl::EnsureVisible(int line) {
    SendMsg(2232, line, 0);
}

// Set some debugging options for folding
void wxStyledTextCtrl::SetFoldFlags(int flags) {
    SendMsg(2233, flags, 0);
}

// How many characters are on a line, not including end of line characters.
int wxStyledTextCtrl::LineLength(int line) {
    return SendMsg(2350, line, 0);
}

// Highlight the characters at two positions.
void wxStyledTextCtrl::BraceHighlight(int pos1, int pos2) {
    SendMsg(2351, pos1, pos2);
}

// Highlight the character at a position indicating there is no matching brace.
void wxStyledTextCtrl::BraceBadLight(int pos) {
    SendMsg(2352, pos, 0);
}

// Find the position of a matching brace or INVALID_POSITION if no match.
int wxStyledTextCtrl::BraceMatch(int pos) {
    return SendMsg(2353, pos, 0);
}

// Are the end of line characters visible.
bool wxStyledTextCtrl::GetViewEOL() {
    return SendMsg(2355, 0, 0) != 0;
}

// Make the end of line characters visible or invisible
void wxStyledTextCtrl::SetViewEOL(bool visible) {
    SendMsg(2356, visible, 0);
}

// Retrieve a pointer to the document object.
void* wxStyledTextCtrl::GetDocPointer() {
                           return (void*)SendMsg(2357);
}

// Change the document object used.
void wxStyledTextCtrl::SetDocPointer(void* docPointer) {
                           SendMsg(2358, (long)docPointer);
}

// Set which document modification events are sent to the container.
void wxStyledTextCtrl::SetModEventMask(int mask) {
    SendMsg(2359, mask, 0);
}

// Retrieve the column number which text should be kept within.
int wxStyledTextCtrl::GetEdgeColumn() {
    return SendMsg(2360, 0, 0);
}

// Set the column number of the edge.
// If text goes past the edge then it is highlighted.
void wxStyledTextCtrl::SetEdgeColumn(int column) {
    SendMsg(2361, column, 0);
}

// Retrieve the edge highlight mode.
int wxStyledTextCtrl::GetEdgeMode() {
    return SendMsg(2362, 0, 0);
}

// The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
// goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
void wxStyledTextCtrl::SetEdgeMode(int mode) {
    SendMsg(2363, mode, 0);
}

// Retrieve the colour used in edge indication.
wxColour wxStyledTextCtrl::GetEdgeColour() {
    long c = SendMsg(2364, 0, 0);
    return wxColourFromLong(c);
}

// Change the colour used in edge indication.
void wxStyledTextCtrl::SetEdgeColour(const wxColour& edgeColour) {
    SendMsg(2365, wxColourAsLong(edgeColour), 0);
}

// Sets the current caret position to be the search anchor.
void wxStyledTextCtrl::SearchAnchor() {
    SendMsg(2366, 0, 0);
}

// Find some text starting at the search anchor.
int wxStyledTextCtrl::SearchNext(int flags, const wxString& text) {
    return SendMsg(2367, flags, (long)text.c_str());
}

// Find some text starting at the search anchor and moving backwards.
int wxStyledTextCtrl::SearchPrev(int flags, const wxString& text) {
    return SendMsg(2368, flags, (long)text.c_str());
}

// Set the way the line the caret is on is kept visible.
void wxStyledTextCtrl::SetCaretPolicy(int caretPolicy, int caretSlop) {
    SendMsg(2369, caretPolicy, caretSlop);
}

// Retrieves the number of lines completely visible.
int wxStyledTextCtrl::LinesOnScreen() {
    return SendMsg(2370, 0, 0);
}

// Set whether a pop up menu is displayed automatically when the user presses
// the wrong mouse button.
void wxStyledTextCtrl::UsePopUp(bool allowPopUp) {
    SendMsg(2371, allowPopUp, 0);
}

// Is the selection a rectangular. The alternative is the more common stream selection.
bool wxStyledTextCtrl::SelectionIsRectangle() {
    return SendMsg(2372, 0, 0) != 0;
}

// Set the zoom level. This number of points is added to the size of all fonts.
// It may be positive to magnify or negative to reduce.
void wxStyledTextCtrl::SetZoom(int zoom) {
    SendMsg(2373, zoom, 0);
}

// Retrieve the zoom level.
int wxStyledTextCtrl::GetZoom() {
    return SendMsg(2374, 0, 0);
}

// Create a new document object.
// Starts with reference count of 1 and not selected into editor.
void* wxStyledTextCtrl::CreateDocument() {
                           return (void*)SendMsg(2375);
}

// Extend life of document.
void wxStyledTextCtrl::AddRefDocument(void* docPointer) {
                           SendMsg(2376, (long)docPointer);
}

// Release a reference to the document, deleting document if it fades to black.
void wxStyledTextCtrl::ReleaseDocument(void* docPointer) {
                           SendMsg(2377, (long)docPointer);
}

// Get which document modification events are sent to the container.
int wxStyledTextCtrl::GetModEventMask() {
    return SendMsg(2378, 0, 0);
}

// Start notifying the container of all key presses and commands.
void wxStyledTextCtrl::StartRecord() {
    SendMsg(3001, 0, 0);
}

// Stop notifying the container of all key presses and commands.
void wxStyledTextCtrl::StopRecord() {
    SendMsg(3002, 0, 0);
}

// Set the lexing language of the document.
void wxStyledTextCtrl::SetLexer(int lexer) {
    SendMsg(4001, lexer, 0);
}

// Retrieve the lexing language of the document.
int wxStyledTextCtrl::GetLexer() {
    return SendMsg(4002, 0, 0);
}

// Colourise a segment of the document using the current lexing language.
void wxStyledTextCtrl::Colourise(int start, int end) {
    SendMsg(4003, start, end);
}

// Set up a value that may be used by a lexer for some optional feature.
void wxStyledTextCtrl::SetProperty(const wxString& key, const wxString& value) {
    SendMsg(4004, (long)key.c_str(), (long)value.c_str());
}

// Set up the key words used by the lexer.
void wxStyledTextCtrl::SetKeyWords(int keywordSet, const wxString& keyWords) {
    SendMsg(4005, keywordSet, (long)keyWords.c_str());
}

// END of generated section
//----------------------------------------------------------------------


// Returns the line number of the line with the caret.
int wxStyledTextCtrl::GetCurrentLine() {
    int line = LineFromPosition(GetCurrentPos());
    return line;
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
//      underline               turns on underlining
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

        else if (option == "underline")
            StyleSetUnderline(styleNum, true);

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


// Set style size, face, bold, italic, and underline attributes from
// a wxFont's attributes.
void wxStyledTextCtrl::StyleSetFont(int styleNum, wxFont& font) {
    int      size     = font.GetPointSize();
    wxString faceName = font.GetFaceName();
    bool     bold     = font.GetWeight() == wxBOLD;
    bool     italic   = font.GetStyle() != wxNORMAL;
    bool     under    = font.GetUnderlined();

    // TODO: add encoding/charset mapping
    StyleSetFontAttr(styleNum, size, faceName, bold, italic, under);
}

// Set all font style attributes at once.
void wxStyledTextCtrl::StyleSetFontAttr(int styleNum, int size,
                                        const wxString& faceName,
                                        bool bold, bool italic,
                                        bool underline) {
    StyleSetSize(styleNum, size);
    StyleSetFaceName(styleNum, faceName);
    StyleSetBold(styleNum, bold);
    StyleSetItalic(styleNum, italic);
    StyleSetUnderline(styleNum, underline);

    // TODO: add encoding/charset mapping
}


// Perform one of the operations defined by the wxSTC_CMD_* constants.
void wxStyledTextCtrl::CmdKeyExecute(int cmd) {
    SendMsg(cmd);
}


// Set the left and right margin in the edit area, measured in pixels.
void wxStyledTextCtrl::SetMargins(int left, int right) {
    SetMarginLeft(left);
    SetMarginRight(right);
}


// Retrieve the start and end positions of the current selection.
void wxStyledTextCtrl::GetSelection(int* startPos, int* endPos) {
    if (startPos != NULL)
        *startPos = SendMsg(SCI_GETSELECTIONSTART);
    if (endPos != NULL)
        *endPos = SendMsg(SCI_GETSELECTIONEND);
}


// Retrieve the point in the window where a position is displayed.
wxPoint wxStyledTextCtrl::PointFromPosition(int pos) {
    int x = SendMsg(SCI_POINTXFROMPOSITION, 0, pos);
    int y = SendMsg(SCI_POINTYFROMPOSITION, 0, pos);
    return wxPoint(x, y);
}

// Scroll enough to make the given line visible
void wxStyledTextCtrl::ScrollToLine(int line) {
    m_swx->DoScrollToLine(line);
}


// Scroll enough to make the given column visible
void wxStyledTextCtrl::ScrollToColumn(int column) {
    m_swx->DoScrollToColumn(column);
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


void wxStyledTextCtrl::OnListBox(wxCommandEvent& evt) {
    m_swx->DoOnListBox();
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
    case SCN_POSCHANGED:
        eventType = wxEVT_STC_POSCHANGED;
        break;
    }
    if (eventType) {
        wxStyledTextEvent evt(eventType, GetId());
        evt.SetPosition(scn.position);
        evt.SetKey(scn.ch);
        evt.SetModifiers(scn.modifiers);
        if (eventType == wxEVT_STC_MODIFIED) {
            evt.SetModificationType(scn.modificationType);
            if (scn.text)
                evt.SetText(wxString(scn.text, scn.length));
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

