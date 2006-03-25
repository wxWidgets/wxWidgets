/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fstream.h>

#include "wx/textctrl.h"
#include "wx/settings.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
#include <alloc.h>
#else
#ifndef __GNUWIN32__
#include <malloc.h>
#endif
#endif

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
	EVT_CHAR(wxTextCtrl::OnChar)
	EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
	EVT_ERASE_BACKGROUND(wxTextCtrl::OnEraseBackground)
END_EVENT_TABLE()

// Text item
wxTextCtrl::wxTextCtrl()
#ifndef NO_TEXT_WINDOW_STREAM
 :streambuf()
#endif
{
    m_fileName = "";
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

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    return TRUE;
}

wxString wxTextCtrl::GetValue() const
{
    // TODO
    return wxString("");
}

void wxTextCtrl::SetValue(const wxString& value)
{
    // TODO
}

void wxTextCtrl::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    // TODO
}

void wxTextCtrl::Cut()
{
    // TODO
}

void wxTextCtrl::Paste()
{
    // TODO
}

void wxTextCtrl::SetEditable(bool editable)
{
    // TODO
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    // TODO
}

void wxTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    // TODO
    return 0;
}

long wxTextCtrl::GetLastPosition() const
{
    // TODO
    return 0;
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
    // TODO
    return 0;
}

void wxTextCtrl::Remove(long from, long to)
{
    // TODO
}

void wxTextCtrl::SetSelection(long from, long to)
{
    // TODO
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if (!wxFileExists(file))
        return FALSE;

    m_fileName = file;

    Clear();

    ifstream input((char*) (const char*) file, ios::nocreate | ios::in);

    if (!input.bad())
    {
        struct stat stat_buf;
        if (stat(file, &stat_buf) < 0)
            return FALSE;
        // This may need to be a bigger buffer than the file size suggests,
        // if it's a UNIX file. Give it an extra 1000 just in case.
        char *tmp_buffer = (char*)malloc((size_t)(stat_buf.st_size+1+1000));
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

         // TODO add line

         free(tmp_buffer);

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

    // TODO get and save text

    return FALSE;
}

void wxTextCtrl::WriteText(const wxString& text)
{
    // TODO write text to control
}

void wxTextCtrl::AppendText(const wxString& text)
{
    // TODO append text to control
}

void wxTextCtrl::Clear()
{
    // TODO
}

bool wxTextCtrl::IsModified() const
{
    // TODO
    return FALSE;
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
    // TODO
}

int wxTextCtrl::GetNumberOfLines() const
{
    // TODO
    return 0;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    // TODO
    return 0;
}

void wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    // TODO
}

void wxTextCtrl::ShowPosition(long pos)
{
    // TODO
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    // TODO
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    // TODO
    return wxString("");
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
// Borland. See note in wb_text.h.
#ifndef NO_TEXT_WINDOW_STREAM
int wxTextCtrl::overflow(int c)
{
  // Make sure there is a holding area
  if ( allocate()==EOF )
  {
    wxError("Streambuf allocation failed","Internal error");
    return EOF;
  }
  
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
    setp( base(), base() );
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
    str.Printf("%.2f", f);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(double d)
{
    wxString str;
    str.Printf("%.2f", d);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(int i)
{
    wxString str;
    str.Printf("%d", i);
    AppendText(str);
    return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(long i)
{
    wxString str;
    str.Printf("%ld", i);
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

