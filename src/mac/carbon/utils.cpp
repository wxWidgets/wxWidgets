/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// Note: this is done in utilscmn.cpp now.
// #pragma implementation
// #pragma implementation "utils.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/mac/uma.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    return wxGetHostName(buf, maxSize);
}

// Get hostname only (without domain name)
bool wxGetHostName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user name e.g. AUTHOR
bool wxGetUserName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

int wxKill(long pid, int sig)
{
    // TODO
    return 0;
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
    // TODO
    return FALSE;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    // TODO
    return 0;
}

void wxSleep(int nSecs)
{
    // TODO
}

// Consume all events until no more left
void wxFlushEvents()
{
}

// Output a debug message, in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

  // wvsprintf(buffer,fmt,ap) ;
  // TODO: output buffer

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
    // TODO
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
    // TODO
}

// Emit a beeeeeep
void wxBell()
{
    // TODO
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    // TODO
    return 0;
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
    // TODO
    return FALSE;
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%.4f", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%ld", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%d", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file)
{
    // TODO
    return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (float)strtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = strtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}
#endif // wxUSE_RESOURCES

int wxBusyCursorCount = 0;
extern CursHandle	gMacCurrentCursor ;
CursHandle			gMacStoredActiveCursor = NULL ;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount ++;
  if (wxBusyCursorCount == 1)
  {
  	gMacStoredActiveCursor = gMacCurrentCursor ;
		::SetCursor( *::GetCursor( watchCursor ) ) ;
  }
  else
  {
        // TODO
  }
}

// Restore cursor to normal
void wxEndBusyCursor()
{
  if (wxBusyCursorCount == 0)
    return;
    
  wxBusyCursorCount --;
  if (wxBusyCursorCount == 0)
  {
    if ( gMacStoredActiveCursor )
    	::SetCursor( *gMacStoredActiveCursor ) ;
    else
    {
		Cursor 		MacArrow ;
    	::SetCursor( GetQDGlobalsArrow( &MacArrow ) ) ;
    }
   	gMacStoredActiveCursor = NULL ;
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}    

wxString wxMacFindFolder( short 					vol,
								 OSType 				folderType,
								 Boolean 				createFolder)
{
	short 		vRefNum  ;
	long 		dirID ;
	wxString strDir ;
		
	if ( FindFolder( vol, folderType, createFolder, &vRefNum, &dirID) == noErr)
	{
		FSSpec file ;
		if ( FSMakeFSSpec( vRefNum , dirID , "\p" , &file ) == noErr )
		{
			strDir = wxMacFSSpec2UnixFilename( &file ) + "/" ;
		}
	}
	return strDir ;
}

char *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    // TODO
    return FALSE;
}

void wxGetMousePosition( int* x, int* y )
{
    Point pt ;
    
    GetMouse( &pt ) ;
    LocalToGlobal( &pt ) ;
    *x = pt.h ;
    *y = pt.v ;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    return TRUE;
}

// Returns depth of screen
int wxDisplayDepth()
{
	Rect globRect ; 
	SetRect(&globRect, -32760, -32760, 32760, 32760);
	GDHandle	theMaxDevice;

	int theDepth = 8;
	theMaxDevice = GetMaxDevice(&globRect);
	if (theMaxDevice != nil)
		theDepth = (**(**theMaxDevice).gdPMap).pixelSize;
		
	return theDepth ;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
	BitMap screenBits;
	GetQDGlobalsScreenBits( &screenBits );

    *width = screenBits.bounds.right - screenBits.bounds.left  ;
    *height = screenBits.bounds.bottom - screenBits.bounds.top ; 
 #if TARGET_CARBON
 	SInt16 mheight ;
 	GetThemeMenuBarHeight( &mheight ) ;
     *height -= mheight ;
#else
     *height -= LMGetMBarHeight() ;
 #endif
}

