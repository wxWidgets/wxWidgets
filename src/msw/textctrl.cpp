/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/textctrl.h"
#include "wx/settings.h"
#include "wx/brush.h"
#include "wx/utils.h"
#include "wx/log.h"
#endif

#if wxUSE_CLIPBOARD
#include "wx/app.h"
#include "wx/clipbrd.h"
#endif

#include "wx/msw/private.h"

#include <windows.h>
#include <stdlib.h>

#if wxUSE_IOSTREAMH
#   include <fstream.h>
#else
#   include <fstream>
#endif

#include <sys/types.h>
#ifndef __MWERKS__
#include <sys/stat.h>
#else
#include <stat.h>
#endif
#if defined(__BORLANDC__) && !defined(__WIN32__)
#include <alloc.h>
#else
#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
#include <malloc.h>
#endif
#define farmalloc malloc
#define farfree free
#endif
#include <windowsx.h>

#include <string.h>

#if wxUSE_RICHEDIT && !defined(__GNUWIN32__)
    #include <richedit.h>
#endif

#if !USE_SHARED_LIBRARY

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
  EVT_CHAR(wxTextCtrl::OnChar)
  EVT_DROP_FILES(wxTextCtrl::OnDropFiles)

  EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
  EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
  EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
  EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
  EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

  EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
  EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
  EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
  EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
  EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()

#endif // USE_SHARED_LIBRARY

// Text item
wxTextCtrl::wxTextCtrl()
#ifndef NO_TEXT_WINDOW_STREAM
          : streambuf()
#endif
{
#if wxUSE_RICHEDIT
  m_isRich = FALSE;
#endif
}

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
  SetName(name);
  SetValidator(validator);
  if (parent) parent->AddChild(this);

  m_windowStyle = style;

  // Should this be taken from the system colours?
//  SetBackgroundColour(wxColour(255, 255, 255));

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));

  SetForegroundColour(parent->GetForegroundColour()) ;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
  m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msStyle = ES_LEFT | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
  if (m_windowStyle & wxTE_MULTILINE)
  {
    wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                  _T("wxTE_PROCESS_ENTER style is ignored for multiline controls") );

    msStyle |= ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL ; // WS_BORDER
    m_windowStyle |= wxTE_PROCESS_ENTER;
  }
  else
    msStyle |= ES_AUTOHSCROLL ;

  if (m_windowStyle & wxTE_READONLY)
    msStyle |= ES_READONLY;

  if (m_windowStyle & wxHSCROLL)
    msStyle |= (WS_HSCROLL | ES_AUTOHSCROLL) ;
  if (m_windowStyle & wxTE_PASSWORD) // hidden input
    msStyle |= ES_PASSWORD;

    // we always want the characters and the arrows
    m_lDlgCode = DLGC_WANTCHARS | DLGC_WANTARROWS;

    // we may have several different cases:
    // 1. normal case: both TAB and ENTER are used for dialog navigation
    // 2. ctrl which wants TAB for itself: ENTER is used to pass to the next
    //    control in the dialog
    // 3. ctrl which wants ENTER for itself: TAB is used for dialog navigation
    // 4. ctrl which wants both TAB and ENTER: Ctrl-ENTER is used to pass to
    //    the next control
    if ( m_windowStyle & wxTE_PROCESS_ENTER )
        m_lDlgCode |= DLGC_WANTMESSAGE;
    if ( m_windowStyle & wxTE_PROCESS_TAB )
        m_lDlgCode |= DLGC_WANTTAB;

  const wxChar *windowClass = _T("EDIT");

#if wxUSE_RICHEDIT
  if ( m_windowStyle & wxTE_MULTILINE )
  {
    msStyle |= ES_AUTOVSCROLL;
    m_isRich = TRUE;
    windowClass = _T("RichEdit") ;
  }
  else
    m_isRich = FALSE;
#endif

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // If we're in Win95, and we want a simple 2D border,
  // then make it an EDIT control instead.
#if wxUSE_RICHEDIT
  if (m_windowStyle & wxSIMPLE_BORDER)
  {
    windowClass = _T("EDIT");
    m_isRich = FALSE;
  }
#endif

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;

  m_hWnd = (WXHWND)::CreateWindowEx(exStyle, windowClass, NULL,
                        msStyle,
                        0, 0, 0, 0, (HWND) ((wxWindow*)parent)->GetHWND(), (HMENU)m_windowId,
                        wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, _T("Failed to create text ctrl") );

#if wxUSE_CTL3D
  if ( want3D )
  {
    Ctl3dSubclassCtl((HWND)m_hWnd);
    m_useCtl3D = TRUE;
  }
#endif

#if wxUSE_RICHEDIT
  if (m_isRich)
  {
    // Have to enable events
    ::SendMessage((HWND)m_hWnd, EM_SETEVENTMASK, 0,
                  ENM_CHANGE | ENM_DROPFILES | ENM_SELCHANGE | ENM_UPDATE);
  }
#endif

  SubclassWin(GetHWND());

  if ( parent->GetFont().Ok() && parent->GetFont().Ok() )
  {
    SetFont(parent->GetFont());
  }
  else
  {
    SetFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
  }

  SetSize(x, y, width, height);

  // Causes a crash for Symantec C++ and WIN32 for some reason
#if !(defined(__SC__) && defined(__WIN32__))
  if ( !value.IsEmpty() )
  {
    SetValue(value);
  }
#endif

  return TRUE;
}

// Make sure the window style (etc.) reflects the HWND style (roughly)
void wxTextCtrl::AdoptAttributesFromHWND()
{
  wxWindow::AdoptAttributesFromHWND();

  HWND hWnd = GetHwnd();
  long style = GetWindowLong((HWND) hWnd, GWL_STYLE);

  // retrieve the style to see whether this is an edit or richedit ctrl
#if wxUSE_RICHEDIT
  wxChar buf[256];

#ifndef __WIN32__
  GetClassName((HWND) hWnd, buf, 256);
#else
#ifdef UNICODE
  GetClassNameW((HWND) hWnd, buf, 256);
#else
#ifdef __TWIN32__
  GetClassName((HWND) hWnd, buf, 256);
#else
  GetClassNameA((HWND) hWnd, buf, 256);
#endif
#endif
#endif

  wxString str(buf);
  str.UpperCase();

  if (str == _T("EDIT"))
    m_isRich = FALSE;
  else
    m_isRich = TRUE;
#endif

  if (style & ES_MULTILINE)
    m_windowStyle |= wxTE_MULTILINE;
  if (style & ES_PASSWORD)
    m_windowStyle |= wxTE_PASSWORD;
  if (style & ES_READONLY)
    m_windowStyle |= wxTE_READONLY;
  if (style & ES_WANTRETURN)
    m_windowStyle |= wxTE_PROCESS_ENTER;
}

void wxTextCtrl::SetupColours()
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
  SetForegroundColour(GetParent()->GetForegroundColour());
}

wxString wxTextCtrl::GetValue() const
{
    return wxGetWindowText(GetHWND());
}

void wxTextCtrl::SetValue(const wxString& value)
{
  // If newlines are denoted by just 10, must stick 13 in front.
  int singletons = 0;
  int len = value.Length();
  int i;
  for (i = 0; i < len; i ++)
  {
    if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      singletons ++;
  }
  if (singletons > 0)
  {
    wxChar *tmp = new wxChar[len + singletons + 1];
    int j = 0;
    for (i = 0; i < len; i ++)
    {
      if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      {
        tmp[j] = 13;
        j ++;
      }
      tmp[j] = value[i];
      j ++;
    }
    tmp[j] = 0;
    SetWindowText(GetHwnd(), tmp);
    delete[] tmp;
  }
  else
    SetWindowText(GetHwnd(), (const wxChar *)value);

  AdjustSpaceLimit();
}

void wxTextCtrl::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  AdjustForParentClientOrigin(x1, y1, sizeFlags);

  int cx; // button font dimensions
  int cy;

  wxGetCharSize(GetHWND(), &cx, &cy, & this->GetFont());

  int control_width, control_height, control_x, control_y;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&w1, &h1);
  }

  // Deal with default size (using -1 values)
  if (w1<=0)
    w1 = DEFAULT_ITEM_WIDTH;

  control_x = x1;
  control_y = y1;
  control_width = w1;
  control_height = h1;

  // Calculations may have made text size too small
  if (control_height <= 0)
    control_height = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

  if (control_width <= 0)
    control_width = DEFAULT_ITEM_WIDTH;

  MoveWindow(GetHwnd(), (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_COPY, 0, 0L);
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_CUT, 0, 0L);
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_PASTE, 0, 0L);
    }
}

void wxTextCtrl::SetEditable(bool editable)
{
  HWND hWnd = GetHwnd();
  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
  HWND hWnd = GetHwnd();
#ifdef __WIN32__
#if wxUSE_RICHEDIT
  if ( m_isRich)
  {
    CHARRANGE range;
    range.cpMin = pos;
    range.cpMax = pos;
    SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM) &range);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
  }
  else
#endif
  {
    SendMessage(hWnd, EM_SETSEL, pos, pos);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
  }
#else
  SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif
  char *nothing = "";
  SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
}

void wxTextCtrl::SetInsertionPointEnd()
{
  long pos = GetLastPosition();
  SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
#if wxUSE_RICHEDIT
  if (m_isRich)
  {
    CHARRANGE range;
    range.cpMin = 0;
    range.cpMax = 0;
    SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) &range);
    return range.cpMin;
  }
#endif

  DWORD Pos=(DWORD)SendMessage(GetHwnd(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
}

long wxTextCtrl::GetLastPosition() const
{
    HWND hWnd = GetHwnd();

    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);

    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *) (const wxChar *)value, 0, 0);

    // Paste into edit control
    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#else
    wxFAIL_MSG("wxTextCtrl::Replace not implemented if wxUSE_CLIPBOARD is 0.");
#endif
}

void wxTextCtrl::Remove(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Cut all selected text
#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxTextCtrl::SetSelection(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;
    // if from and to are both -1, it means
    // (in wxWindows) that all text should be selected.
    // This translates into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }

#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
  if (!wxFileExists(WXSTRINGCAST file))
    return FALSE;

  m_fileName = file;

  Clear();

//  ifstream input(WXSTRINGCAST file, ios::nocreate | ios::in);
  ifstream input(MBSTRINGCAST file.mb_str(wxConvFile), ios::in);

  if (!input.bad())
  {
      // Previously a SETSEL/REPLACESEL call-pair were done to insert
      // line by line into the control. Apart from being very slow this
      // was limited to 32K of text by the external interface presenting
      // positions as signed shorts. Now load in one chunk...
      // Note use of 'farmalloc' as in Borland 3.1 'size_t' is 16-bits...

#ifdef __SALFORDC__
      struct _stat stat_buf;
      if (stat(MBSTRINGCAST file.mb_str(wxConvFile), &stat_buf) < 0)
        return FALSE;
#else
      struct stat stat_buf;
      if (stat(file.mb_str(wxConvFile), &stat_buf) < 0)
        return FALSE;
#endif

//      wxChar *tmp_buffer = (wxChar*)farmalloc(stat_buf.st_size+1);
      // This may need to be a bigger buffer than the file size suggests,
      // if it's a UNIX file. Give it an extra 1000 just in case.
      wxChar *tmp_buffer = (wxChar*)farmalloc((size_t)(stat_buf.st_size+1+1000));
      char *read_buffer = new char[512];
      long no_lines = 0;
      long pos = 0;
      while (!input.eof() && input.peek() != EOF)
      {
        input.getline(read_buffer, 500);
  int len = strlen(read_buffer);
  wxBuffer[len] = 13;
  wxBuffer[len+1] = 10;
  wxBuffer[len+2] = 0;
#if wxUSE_UNICODE
  pos += wxConvCurrent->MB2WC(tmp_buffer+pos, read_buffer, (size_t)-1);
#else
  strcpy(tmp_buffer+pos, read_buffer);
  pos += strlen(read_buffer);
#endif
  no_lines++;
      }
      delete[] read_buffer;

      SetWindowText(GetHwnd(), tmp_buffer);
      SendMessage(GetHwnd(), EM_SETMODIFY, FALSE, 0L);
      farfree(tmp_buffer);

      // update the size limit if needed
      AdjustSpaceLimit();

      return TRUE;
  }
  return FALSE;
}

// If file is null, try saved file name first
// Returns TRUE if succeeds.
bool wxTextCtrl::SaveFile(const wxString& file)
{
    wxString theFile(file);

    if (theFile == _T(""))
        theFile = m_fileName;

    if (theFile == _T(""))
        return FALSE;

    m_fileName = theFile;

    ofstream output(MBSTRINGCAST theFile.mb_str(wxConvFile));
    if (output.bad())
        return FALSE;

    // This will only save 64K max
    unsigned long nbytes = SendMessage(GetHwnd(), WM_GETTEXTLENGTH, 0, 0);
    char *tmp_buffer = (char*)farmalloc((size_t)(nbytes+1));
    SendMessage(GetHwnd(), WM_GETTEXT, (WPARAM)(nbytes+1), (LPARAM)tmp_buffer);
    char *pstr = tmp_buffer;

    // Convert \r\n to just \n
    while (*pstr)
    {
      if (*pstr != '\r')
        output << *pstr;
      pstr++;
    }

    farfree(tmp_buffer);
    SendMessage(GetHwnd(), EM_SETMODIFY, FALSE, 0L);

    return TRUE;
}

void wxTextCtrl::WriteText(const wxString& text)
{
    // Covert \n to \r\n
    int len = text.Length();
    char *newtext = new char[(len*2)+1];
    int i = 0;
    int j = 0;
    while (i < len)
    {
      if (text[i] == '\n')
      {
        newtext[j] = '\r';
        j ++;
      }
      newtext[j] = text[i];
      i ++;
      j ++;
    }
    newtext[j] = 0;
    SendMessage(GetHwnd(), EM_REPLACESEL, 0, (LPARAM)newtext);
    delete[] newtext;

    AdjustSpaceLimit();
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
    SetWindowText(GetHwnd(), _T(""));
}

bool wxTextCtrl::IsModified() const
{
    return (SendMessage(GetHwnd(), EM_GETMODIFY, 0, 0) != 0);
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
  SendMessage(GetHwnd(), EM_SETMODIFY, FALSE, 0L);
}

/*
 * Some of the following functions are yet to be implemented
 *
 */

int wxTextCtrl::GetNumberOfLines() const
{
    return (int)SendMessage(GetHwnd(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    HWND hWnd = GetHwnd();

    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
}

void wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = GetHwnd();

    // This gets the line number containing the character
    int lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0);
    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    // The X position must therefore be the different between pos and charIndex
    *x = (long)(pos - charIndex);
    *y = (long)lineNo;
}

void wxTextCtrl::ShowPosition(long pos)
{
    HWND hWnd = GetHwnd();

    // To scroll to a position, we pass the number of lines and characters
    // to scroll *by*. This means that we need to:
    // (1) Find the line position of the current line.
    // (2) Find the line position of pos.
    // (3) Scroll by (pos - current).
    // For now, ignore the horizontal scrolling.

    // Is this where scrolling is relative to - the line containing the caret?
    // Or is the first visible line??? Try first visible line.
//    int currentLineLineNo1 = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)-1, (LPARAM)0L);

    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)linesToScroll);
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    long charIndex = XYToPosition(0, lineNo);
    HWND hWnd = GetHwnd();
    int len = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0);
    return len;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    HWND hWnd = GetHwnd();
    *(WORD *)wxBuffer = 512;
    int noChars = (int)SendMessage(hWnd, EM_GETLINE, (WPARAM)lineNo, (LPARAM)wxBuffer);
    wxBuffer[noChars] = 0;
  return wxString(wxBuffer);
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to) ;
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to) ;
}

bool wxTextCtrl::CanPaste() const
{
#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        int dataFormat = 0; // 0 == any format
        return (::SendMessage( GetHwnd(), EM_CANPASTE, (WPARAM) (UINT) dataFormat, 0) != 0);
    }
#endif
    if (!IsEditable())
        return FALSE;

    // Standard edit control: check for straight text on clipboard
    bool isTextAvailable = FALSE;
    if (::OpenClipboard((HWND) wxTheApp->GetTopWindow()->GetHWND()))
    {
        isTextAvailable = (::IsClipboardFormatAvailable(CF_TEXT) != 0);
        ::CloseClipboard();
    }
    return isTextAvailable;
}

// Undo/redo
void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        // Same as Undo, since Undo undoes the undo, i.e. a redo.
        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

bool wxTextCtrl::CanUndo() const
{
    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
}

bool wxTextCtrl::CanRedo() const
{
    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        CHARRANGE charRange;
        ::SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) (CHARRANGE*) & charRange);

        *from = charRange.cpMin;
        *to = charRange.cpMax;

        return;
    }
#endif
    DWORD dwStart, dwEnd;
    WPARAM wParam = (WPARAM) (DWORD*) & dwStart; // receives starting position
    LPARAM lParam = (LPARAM) (DWORD*) & dwEnd;   // receives ending position

    ::SendMessage(GetHwnd(), EM_GETSEL, wParam, lParam);

    *from = dwStart;
    *to = dwEnd;
}

bool wxTextCtrl::IsEditable() const
{
    long style = ::GetWindowLong(GetHwnd(), GWL_STYLE);

    return ((style & ES_READONLY) == 0);
}

void wxTextCtrl::Command(wxCommandEvent & event)
{
  SetValue (event.GetString());
  ProcessCommand (event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
  // By default, load the first file into the text window.
  if (event.GetNumberOfFiles() > 0)
  {
    LoadFile(event.GetFiles()[0]);
  }
}

// The streambuf code was partly taken from chapter 3 by Jerry Schwarz of
// AT&T's "C++ Lanuage System Release 3.0 Library Manual" - Stein Somers

//=========================================================================
// Called then the buffer is full (gcc 2.6.3)
// or when "endl" is output (Borland 4.5)
//=========================================================================
// Class declaration using multiple inheritance doesn't work properly for
// Borland. See note in textctrl.h.
#ifndef NO_TEXT_WINDOW_STREAM
int wxTextCtrl::overflow(int c)
{
  // Make sure there is a holding area
  // this is not needed in <iostream> usage as it automagically allocates
  // it, but does someone want to emulate it for safety's sake?
#if wxUSE_IOSTREAMH
  if ( allocate()==EOF )
  {
    wxLogError("Streambuf allocation failed");
    return EOF;
  }
#endif

  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  // Reset get area
  setg(0,0,0);

  // Make sure there is a put area
  if ( ! pptr() )
  {
/* This doesn't seem to be fatal so comment out error message */
//    wxError("Put area not opened","Internal error");

#if wxUSE_IOSTREAMH
	  setp( base(), base() );
#else
	  setp( pbase(), pbase() );
#endif
  }

  // Determine how many characters have been inserted but no consumed
  int plen = pptr() - pbase();

  // Now Jerry relies on the fact that the buffer is at least 2 chars
  // long, but the holding area "may be as small as 1" ???
  // And we need an additional \0, so let's keep this inefficient but
  // safe copy.

  // If c!=EOF, it is a character that must also be comsumed
  int xtra = c==EOF? 0 : 1;

  // Write temporary C-string to wxTextWindow
  {
  char *txt = new char[plen+xtra+1];
  memcpy(txt, pbase(), plen);
  txt[plen] = (char)c;     // append c
  txt[plen+xtra] = '\0';   // append '\0' or overwrite c
    // If the put area already contained \0, output will be truncated there
  AppendText(txt);
    delete[] txt;
  }

  // Reset put area
  setp(pbase(), epptr());

#if defined(__WATCOMC__)
  return __NOT_EOF;
#elif defined(zapeof)     // HP-UX (all cfront based?)
  return zapeof(c);
#else
  return c!=EOF ? c : 0;  // this should make everybody happy
#endif

/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return EOF;
*/
}

//=========================================================================
// called then "endl" is output (gcc) or then explicit sync is done (Borland)
//=========================================================================
int wxTextCtrl::sync()
{
  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  if ( pptr() && pptr() > pbase() ) return overflow(EOF);

  return 0;
/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return 0;
*/
}

//=========================================================================
// Should not be called by a "ostream". Used by a "istream"
//=========================================================================
int wxTextCtrl::underflow()
{
  return EOF;
}
#endif

wxTextCtrl& wxTextCtrl::operator<<(const wxString& s)
{
    AppendText(s);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(float f)
{
    wxString str;
    str.Printf(_T("%.2f"), f);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(double d)
{
    wxString str;
    str.Printf(_T("%.2f"), d);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(int i)
{
    wxString str;
    str.Printf(_T("%d"), i);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(long i)
{
    wxString str;
    str.Printf(_T("%ld"), i);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const char c)
{
    char buf[2];

    buf[0] = c;
    buf[1] = 0;
    AppendText(buf);
    return *this;
}

WXHBRUSH wxTextCtrl::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if wxUSE_CTL3D
  if ( m_useCtl3D )
  {
    HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
    return (WXHBRUSH) hbrush;
  }
#endif

  if (GetParent()->GetTransparentBackground())
    SetBkMode((HDC) pDC, TRANSPARENT);
  else
    SetBkMode((HDC) pDC, OPAQUE);

  ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
  // has a zero usage count.
  // NOT NOW - will be cleaned up at end of app.
//  backgroundBrush->RealizeResource();
  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_RETURN:
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            //else: multiline controls need Enter for themselves

            break;

        case WXK_TAB:
            // only produce navigation event if we don't process TAB ourself or
            // if it's a Shift-Tab keypress (we assume nobody will ever need
            // this key combo for himself)
            //
            // NB: Notice that Ctrl-Tab is handled elsewhere and Alt-Tab is
            //     handled by Windows
            if ( event.ShiftDown() || !(m_windowStyle & wxTE_PROCESS_TAB) )
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(FALSE);
                eventNav.SetEventObject(this);

                if ( GetEventHandler()->ProcessEvent(eventNav) )
                    return;
            }
            break;

        default:
            event.Skip();
            return;
    }

    // don't just call event.Skip() because this will cause TABs and ENTERs
    // be passed upwards and we don't always want this - instead process it
    // right here

    // FIXME
    event.Skip();
}

bool wxTextCtrl::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    switch (param)
    {
        case EN_SETFOCUS:
        case EN_KILLFOCUS:
            {
                wxFocusEvent event(param == EN_KILLFOCUS ? wxEVT_KILL_FOCUS
                        : wxEVT_SET_FOCUS,
                        m_windowId);
                event.SetEventObject( this );
                GetEventHandler()->ProcessEvent(event);
            }
            break;

        case EN_CHANGE:
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
                wxString val(GetValue());
                if ( !val.IsNull() )
                    event.m_commandString = WXSTRINGCAST val;
                event.SetEventObject( this );
                ProcessCommand(event);
            }
            break;

        case EN_ERRSPACE:
            // the text size limit has been hit - increase it
            AdjustSpaceLimit();
            break;

            // the other notification messages are not processed
        case EN_UPDATE:
        case EN_MAXTEXT:
        case EN_HSCROLL:
        case EN_VSCROLL:
        default:
            return FALSE;
    }

    // processed
    return TRUE;
}

void wxTextCtrl::AdjustSpaceLimit()
{
#ifndef __WIN16__
    unsigned int len = ::GetWindowTextLength(GetHwnd()),
    limit = ::SendMessage(GetHwnd(), EM_GETLIMITTEXT, 0, 0);
    if ( len > limit )
    {
        limit = len + 0x8000;    // 32Kb

#if wxUSE_RICHEDIT
        if ( m_isRich || limit > 0xffff )
#else
        if ( limit > 0xffff )
#endif
            ::SendMessage(GetHwnd(), EM_LIMITTEXT, 0, limit);
        else
            ::SendMessage(GetHwnd(), EM_LIMITTEXT, limit, 0);
    }
#endif
}

// For Rich Edit controls. Do we need it?
#if 0
#if wxUSE_RICHEDIT
bool wxTextCtrl::MSWOnNotify(WXWPARAM wParam, WXLPARAM lParam)
{
  wxCommandEvent event(0, m_windowId);
  int eventType = 0;
  NMHDR *hdr1 = (NMHDR *) lParam;
  switch ( hdr1->code )
  {
    // Insert case code here
    default :
      return wxControl::MSWOnNotify(wParam, lParam);
      break;
  }

  event.SetEventObject( this );
  event.SetEventType(eventType);

  if ( !GetEventHandler()->ProcessEvent(event) )
    return FALSE;

    return TRUE;
}
#endif
#endif

void wxTextCtrl::OnCut(wxCommandEvent& event)
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& event)
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& event)
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& event)
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& event)
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

