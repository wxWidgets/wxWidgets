////////////////////////////////////////////////////////////////////////////
// Name:        stc.cpp
// Purpose:     A wxWidgets implementation of Scintilla.  This class is the
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

#include "wx/wx.h"
#include "wx/tokenzr.h"
#include "wx/mstream.h"
#include "wx/image.h"
#include "wx/file.h"

#include "wx/stc/stc.h"
#include "ScintillaWX.h"

//----------------------------------------------------------------------

const wxChar* wxSTCNameStr = wxT("stcwindow");

#ifdef MAKELONG
#undef MAKELONG
#endif

#define MAKELONG(a, b) ((a) | ((b) << 16))


static long wxColourAsLong(const wxColour& co) {
    return (((long)co.Blue()  << 16) |
            ((long)co.Green() <<  8) |
            ((long)co.Red()));
}

static wxColour wxColourFromLong(long c) {
    wxColour clr;
    clr.Set((unsigned char)(c & 0xff),
            (unsigned char)((c >> 8) & 0xff),
            (unsigned char)((c >> 16) & 0xff));
    return clr;
}


static wxColour wxColourFromSpec(const wxString& spec) {
    // spec should be a colour name or "#RRGGBB"
    if (spec.GetChar(0) == wxT('#')) {

        long red, green, blue;
        red = green = blue = 0;
        spec.Mid(1,2).ToLong(&red,   16);
        spec.Mid(3,2).ToLong(&green, 16);
        spec.Mid(5,2).ToLong(&blue,  16);
        return wxColour((unsigned char)red,
                        (unsigned char)green,
                        (unsigned char)blue);
    }
    else
        return wxColour(spec);
}

//----------------------------------------------------------------------

DEFINE_EVENT_TYPE( wxEVT_STC_CHANGE )
DEFINE_EVENT_TYPE( wxEVT_STC_STYLENEEDED )
DEFINE_EVENT_TYPE( wxEVT_STC_CHARADDED )
DEFINE_EVENT_TYPE( wxEVT_STC_SAVEPOINTREACHED )
DEFINE_EVENT_TYPE( wxEVT_STC_SAVEPOINTLEFT )
DEFINE_EVENT_TYPE( wxEVT_STC_ROMODIFYATTEMPT )
DEFINE_EVENT_TYPE( wxEVT_STC_KEY )
DEFINE_EVENT_TYPE( wxEVT_STC_DOUBLECLICK )
DEFINE_EVENT_TYPE( wxEVT_STC_UPDATEUI )
DEFINE_EVENT_TYPE( wxEVT_STC_MODIFIED )
DEFINE_EVENT_TYPE( wxEVT_STC_MACRORECORD )
DEFINE_EVENT_TYPE( wxEVT_STC_MARGINCLICK )
DEFINE_EVENT_TYPE( wxEVT_STC_NEEDSHOWN )
DEFINE_EVENT_TYPE( wxEVT_STC_PAINTED )
DEFINE_EVENT_TYPE( wxEVT_STC_USERLISTSELECTION )
DEFINE_EVENT_TYPE( wxEVT_STC_URIDROPPED )
DEFINE_EVENT_TYPE( wxEVT_STC_DWELLSTART )
DEFINE_EVENT_TYPE( wxEVT_STC_DWELLEND )
DEFINE_EVENT_TYPE( wxEVT_STC_START_DRAG )
DEFINE_EVENT_TYPE( wxEVT_STC_DRAG_OVER )
DEFINE_EVENT_TYPE( wxEVT_STC_DO_DROP )
DEFINE_EVENT_TYPE( wxEVT_STC_ZOOM )
DEFINE_EVENT_TYPE( wxEVT_STC_HOTSPOT_CLICK )
DEFINE_EVENT_TYPE( wxEVT_STC_HOTSPOT_DCLICK )
DEFINE_EVENT_TYPE( wxEVT_STC_CALLTIP_CLICK )
DEFINE_EVENT_TYPE( wxEVT_STC_AUTOCOMP_SELECTION )    



BEGIN_EVENT_TABLE(wxStyledTextCtrl, wxControl)
    EVT_PAINT                   (wxStyledTextCtrl::OnPaint)
    EVT_SCROLLWIN               (wxStyledTextCtrl::OnScrollWin)
    EVT_SCROLL                  (wxStyledTextCtrl::OnScroll)
    EVT_SIZE                    (wxStyledTextCtrl::OnSize)
    EVT_LEFT_DOWN               (wxStyledTextCtrl::OnMouseLeftDown)
    // Let Scintilla see the double click as a second click
    EVT_LEFT_DCLICK             (wxStyledTextCtrl::OnMouseLeftDown)
    EVT_MOTION                  (wxStyledTextCtrl::OnMouseMove)
    EVT_LEFT_UP                 (wxStyledTextCtrl::OnMouseLeftUp)
#if defined(__WXGTK__) || defined(__WXMAC__)
    EVT_RIGHT_UP                (wxStyledTextCtrl::OnMouseRightUp)
#else
    EVT_CONTEXT_MENU            (wxStyledTextCtrl::OnContextMenu)
#endif
    EVT_MOUSEWHEEL              (wxStyledTextCtrl::OnMouseWheel)
    EVT_MIDDLE_UP               (wxStyledTextCtrl::OnMouseMiddleUp)
    EVT_CHAR                    (wxStyledTextCtrl::OnChar)
    EVT_KEY_DOWN                (wxStyledTextCtrl::OnKeyDown)
    EVT_KILL_FOCUS              (wxStyledTextCtrl::OnLoseFocus)
    EVT_SET_FOCUS               (wxStyledTextCtrl::OnGainFocus)
    EVT_SYS_COLOUR_CHANGED      (wxStyledTextCtrl::OnSysColourChanged)
    EVT_ERASE_BACKGROUND        (wxStyledTextCtrl::OnEraseBackground)
    EVT_MENU_RANGE              (10, 16, wxStyledTextCtrl::OnMenu)
    EVT_LISTBOX_DCLICK          (wxID_ANY, wxStyledTextCtrl::OnListBox)
END_EVENT_TABLE()


IMPLEMENT_CLASS(wxStyledTextCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxStyledTextEvent, wxCommandEvent)

#ifdef LINK_LEXERS
// forces the linking of the lexer modules
int Scintilla_LinkLexers();
#endif

//----------------------------------------------------------------------
// Constructor and Destructor

wxStyledTextCtrl::wxStyledTextCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
{
    m_swx = NULL;
    Create(parent, id, pos, size, style, name);
}


bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
#ifdef __WXMAC__
    style |= wxVSCROLL | wxHSCROLL;
#endif
    if (!wxControl::Create(parent, id, pos, size,
                           style | wxWANTS_CHARS | wxCLIP_CHILDREN,
                           wxDefaultValidator, name))
        return false;

#ifdef LINK_LEXERS
    Scintilla_LinkLexers();
#endif
    m_swx = new ScintillaWX(this);
    m_stopWatch.Start();
    m_lastKeyDownConsumed = false;
    m_vScrollBar = NULL;
    m_hScrollBar = NULL;
#if wxUSE_UNICODE
    // Put Scintilla into unicode (UTF-8) mode
    SetCodePage(wxSTC_CP_UTF8);
#endif

    SetInitialSize(size);

    // Reduces flicker on GTK+/X11
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    return true;
}


wxStyledTextCtrl::~wxStyledTextCtrl() {
    delete m_swx;
}


//----------------------------------------------------------------------

long wxStyledTextCtrl::SendMsg(int msg, long wp, long lp) {

    return m_swx->WndProc(msg, wp, lp);
}

//----------------------------------------------------------------------

// Set the vertical scrollbar to use instead of the ont that's built-in.
void wxStyledTextCtrl::SetVScrollBar(wxScrollBar* bar)  {
    m_vScrollBar = bar;
    if (bar != NULL) {
        // ensure that the built-in scrollbar is not visible
        SetScrollbar(wxVERTICAL, 0, 0, 0);
    }
}


// Set the horizontal scrollbar to use instead of the ont that's built-in.
void wxStyledTextCtrl::SetHScrollBar(wxScrollBar* bar)  {
    m_hScrollBar = bar;
    if (bar != NULL) {
        // ensure that the built-in scrollbar is not visible
        SetScrollbar(wxHORIZONTAL, 0, 0, 0);
    }
}

//----------------------------------------------------------------------
// BEGIN generated section.  The following code is automatically generated
//       by gen_iface.py from the contents of Scintilla.iface.  Do not edit
//       this file.  Edit stc.cpp.in or gen_iface.py instead and regenerate.


// Add text to the document at current position.
void wxStyledTextCtrl::AddText(const wxString& text) {
                    wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
                    SendMsg(2001, strlen(buf), (long)(const char*)buf);
}

// Add array of cells to document.
void wxStyledTextCtrl::AddStyledText(const wxMemoryBuffer& data) {
                          SendMsg(2002, data.GetDataLen(), (long)data.GetData());
}

// Insert string at a position.
void wxStyledTextCtrl::InsertText(int pos, const wxString& text) {
    SendMsg(2003, pos, (long)(const char*)wx2stc(text));
}

// Delete all text in the document.
void wxStyledTextCtrl::ClearAll() {
    SendMsg(2004, 0, 0);
}

// Set all style bytes to 0, remove all folding information.
void wxStyledTextCtrl::ClearDocumentStyle() {
    SendMsg(2005, 0, 0);
}

// Returns the number of characters in the document.
int wxStyledTextCtrl::GetLength() {
    return SendMsg(2006, 0, 0);
}

// Returns the character byte at the position.
int wxStyledTextCtrl::GetCharAt(int pos) {
         return (unsigned char)SendMsg(2007, pos, 0);
}

// Returns the position of the caret.
int wxStyledTextCtrl::GetCurrentPos() {
    return SendMsg(2008, 0, 0);
}

// Returns the position of the opposite end of the selection to the caret.
int wxStyledTextCtrl::GetAnchor() {
    return SendMsg(2009, 0, 0);
}

// Returns the style byte at the position.
int wxStyledTextCtrl::GetStyleAt(int pos) {
         return (unsigned char)SendMsg(2010, pos, 0);
}

// Redoes the next action on the undo history.
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
wxMemoryBuffer wxStyledTextCtrl::GetStyledText(int startPos, int endPos) {
        wxMemoryBuffer buf;
        if (endPos < startPos) {
            int temp = startPos;
            startPos = endPos;
            endPos = temp;
        }
        int len = endPos - startPos;
        if (!len) return buf;
        TextRange tr;
        tr.lpstrText = (char*)buf.GetWriteBuf(len*2+1);
        tr.chrg.cpMin = startPos;
        tr.chrg.cpMax = endPos;
        len = SendMsg(2015, 0, (long)&tr);
        buf.UngetWriteBuf(len);
        return buf;
}

// Are there any redoable actions in the undo history?
bool wxStyledTextCtrl::CanRedo() {
    return SendMsg(2016, 0, 0) != 0;
}

// Retrieve the line number at which a particular marker is located.
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

// Find the position from a point within the window but return
// INVALID_POSITION if not close to text.
int wxStyledTextCtrl::PositionFromPointClose(int x, int y) {
    return SendMsg(2023, x, y);
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
        int len = LineLength(GetCurrentLine());
        if (!len) {
            if (linePos)  *linePos = 0;
            return wxEmptyString;
        }

        wxMemoryBuffer mbuf(len+1);
        char* buf = (char*)mbuf.GetWriteBuf(len+1);

        int pos = SendMsg(2027, len+1, (long)buf);
        mbuf.UngetWriteBuf(len);
        mbuf.AppendByte(0);
        if (linePos)  *linePos = pos;
        return stc2wx(buf);
}

// Retrieve the position of the last correctly styled character.
int wxStyledTextCtrl::GetEndStyled() {
    return SendMsg(2028, 0, 0);
}

// Convert all line endings in the document to one mode.
void wxStyledTextCtrl::ConvertEOLs(int eolMode) {
    SendMsg(2029, eolMode, 0);
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
// The styling mask can be used to protect some bits in each styling byte from modification.
void wxStyledTextCtrl::StartStyling(int pos, int mask) {
    SendMsg(2032, pos, mask);
}

// Change style from current styling position for length characters to a style
// and move the current styling position to after this newly styled segment.
void wxStyledTextCtrl::SetStyling(int length, int style) {
    SendMsg(2033, length, style);
}

// Is drawing done first into a buffer or direct to the screen?
bool wxStyledTextCtrl::GetBufferedDraw() {
    return SendMsg(2034, 0, 0) != 0;
}

// If drawing is buffered then each line of text is drawn into a bitmap buffer
// before drawing it to the screen to avoid flicker.
void wxStyledTextCtrl::SetBufferedDraw(bool buffered) {
    SendMsg(2035, buffered, 0);
}

// Change the visible size of a tab to be a multiple of the width of a space character.
void wxStyledTextCtrl::SetTabWidth(int tabWidth) {
    SendMsg(2036, tabWidth, 0);
}

// Retrieve the visible size of a tab.
int wxStyledTextCtrl::GetTabWidth() {
    return SendMsg(2121, 0, 0);
}

// Set the code page used to interpret the bytes of the document as characters.
void wxStyledTextCtrl::SetCodePage(int codePage) {
#if wxUSE_UNICODE
    wxASSERT_MSG(codePage == wxSTC_CP_UTF8,
                 wxT("Only wxSTC_CP_UTF8 may be used when wxUSE_UNICODE is on."));
#else
    wxASSERT_MSG(codePage != wxSTC_CP_UTF8,
                 wxT("wxSTC_CP_UTF8 may not be used when wxUSE_UNICODE is off."));
#endif
    SendMsg(2037, codePage);
}

// Set the symbol used for a particular marker number,
// and optionally the fore and background colours.
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

// Add a marker to a line, returning an ID which can be used to find or delete the marker.
int wxStyledTextCtrl::MarkerAdd(int line, int markerNumber) {
    return SendMsg(2043, line, markerNumber);
}

// Delete a marker from a line.
void wxStyledTextCtrl::MarkerDelete(int line, int markerNumber) {
    SendMsg(2044, line, markerNumber);
}

// Delete all markers with a particular number from all lines.
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

// Define a marker from a bitmap
void wxStyledTextCtrl::MarkerDefineBitmap(int markerNumber, const wxBitmap& bmp) {
        // convert bmp to a xpm in a string
        wxMemoryOutputStream strm;
        wxImage img = bmp.ConvertToImage();
        if (img.HasAlpha())
            img.ConvertAlphaToMask();
        img.SaveFile(strm, wxBITMAP_TYPE_XPM);
        size_t len = strm.GetSize();
        char* buff = new char[len+1];
        strm.CopyTo(buff, len);
        buff[len] = 0;
        SendMsg(2049, markerNumber, (long)buff);
        delete [] buff;
        
}

// Add a set of markers to a line.
void wxStyledTextCtrl::MarkerAddSet(int line, int set) {
    SendMsg(2466, line, set);
}

// Set the alpha used for a marker that is drawn in the text area, not the margin.
void wxStyledTextCtrl::MarkerSetAlpha(int markerNumber, int alpha) {
    SendMsg(2476, markerNumber, alpha);
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
    SendMsg(2056, style, (long)(const char*)wx2stc(fontName));
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

// Set a style to be mixed case, or to force upper or lower case.
void wxStyledTextCtrl::StyleSetCase(int style, int caseForce) {
    SendMsg(2060, style, caseForce);
}

// Set a style to be a hotspot or not.
void wxStyledTextCtrl::StyleSetHotSpot(int style, bool hotspot) {
    SendMsg(2409, style, hotspot);
}

// Set the foreground colour of the selection and whether to use this setting.
void wxStyledTextCtrl::SetSelForeground(bool useSetting, const wxColour& fore) {
    SendMsg(2067, useSetting, wxColourAsLong(fore));
}

// Set the background colour of the selection and whether to use this setting.
void wxStyledTextCtrl::SetSelBackground(bool useSetting, const wxColour& back) {
    SendMsg(2068, useSetting, wxColourAsLong(back));
}

// Get the alpha of the selection.
int wxStyledTextCtrl::GetSelAlpha() {
    return SendMsg(2477, 0, 0);
}

// Set the alpha of the selection.
void wxStyledTextCtrl::SetSelAlpha(int alpha) {
    SendMsg(2478, alpha, 0);
}

// Set the foreground colour of the caret.
void wxStyledTextCtrl::SetCaretForeground(const wxColour& fore) {
    SendMsg(2069, wxColourAsLong(fore), 0);
}

// When key+modifier combination km is pressed perform msg.
void wxStyledTextCtrl::CmdKeyAssign(int key, int modifiers, int cmd) {
         SendMsg(2070, MAKELONG(key, modifiers), cmd);
}

// When key+modifier combination km is pressed do nothing.
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

// Set the set of characters making up words for when moving or selecting by word.
// First sets deaults like SetCharsDefault.
void wxStyledTextCtrl::SetWordChars(const wxString& characters) {
    SendMsg(2077, 0, (long)(const char*)wx2stc(characters));
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

// Set the foreground colour of all whitespace and whether to use this setting.
void wxStyledTextCtrl::SetWhitespaceForeground(bool useSetting, const wxColour& fore) {
    SendMsg(2084, useSetting, wxColourAsLong(fore));
}

// Set the background colour of all whitespace and whether to use this setting.
void wxStyledTextCtrl::SetWhitespaceBackground(bool useSetting, const wxColour& back) {
    SendMsg(2085, useSetting, wxColourAsLong(back));
}

// Divide each styling byte into lexical class bits (default: 5) and indicator
// bits (default: 3). If a lexer requires more than 32 lexical states, then this
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

// Is the background of the line containing the caret in a different colour?
bool wxStyledTextCtrl::GetCaretLineVisible() {
    return SendMsg(2095, 0, 0) != 0;
}

// Display the background of the line containing the caret in a different colour.
void wxStyledTextCtrl::SetCaretLineVisible(bool show) {
    SendMsg(2096, show, 0);
}

// Get the colour of the background of the line containing the caret.
wxColour wxStyledTextCtrl::GetCaretLineBackground() {
    long c = SendMsg(2097, 0, 0);
    return wxColourFromLong(c);
}

// Set the colour of the background of the line containing the caret.
void wxStyledTextCtrl::SetCaretLineBackground(const wxColour& back) {
    SendMsg(2098, wxColourAsLong(back), 0);
}

// Set a style to be changeable or not (read only).
// Experimental feature, currently buggy.
void wxStyledTextCtrl::StyleSetChangeable(int style, bool changeable) {
    SendMsg(2099, style, changeable);
}

// Display a auto-completion list.
// The lenEntered parameter indicates how many characters before
// the caret should be used to provide context.
void wxStyledTextCtrl::AutoCompShow(int lenEntered, const wxString& itemList) {
    SendMsg(2100, lenEntered, (long)(const char*)wx2stc(itemList));
}

// Remove the auto-completion list from the screen.
void wxStyledTextCtrl::AutoCompCancel() {
    SendMsg(2101, 0, 0);
}

// Is there an auto-completion list visible?
bool wxStyledTextCtrl::AutoCompActive() {
    return SendMsg(2102, 0, 0) != 0;
}

// Retrieve the position of the caret when the auto-completion list was displayed.
int wxStyledTextCtrl::AutoCompPosStart() {
    return SendMsg(2103, 0, 0);
}

// User has selected an item so remove the list and insert the selection.
void wxStyledTextCtrl::AutoCompComplete() {
    SendMsg(2104, 0, 0);
}

// Define a set of character that when typed cancel the auto-completion list.
void wxStyledTextCtrl::AutoCompStops(const wxString& characterSet) {
    SendMsg(2105, 0, (long)(const char*)wx2stc(characterSet));
}

// Change the separator character in the string setting up an auto-completion list.
// Default is space but can be changed if items contain space.
void wxStyledTextCtrl::AutoCompSetSeparator(int separatorCharacter) {
    SendMsg(2106, separatorCharacter, 0);
}

// Retrieve the auto-completion list separator character.
int wxStyledTextCtrl::AutoCompGetSeparator() {
    return SendMsg(2107, 0, 0);
}

// Select the item in the auto-completion list that starts with a string.
void wxStyledTextCtrl::AutoCompSelect(const wxString& text) {
    SendMsg(2108, 0, (long)(const char*)wx2stc(text));
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

// Define a set of characters that when typed will cause the autocompletion to
// choose the selected item.
void wxStyledTextCtrl::AutoCompSetFillUps(const wxString& characterSet) {
    SendMsg(2112, 0, (long)(const char*)wx2stc(characterSet));
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

// Display a list of strings and send notification when user chooses one.
void wxStyledTextCtrl::UserListShow(int listType, const wxString& itemList) {
    SendMsg(2117, listType, (long)(const char*)wx2stc(itemList));
}

// Set whether or not autocompletion is hidden automatically when nothing matches.
void wxStyledTextCtrl::AutoCompSetAutoHide(bool autoHide) {
    SendMsg(2118, autoHide, 0);
}

// Retrieve whether or not autocompletion is hidden automatically when nothing matches.
bool wxStyledTextCtrl::AutoCompGetAutoHide() {
    return SendMsg(2119, 0, 0) != 0;
}

// Set whether or not autocompletion deletes any word characters
// after the inserted text upon completion.
void wxStyledTextCtrl::AutoCompSetDropRestOfWord(bool dropRestOfWord) {
    SendMsg(2270, dropRestOfWord, 0);
}

// Retrieve whether or not autocompletion deletes any word characters
// after the inserted text upon completion.
bool wxStyledTextCtrl::AutoCompGetDropRestOfWord() {
    return SendMsg(2271, 0, 0) != 0;
}

// Register an image for use in autocompletion lists.
void wxStyledTextCtrl::RegisterImage(int type, const wxBitmap& bmp) {
        // convert bmp to a xpm in a string
        wxMemoryOutputStream strm;
        wxImage img = bmp.ConvertToImage();
        if (img.HasAlpha())
            img.ConvertAlphaToMask();
        img.SaveFile(strm, wxBITMAP_TYPE_XPM);
        size_t len = strm.GetSize();
        char* buff = new char[len+1];
        strm.CopyTo(buff, len);
        buff[len] = 0;
        SendMsg(2405, type, (long)buff);
        delete [] buff;
     
}

// Clear all the registered images.
void wxStyledTextCtrl::ClearRegisteredImages() {
    SendMsg(2408, 0, 0);
}

// Retrieve the auto-completion list type-separator character.
int wxStyledTextCtrl::AutoCompGetTypeSeparator() {
    return SendMsg(2285, 0, 0);
}

// Change the type-separator character in the string setting up an auto-completion list.
// Default is '?' but can be changed if items contain '?'.
void wxStyledTextCtrl::AutoCompSetTypeSeparator(int separatorCharacter) {
    SendMsg(2286, separatorCharacter, 0);
}

// Set the maximum width, in characters, of auto-completion and user lists.
// Set to 0 to autosize to fit longest item, which is the default.
void wxStyledTextCtrl::AutoCompSetMaxWidth(int characterCount) {
    SendMsg(2208, characterCount, 0);
}

// Get the maximum width, in characters, of auto-completion and user lists.
int wxStyledTextCtrl::AutoCompGetMaxWidth() {
    return SendMsg(2209, 0, 0);
}

// Set the maximum height, in rows, of auto-completion and user lists.
// The default is 5 rows.
void wxStyledTextCtrl::AutoCompSetMaxHeight(int rowCount) {
    SendMsg(2210, rowCount, 0);
}

// Set the maximum height, in rows, of auto-completion and user lists.
int wxStyledTextCtrl::AutoCompGetMaxHeight() {
    return SendMsg(2211, 0, 0);
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
               int flags) {
            TextToFind  ft;
            ft.chrg.cpMin = minPos;
            ft.chrg.cpMax = maxPos;
            wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
            ft.lpstrText = (char*)(const char*)buf;

            return SendMsg(2150, flags, (long)&ft);
}

// On Windows, will draw the document into a display context such as a printer.
 int wxStyledTextCtrl::FormatRange(bool   doDraw,
                int    startPos,
                int    endPos,
                wxDC*  draw,
                wxDC*  target, 
                wxRect renderRect,
                wxRect pageRect) {
             RangeToFormat fr;

             if (endPos < startPos) {
                 int temp = startPos;
                 startPos = endPos;
                 endPos = temp;
             }
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

// Retrieve the display line at the top of the display.
int wxStyledTextCtrl::GetFirstVisibleLine() {
    return SendMsg(2152, 0, 0);
}

// Retrieve the contents of a line.
wxString wxStyledTextCtrl::GetLine(int line) {
         int len = LineLength(line);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(2153, line, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
}

// Returns the number of lines in the document. There is always at least one.
int wxStyledTextCtrl::GetLineCount() {
    return SendMsg(2154, 0, 0);
}

// Sets the size in pixels of the left margin.
void wxStyledTextCtrl::SetMarginLeft(int pixelWidth) {
    SendMsg(2155, 0, pixelWidth);
}

// Returns the size in pixels of the left margin.
int wxStyledTextCtrl::GetMarginLeft() {
    return SendMsg(2156, 0, 0);
}

// Sets the size in pixels of the right margin.
void wxStyledTextCtrl::SetMarginRight(int pixelWidth) {
    SendMsg(2157, 0, pixelWidth);
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
         int   start;
         int   end;

         GetSelection(&start, &end);
         int   len  = end - start;
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+2);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(2161, 0, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
}

// Retrieve a range of text.
wxString wxStyledTextCtrl::GetTextRange(int startPos, int endPos) {
         if (endPos < startPos) {
             int temp = startPos;
             startPos = endPos;
             endPos = temp;
         }
         int   len  = endPos - startPos;
         if (!len) return wxEmptyString;
         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len);
         TextRange tr;
         tr.lpstrText = buf;
         tr.chrg.cpMin = startPos;
         tr.chrg.cpMax = endPos;
         SendMsg(2162, 0, (long)&tr);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
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
    SendMsg(2170, 0, (long)(const char*)wx2stc(text));
}

// Set to read only or read write.
void wxStyledTextCtrl::SetReadOnly(bool readOnly) {
    SendMsg(2171, readOnly, 0);
}

// Will a paste succeed?
bool wxStyledTextCtrl::CanPaste() {
    return SendMsg(2173, 0, 0) != 0;
}

// Are there any undoable actions in the undo history?
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
    SendMsg(2181, 0, (long)(const char*)wx2stc(text));
}

// Retrieve all the text in the document.
wxString wxStyledTextCtrl::GetText() {
         int len  = GetTextLength();
         wxMemoryBuffer mbuf(len+1);   // leave room for the null...
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(2182, len+1, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
}

// Retrieve the number of characters in the document.
int wxStyledTextCtrl::GetTextLength() {
    return SendMsg(2183, 0, 0);
}

// Set to overtype (true) or insert mode.
void wxStyledTextCtrl::SetOvertype(bool overtype) {
    SendMsg(2186, overtype, 0);
}

// Returns true if overtype mode is active otherwise false is returned.
bool wxStyledTextCtrl::GetOvertype() {
    return SendMsg(2187, 0, 0) != 0;
}

// Set the width of the insert mode caret.
void wxStyledTextCtrl::SetCaretWidth(int pixelWidth) {
    SendMsg(2188, pixelWidth, 0);
}

// Returns the width of the insert mode caret.
int wxStyledTextCtrl::GetCaretWidth() {
    return SendMsg(2189, 0, 0);
}

// Sets the position that starts the target which is used for updating the
// document without affecting the scroll position.
void wxStyledTextCtrl::SetTargetStart(int pos) {
    SendMsg(2190, pos, 0);
}

// Get the position that starts the target.
int wxStyledTextCtrl::GetTargetStart() {
    return SendMsg(2191, 0, 0);
}

// Sets the position that ends the target which is used for updating the
// document without affecting the scroll position.
void wxStyledTextCtrl::SetTargetEnd(int pos) {
    SendMsg(2192, pos, 0);
}

// Get the position that ends the target.
int wxStyledTextCtrl::GetTargetEnd() {
    return SendMsg(2193, 0, 0);
}

// Replace the target text with the argument text.
// Text is counted so it can contain NULs.
// Returns the length of the replacement text.

     int wxStyledTextCtrl::ReplaceTarget(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(2194, strlen(buf), (long)(const char*)buf);
}

// Replace the target text with the argument text after \d processing.
// Text is counted so it can contain NULs.
// Looks for \d where d is between 1 and 9 and replaces these with the strings
// matched in the last search operation which were surrounded by \( and \).
// Returns the length of the replacement text including any change
// caused by processing the \d patterns.

     int wxStyledTextCtrl::ReplaceTargetRE(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(2195, strlen(buf), (long)(const char*)buf);
}

// Search for a counted string in the target and set the target to the found
// range. Text is counted so it can contain NULs.
// Returns length of range or -1 for failure in which case target is not moved.

     int wxStyledTextCtrl::SearchInTarget(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(2197, strlen(buf), (long)(const char*)buf);
}

// Set the search flags used by SearchInTarget.
void wxStyledTextCtrl::SetSearchFlags(int flags) {
    SendMsg(2198, flags, 0);
}

// Get the search flags used by SearchInTarget.
int wxStyledTextCtrl::GetSearchFlags() {
    return SendMsg(2199, 0, 0);
}

// Show a call tip containing a definition near position pos.
void wxStyledTextCtrl::CallTipShow(int pos, const wxString& definition) {
    SendMsg(2200, pos, (long)(const char*)wx2stc(definition));
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

// Set the foreground colour for the call tip.
void wxStyledTextCtrl::CallTipSetForeground(const wxColour& fore) {
    SendMsg(2206, wxColourAsLong(fore), 0);
}

// Set the foreground colour for the highlighted part of the call tip.
void wxStyledTextCtrl::CallTipSetForegroundHighlight(const wxColour& fore) {
    SendMsg(2207, wxColourAsLong(fore), 0);
}

// Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
void wxStyledTextCtrl::CallTipUseStyle(int tabSize) {
    SendMsg(2212, tabSize, 0);
}

// Find the display line of a document line taking hidden lines into account.
int wxStyledTextCtrl::VisibleFromDocLine(int line) {
    return SendMsg(2220, line, 0);
}

// Find the document line of a display line taking hidden lines into account.
int wxStyledTextCtrl::DocLineFromVisible(int lineDisplay) {
    return SendMsg(2221, lineDisplay, 0);
}

// The number of display lines needed to wrap a document line
int wxStyledTextCtrl::WrapCount(int line) {
    return SendMsg(2235, line, 0);
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

// Set some style options for folding.
void wxStyledTextCtrl::SetFoldFlags(int flags) {
    SendMsg(2233, flags, 0);
}

// Ensure a particular line is visible by expanding any header line hiding it.
// Use the currently set visibility policy to determine which range to display.
void wxStyledTextCtrl::EnsureVisibleEnforcePolicy(int line) {
    SendMsg(2234, line, 0);
}

// Sets whether a tab pressed when caret is within indentation indents.
void wxStyledTextCtrl::SetTabIndents(bool tabIndents) {
    SendMsg(2260, tabIndents, 0);
}

// Does a tab pressed when caret is within indentation indent?
bool wxStyledTextCtrl::GetTabIndents() {
    return SendMsg(2261, 0, 0) != 0;
}

// Sets whether a backspace pressed when caret is within indentation unindents.
void wxStyledTextCtrl::SetBackSpaceUnIndents(bool bsUnIndents) {
    SendMsg(2262, bsUnIndents, 0);
}

// Does a backspace pressed when caret is within indentation unindent?
bool wxStyledTextCtrl::GetBackSpaceUnIndents() {
    return SendMsg(2263, 0, 0) != 0;
}

// Sets the time the mouse must sit still to generate a mouse dwell event.
void wxStyledTextCtrl::SetMouseDwellTime(int periodMilliseconds) {
    SendMsg(2264, periodMilliseconds, 0);
}

// Retrieve the time the mouse must sit still to generate a mouse dwell event.
int wxStyledTextCtrl::GetMouseDwellTime() {
    return SendMsg(2265, 0, 0);
}

// Get position of start of word.
int wxStyledTextCtrl::WordStartPosition(int pos, bool onlyWordCharacters) {
    return SendMsg(2266, pos, onlyWordCharacters);
}

// Get position of end of word.
int wxStyledTextCtrl::WordEndPosition(int pos, bool onlyWordCharacters) {
    return SendMsg(2267, pos, onlyWordCharacters);
}

// Sets whether text is word wrapped.
void wxStyledTextCtrl::SetWrapMode(int mode) {
    SendMsg(2268, mode, 0);
}

// Retrieve whether text is word wrapped.
int wxStyledTextCtrl::GetWrapMode() {
    return SendMsg(2269, 0, 0);
}

// Set the display mode of visual flags for wrapped lines.
void wxStyledTextCtrl::SetWrapVisualFlags(int wrapVisualFlags) {
    SendMsg(2460, wrapVisualFlags, 0);
}

// Retrive the display mode of visual flags for wrapped lines.
int wxStyledTextCtrl::GetWrapVisualFlags() {
    return SendMsg(2461, 0, 0);
}

// Set the location of visual flags for wrapped lines.
void wxStyledTextCtrl::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation) {
    SendMsg(2462, wrapVisualFlagsLocation, 0);
}

// Retrive the location of visual flags for wrapped lines.
int wxStyledTextCtrl::GetWrapVisualFlagsLocation() {
    return SendMsg(2463, 0, 0);
}

// Set the start indent for wrapped lines.
void wxStyledTextCtrl::SetWrapStartIndent(int indent) {
    SendMsg(2464, indent, 0);
}

// Retrive the start indent for wrapped lines.
int wxStyledTextCtrl::GetWrapStartIndent() {
    return SendMsg(2465, 0, 0);
}

// Sets the degree of caching of layout information.
void wxStyledTextCtrl::SetLayoutCache(int mode) {
    SendMsg(2272, mode, 0);
}

// Retrieve the degree of caching of layout information.
int wxStyledTextCtrl::GetLayoutCache() {
    return SendMsg(2273, 0, 0);
}

// Sets the document width assumed for scrolling.
void wxStyledTextCtrl::SetScrollWidth(int pixelWidth) {
    SendMsg(2274, pixelWidth, 0);
}

// Retrieve the document width assumed for scrolling.
int wxStyledTextCtrl::GetScrollWidth() {
    return SendMsg(2275, 0, 0);
}

// Measure the pixel width of some text in a particular style.
// NUL terminated text argument.
// Does not handle tab or control characters.
int wxStyledTextCtrl::TextWidth(int style, const wxString& text) {
    return SendMsg(2276, style, (long)(const char*)wx2stc(text));
}

// Sets the scroll range so that maximum scroll position has
// the last line at the bottom of the view (default).
// Setting this to false allows scrolling one page below the last line.
void wxStyledTextCtrl::SetEndAtLastLine(bool endAtLastLine) {
    SendMsg(2277, endAtLastLine, 0);
}

// Retrieve whether the maximum scroll position has the last
// line at the bottom of the view.
bool wxStyledTextCtrl::GetEndAtLastLine() {
    return SendMsg(2278, 0, 0) != 0;
}

// Retrieve the height of a particular line of text in pixels.
int wxStyledTextCtrl::TextHeight(int line) {
    return SendMsg(2279, line, 0);
}

// Show or hide the vertical scroll bar.
void wxStyledTextCtrl::SetUseVerticalScrollBar(bool show) {
    SendMsg(2280, show, 0);
}

// Is the vertical scroll bar visible?
bool wxStyledTextCtrl::GetUseVerticalScrollBar() {
    return SendMsg(2281, 0, 0) != 0;
}

// Append a string to the end of the document without changing the selection.
void wxStyledTextCtrl::AppendText(const wxString& text) {
                    wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
                    SendMsg(2282, strlen(buf), (long)(const char*)buf);
}

// Is drawing done in two phases with backgrounds drawn before foregrounds?
bool wxStyledTextCtrl::GetTwoPhaseDraw() {
    return SendMsg(2283, 0, 0) != 0;
}

// In twoPhaseDraw mode, drawing is performed in two phases, first the background
// and then the foreground. This avoids chopping off characters that overlap the next run.
void wxStyledTextCtrl::SetTwoPhaseDraw(bool twoPhase) {
    SendMsg(2284, twoPhase, 0);
}

// Make the target range start and end be the same as the selection range start and end.
void wxStyledTextCtrl::TargetFromSelection() {
    SendMsg(2287, 0, 0);
}

// Join the lines in the target.
void wxStyledTextCtrl::LinesJoin() {
    SendMsg(2288, 0, 0);
}

// Split the lines in the target into lines that are less wide than pixelWidth
// where possible.
void wxStyledTextCtrl::LinesSplit(int pixelWidth) {
    SendMsg(2289, pixelWidth, 0);
}

// Set the colours used as a chequerboard pattern in the fold margin
void wxStyledTextCtrl::SetFoldMarginColour(bool useSetting, const wxColour& back) {
    SendMsg(2290, useSetting, wxColourAsLong(back));
}
void wxStyledTextCtrl::SetFoldMarginHiColour(bool useSetting, const wxColour& fore) {
    SendMsg(2291, useSetting, wxColourAsLong(fore));
}

// Move caret down one line.
void wxStyledTextCtrl::LineDown() {
    SendMsg(2300, 0, 0);
}

// Move caret down one line extending selection to new caret position.
void wxStyledTextCtrl::LineDownExtend() {
    SendMsg(2301, 0, 0);
}

// Move caret up one line.
void wxStyledTextCtrl::LineUp() {
    SendMsg(2302, 0, 0);
}

// Move caret up one line extending selection to new caret position.
void wxStyledTextCtrl::LineUpExtend() {
    SendMsg(2303, 0, 0);
}

// Move caret left one character.
void wxStyledTextCtrl::CharLeft() {
    SendMsg(2304, 0, 0);
}

// Move caret left one character extending selection to new caret position.
void wxStyledTextCtrl::CharLeftExtend() {
    SendMsg(2305, 0, 0);
}

// Move caret right one character.
void wxStyledTextCtrl::CharRight() {
    SendMsg(2306, 0, 0);
}

// Move caret right one character extending selection to new caret position.
void wxStyledTextCtrl::CharRightExtend() {
    SendMsg(2307, 0, 0);
}

// Move caret left one word.
void wxStyledTextCtrl::WordLeft() {
    SendMsg(2308, 0, 0);
}

// Move caret left one word extending selection to new caret position.
void wxStyledTextCtrl::WordLeftExtend() {
    SendMsg(2309, 0, 0);
}

// Move caret right one word.
void wxStyledTextCtrl::WordRight() {
    SendMsg(2310, 0, 0);
}

// Move caret right one word extending selection to new caret position.
void wxStyledTextCtrl::WordRightExtend() {
    SendMsg(2311, 0, 0);
}

// Move caret to first position on line.
void wxStyledTextCtrl::Home() {
    SendMsg(2312, 0, 0);
}

// Move caret to first position on line extending selection to new caret position.
void wxStyledTextCtrl::HomeExtend() {
    SendMsg(2313, 0, 0);
}

// Move caret to last position on line.
void wxStyledTextCtrl::LineEnd() {
    SendMsg(2314, 0, 0);
}

// Move caret to last position on line extending selection to new caret position.
void wxStyledTextCtrl::LineEndExtend() {
    SendMsg(2315, 0, 0);
}

// Move caret to first position in document.
void wxStyledTextCtrl::DocumentStart() {
    SendMsg(2316, 0, 0);
}

// Move caret to first position in document extending selection to new caret position.
void wxStyledTextCtrl::DocumentStartExtend() {
    SendMsg(2317, 0, 0);
}

// Move caret to last position in document.
void wxStyledTextCtrl::DocumentEnd() {
    SendMsg(2318, 0, 0);
}

// Move caret to last position in document extending selection to new caret position.
void wxStyledTextCtrl::DocumentEndExtend() {
    SendMsg(2319, 0, 0);
}

// Move caret one page up.
void wxStyledTextCtrl::PageUp() {
    SendMsg(2320, 0, 0);
}

// Move caret one page up extending selection to new caret position.
void wxStyledTextCtrl::PageUpExtend() {
    SendMsg(2321, 0, 0);
}

// Move caret one page down.
void wxStyledTextCtrl::PageDown() {
    SendMsg(2322, 0, 0);
}

// Move caret one page down extending selection to new caret position.
void wxStyledTextCtrl::PageDownExtend() {
    SendMsg(2323, 0, 0);
}

// Switch from insert to overtype mode or the reverse.
void wxStyledTextCtrl::EditToggleOvertype() {
    SendMsg(2324, 0, 0);
}

// Cancel any modes such as call tip or auto-completion list display.
void wxStyledTextCtrl::Cancel() {
    SendMsg(2325, 0, 0);
}

// Delete the selection or if no selection, the character before the caret.
void wxStyledTextCtrl::DeleteBack() {
    SendMsg(2326, 0, 0);
}

// If selection is empty or all on one line replace the selection with a tab character.
// If more than one line selected, indent the lines.
void wxStyledTextCtrl::Tab() {
    SendMsg(2327, 0, 0);
}

// Dedent the selected lines.
void wxStyledTextCtrl::BackTab() {
    SendMsg(2328, 0, 0);
}

// Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
void wxStyledTextCtrl::NewLine() {
    SendMsg(2329, 0, 0);
}

// Insert a Form Feed character.
void wxStyledTextCtrl::FormFeed() {
    SendMsg(2330, 0, 0);
}

// Move caret to before first visible character on line.
// If already there move to first character on line.
void wxStyledTextCtrl::VCHome() {
    SendMsg(2331, 0, 0);
}

// Like VCHome but extending selection to new caret position.
void wxStyledTextCtrl::VCHomeExtend() {
    SendMsg(2332, 0, 0);
}

// Magnify the displayed text by increasing the sizes by 1 point.
void wxStyledTextCtrl::ZoomIn() {
    SendMsg(2333, 0, 0);
}

// Make the displayed text smaller by decreasing the sizes by 1 point.
void wxStyledTextCtrl::ZoomOut() {
    SendMsg(2334, 0, 0);
}

// Delete the word to the left of the caret.
void wxStyledTextCtrl::DelWordLeft() {
    SendMsg(2335, 0, 0);
}

// Delete the word to the right of the caret.
void wxStyledTextCtrl::DelWordRight() {
    SendMsg(2336, 0, 0);
}

// Cut the line containing the caret.
void wxStyledTextCtrl::LineCut() {
    SendMsg(2337, 0, 0);
}

// Delete the line containing the caret.
void wxStyledTextCtrl::LineDelete() {
    SendMsg(2338, 0, 0);
}

// Switch the current line with the previous.
void wxStyledTextCtrl::LineTranspose() {
    SendMsg(2339, 0, 0);
}

// Duplicate the current line.
void wxStyledTextCtrl::LineDuplicate() {
    SendMsg(2404, 0, 0);
}

// Transform the selection to lower case.
void wxStyledTextCtrl::LowerCase() {
    SendMsg(2340, 0, 0);
}

// Transform the selection to upper case.
void wxStyledTextCtrl::UpperCase() {
    SendMsg(2341, 0, 0);
}

// Scroll the document down, keeping the caret visible.
void wxStyledTextCtrl::LineScrollDown() {
    SendMsg(2342, 0, 0);
}

// Scroll the document up, keeping the caret visible.
void wxStyledTextCtrl::LineScrollUp() {
    SendMsg(2343, 0, 0);
}

// Delete the selection or if no selection, the character before the caret.
// Will not delete the character before at the start of a line.
void wxStyledTextCtrl::DeleteBackNotLine() {
    SendMsg(2344, 0, 0);
}

// Move caret to first position on display line.
void wxStyledTextCtrl::HomeDisplay() {
    SendMsg(2345, 0, 0);
}

// Move caret to first position on display line extending selection to
// new caret position.
void wxStyledTextCtrl::HomeDisplayExtend() {
    SendMsg(2346, 0, 0);
}

// Move caret to last position on display line.
void wxStyledTextCtrl::LineEndDisplay() {
    SendMsg(2347, 0, 0);
}

// Move caret to last position on display line extending selection to new
// caret position.
void wxStyledTextCtrl::LineEndDisplayExtend() {
    SendMsg(2348, 0, 0);
}

// These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
// except they behave differently when word-wrap is enabled:
// They go first to the start / end of the display line, like (Home|LineEnd)Display
// The difference is that, the cursor is already at the point, it goes on to the start
// or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
void wxStyledTextCtrl::HomeWrap() {
    SendMsg(2349, 0, 0);
}
void wxStyledTextCtrl::HomeWrapExtend() {
    SendMsg(2450, 0, 0);
}
void wxStyledTextCtrl::LineEndWrap() {
    SendMsg(2451, 0, 0);
}
void wxStyledTextCtrl::LineEndWrapExtend() {
    SendMsg(2452, 0, 0);
}
void wxStyledTextCtrl::VCHomeWrap() {
    SendMsg(2453, 0, 0);
}
void wxStyledTextCtrl::VCHomeWrapExtend() {
    SendMsg(2454, 0, 0);
}

// Copy the line containing the caret.
void wxStyledTextCtrl::LineCopy() {
    SendMsg(2455, 0, 0);
}

// Move the caret inside current view if it's not there already.
void wxStyledTextCtrl::MoveCaretInsideView() {
    SendMsg(2401, 0, 0);
}

// How many characters are on a line, not including end of line characters?
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

// Are the end of line characters visible?
bool wxStyledTextCtrl::GetViewEOL() {
    return SendMsg(2355, 0, 0) != 0;
}

// Make the end of line characters visible or invisible.
void wxStyledTextCtrl::SetViewEOL(bool visible) {
    SendMsg(2356, visible, 0);
}

// Retrieve a pointer to the document object.
void* wxStyledTextCtrl::GetDocPointer() {
         return (void*)SendMsg(2357);
}

// Change the document object used.
void wxStyledTextCtrl::SetDocPointer(void* docPointer) {
         SendMsg(2358, 0, (long)docPointer);
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
// Does not ensure the selection is visible.
int wxStyledTextCtrl::SearchNext(int flags, const wxString& text) {
    return SendMsg(2367, flags, (long)(const char*)wx2stc(text));
}

// Find some text starting at the search anchor and moving backwards.
// Does not ensure the selection is visible.
int wxStyledTextCtrl::SearchPrev(int flags, const wxString& text) {
    return SendMsg(2368, flags, (long)(const char*)wx2stc(text));
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

// Is the selection rectangular? The alternative is the more common stream selection.
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
         SendMsg(2376, 0, (long)docPointer);
}

// Release a reference to the document, deleting document if it fades to black.
void wxStyledTextCtrl::ReleaseDocument(void* docPointer) {
         SendMsg(2377, 0, (long)docPointer);
}

// Get which document modification events are sent to the container.
int wxStyledTextCtrl::GetModEventMask() {
    return SendMsg(2378, 0, 0);
}

// Change internal focus flag.
void wxStyledTextCtrl::SetSTCFocus(bool focus) {
    SendMsg(2380, focus, 0);
}

// Get internal focus flag.
bool wxStyledTextCtrl::GetSTCFocus() {
    return SendMsg(2381, 0, 0) != 0;
}

// Change error status - 0 = OK.
void wxStyledTextCtrl::SetStatus(int statusCode) {
    SendMsg(2382, statusCode, 0);
}

// Get error status.
int wxStyledTextCtrl::GetStatus() {
    return SendMsg(2383, 0, 0);
}

// Set whether the mouse is captured when its button is pressed.
void wxStyledTextCtrl::SetMouseDownCaptures(bool captures) {
    SendMsg(2384, captures, 0);
}

// Get whether mouse gets captured.
bool wxStyledTextCtrl::GetMouseDownCaptures() {
    return SendMsg(2385, 0, 0) != 0;
}

// Sets the cursor to one of the SC_CURSOR* values.
void wxStyledTextCtrl::SetSTCCursor(int cursorType) {
    SendMsg(2386, cursorType, 0);
}

// Get cursor type.
int wxStyledTextCtrl::GetSTCCursor() {
    return SendMsg(2387, 0, 0);
}

// Change the way control characters are displayed:
// If symbol is < 32, keep the drawn way, else, use the given character.
void wxStyledTextCtrl::SetControlCharSymbol(int symbol) {
    SendMsg(2388, symbol, 0);
}

// Get the way control characters are displayed.
int wxStyledTextCtrl::GetControlCharSymbol() {
    return SendMsg(2389, 0, 0);
}

// Move to the previous change in capitalisation.
void wxStyledTextCtrl::WordPartLeft() {
    SendMsg(2390, 0, 0);
}

// Move to the previous change in capitalisation extending selection
// to new caret position.
void wxStyledTextCtrl::WordPartLeftExtend() {
    SendMsg(2391, 0, 0);
}

// Move to the change next in capitalisation.
void wxStyledTextCtrl::WordPartRight() {
    SendMsg(2392, 0, 0);
}

// Move to the next change in capitalisation extending selection
// to new caret position.
void wxStyledTextCtrl::WordPartRightExtend() {
    SendMsg(2393, 0, 0);
}

// Set the way the display area is determined when a particular line
// is to be moved to by Find, FindNext, GotoLine, etc.
void wxStyledTextCtrl::SetVisiblePolicy(int visiblePolicy, int visibleSlop) {
    SendMsg(2394, visiblePolicy, visibleSlop);
}

// Delete back from the current position to the start of the line.
void wxStyledTextCtrl::DelLineLeft() {
    SendMsg(2395, 0, 0);
}

// Delete forwards from the current position to the end of the line.
void wxStyledTextCtrl::DelLineRight() {
    SendMsg(2396, 0, 0);
}

// Get and Set the xOffset (ie, horizonal scroll position).
void wxStyledTextCtrl::SetXOffset(int newOffset) {
    SendMsg(2397, newOffset, 0);
}
int wxStyledTextCtrl::GetXOffset() {
    return SendMsg(2398, 0, 0);
}

// Set the last x chosen value to be the caret x position.
void wxStyledTextCtrl::ChooseCaretX() {
    SendMsg(2399, 0, 0);
}

// Set the way the caret is kept visible when going sideway.
// The exclusion zone is given in pixels.
void wxStyledTextCtrl::SetXCaretPolicy(int caretPolicy, int caretSlop) {
    SendMsg(2402, caretPolicy, caretSlop);
}

// Set the way the line the caret is on is kept visible.
// The exclusion zone is given in lines.
void wxStyledTextCtrl::SetYCaretPolicy(int caretPolicy, int caretSlop) {
    SendMsg(2403, caretPolicy, caretSlop);
}

// Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
void wxStyledTextCtrl::SetPrintWrapMode(int mode) {
    SendMsg(2406, mode, 0);
}

// Is printing line wrapped?
int wxStyledTextCtrl::GetPrintWrapMode() {
    return SendMsg(2407, 0, 0);
}

// Set a fore colour for active hotspots.
void wxStyledTextCtrl::SetHotspotActiveForeground(bool useSetting, const wxColour& fore) {
    SendMsg(2410, useSetting, wxColourAsLong(fore));
}

// Set a back colour for active hotspots.
void wxStyledTextCtrl::SetHotspotActiveBackground(bool useSetting, const wxColour& back) {
    SendMsg(2411, useSetting, wxColourAsLong(back));
}

// Enable / Disable underlining active hotspots.
void wxStyledTextCtrl::SetHotspotActiveUnderline(bool underline) {
    SendMsg(2412, underline, 0);
}

// Limit hotspots to single line so hotspots on two lines don't merge.
void wxStyledTextCtrl::SetHotspotSingleLine(bool singleLine) {
    SendMsg(2421, singleLine, 0);
}

// Move caret between paragraphs (delimited by empty lines).
void wxStyledTextCtrl::ParaDown() {
    SendMsg(2413, 0, 0);
}
void wxStyledTextCtrl::ParaDownExtend() {
    SendMsg(2414, 0, 0);
}
void wxStyledTextCtrl::ParaUp() {
    SendMsg(2415, 0, 0);
}
void wxStyledTextCtrl::ParaUpExtend() {
    SendMsg(2416, 0, 0);
}

// Given a valid document position, return the previous position taking code
// page into account. Returns 0 if passed 0.
int wxStyledTextCtrl::PositionBefore(int pos) {
    return SendMsg(2417, pos, 0);
}

// Given a valid document position, return the next position taking code
// page into account. Maximum value returned is the last position in the document.
int wxStyledTextCtrl::PositionAfter(int pos) {
    return SendMsg(2418, pos, 0);
}

// Copy a range of text to the clipboard. Positions are clipped into the document.
void wxStyledTextCtrl::CopyRange(int start, int end) {
    SendMsg(2419, start, end);
}

// Copy argument text to the clipboard.
void wxStyledTextCtrl::CopyText(int length, const wxString& text) {
    SendMsg(2420, length, (long)(const char*)wx2stc(text));
}

// Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE) or
// by lines (SC_SEL_LINES).
void wxStyledTextCtrl::SetSelectionMode(int mode) {
    SendMsg(2422, mode, 0);
}

// Get the mode of the current selection.
int wxStyledTextCtrl::GetSelectionMode() {
    return SendMsg(2423, 0, 0);
}

// Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
int wxStyledTextCtrl::GetLineSelStartPosition(int line) {
    return SendMsg(2424, line, 0);
}

// Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
int wxStyledTextCtrl::GetLineSelEndPosition(int line) {
    return SendMsg(2425, line, 0);
}

// Move caret down one line, extending rectangular selection to new caret position.
void wxStyledTextCtrl::LineDownRectExtend() {
    SendMsg(2426, 0, 0);
}

// Move caret up one line, extending rectangular selection to new caret position.
void wxStyledTextCtrl::LineUpRectExtend() {
    SendMsg(2427, 0, 0);
}

// Move caret left one character, extending rectangular selection to new caret position.
void wxStyledTextCtrl::CharLeftRectExtend() {
    SendMsg(2428, 0, 0);
}

// Move caret right one character, extending rectangular selection to new caret position.
void wxStyledTextCtrl::CharRightRectExtend() {
    SendMsg(2429, 0, 0);
}

// Move caret to first position on line, extending rectangular selection to new caret position.
void wxStyledTextCtrl::HomeRectExtend() {
    SendMsg(2430, 0, 0);
}

// Move caret to before first visible character on line.
// If already there move to first character on line.
// In either case, extend rectangular selection to new caret position.
void wxStyledTextCtrl::VCHomeRectExtend() {
    SendMsg(2431, 0, 0);
}

// Move caret to last position on line, extending rectangular selection to new caret position.
void wxStyledTextCtrl::LineEndRectExtend() {
    SendMsg(2432, 0, 0);
}

// Move caret one page up, extending rectangular selection to new caret position.
void wxStyledTextCtrl::PageUpRectExtend() {
    SendMsg(2433, 0, 0);
}

// Move caret one page down, extending rectangular selection to new caret position.
void wxStyledTextCtrl::PageDownRectExtend() {
    SendMsg(2434, 0, 0);
}

// Move caret to top of page, or one page up if already at top of page.
void wxStyledTextCtrl::StutteredPageUp() {
    SendMsg(2435, 0, 0);
}

// Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
void wxStyledTextCtrl::StutteredPageUpExtend() {
    SendMsg(2436, 0, 0);
}

// Move caret to bottom of page, or one page down if already at bottom of page.
void wxStyledTextCtrl::StutteredPageDown() {
    SendMsg(2437, 0, 0);
}

// Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
void wxStyledTextCtrl::StutteredPageDownExtend() {
    SendMsg(2438, 0, 0);
}

// Move caret left one word, position cursor at end of word.
void wxStyledTextCtrl::WordLeftEnd() {
    SendMsg(2439, 0, 0);
}

// Move caret left one word, position cursor at end of word, extending selection to new caret position.
void wxStyledTextCtrl::WordLeftEndExtend() {
    SendMsg(2440, 0, 0);
}

// Move caret right one word, position cursor at end of word.
void wxStyledTextCtrl::WordRightEnd() {
    SendMsg(2441, 0, 0);
}

// Move caret right one word, position cursor at end of word, extending selection to new caret position.
void wxStyledTextCtrl::WordRightEndExtend() {
    SendMsg(2442, 0, 0);
}

// Set the set of characters making up whitespace for when moving or selecting by word.
// Should be called after SetWordChars.
void wxStyledTextCtrl::SetWhitespaceChars(const wxString& characters) {
    SendMsg(2443, 0, (long)(const char*)wx2stc(characters));
}

// Reset the set of characters for whitespace and word characters to the defaults.
void wxStyledTextCtrl::SetCharsDefault() {
    SendMsg(2444, 0, 0);
}

// Get currently selected item position in the auto-completion list
int wxStyledTextCtrl::AutoCompGetCurrent() {
    return SendMsg(2445, 0, 0);
}

// Enlarge the document to a particular size of text bytes.
void wxStyledTextCtrl::Allocate(int bytes) {
    SendMsg(2446, bytes, 0);
}

// Find the position of a column on a line taking into account tabs and
// multi-byte characters. If beyond end of line, return line end position.
int wxStyledTextCtrl::FindColumn(int line, int column) {
    return SendMsg(2456, line, column);
}

// Can the caret preferred x position only be changed by explicit movement commands?
bool wxStyledTextCtrl::GetCaretSticky() {
    return SendMsg(2457, 0, 0) != 0;
}

// Stop the caret preferred x position changing when the user types.
void wxStyledTextCtrl::SetCaretSticky(bool useCaretStickyBehaviour) {
    SendMsg(2458, useCaretStickyBehaviour, 0);
}

// Switch between sticky and non-sticky: meant to be bound to a key.
void wxStyledTextCtrl::ToggleCaretSticky() {
    SendMsg(2459, 0, 0);
}

// Enable/Disable convert-on-paste for line endings
void wxStyledTextCtrl::SetPasteConvertEndings(bool convert) {
    SendMsg(2467, convert, 0);
}

// Get convert-on-paste setting
bool wxStyledTextCtrl::GetPasteConvertEndings() {
    return SendMsg(2468, 0, 0) != 0;
}

// Duplicate the selection. If selection empty duplicate the line containing the caret.
void wxStyledTextCtrl::SelectionDuplicate() {
    SendMsg(2469, 0, 0);
}

// Set background alpha of the caret line.
void wxStyledTextCtrl::SetCaretLineBackAlpha(int alpha) {
    SendMsg(2470, alpha, 0);
}

// Get the background alpha of the caret line.
int wxStyledTextCtrl::GetCaretLineBackAlpha() {
    return SendMsg(2471, 0, 0);
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
    SendMsg(4004, (long)(const char*)wx2stc(key), (long)(const char*)wx2stc(value));
}

// Set up the key words used by the lexer.
void wxStyledTextCtrl::SetKeyWords(int keywordSet, const wxString& keyWords) {
    SendMsg(4005, keywordSet, (long)(const char*)wx2stc(keyWords));
}

// Set the lexing language of the document based on string name.
void wxStyledTextCtrl::SetLexerLanguage(const wxString& language) {
    SendMsg(4006, 0, (long)(const char*)wx2stc(language));
}

// Retrieve a 'property' value previously set with SetProperty.
wxString wxStyledTextCtrl::GetProperty(const wxString& key) {
         int len = SendMsg(SCI_GETPROPERTY, (long)(const char*)wx2stc(key), (long)NULL);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(4008, (long)(const char*)wx2stc(key), (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
}

// Retrieve a 'property' value previously set with SetProperty,
// with '$()' variable replacement on returned buffer.
wxString wxStyledTextCtrl::GetPropertyExpanded(const wxString& key) {
         int len = SendMsg(SCI_GETPROPERTYEXPANDED, (long)(const char*)wx2stc(key), (long)NULL);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(4009, (long)(const char*)wx2stc(key), (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);
}

// Retrieve a 'property' value previously set with SetProperty,
// interpreted as an int AFTER any '$()' variable replacement.
int wxStyledTextCtrl::GetPropertyInt(const wxString& key) {
    return SendMsg(4010, (long)(const char*)wx2stc(key), 0);
}

// Retrieve the number of bits the current lexer needs for styling.
int wxStyledTextCtrl::GetStyleBitsNeeded() {
    return SendMsg(4011, 0, 0);
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
//      fore:[name or #RRGGBB]  sets the foreground colour
//      back:[name or #RRGGBB]  sets the background colour
//      face:[facename]         sets the font face name to use
//      size:[num]              sets the font size in points
//      eol                     turns on eol filling
//      underline               turns on underlining
//
void wxStyledTextCtrl::StyleSetSpec(int styleNum, const wxString& spec) {

    wxStringTokenizer tkz(spec, wxT(","));
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();

        wxString option = token.BeforeFirst(':');
        wxString val = token.AfterFirst(':');

        if (option == wxT("bold"))
            StyleSetBold(styleNum, true);

        else if (option == wxT("italic"))
            StyleSetItalic(styleNum, true);

        else if (option == wxT("underline"))
            StyleSetUnderline(styleNum, true);

        else if (option == wxT("eol"))
            StyleSetEOLFilled(styleNum, true);

        else if (option == wxT("size")) {
            long points;
            if (val.ToLong(&points))
                StyleSetSize(styleNum, points);
        }

        else if (option == wxT("face"))
            StyleSetFaceName(styleNum, val);

        else if (option == wxT("fore"))
            StyleSetForeground(styleNum, wxColourFromSpec(val));

        else if (option == wxT("back"))
            StyleSetBackground(styleNum, wxColourFromSpec(val));
    }
}


// Set style size, face, bold, italic, and underline attributes from
// a wxFont's attributes.
void wxStyledTextCtrl::StyleSetFont(int styleNum, wxFont& font) {
#ifdef __WXGTK__
    // Ensure that the native font is initialized
    int x, y;
    GetTextExtent(wxT("X"), &x, &y, NULL, NULL, &font);
#endif
    int            size     = font.GetPointSize();
    wxString       faceName = font.GetFaceName();
    bool           bold     = font.GetWeight() == wxBOLD;
    bool           italic   = font.GetStyle() != wxNORMAL;
    bool           under    = font.GetUnderlined();
    wxFontEncoding encoding = font.GetEncoding();
    
    StyleSetFontAttr(styleNum, size, faceName, bold, italic, under, encoding);
}

// Set all font style attributes at once.
void wxStyledTextCtrl::StyleSetFontAttr(int styleNum, int size,
                                        const wxString& faceName,
                                        bool bold, bool italic,
                                        bool underline,
                                        wxFontEncoding encoding) {
    StyleSetSize(styleNum, size);
    StyleSetFaceName(styleNum, faceName);
    StyleSetBold(styleNum, bold);
    StyleSetItalic(styleNum, italic);
    StyleSetUnderline(styleNum, underline);
    StyleSetFontEncoding(styleNum, encoding);
}


// Set the character set of the font in a style.  Converts the Scintilla
// character set values to a wxFontEncoding.
void wxStyledTextCtrl::StyleSetCharacterSet(int style, int characterSet)
{
    wxFontEncoding encoding;

    // Translate the Scintilla characterSet to a wxFontEncoding
    switch (characterSet) {
        default:
        case wxSTC_CHARSET_ANSI:
        case wxSTC_CHARSET_DEFAULT:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_BALTIC:
            encoding = wxFONTENCODING_ISO8859_13;
            break;

        case wxSTC_CHARSET_CHINESEBIG5:
            encoding = wxFONTENCODING_CP950;
            break;

        case wxSTC_CHARSET_EASTEUROPE:
            encoding = wxFONTENCODING_ISO8859_2;
            break;

        case wxSTC_CHARSET_GB2312:
            encoding = wxFONTENCODING_CP936;
            break;

        case wxSTC_CHARSET_GREEK:
            encoding = wxFONTENCODING_ISO8859_7;
            break;

        case wxSTC_CHARSET_HANGUL:
            encoding = wxFONTENCODING_CP949;
            break;

        case wxSTC_CHARSET_MAC:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_OEM:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_RUSSIAN:
            encoding = wxFONTENCODING_KOI8;
            break;

        case wxSTC_CHARSET_SHIFTJIS:
            encoding = wxFONTENCODING_CP932;
            break;

        case wxSTC_CHARSET_SYMBOL:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_TURKISH:
            encoding = wxFONTENCODING_ISO8859_9;
            break;

        case wxSTC_CHARSET_JOHAB:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_HEBREW:
            encoding = wxFONTENCODING_ISO8859_8;
            break;

        case wxSTC_CHARSET_ARABIC:
            encoding = wxFONTENCODING_ISO8859_6;
            break;

        case wxSTC_CHARSET_VIETNAMESE:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_THAI:
            encoding = wxFONTENCODING_ISO8859_11;
            break;

        case wxSTC_CHARSET_CYRILLIC:
            encoding = wxFONTENCODING_ISO8859_5;
            break;
                
        case wxSTC_CHARSET_8859_15:
            encoding = wxFONTENCODING_ISO8859_15;;
            break;
    }

    // We just have Scintilla track the wxFontEncoding for us.  It gets used
    // in Font::Create in PlatWX.cpp.  We add one to the value so that the
    // effective wxFONENCODING_DEFAULT == SC_SHARSET_DEFAULT and so when
    // Scintilla internally uses SC_CHARSET_DEFAULT we will translate it back
    // to wxFONENCODING_DEFAULT in Font::Create.
    SendMsg(SCI_STYLESETCHARACTERSET, style, encoding+1);
}


// Set the font encoding to be used by a style.
void wxStyledTextCtrl::StyleSetFontEncoding(int style, wxFontEncoding encoding)
{
    SendMsg(SCI_STYLESETCHARACTERSET, style, encoding+1);
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


bool wxStyledTextCtrl::SaveFile(const wxString& filename)
{
    wxFile file(filename, wxFile::write);

    if (!file.IsOpened())
        return false;

    bool success = file.Write(GetText(), *wxConvCurrent);

    if (success)
        SetSavePoint();

    return success;
}

bool wxStyledTextCtrl::LoadFile(const wxString& filename)
{
    bool success = false;
    wxFile file(filename, wxFile::read);

    if (file.IsOpened())
    {
        wxString contents;
        // get the file size (assume it is not huge file...)
        ssize_t len = (ssize_t)file.Length();

        if (len > 0)
        {
#if wxUSE_UNICODE
            wxMemoryBuffer buffer(len+1);
            success = (file.Read(buffer.GetData(), len) == len);
            if (success) {
                ((char*)buffer.GetData())[len] = 0;
                contents = wxString(buffer, *wxConvCurrent, len);
            }
#else
            wxString buffer;
            success = (file.Read(wxStringBuffer(buffer, len), len) == len);
            contents = buffer;
#endif
        }
        else
        {
            if (len == 0)
                success = true;  // empty file is ok
            else
                success = false; // len == wxInvalidOffset
        }

        if (success)
        {
            SetText(contents);
            EmptyUndoBuffer();
            SetSavePoint();
        }
    }

    return success;
}


#if wxUSE_DRAG_AND_DROP
wxDragResult wxStyledTextCtrl::DoDragOver(wxCoord x, wxCoord y, wxDragResult def) {
        return m_swx->DoDragOver(x, y, def);
}


bool wxStyledTextCtrl::DoDropText(long x, long y, const wxString& data) {
    return m_swx->DoDropText(x, y, data);
}
#endif


void wxStyledTextCtrl::SetUseAntiAliasing(bool useAA) {
    m_swx->SetUseAntiAliasing(useAA);
}

bool wxStyledTextCtrl::GetUseAntiAliasing() {
    return m_swx->GetUseAntiAliasing();
}





void wxStyledTextCtrl::AddTextRaw(const char* text)
{
    SendMsg(SCI_ADDTEXT, strlen(text), (long)text);
}

void wxStyledTextCtrl::InsertTextRaw(int pos, const char* text)
{
    SendMsg(SCI_INSERTTEXT, pos, (long)text);
}

wxCharBuffer wxStyledTextCtrl::GetCurLineRaw(int* linePos)
{
    int len = LineLength(GetCurrentLine());
    if (!len) {
        if (linePos)  *linePos = 0;
        wxCharBuffer empty;
        return empty;
    }

    wxCharBuffer buf(len);
    int pos = SendMsg(SCI_GETCURLINE, len, (long)buf.data());
    if (linePos)  *linePos = pos;
    return buf;
}

wxCharBuffer wxStyledTextCtrl::GetLineRaw(int line)
{
    int len = LineLength(line);
    if (!len) {
        wxCharBuffer empty;
        return empty;
    }

    wxCharBuffer buf(len);
    SendMsg(SCI_GETLINE, line, (long)buf.data());
    return buf;
}

wxCharBuffer wxStyledTextCtrl::GetSelectedTextRaw()
{
    int   start;
    int   end;

    GetSelection(&start, &end);
    int   len  = end - start;
    if (!len) {
        wxCharBuffer empty;
        return empty;
    }        

    wxCharBuffer buf(len);
    SendMsg(SCI_GETSELTEXT, 0, (long)buf.data());
    return buf;
}

wxCharBuffer wxStyledTextCtrl::GetTextRangeRaw(int startPos, int endPos)
{
    if (endPos < startPos) {
        int temp = startPos;
        startPos = endPos;
        endPos = temp;
    }
    int len  = endPos - startPos;
    if (!len) {
        wxCharBuffer empty;
        return empty;
    }        

    wxCharBuffer buf(len);
    TextRange tr;
    tr.lpstrText = buf.data();
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    SendMsg(SCI_GETTEXTRANGE, 0, (long)&tr);
    return buf;
}

void wxStyledTextCtrl::SetTextRaw(const char* text)
{
    SendMsg(SCI_SETTEXT, 0, (long)text);
}

wxCharBuffer wxStyledTextCtrl::GetTextRaw()
{
    int len  = GetTextLength();
    wxCharBuffer buf(len);
    SendMsg(SCI_GETTEXT, len, (long)buf.data());
    return buf;
}

void wxStyledTextCtrl::AppendTextRaw(const char* text)
{
    SendMsg(SCI_APPENDTEXT, strlen(text), (long)text);
}





//----------------------------------------------------------------------
// Event handlers

void wxStyledTextCtrl::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
    wxPaintDC dc(this);
    m_swx->DoPaint(&dc, GetUpdateRegion().GetBox());
}

void wxStyledTextCtrl::OnScrollWin(wxScrollWinEvent& evt) {
    if (evt.GetOrientation() == wxHORIZONTAL)
        m_swx->DoHScroll(evt.GetEventType(), evt.GetPosition());
    else
        m_swx->DoVScroll(evt.GetEventType(), evt.GetPosition());
}

void wxStyledTextCtrl::OnScroll(wxScrollEvent& evt) {
    wxScrollBar* sb = wxDynamicCast(evt.GetEventObject(), wxScrollBar);
    if (sb) {
        if (sb->IsVertical())
            m_swx->DoVScroll(evt.GetEventType(), evt.GetPosition());
        else
            m_swx->DoHScroll(evt.GetEventType(), evt.GetPosition());
    }
}

void wxStyledTextCtrl::OnSize(wxSizeEvent& WXUNUSED(evt)) {
    if (m_swx) {
        wxSize sz = GetClientSize();
        m_swx->DoSize(sz.x, sz.y);
    }
}

void wxStyledTextCtrl::OnMouseLeftDown(wxMouseEvent& evt) {
    SetFocus();
    wxPoint pt = evt.GetPosition();
    m_swx->DoLeftButtonDown(Point(pt.x, pt.y), m_stopWatch.Time(),
                      evt.ShiftDown(), evt.ControlDown(), evt.AltDown());
}

void wxStyledTextCtrl::OnMouseMove(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoLeftButtonMove(Point(pt.x, pt.y));
}

void wxStyledTextCtrl::OnMouseLeftUp(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoLeftButtonUp(Point(pt.x, pt.y), m_stopWatch.Time(),
                      evt.ControlDown());
}


void wxStyledTextCtrl::OnMouseRightUp(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoContextMenu(Point(pt.x, pt.y));
}


void wxStyledTextCtrl::OnMouseMiddleUp(wxMouseEvent& evt) {
    wxPoint pt = evt.GetPosition();
    m_swx->DoMiddleButtonUp(Point(pt.x, pt.y));
}

void wxStyledTextCtrl::OnContextMenu(wxContextMenuEvent& evt) {
    wxPoint pt = evt.GetPosition();
    ScreenToClient(&pt.x, &pt.y);
    /*
      Show context menu at event point if it's within the window,
      or at caret location if not
    */
    wxHitTest ht = this->HitTest(pt);
    if (ht != wxHT_WINDOW_INSIDE) {
        pt = this->PointFromPosition(this->GetCurrentPos());
    }
    m_swx->DoContextMenu(Point(pt.x, pt.y));
}


void wxStyledTextCtrl::OnMouseWheel(wxMouseEvent& evt) {
    m_swx->DoMouseWheel(evt.GetWheelRotation(),
                        evt.GetWheelDelta(),
                        evt.GetLinesPerAction(),
                        evt.ControlDown(),
                        evt.IsPageScroll());
}


void wxStyledTextCtrl::OnChar(wxKeyEvent& evt) {
    // On (some?) non-US PC keyboards the AltGr key is required to enter some
    // common characters.  It comes to us as both Alt and Ctrl down so we need
    // to let the char through in that case, otherwise if only ctrl or only
    // alt let's skip it.
    bool ctrl = evt.ControlDown();
#ifdef __WXMAC__
    // On the Mac the Alt key is just a modifier key (like Shift) so we need
    // to allow the char events to be processed when Alt is pressed.
    // TODO:  Should we check MetaDown instead in this case?
    bool alt = false;
#else
    bool alt  = evt.AltDown();
#endif
    bool skip = ((ctrl || alt) && ! (ctrl && alt));

    if (!m_lastKeyDownConsumed && !skip) {
#if wxUSE_UNICODE
        int key = evt.GetUnicodeKey();
        bool keyOk = true;

        // if the unicode key code is not really a unicode character (it may
        // be a function key or etc., the platforms appear to always give us a
        // small value in this case) then fallback to the ascii key code but
        // don't do anything for function keys or etc.
        if (key <= 127) {
            key = evt.GetKeyCode();
            keyOk = (key <= 127);
        }
        if (keyOk) {
            m_swx->DoAddChar(key);
            return;
        }
#else
        int key = evt.GetKeyCode();
        if (key <= WXK_START || key > WXK_COMMAND) {
            m_swx->DoAddChar(key);
            return;
        }
#endif
    }
    
    evt.Skip();
}


void wxStyledTextCtrl::OnKeyDown(wxKeyEvent& evt) {
    int processed = m_swx->DoKeyDown(evt, &m_lastKeyDownConsumed);
    if (!processed && !m_lastKeyDownConsumed)
        evt.Skip();
}


void wxStyledTextCtrl::OnLoseFocus(wxFocusEvent& evt) {
    m_swx->DoLoseFocus();
    evt.Skip();
}


void wxStyledTextCtrl::OnGainFocus(wxFocusEvent& evt) {
    m_swx->DoGainFocus();
    evt.Skip();
}


void wxStyledTextCtrl::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(evt)) {
    m_swx->DoSysColourChange();
}


void wxStyledTextCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(evt)) {
    // do nothing to help avoid flashing
}



void wxStyledTextCtrl::OnMenu(wxCommandEvent& evt) {
    m_swx->DoCommand(evt.GetId());
}


void wxStyledTextCtrl::OnListBox(wxCommandEvent& WXUNUSED(evt)) {
    m_swx->DoOnListBox();
}


void wxStyledTextCtrl::OnIdle(wxIdleEvent& evt) {
    m_swx->DoOnIdle(evt);
}


wxSize wxStyledTextCtrl::DoGetBestSize() const
{
    // What would be the best size for a wxSTC?
    // Just give a reasonable minimum until something else can be figured out.
    return wxSize(200,100);
}


//----------------------------------------------------------------------
// Turn notifications from Scintilla into events


void wxStyledTextCtrl::NotifyChange() {
    wxStyledTextEvent evt(wxEVT_STC_CHANGE, GetId());
    evt.SetEventObject(this);
    GetEventHandler()->ProcessEvent(evt);
}


static void SetEventText(wxStyledTextEvent& evt, const char* text,
                         size_t length) {
    if(!text) return;

    evt.SetText(stc2wx(text, length));
}


void wxStyledTextCtrl::NotifyParent(SCNotification* _scn) {
    SCNotification& scn = *_scn;
    wxStyledTextEvent evt(0, GetId());

    evt.SetEventObject(this);
    evt.SetPosition(scn.position);
    evt.SetKey(scn.ch);
    evt.SetModifiers(scn.modifiers);

    switch (scn.nmhdr.code) {
    case SCN_STYLENEEDED:
        evt.SetEventType(wxEVT_STC_STYLENEEDED);
        break;

    case SCN_CHARADDED:
        evt.SetEventType(wxEVT_STC_CHARADDED);
        break;

    case SCN_SAVEPOINTREACHED:
        evt.SetEventType(wxEVT_STC_SAVEPOINTREACHED);
        break;

    case SCN_SAVEPOINTLEFT:
        evt.SetEventType(wxEVT_STC_SAVEPOINTLEFT);
        break;

    case SCN_MODIFYATTEMPTRO:
        evt.SetEventType(wxEVT_STC_ROMODIFYATTEMPT);
        break;

    case SCN_KEY:
        evt.SetEventType(wxEVT_STC_KEY);
        break;

    case SCN_DOUBLECLICK:
        evt.SetEventType(wxEVT_STC_DOUBLECLICK);
        break;

    case SCN_UPDATEUI:
        evt.SetEventType(wxEVT_STC_UPDATEUI);
        break;

    case SCN_MODIFIED:
        evt.SetEventType(wxEVT_STC_MODIFIED);
        evt.SetModificationType(scn.modificationType);
        SetEventText(evt, scn.text, scn.length);
        evt.SetLength(scn.length);
        evt.SetLinesAdded(scn.linesAdded);
        evt.SetLine(scn.line);
        evt.SetFoldLevelNow(scn.foldLevelNow);
        evt.SetFoldLevelPrev(scn.foldLevelPrev);
        break;

    case SCN_MACRORECORD:
        evt.SetEventType(wxEVT_STC_MACRORECORD);
        evt.SetMessage(scn.message);
        evt.SetWParam(scn.wParam);
        evt.SetLParam(scn.lParam);
        break;

    case SCN_MARGINCLICK:
        evt.SetEventType(wxEVT_STC_MARGINCLICK);
        evt.SetMargin(scn.margin);
        break;

    case SCN_NEEDSHOWN:
        evt.SetEventType(wxEVT_STC_NEEDSHOWN);
        evt.SetLength(scn.length);
        break;

    case SCN_PAINTED:
        evt.SetEventType(wxEVT_STC_PAINTED);
        break;

    case SCN_USERLISTSELECTION:
        evt.SetEventType(wxEVT_STC_USERLISTSELECTION);
        evt.SetListType(scn.listType);
        SetEventText(evt, scn.text, strlen(scn.text));
        break;

    case SCN_URIDROPPED:
        evt.SetEventType(wxEVT_STC_URIDROPPED);
        SetEventText(evt, scn.text, strlen(scn.text));
        break;

    case SCN_DWELLSTART:
        evt.SetEventType(wxEVT_STC_DWELLSTART);
        evt.SetX(scn.x);
        evt.SetY(scn.y);
        break;

    case SCN_DWELLEND:
        evt.SetEventType(wxEVT_STC_DWELLEND);
        evt.SetX(scn.x);
        evt.SetY(scn.y);
        break;

    case SCN_ZOOM:
        evt.SetEventType(wxEVT_STC_ZOOM);
        break;

    case SCN_HOTSPOTCLICK:
        evt.SetEventType(wxEVT_STC_HOTSPOT_CLICK);
        break;

    case SCN_HOTSPOTDOUBLECLICK:
        evt.SetEventType(wxEVT_STC_HOTSPOT_DCLICK);
        break;

    case SCN_CALLTIPCLICK:
        evt.SetEventType(wxEVT_STC_CALLTIP_CLICK);
        break;

    case SCN_AUTOCSELECTION:
        evt.SetEventType(wxEVT_STC_AUTOCOMP_SELECTION);
        break;
        
    default:
        return;
    }

    GetEventHandler()->ProcessEvent(evt);
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
    m_listType = 0;
    m_x = 0;
    m_y = 0;
    m_dragAllowMove = false;
#if wxUSE_DRAG_AND_DROP
    m_dragResult = wxDragNone;
#endif
}

bool wxStyledTextEvent::GetShift() const { return (m_modifiers & SCI_SHIFT) != 0; }
bool wxStyledTextEvent::GetControl() const { return (m_modifiers & SCI_CTRL) != 0; }
bool wxStyledTextEvent::GetAlt() const { return (m_modifiers & SCI_ALT) != 0; }


wxStyledTextEvent::wxStyledTextEvent(const wxStyledTextEvent& event):
  wxCommandEvent(event)
{
    m_position =      event.m_position;
    m_key =           event.m_key;
    m_modifiers =     event.m_modifiers;
    m_modificationType = event.m_modificationType;
    m_text =          event.m_text;
    m_length =        event.m_length;
    m_linesAdded =    event.m_linesAdded;
    m_line =          event.m_line;
    m_foldLevelNow =  event.m_foldLevelNow;
    m_foldLevelPrev = event.m_foldLevelPrev;

    m_margin =        event.m_margin;

    m_message =       event.m_message;
    m_wParam =        event.m_wParam;
    m_lParam =        event.m_lParam;

    m_listType =     event.m_listType;
    m_x =            event.m_x;
    m_y =            event.m_y;

    m_dragText =     event.m_dragText;
    m_dragAllowMove =event.m_dragAllowMove;
#if wxUSE_DRAG_AND_DROP
    m_dragResult =   event.m_dragResult;
#endif
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------









