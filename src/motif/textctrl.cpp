/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/filefn.h"
#include "wx/utils.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
#include <alloc.h>
#else
#ifndef __GNUWIN32__
#include <malloc.h>
#endif
#endif

#include <Xm/Text.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "wx/motif/private.h"

static void 
wxTextWindowChangedProc (Widget w, XtPointer clientData, XtPointer ptr);
static void 
wxTextWindowModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs);
static void 
wxTextWindowGainFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs);
static void 
wxTextWindowLoseFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
	EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
	EVT_CHAR(wxTextCtrl::OnChar)
END_EVENT_TABLE()
#endif

// Text item
wxTextCtrl::wxTextCtrl()
#ifndef NO_TEXT_WINDOW_STREAM
 :streambuf()
#endif
{
    m_fileName = "";
    m_tempCallbackStruct = (void*) NULL;
    m_modified = FALSE;
}

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
		   const wxString& value,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_tempCallbackStruct = (void*) NULL;
    m_modified = FALSE;
    m_fileName = "";

    SetName(name);
    SetValidator(validator);
    if (parent) parent->AddChild(this);

    m_windowStyle = style;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    bool wantHorizScrolling = ((m_windowStyle & wxHSCROLL) != 0);

    // If we don't have horizontal scrollbars, we want word wrap.
    bool wantWordWrap = !wantHorizScrolling;

    if (m_windowStyle & wxTE_MULTILINE)
    {
        Arg args[2];
        XtSetArg (args[0], XmNscrollHorizontal, wantHorizScrolling ? True : False);
        XtSetArg (args[1], XmNwordWrap, wantWordWrap ? True : False);

        m_mainWidget = (WXWidget) XmCreateScrolledText (parentWidget, (char*) (const char*) name, args, 2);

        XtVaSetValues ((Widget) m_mainWidget,
		 XmNeditable, ((style & wxTE_READONLY) ? False : True),
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 NULL);
        XtManageChild ((Widget) m_mainWidget);
    }
    else
    {
        m_mainWidget = (WXWidget) XtVaCreateManagedWidget ((char*) (const char*) name,
                 xmTextWidgetClass, parentWidget,
		 NULL);

        // TODO: Is this relevant? What does it do?
        int noCols = 2;
        if (!value.IsNull() && (value.Length() > (unsigned int) noCols))
            noCols = value.Length();
        XtVaSetValues ((Widget) m_mainWidget,
		 XmNcolumns, noCols,
		 NULL);
    }

    if (!value.IsNull())
        XmTextSetString ((Widget) m_mainWidget, (char*) (const char*) value);

    XtAddCallback((Widget) m_mainWidget, XmNvalueChangedCallback, (XtCallbackProc)wxTextWindowChangedProc, (XtPointer)this);

    XtAddCallback((Widget) m_mainWidget, XmNmodifyVerifyCallback, (XtCallbackProc)wxTextWindowModifyProc, (XtPointer)this);

//    XtAddCallback((Widget) m_mainWidget, XmNactivateCallback, (XtCallbackProc)wxTextWindowModifyProc, (XtPointer)this);

    XtAddCallback((Widget) m_mainWidget, XmNfocusCallback, (XtCallbackProc)wxTextWindowGainFocusProc, (XtPointer)this);

    XtAddCallback((Widget) m_mainWidget, XmNlosingFocusCallback, (XtCallbackProc)wxTextWindowLoseFocusProc, (XtPointer)this);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    SetFont(* parent->GetFont());
    ChangeColour(m_mainWidget);

    return TRUE;
}

WXWidget wxTextCtrl::GetTopWidget() const
{
  return ((m_windowStyle & wxTE_MULTILINE) ? (WXWidget) XtParent((Widget) m_mainWidget) : m_mainWidget);
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

void wxTextCtrl::Clear()
{
    XmTextSetString ((Widget) m_mainWidget, "");
    // TODO: do we need position flag?
    //    m_textPosition = 0;
    m_modified = FALSE;
}

bool wxTextCtrl::IsModified() const
{
    return m_modified;
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

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
  if (m_tempCallbackStruct)
  {
    XmTextVerifyCallbackStruct *textStruct =
        (XmTextVerifyCallbackStruct *) m_tempCallbackStruct;
    textStruct->doit = True;
    if (isascii(event.m_keyCode) && (textStruct->text->length == 1))
    {
      textStruct->text->ptr[0] = ((event.m_keyCode == WXK_RETURN) ? 10 : event.m_keyCode);
    }
  }
}

static void 
wxTextWindowChangedProc (Widget w, XtPointer clientData, XtPointer ptr)
{
  if (!wxGetWindowFromTable(w))
    // Widget has been deleted!
    return;

  wxTextCtrl *tw = (wxTextCtrl *) clientData;
  tw->SetModified(TRUE);
}

static void 
wxTextWindowModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs)
{
  wxTextCtrl *tw = (wxTextCtrl *) clientData;

  // If we're already within an OnChar, return: probably
  // a programmatic insertion.
  if (tw->m_tempCallbackStruct)
    return;

  // Check for a backspace
  if (cbs->startPos == (cbs->currInsert - 1))
  {
    tw->m_tempCallbackStruct = (void*) cbs;

    wxKeyEvent event (wxEVT_CHAR);
    event.SetId(tw->GetId());
    event.m_keyCode = WXK_DELETE;
    event.SetEventObject(tw);

    // Only if wxTextCtrl::OnChar is called
    // will this be set to True (and the character
    // passed through)
    cbs->doit = False;

    tw->GetEventHandler()->ProcessEvent(event);

    tw->m_tempCallbackStruct = NULL;

    return;
  }

  // Pasting operation: let it through without
  // calling OnChar
  if (cbs->text->length > 1)
    return;

  // Something other than text
  if (cbs->text->ptr == NULL)
    return;

  tw->m_tempCallbackStruct = (void*) cbs;

  wxKeyEvent event (wxEVT_CHAR);
  event.SetId(tw->GetId());
  event.SetEventObject(tw);
  event.m_keyCode = (cbs->text->ptr[0] == 10 ? 13 : cbs->text->ptr[0]);

  // Only if wxTextCtrl::OnChar is called
  // will this be set to True (and the character
  // passed through)
  cbs->doit = False;

  tw->GetEventHandler()->ProcessEvent(event);

  tw->m_tempCallbackStruct = NULL;
}

static void 
wxTextWindowGainFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs)
{
  if (!wxGetWindowFromTable(w))
    return;

  wxTextCtrl *tw = (wxTextCtrl *) clientData;
  wxFocusEvent event(wxEVT_SET_FOCUS, tw->GetId());
  event.SetEventObject(tw);
  tw->GetEventHandler()->ProcessEvent(event);
}

static void 
wxTextWindowLoseFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs)
{
  if (!wxGetWindowFromTable(w))
    return;

  wxTextCtrl *tw = (wxTextCtrl *) clientData;
  wxFocusEvent event(wxEVT_KILL_FOCUS, tw->GetId());
  event.SetEventObject(tw);
  tw->GetEventHandler()->ProcessEvent(event);
}
