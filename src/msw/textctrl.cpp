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
#include <fstream.h>
#else
#include <fstream>
#  ifdef _MSC_VER
      using namespace std;
#  endif
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
  EVT_ERASE_BACKGROUND(wxTextCtrl::OnEraseBackground)
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
  m_fileName = "";
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

  const char *windowClass = "EDIT";

#if wxUSE_RICHEDIT
  if ( m_windowStyle & wxTE_MULTILINE )
  {
    msStyle |= ES_AUTOVSCROLL;
    m_isRich = TRUE;
    windowClass = "RichEdit" ;
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
    windowClass = "EDIT";
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

  wxCHECK_MSG( m_hWnd, FALSE, "Failed to create text ctrl" );

#if CTL3D
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

  HWND hWnd = (HWND) GetHWND();
  long style = GetWindowLong((HWND) hWnd, GWL_STYLE);

  // retrieve the style to see whether this is an edit or richedit ctrl
#if wxUSE_RICHEDIT
  char buf[256];

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

  if (str == "EDIT")
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
    int length = GetWindowTextLength((HWND) GetHWND());
    char *s = new char[length+1];
    GetWindowText((HWND) GetHWND(), s, length+1);
    wxString str(s);
    delete[] s;
  return str;
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
    char *tmp = new char[len + singletons + 1];
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
    SetWindowText((HWND) GetHWND(), tmp);
    delete[] tmp;
  }
  else
    SetWindowText((HWND) GetHWND(), (const char *)value);
}

void wxTextCtrl::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  MoveWindow((HWND) GetHWND(), (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);
}

// Clipboard operations
void wxTextCtrl::Copy()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxTextCtrl::Cut()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxTextCtrl::Paste()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxTextCtrl::SetEditable(bool editable)
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
  HWND hWnd = (HWND) GetHWND();
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
    SendMessage((HWND) GetHWND(), EM_EXGETSEL, 0, (LPARAM) &range);
    return range.cpMin;
  }
#endif

  DWORD Pos=(DWORD)SendMessage((HWND) GetHWND(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
}

long wxTextCtrl::GetLastPosition() const
{
    HWND hWnd = (HWND) GetHWND();

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
    HWND hWnd = (HWND) GetHWND();
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
    wxSetClipboardData(wxDF_TEXT, (wxObject *) (const char *)value, 0, 0);

    // Paste into edit control
    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
}

void wxTextCtrl::Remove(long from, long to)
{
    HWND hWnd = (HWND) GetHWND();
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
    HWND hWnd = (HWND) GetHWND();
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
  ifstream input(WXSTRINGCAST file, ios::in);

  if (!input.bad())
  {
      // Previously a SETSEL/REPLACESEL call-pair were done to insert
      // line by line into the control. Apart from being very slow this
      // was limited to 32K of text by the external interface presenting
      // positions as signed shorts. Now load in one chunk...
      // Note use of 'farmalloc' as in Borland 3.1 'size_t' is 16-bits...

#ifdef __SALFORDC__
      struct _stat stat_buf;
      if (stat((char*) (const char*) file, &stat_buf) < 0)
        return FALSE;
#else
      struct stat stat_buf;
      if (stat(file, &stat_buf) < 0)
        return FALSE;
#endif

//      char *tmp_buffer = (char*)farmalloc(stat_buf.st_size+1);
      // This may need to be a bigger buffer than the file size suggests,
      // if it's a UNIX file. Give it an extra 1000 just in case.
      char *tmp_buffer = (char*)farmalloc((size_t)(stat_buf.st_size+1+1000));
      long no_lines = 0;
      long pos = 0;
      while (!input.eof() && input.peek() != EOF)
      {
        input.getline(wxBuffer, 500);
  int len = strlen(wxBuffer);
  wxBuffer[len] = 13;
  wxBuffer[len+1] = 10;
  wxBuffer[len+2] = 0;
  strcpy(tmp_buffer+pos, wxBuffer);
  pos += strlen(wxBuffer);
  no_lines++;
      }

//      SendMessage((HWND) GetHWND(), WM_SETTEXT, 0, (LPARAM)tmp_buffer);
      SetWindowText((HWND) GetHWND(), tmp_buffer);
      SendMessage((HWND) GetHWND(), EM_SETMODIFY, FALSE, 0L);
      farfree(tmp_buffer);

      return TRUE;
  }
  return FALSE;
}

// If file is null, try saved file name first
// Returns TRUE if succeeds.
bool wxTextCtrl::SaveFile(const wxString& file)
{
    wxString theFile(file);

    if (theFile == "")
        theFile = m_fileName;

    if (theFile == "")
        return FALSE;

    m_fileName = theFile;

    ofstream output((char*) (const char*) theFile);
    if (output.bad())
        return FALSE;

    // This will only save 64K max
    unsigned long nbytes = SendMessage((HWND) GetHWND(), WM_GETTEXTLENGTH, 0, 0);
    char *tmp_buffer = (char*)farmalloc((size_t)(nbytes+1));
    SendMessage((HWND) GetHWND(), WM_GETTEXT, (WPARAM)(nbytes+1), (LPARAM)tmp_buffer);
    char *pstr = tmp_buffer;

    // Convert \r\n to just \n
    while (*pstr)
    {
      if (*pstr != '\r')
        output << *pstr;
      pstr++;
    }

    farfree(tmp_buffer);
    SendMessage((HWND) GetHWND(), EM_SETMODIFY, FALSE, 0L);

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
    SendMessage((HWND) GetHWND(), EM_REPLACESEL, 0, (LPARAM)newtext);
    delete[] newtext;
}

void wxTextCtrl::Clear()
{
//    SendMessage((HWND) GetHWND(), WM_SETTEXT, 0, (LPARAM)"");
    SetWindowText((HWND) GetHWND(), "");
}

bool wxTextCtrl::IsModified() const
{
    return (SendMessage((HWND) GetHWND(), EM_GETMODIFY, 0, 0) != 0);
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
  SendMessage((HWND) GetHWND(), EM_SETMODIFY, FALSE, 0L);
}

/*
 * Some of the following functions are yet to be implemented
 *
 */
 
int wxTextCtrl::GetNumberOfLines() const
{
    return (int)SendMessage((HWND) GetHWND(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    HWND hWnd = (HWND) GetHWND();

    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
}

void wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = (HWND) GetHWND();

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
    HWND hWnd = (HWND) GetHWND();

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

/*
    wxDebugMsg("Caret line: %d; Current visible line: %d; Specified line: %d; lines to scroll: %d\n",
      currentLineLineNo1, currentLineLineNo, specifiedLineLineNo, linesToScroll);
*/

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)MAKELPARAM(linesToScroll, 0));
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    long charIndex = XYToPosition(0, lineNo);
    HWND hWnd = (HWND) GetHWND();
    int len = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0);
    return len;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    HWND hWnd = (HWND) GetHWND();
    *(WORD *)wxBuffer = 512;
    int noChars = (int)SendMessage(hWnd, EM_GETLINE, (WPARAM)lineNo, (LPARAM)wxBuffer);
    wxBuffer[noChars] = 0;
  return wxString(wxBuffer);
}

/*
 * Text item
 */
 
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
  WriteText(txt);
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
  WriteText(s);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(float f)
{
    wxString str;
    str.Printf("%.2f", f);
    WriteText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(double d)
{
    wxString str;
    str.Printf("%.2f", d);
    WriteText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(int i)
{
    wxString str;
    str.Printf("%d", i);
    WriteText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(long i)
{
    wxString str;
    str.Printf("%ld", i);
    WriteText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const char c)
{
    char buf[2];

    buf[0] = c;
    buf[1] = 0;
    WriteText(buf);
    return *this;
}

WXHBRUSH wxTextCtrl::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if CTL3D
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
    switch( event.KeyCode() )
    {
        // Fix by Marcel Rasche to allow Alt-Ctrl insertion of special characters
        case '{':
        case '}':
        case '[':
        case ']':
        case '|':
        case '~':
        case '\\':
            {
                char c = (char)event.KeyCode();
                *this << c;
            }
            break;

        case WXK_RETURN:
            wxASSERT_MSG( m_windowStyle & wxTE_PROCESS_ENTER,
                          "this text ctrl should never receive return" );
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }

        case WXK_TAB:
            // only produce navigation event if we don't process TAB ourself
            if ( !(m_windowStyle & wxTE_PROCESS_TAB) )
            {
                wxNavigationKeyEvent event;
                event.SetDirection(!(::GetKeyState(VK_SHIFT) & 0x100));
                event.SetWindowChange(FALSE);
                event.SetEventObject(this);
    
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
    }
    
    // don't just call event.Skip() because this will cause TABs and ENTERs
    // be passed upwards and we don't always want this - instead process it
    // right here
    Default();
}

long wxTextCtrl::MSWGetDlgCode()
{
    // we always want the characters and the arrows
    long lRc = DLGC_WANTCHARS | DLGC_WANTARROWS;

    // we may have several different cases:
    // 1. normal case: both TAB and ENTER are used for dialog navigation
    // 2. ctrl which wants TAB for itself: ENTER is used to pass to the next
    //    control in the dialog
    // 3. ctrl which wants ENTER for itself: TAB is used for dialog navigation
    // 4. ctrl which wants both TAB and ENTER: Ctrl-ENTER is used to pass to
    //    the next control
    if ( m_windowStyle & wxTE_PROCESS_ENTER )
        lRc |= DLGC_WANTMESSAGE;
    if ( m_windowStyle & wxTE_PROCESS_TAB )
        lRc |= DLGC_WANTTAB;
    
    return lRc;
}

void wxTextCtrl::OnEraseBackground(wxEraseEvent& event)
{
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        // No flicker - only problem is we probably can't change the background
        Default();
/*
        RECT rect;
        ::GetClientRect((HWND) GetHWND(), &rect);

        HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
        int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

        ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
        ::DeleteObject(hBrush);
        ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
*/
    }
//        wxWindow::OnEraseBackground(event);
}

bool wxTextCtrl::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
/*
  // Debugging
  wxDebugMsg("Edit control %d: ", (int)id);
  switch (param)
  {
    case EN_SETFOCUS:
      wxDebugMsg("EN_SETFOCUS\n");
      break;
    case EN_KILLFOCUS:
      wxDebugMsg("EN_KILLFOCUS\n");
      break;
    case EN_CHANGE:
      wxDebugMsg("EN_CHANGE\n");
      break;
    case EN_UPDATE:
      wxDebugMsg("EN_UPDATE\n");
      break;
    case EN_ERRSPACE:
      wxDebugMsg("EN_ERRSPACE\n");
      break;
    case EN_MAXTEXT:
      wxDebugMsg("EN_MAXTEXT\n");
      break;
    case EN_HSCROLL:
      wxDebugMsg("EN_HSCROLL\n");
      break;
    case EN_VSCROLL:
      wxDebugMsg("EN_VSCROLL\n");
      break;
    default:
      wxDebugMsg("Unknown EDIT notification\n");
      break;
  }
*/
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

    // the other notification messages are not processed
    case EN_UPDATE:
    case EN_ERRSPACE:
    case EN_MAXTEXT:
    case EN_HSCROLL:
    case EN_VSCROLL:
    default:
      return FALSE;
  }

  // processed
  return TRUE;
}


// For Rich Edit controls. Do we need it?
#if 0
#if wxUSE_RICHEDIT
bool wxTextCtrl::MSWNotify(WXWPARAM wParam, WXLPARAM lParam)
{
  wxCommandEvent event(0, m_windowId);
  int eventType = 0;
  NMHDR *hdr1 = (NMHDR *) lParam;
  switch ( hdr1->code )
  {
    // Insert case code here
    default :
      return wxControl::MSWNotify(wParam, lParam);
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

