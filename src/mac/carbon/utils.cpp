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

#include "MoreFiles.h"
#include "MoreFilesExtras.h"

#ifndef __DARWIN__
#include <Threads.h>
#include <Sound.h>
#endif

#ifndef __DARWIN__
// defined in unix/utilsunx.cpp for Mac OS X

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    return wxGetHostName(buf, maxSize);
}

// Get hostname only (without domain name)
bool wxGetHostName(char *buf, int maxSize)
{
	// Gets Chooser name of user by examining a System resource.

	const short kComputerNameID = -16413;
	
	short oldResFile = CurResFile() ;
	UseResFile(0);
	StringHandle chooserName = (StringHandle)::GetString(kComputerNameID);
	UseResFile(oldResFile);

	if (chooserName && *chooserName)
	{
	  int length = (*chooserName)[0] ;
	  if ( length + 1 > maxSize )
	  {
	    length = maxSize - 1 ;
	  }
	  strncpy( buf , (char*) &(*chooserName)[1] , length ) ;
	  buf[length] = 0 ;
	}
	else
		buf[0] = 0 ;

  return TRUE;
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
  return wxGetUserName( buf , maxSize ) ;
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
	*pstr = wxMacFindFolder(  (short) kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder ) ;
	return pstr->c_str() ;
}

// Get user name e.g. AUTHOR
bool wxGetUserName(char *buf, int maxSize)
{
	// Gets Chooser name of user by examining a System resource.

	const short kChooserNameID = -16096;
	
	short oldResFile = CurResFile() ;
	UseResFile(0);
	StringHandle chooserName = (StringHandle)::GetString(kChooserNameID);
	UseResFile(oldResFile);

	if (chooserName && *chooserName)
	{
	  int length = (*chooserName)[0] ;
	  if ( length + 1 > maxSize )
	  {
	    length = maxSize - 1 ;
	  }
	  strncpy( buf , (char*) &(*chooserName)[1] , length ) ;
	  buf[length] = 0 ;
	}
	else
		buf[0] = 0 ;

  return TRUE;
}

int wxKill(long pid, wxSignal sig , wxKillError *rc )
{
    // TODO
    return 0;
}

WXDLLEXPORT bool wxGetEnv(const wxString& var, wxString *value)
{
  // TODO : under classic there is no environement support, under X yes
  return false ;
}

// set the env var name to the given value, return TRUE on success
WXDLLEXPORT bool wxSetEnv(const wxString& var, const wxChar *value)
{
  // TODO : under classic there is no environement support, under X yes
  return false ;
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
    // TODO
    return FALSE;
}

// Shutdown or reboot the PC 
bool wxShutdown(wxShutdownFlags wFlags)
{
    // TODO
    return FALSE;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    return FreeMem() ;
}

void wxUsleep(unsigned long milliseconds)
{
    clock_t start = clock() ;
    do 
    {
	YieldToAnyThread() ;
    } while( clock() - start < milliseconds /  1000.0 * CLOCKS_PER_SEC ) ;
}

void wxSleep(int nSecs)
{
    wxUsleep(1000*nSecs);
}

// Consume all events until no more left
void wxFlushEvents()
{
}

#if WXWIN_COMPATIBILITY_2_2

// Output a debug message, in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

  vsprintf(buffer,fmt,ap) ;
  strcat(buffer,";g") ;
  c2pstr(buffer) ;
  DebugStr((unsigned char*) buffer) ;

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
  wxSprintf(wxBuffer, wxT("%s\nContinue?"), WXSTRINGCAST msg);
  if (wxMessageBox(wxBuffer, title, wxYES_NO) == wxID_NO )
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
  wxSprintf(wxBuffer, wxT("%s: %s"), WXSTRINGCAST title, WXSTRINGCAST msg);
  wxMessageBox(wxBuffer);
  wxExit();
}

#endif // WXWIN_COMPATIBILITY_2_2

#endif // !__DARWIN__

// Emit a beeeeeep
void wxBell()
{
    SysBeep(30);
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    long theSystem ;
    
    // are there x-platform conventions ?

    Gestalt(gestaltSystemVersion, &theSystem) ;
    if (minorVsn != NULL) {
	*minorVsn = (theSystem & 0xFF ) ;
    }
    if (majorVsn != NULL) {
	*majorVsn = (theSystem >> 8 ) ;
    }
#ifdef __DARWIN__
    return wxMAC_DARWIN;
#else
    return wxMAC;
#endif
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

int gs_wxBusyCursorCount = 0;
extern wxCursor    gMacCurrentCursor ;
wxCursor        gMacStoredActiveCursor ;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
    if (gs_wxBusyCursorCount++ == 0)
  {
  	gMacStoredActiveCursor = gMacCurrentCursor ;
        cursor->MacInstall() ;
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0,
        wxT("no matching wxBeginBusyCursor() for wxEndBusyCursor()") );
    
    if (--gs_wxBusyCursorCount == 0)
    {
        gMacStoredActiveCursor.MacInstall() ;
        gMacStoredActiveCursor = wxNullCursor ;
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
    return (gs_wxBusyCursorCount > 0);
}

wxString wxMacFindFolder( short        vol,
			  OSType       folderType,
			  Boolean      createFolder)
{
	short 		vRefNum  ;
	long 		dirID ;
	wxString strDir ;

	if ( FindFolder( vol, folderType, createFolder, &vRefNum, &dirID) == noErr)
	{
		FSSpec file ;
		if ( FSMakeFSSpec( vRefNum , dirID , "\p" , &file ) == noErr )
		{
			strDir = wxMacFSSpec2MacFilename( &file ) + wxFILE_SEP_PATH ;
		}
	}
	return strDir ;
}

#ifndef __DARWIN__
char *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

bool wxGetDiskSpace(const wxString& path, wxLongLong *pTotal, wxLongLong *pFree)
{
    if ( path.empty() )
        return FALSE;
        
    wxString p = path ;
    if (p[0] == ':' ) {
      p = wxGetCwd() + p ;
    }
    
    int pos = p.Find(':') ;
    if ( pos != wxNOT_FOUND ) {
      p = p.Mid(1,pos) ;
    }
    
    p = p + ":" ;
    
    Str255 volumeName ;
    XVolumeParam pb ;

    wxMacStringToPascal( p  , volumeName ) ;
    OSErr err = XGetVolumeInfoNoName( volumeName , 0 , &pb ) ;
    if ( err == noErr ) {
      if ( pTotal ) {
        (*pTotal) = wxLongLong( pb.ioVTotalBytes ) ;
      }
      if ( pFree ) {
        (*pFree) = wxLongLong( pb.ioVFreeBytes ) ;
      }
    }

    return err == noErr ;
}
#endif

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
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxDisplaySize(width, height);
    // on mac 72 is fixed (at least now ;-)
    float cvPt2Mm = 25.4 / 72;
    *width = int( *width * cvPt2Mm );
    *height = int( *height * cvPt2Mm );
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
  	BitMap screenBits;
  	GetQDGlobalsScreenBits( &screenBits );

    if (x) *x = 0;
    if (y) *y = 0;

    *width = screenBits.bounds.right - screenBits.bounds.left  ;
    *height = screenBits.bounds.bottom - screenBits.bounds.top ;

   	SInt16 mheight ;
  #if TARGET_CARBON
   	GetThemeMenuBarHeight( &mheight ) ;
  #else
    mheight = LMGetMBarHeight() ;
  #endif
    *height -= mheight ;
    if ( y )
      *y = mheight ;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

wxString wxGetOsDescription()
{
#ifdef WXWIN_OS_DESCRIPTION
    // use configure generated description if available
    return wxString("MacOS (") + WXWIN_OS_DESCRIPTION + wxString(")");
#else
    return "MacOS" ; //TODO:define further
#endif
}

#if TARGET_CARBON
// converts this string into a carbon foundation string with optional pc 2 mac encoding
void wxMacCFStringHolder::Assign( const wxString &str ) 
{
    m_cfs = CFStringCreateWithCString( kCFAllocatorSystemDefault , str.c_str() ,
        wxApp::s_macDefaultEncodingIsPC ? 
        kCFStringEncodingWindowsLatin1 : CFStringGetSystemEncoding() ) ;
    m_release = true ;
}

wxString wxMacCFStringHolder::AsString() 
{
    wxString result ;
    Size len = CFStringGetLength( m_cfs )  ;
    wxChar* buf = result.GetWriteBuf( len ) ;

    CFStringGetCString( m_cfs , buf , len+1 , wxApp::s_macDefaultEncodingIsPC ? 
        kCFStringEncodingWindowsLatin1 : CFStringGetSystemEncoding() ) ;

    buf[len] = 0 ;
    result.UngetWriteBuf() ;
    return result ;
}
#endif
