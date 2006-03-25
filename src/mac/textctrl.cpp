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

#ifndef __WXMAC__
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <stat.h>
#endif
#include <fstream.h>

#include "wx/textctrl.h"
#include "wx/settings.h"
#include "wx/filefn.h"
#include "wx/utils.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
#include <alloc.h>
#else
#ifndef __MWERKS__
#ifndef __GNUWIN32__
#include <malloc.h>
#endif
#endif
#endif

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
	EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
	EVT_CHAR(wxTextCtrl::OnChar)
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
		   const wxString& st,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
	m_macHorizontalBorder = 2 ; // additional pixels around the real control
	m_macVerticalBorder = 2 ;

	wxSize mySize = size ;

	Rect bounds ;
	Str255 title ;
	
	if ( mySize.y == -1 )
	{
		if ( UMAHasAppearance() )
			mySize.y = 16 ;
		else
			mySize.y = 24 ;
	}
	MacPreControlCreate( parent , id ,  "" , pos , mySize ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
	  	kControlEditTextProc , (long) this ) ;
	MacPostControlCreate() ;

	wxString value ;
	
	if( wxApp::s_macDefaultEncodingIsPC )
		value = wxMacMakeMacStringFromPC( st ) ;
	else
		value = st ;
	UMASetControlData( m_macControl, 0, kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;

  return TRUE;
}

wxString wxTextCtrl::GetValue() const
{
	Size actualsize;
	UMAGetControlData( m_macControl, 0, kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
	wxBuffer[actualsize] = 0 ;
	if( wxApp::s_macDefaultEncodingIsPC )
		return wxMacMakePCStringFromMac( wxBuffer ) ;
	else
    	return wxString(wxBuffer);
}

void wxTextCtrl::SetValue(const wxString& st)
{
	wxString value ;
	
	if( wxApp::s_macDefaultEncodingIsPC )
		value = wxMacMakeMacStringFromPC( st ) ;
	else
		value = st ;
	UMASetControlData( m_macControl, 0, kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;
	Refresh() ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize( x , y , width , height , sizeFlags ) ;
}

// Clipboard operations
void wxTextCtrl::Copy()
{
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
	TECopy( teH ) ;
}

void wxTextCtrl::Cut()
{
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
	TECut( teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::Paste()
{
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
	TEPaste( teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable )
    	UMAActivateControl( m_macControl ) ;
    else
    	UMADeactivateControl( m_macControl ) ;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
	SetSelection( pos , pos ) ;
}

void wxTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
//   UMAGetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).selStart ;
}

long wxTextCtrl::GetLastPosition() const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
//   UMAGetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).teLength ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
   	TEHandle teH ;
   	long size ;
   
   	ControlEditTextSelectionRec selection ;
   
   	selection.selStart = from ;
   	selection.selEnd = to ;
   	UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
		UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   	TESetSelect( from , to  , teH ) ;
   	TEDelete( teH ) ;
		TEInsert( value , value.Length() , teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::Remove(long from, long to)
{
   	TEHandle teH ;
   	long size ;
   
   	ControlEditTextSelectionRec selection ;
   
   	selection.selStart = from ;
   	selection.selEnd = to ;
   	UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
	UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   	TEDelete( teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
   selection.selStart = from ;
   selection.selEnd = to ;
   
   UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
   TESetSelect( selection.selStart , selection.selEnd , teH ) ;
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if (!wxFileExists(file))
        return FALSE;

    m_fileName = file;

    Clear();

#ifndef __WXMAC__
    ifstream input((char*) (const char*) file, ios::nocreate | ios::in);
#else
    ifstream input((char*) (const char*) file, ios::in);
#endif
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
    TEHandle teH ;
    long size ;
   
   	memcpy( wxBuffer, text , text.Length() ) ;
   	wxBuffer[text.Length() ] = 0 ;
//    wxMacConvertNewlines( wxBuffer , wxBuffer ) ;
   
    UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
		TEInsert( wxBuffer , strlen( wxBuffer) , teH ) ;
		Refresh() ;
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
    TEHandle teH ;
    long size ;
   	ControlEditTextSelectionRec selection ;
   
  	selection.selStart = 0 ;
   	selection.selEnd = 32767 ;
   
    UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
   
    UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
		TECut( teH ) ;
//	MacInvalidateControl() ;
}

bool wxTextCtrl::IsModified() const
{
    return TRUE;
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
    return GetValue().Length();
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return GetValue();
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

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch( event.KeyCode() )
    {
        case WXK_RETURN:
        {
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            //else: multiline controls need Enter for themselves

            break;
        }
        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            //
            // NB: Notice that Ctrl-Tab is handled elsewhere and Alt-Tab is
            //     handled by Windows
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

