/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// Note: this is done in utilscmn.cpp now.
// #pragma implementation "utils.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/apptrait.h"

#if wxUSE_GUI
    #include "wx/mac/uma.h"
	#include "wx/font.h"
#else
	#include "wx/intl.h"
#endif

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __DARWIN__
#  include "MoreFilesX.h"
#else
#  include "MoreFiles.h"
#  include "MoreFilesExtras.h"
#endif

#ifndef __DARWIN__
#include <Threads.h>
#include <Sound.h>
#endif

#include "ATSUnicode.h"
#include "TextCommon.h"
#include "TextEncodingConverter.h"

#include  "wx/mac/private.h"  // includes mac headers

#if defined(__MWERKS__) && wxUSE_UNICODE
    #include <wtime.h>
#endif

// ---------------------------------------------------------------------------
// code used in both base and GUI compilation
// ---------------------------------------------------------------------------

// our OS version is the same in non GUI and GUI cases
static int DoGetOSVersion(int *majorVsn, int *minorVsn)
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

#if wxUSE_BASE

#ifndef __DARWIN__
// defined in unix/utilsunx.cpp for Mac OS X

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    return wxGetHostName(buf, maxSize);
}

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
    // Gets Chooser name of user by examining a System resource.

    const short kComputerNameID = -16413;

    short oldResFile = CurResFile() ;
    UseResFile(0);
    StringHandle chooserName = (StringHandle)::GetString(kComputerNameID);
    UseResFile(oldResFile);

    if (chooserName && *chooserName)
    {
        HLock( (Handle) chooserName ) ;
        wxString name = wxMacMakeStringFromPascal( *chooserName ) ;
        HUnlock( (Handle) chooserName ) ;
        ReleaseResource( (Handle) chooserName ) ;
        wxStrncpy( buf , name , maxSize - 1 ) ;
    }
    else
        buf[0] = 0 ;

  return TRUE;
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
  return wxGetUserName( buf , maxSize ) ;
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
    *pstr = wxMacFindFolder(  (short) kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder ) ;
    return pstr->c_str() ;
}

// Get user name e.g. Stefan Csomor
bool wxGetUserName(wxChar *buf, int maxSize)
{
    // Gets Chooser name of user by examining a System resource.

    const short kChooserNameID = -16096;

    short oldResFile = CurResFile() ;
    UseResFile(0);
    StringHandle chooserName = (StringHandle)::GetString(kChooserNameID);
    UseResFile(oldResFile);

    if (chooserName && *chooserName)
    {
        HLock( (Handle) chooserName ) ;
        wxString name = wxMacMakeStringFromPascal( *chooserName ) ;
        HUnlock( (Handle) chooserName ) ;
        ReleaseResource( (Handle) chooserName ) ;
        wxStrncpy( buf , name , maxSize - 1 ) ;
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

#endif // !__DARWIN__

// Emit a beeeeeep
void wxBell()
{
    SysBeep(30);
}

wxToolkitInfo& wxConsoleAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.os = DoGetOSVersion(&info.versionMajor, &info.versionMinor);
    info.name = _T("wxBase");
    return info;
}

#endif // wxUSE_BASE

#if wxUSE_GUI

wxToolkitInfo& wxGUIAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.os = DoGetOSVersion(&info.versionMajor, &info.versionMinor);
    info.shortName = _T("mac");
    info.name = _T("wxMac");
#ifdef __WXUNIVERSAL__
    info.shortName << _T("univ");
    info.name << _T("/wxUniversal");
#endif
    return info;
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
    wxString buf;
    buf.Printf(wxT("%.4f"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
    wxString buf;
    buf.Printf(wxT("%ld"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
    wxString buf;
    buf.Printf(wxT("%d"), value);

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

#endif // wxUSE_GUI

#if wxUSE_BASE

wxString wxMacFindFolder( short        vol,
              OSType       folderType,
              Boolean      createFolder)
{
    short    vRefNum  ;
    long     dirID ;
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

#endif // wxUSE_BASE

#if wxUSE_GUI

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
    GDHandle    theMaxDevice;

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

    if (width != NULL) {
        *width = screenBits.bounds.right - screenBits.bounds.left  ;
    }
    if (height != NULL) {
        *height = screenBits.bounds.bottom - screenBits.bounds.top ;
    }
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxDisplaySize(width, height);
    // on mac 72 is fixed (at least now ;-)
    float cvPt2Mm = 25.4 / 72;

    if (width != NULL) {
        *width = int( *width * cvPt2Mm );
    }
    if (height != NULL) {
        *height = int( *height * cvPt2Mm );
    }
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    BitMap screenBits;
    GetQDGlobalsScreenBits( &screenBits );

    if (x) *x = 0;
    if (y) *y = 0;

    if (width != NULL) {
        *width = screenBits.bounds.right - screenBits.bounds.left  ;
    }
    if (height != NULL) {
        *height = screenBits.bounds.bottom - screenBits.bounds.top ;
    }

    SInt16 mheight ;
#if TARGET_CARBON
    GetThemeMenuBarHeight( &mheight ) ;
#else
    mheight = LMGetMBarHeight() ;
#endif
    if (height != NULL) {
        *height -= mheight ;
    }
    if (y)
        *y = mheight ;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

#endif // wxUSE_GUI

#if wxUSE_BASE

wxString wxGetOsDescription()
{
#ifdef WXWIN_OS_DESCRIPTION
    // use configure generated description if available
    return wxString(wxT("MacOS (")) + wxT(WXWIN_OS_DESCRIPTION) + wxString(wxT(")"));
#else
    return wxT("MacOS") ; //TODO:define further
#endif
}

#ifndef __DARWIN__
wxChar *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

bool wxGetDiskSpace(const wxString& path, wxLongLong *pTotal, wxLongLong *pFree)
{
    if ( path.empty() )
        return FALSE;

    wxString p = path ;
    if (p[0u] == ':' ) {
      p = wxGetCwd() + p ;
    }

    int pos = p.Find(':') ;
    if ( pos != wxNOT_FOUND ) {
      p = p.Mid(1,pos) ;
    }

    p = p + wxT(":") ;

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
#endif // !__DARWIN__

//---------------------------------------------------------------------------
// wxMac Specific utility functions
//---------------------------------------------------------------------------

#if 0

char StringMac[] =  "\x0d\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
                    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
                    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xae\xaf"
                    "\xb1\xb4\xb5\xb6\xbb\xbc\xbe\xbf"
                    "\xc0\xc1\xc2\xc4\xc7\xc8\xc9\xcb\xcc\xcd\xce\xcf"
                    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd8\xca\xdb" ;

char StringANSI[] = "\x0a\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"
                    "\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"
                    "\x86\xBA\xA2\xA3\xA7\x95\xB6\xDF\xAE\xA9\x99\xB4\xA8\xC6\xD8"
                    "\xB1\xA5\xB5\xF0\xAA\xBA\xE6\xF8"
                    "\xBF\xA1\xAC\x83\xAB\xBB\x85\xC0\xC3\xD5\x8C\x9C"
                    "\x96\x97\x93\x94\x91\x92\xF7\xFF\xA0\x80" ;

void wxMacConvertFromPC( const char *from , char *to , int len )
{
    char *c ;
    if ( from == to )
    {
        for( int i = 0 ; i < len ; ++ i )
        {
            c = strchr( StringANSI , *from ) ;
            if ( c != NULL )
            {
                *to = StringMac[ c - StringANSI] ;
            }
            ++to ;
            ++from ;
        }
    }
    else
    {
        for( int i = 0 ; i < len ; ++ i )
        {
            c = strchr( StringANSI , *from ) ;
            if ( c != NULL )
            {
                *to = StringMac[ c - StringANSI] ;
            }
            else
            {
                *to = *from ;
            }
            ++to ;
            ++from ;
        }
    }
}

void wxMacConvertToPC( const char *from , char *to , int len )
{
    char *c ;
    if ( from == to )
    {
        for( int i = 0 ; i < len ; ++ i )
        {
            c = strchr( StringMac , *from ) ;
            if ( c != NULL )
            {
                *to = StringANSI[ c - StringMac] ;
            }
            ++to ;
            ++from ;
        }
    }
    else
    {
        for( int i = 0 ; i < len ; ++ i )
        {
            c = strchr( StringMac , *from ) ;
            if ( c != NULL )
            {
                *to = StringANSI[ c - StringMac] ;
            }
            else
            {
                *to = *from ;
            }
            ++to ;
            ++from ;
        }
    }
}

TECObjectRef s_TECNativeCToUnicode = NULL ;
TECObjectRef s_TECUnicodeToNativeC = NULL ;

void wxMacSetupConverters()
{
    // if we assume errors are happening here we need low level debugging
    // since the high level assert will use the encoders that are not yet
    // setup...
#if TARGET_CARBON
    const TextEncodingBase kEncoding = CFStringGetSystemEncoding();
#else
    const TextEncodingBase kEncoding = kTextEncodingMacRoman;
#endif
    OSStatus status = noErr ;
    status = TECCreateConverter(&s_TECNativeCToUnicode,
                                kEncoding,
                                kTextEncodingUnicodeDefault);

    status = TECCreateConverter(&s_TECUnicodeToNativeC,
                                kTextEncodingUnicodeDefault,
                                kEncoding);

#if (wxUSE_UNICODE == 1) && (SIZEOF_WCHAR_T == 4)
	TextEncoding kUnicode32 = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    
    status = TECCreateConverter(&s_TECUnicode16To32,
                                kTextEncodingUnicodeDefault,
                                kUnicode32);
    status = TECCreateConverter(&s_TECUnicode32To16,
                                kUnicode32,
                                kTextEncodingUnicodeDefault);
#endif
}

void wxMacCleanupConverters()
{
    OSStatus status = noErr ;
    status = TECDisposeConverter(s_TECNativeCToUnicode);
    status = TECDisposeConverter(s_TECUnicodeToNativeC);
}

wxWCharBuffer wxMacStringToWString( const wxString &from )
{
#if wxUSE_UNICODE
    wxWCharBuffer result( from.wc_str() ) ;
#else
    OSStatus status = noErr ;
    ByteCount byteOutLen ;
    ByteCount byteInLen = from.Length() ;
    ByteCount byteBufferLen = byteInLen * SIZEOF_WCHAR_T ;
    wxWCharBuffer result( from.Length() ) ;
    status = TECConvertText(s_TECNativeCToUnicode, (ConstTextPtr)from.c_str() , byteInLen, &byteInLen,
        (TextPtr)result.data(), byteBufferLen, &byteOutLen);
    result.data()[byteOutLen/SIZEOF_WCHAR_T] = 0 ;
#endif
    return result ;
}


wxString wxMacMakeStringFromCString( const char * from , int len )
{
    OSStatus status = noErr ;
    wxString result ;
    wxChar* buf = result.GetWriteBuf( len ) ;
#if wxUSE_UNICODE
    ByteCount byteOutLen ;
    ByteCount byteInLen = len ;
    ByteCount byteBufferLen = len * SIZEOF_WCHAR_T;

    status = TECConvertText(s_TECNativeCToUnicode, (ConstTextPtr)from , byteInLen, &byteInLen,
        (TextPtr)buf, byteBufferLen, &byteOutLen);
#else
    memcpy( buf , from , len ) ;
#endif
    buf[len] = 0 ;
    result.UngetWriteBuf() ;
    return result ;
}

wxString wxMacMakeStringFromCString( const char * from )
{
    return wxMacMakeStringFromCString( from , strlen(from) ) ;
}

wxCharBuffer wxMacStringToCString( const wxString &from )
{
#if wxUSE_UNICODE
    OSStatus status = noErr ;
    ByteCount byteOutLen ;
    ByteCount byteInLen = from.Length() * SIZEOF_WCHAR_T ;
    ByteCount byteBufferLen = from.Length() ;
    wxCharBuffer result( from.Length() ) ;
    status = TECConvertText(s_TECUnicodeToNativeC , (ConstTextPtr)from.wc_str() , byteInLen, &byteInLen,
        (TextPtr)result.data(), byteBufferLen, &byteOutLen);
    return result ;
#else
    return wxCharBuffer( from.c_str() ) ;
#endif
}
#endif

void wxMacStringToPascal( const wxString&from , StringPtr to )
{
    wxCharBuffer buf = from.mb_str( wxConvLocal ) ;
    int len = strlen(buf) ;

    if ( len > 255 )
        len = 255 ;
    to[0] = len ;
    memcpy( (char*) &to[1] , buf , len ) ;
}

wxString wxMacMakeStringFromPascal( ConstStringPtr from )
{
    return wxString( (char*) &from[1] , wxConvLocal , from[0] ) ;
}


wxUint32 wxMacGetSystemEncFromFontEnc(wxFontEncoding encoding)
{    	
	TextEncodingBase enc = 0 ;
	if ( encoding == wxFONTENCODING_DEFAULT )
	{
#if wxUSE_GUI
		encoding = wxFont::GetDefaultEncoding() ;
#else
		encoding = wxLocale::GetSystemEncoding() ;
#endif
	}

	switch( encoding)
	{
		case wxFONTENCODING_ISO8859_1 :
    		enc = kTextEncodingISOLatin1 ;
    		break ;
		case wxFONTENCODING_ISO8859_2 :
    		enc = kTextEncodingISOLatin2;
    		break ;
		case wxFONTENCODING_ISO8859_3 :
    		enc = kTextEncodingISOLatin3 ;
    		break ;
		case wxFONTENCODING_ISO8859_4 :
    		enc = kTextEncodingISOLatin4;
    		break ;
		case wxFONTENCODING_ISO8859_5 :
    		enc = kTextEncodingISOLatinCyrillic;
    		break ;
		case wxFONTENCODING_ISO8859_6 :
    		enc = kTextEncodingISOLatinArabic;
    		break ;
		case wxFONTENCODING_ISO8859_7 :
    		enc = kTextEncodingISOLatinGreek;
    		break ;
		case wxFONTENCODING_ISO8859_8 :
    		enc = kTextEncodingISOLatinHebrew;
    		break ;
		case wxFONTENCODING_ISO8859_9 :
    		enc = kTextEncodingISOLatin5;
    		break ;
		case wxFONTENCODING_ISO8859_10 :
    		enc = kTextEncodingISOLatin6;
    		break ;
		case wxFONTENCODING_ISO8859_13 :
    		enc = kTextEncodingISOLatin7;
    		break ;
		case wxFONTENCODING_ISO8859_14 :
    		enc = kTextEncodingISOLatin8;
    		break ;
		case wxFONTENCODING_ISO8859_15 :
    		enc = kTextEncodingISOLatin9;
    		break ;

		case wxFONTENCODING_KOI8 :
    		enc = kTextEncodingKOI8_R;
    		break ;
		case wxFONTENCODING_ALTERNATIVE : // MS-DOS CP866
    		enc = kTextEncodingDOSRussian;
    		break ;
/*
		case wxFONTENCODING_BULGARIAN : 
    		enc = ;
    		break ;
*/	    		
		case wxFONTENCODING_CP437 : 
    		enc =kTextEncodingDOSLatinUS ;
    		break ;
		case wxFONTENCODING_CP850 :
    		enc = kTextEncodingDOSLatin1;
    		break ;
		case wxFONTENCODING_CP852 : 
    		enc = kTextEncodingDOSLatin2;
    		break ;
		case wxFONTENCODING_CP855 :
    		enc = kTextEncodingDOSCyrillic;
    		break ;
		case wxFONTENCODING_CP866 :
    		enc =kTextEncodingDOSRussian ;
    		break ;
		case wxFONTENCODING_CP874 :
    		enc = kTextEncodingDOSThai;
    		break ;
		case wxFONTENCODING_CP932 : 
    		enc = kTextEncodingDOSJapanese;
    		break ;
		case wxFONTENCODING_CP936 : 
    		enc =kTextEncodingDOSChineseSimplif ;
    		break ;
		case wxFONTENCODING_CP949 : 
    		enc = kTextEncodingDOSKorean;
    		break ;
		case wxFONTENCODING_CP950 : 
    		enc = kTextEncodingDOSChineseTrad;
    		break ;
    		
		case wxFONTENCODING_CP1250 : 
    		enc = kTextEncodingWindowsLatin2;
    		break ;
		case wxFONTENCODING_CP1251 : 
    		enc =kTextEncodingWindowsCyrillic ;
    		break ;
		case wxFONTENCODING_CP1252 : 
    		enc =kTextEncodingWindowsLatin1 ;
    		break ;
		case wxFONTENCODING_CP1253 : 
    		enc = kTextEncodingWindowsGreek;
    		break ;
		case wxFONTENCODING_CP1254 : 
    		enc = kTextEncodingWindowsLatin5;
    		break ;
		case wxFONTENCODING_CP1255 : 
    		enc =kTextEncodingWindowsHebrew ;
    		break ;
		case wxFONTENCODING_CP1256 : 
    		enc =kTextEncodingWindowsArabic ;
    		break ;
		case wxFONTENCODING_CP1257 : 
    		enc = kTextEncodingWindowsBalticRim;
    		break ;
    		
		case wxFONTENCODING_UTF7 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF7Format) ;
    		break ;
		case wxFONTENCODING_UTF8 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF8Format) ;
    		break ;
		case wxFONTENCODING_EUC_JP : 
    		enc = kTextEncodingEUC_JP;
    		break ;
		case wxFONTENCODING_UTF16BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF16LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;

        case wxFONTENCODING_MACROMAN :
            enc = kTextEncodingMacRoman ;
            break ;
        case wxFONTENCODING_MACJAPANESE :
            enc = kTextEncodingMacJapanese ;
            break ;
        case wxFONTENCODING_MACCHINESETRAD :
            enc = kTextEncodingMacChineseTrad ;
            break ;
        case wxFONTENCODING_MACKOREAN :
            enc = kTextEncodingMacKorean ;
            break ;
        case wxFONTENCODING_MACARABIC :
            enc = kTextEncodingMacArabic ;
            break ;
        case wxFONTENCODING_MACHEBREW :
            enc = kTextEncodingMacHebrew ;
            break ;
        case wxFONTENCODING_MACGREEK :
            enc = kTextEncodingMacGreek ;
            break ;
        case wxFONTENCODING_MACCYRILLIC :
            enc = kTextEncodingMacCyrillic ;
            break ;
        case wxFONTENCODING_MACDEVANAGARI :
            enc = kTextEncodingMacDevanagari ;
            break ;
        case wxFONTENCODING_MACGURMUKHI :
            enc = kTextEncodingMacGurmukhi ;
            break ;
        case wxFONTENCODING_MACGUJARATI :
            enc = kTextEncodingMacGujarati ;
            break ;
        case wxFONTENCODING_MACORIYA :
            enc = kTextEncodingMacOriya ;
            break ;
        case wxFONTENCODING_MACBENGALI :
            enc = kTextEncodingMacBengali ;
            break ;
        case wxFONTENCODING_MACTAMIL :
            enc = kTextEncodingMacTamil ;
            break ;
        case wxFONTENCODING_MACTELUGU :
            enc = kTextEncodingMacTelugu ;
            break ;
        case wxFONTENCODING_MACKANNADA :
            enc = kTextEncodingMacKannada ;
            break ;
        case wxFONTENCODING_MACMALAJALAM :
            enc = kTextEncodingMacMalayalam ;
            break ;
        case wxFONTENCODING_MACSINHALESE :
            enc = kTextEncodingMacSinhalese ;
            break ;
        case wxFONTENCODING_MACBURMESE :
            enc = kTextEncodingMacBurmese ;
            break ;
        case wxFONTENCODING_MACKHMER :
            enc = kTextEncodingMacKhmer ;
            break ;
        case wxFONTENCODING_MACTHAI :
            enc = kTextEncodingMacThai ;
            break ;
        case wxFONTENCODING_MACLAOTIAN :
            enc = kTextEncodingMacLaotian ;
            break ;
        case wxFONTENCODING_MACGEORGIAN :
            enc = kTextEncodingMacGeorgian ;
            break ;
        case wxFONTENCODING_MACARMENIAN :
            enc = kTextEncodingMacArmenian ;
            break ;
        case wxFONTENCODING_MACCHINESESIMP :
            enc = kTextEncodingMacChineseSimp ;
            break ;
        case wxFONTENCODING_MACTIBETAN :
            enc = kTextEncodingMacTibetan ;
            break ;
        case wxFONTENCODING_MACMONGOLIAN :
            enc = kTextEncodingMacMongolian ;
            break ;
        case wxFONTENCODING_MACETHIOPIC :
            enc = kTextEncodingMacEthiopic ;
            break ;
        case wxFONTENCODING_MACCENTRALEUR :
            enc = kTextEncodingMacCentralEurRoman ;
            break ;
        case wxFONTENCODING_MACVIATNAMESE :
            enc = kTextEncodingMacVietnamese ;
            break ;
        case wxFONTENCODING_MACARABICEXT :
            enc = kTextEncodingMacExtArabic ;
            break ;
        case wxFONTENCODING_MACSYMBOL :
            enc = kTextEncodingMacSymbol ;
            break ;
        case wxFONTENCODING_MACDINGBATS :
            enc = kTextEncodingMacDingbats ;
            break ;
        case wxFONTENCODING_MACTURKISH :
            enc = kTextEncodingMacTurkish ;
            break ;
        case wxFONTENCODING_MACCROATIAN :
            enc = kTextEncodingMacCroatian ;
            break ;
        case wxFONTENCODING_MACICELANDIC :
            enc = kTextEncodingMacIcelandic ;
            break ;
        case wxFONTENCODING_MACROMANIAN :
            enc = kTextEncodingMacRomanian ;
            break ;
        case wxFONTENCODING_MACCELTIC :
            enc = kTextEncodingMacCeltic ;
            break ;
        case wxFONTENCODING_MACGAELIC :
            enc = kTextEncodingMacGaelic ;
            break ;
        case wxFONTENCODING_MACKEYBOARD :
            enc = kTextEncodingMacKeyboardGlyphs ;
            break ;       
	} ;
	return enc ;
}

wxFontEncoding wxMacGetFontEncFromSystemEnc(wxUint32 encoding)
{    	
	wxFontEncoding enc = wxFONTENCODING_DEFAULT ;

	switch( encoding)
	{
		case kTextEncodingISOLatin1  :
    		enc = wxFONTENCODING_ISO8859_1 ;
    		break ;
		case kTextEncodingISOLatin2 :
    		enc = wxFONTENCODING_ISO8859_2;
    		break ;
		case kTextEncodingISOLatin3 :
    		enc = wxFONTENCODING_ISO8859_3 ;
    		break ;
		case kTextEncodingISOLatin4 :
    		enc = wxFONTENCODING_ISO8859_4;
    		break ;
		case kTextEncodingISOLatinCyrillic :
    		enc = wxFONTENCODING_ISO8859_5;
    		break ;
		case kTextEncodingISOLatinArabic :
    		enc = wxFONTENCODING_ISO8859_6;
    		break ;
		case kTextEncodingISOLatinGreek :
    		enc = wxFONTENCODING_ISO8859_7;
    		break ;
		case kTextEncodingISOLatinHebrew :
    		enc = wxFONTENCODING_ISO8859_8;
    		break ;
		case kTextEncodingISOLatin5 :
    		enc = wxFONTENCODING_ISO8859_9;
    		break ;
		case kTextEncodingISOLatin6 :
    		enc = wxFONTENCODING_ISO8859_10;
    		break ;
		case kTextEncodingISOLatin7 :
    		enc = wxFONTENCODING_ISO8859_13;
    		break ;
		case kTextEncodingISOLatin8 :
    		enc = wxFONTENCODING_ISO8859_14;
    		break ;
		case kTextEncodingISOLatin9 :
    		enc =wxFONTENCODING_ISO8859_15 ;
    		break ;

		case kTextEncodingKOI8_R :
    		enc = wxFONTENCODING_KOI8;
    		break ;
/*
		case  : 
    		enc = wxFONTENCODING_BULGARIAN;
    		break ;
*/	    		
		case kTextEncodingDOSLatinUS : 
    		enc = wxFONTENCODING_CP437;
    		break ;
		case kTextEncodingDOSLatin1 :
    		enc = wxFONTENCODING_CP850;
    		break ;
		case kTextEncodingDOSLatin2 : 
    		enc =wxFONTENCODING_CP852 ;
    		break ;
		case kTextEncodingDOSCyrillic :
    		enc = wxFONTENCODING_CP855;
    		break ;
		case kTextEncodingDOSRussian :
    		enc = wxFONTENCODING_CP866;
    		break ;
		case kTextEncodingDOSThai :
    		enc =wxFONTENCODING_CP874 ;
    		break ;
		case kTextEncodingDOSJapanese : 
    		enc = wxFONTENCODING_CP932;
    		break ;
		case kTextEncodingDOSChineseSimplif : 
    		enc = wxFONTENCODING_CP936;
    		break ;
		case kTextEncodingDOSKorean : 
    		enc = wxFONTENCODING_CP949;
    		break ;
		case kTextEncodingDOSChineseTrad : 
    		enc = wxFONTENCODING_CP950;
    		break ;
    		
		case kTextEncodingWindowsLatin2 : 
    		enc = wxFONTENCODING_CP1250;
    		break ;
		case kTextEncodingWindowsCyrillic : 
    		enc = wxFONTENCODING_CP1251;
    		break ;
		case kTextEncodingWindowsLatin1 : 
    		enc = wxFONTENCODING_CP1252;
    		break ;
		case kTextEncodingWindowsGreek : 
    		enc = wxFONTENCODING_CP1253;
    		break ;
		case kTextEncodingWindowsLatin5 : 
    		enc = wxFONTENCODING_CP1254;
    		break ;
		case kTextEncodingWindowsHebrew : 
    		enc = wxFONTENCODING_CP1255;
    		break ;
		case kTextEncodingWindowsArabic : 
    		enc = wxFONTENCODING_CP1256;
    		break ;
		case kTextEncodingWindowsBalticRim : 
    		enc =wxFONTENCODING_CP1257 ;
    		break ;
		case kTextEncodingEUC_JP : 
    		enc = wxFONTENCODING_EUC_JP;
    		break ;
    		/*
		case wxFONTENCODING_UTF7 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF7Format) ;
    		break ;
		case wxFONTENCODING_UTF8 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF8Format) ;
    		break ;
		case wxFONTENCODING_UTF16BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF16LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
        */
        case kTextEncodingMacRoman :
            enc = wxFONTENCODING_MACROMAN ;
            break ;
        case kTextEncodingMacJapanese :
            enc = wxFONTENCODING_MACJAPANESE ;
            break ;
        case kTextEncodingMacChineseTrad :
            enc = wxFONTENCODING_MACCHINESETRAD ;
            break ;
        case kTextEncodingMacKorean :
            enc = wxFONTENCODING_MACKOREAN ;
            break ;
        case kTextEncodingMacArabic :
            enc =wxFONTENCODING_MACARABIC ;
            break ;
        case kTextEncodingMacHebrew :
            enc = wxFONTENCODING_MACHEBREW ;
            break ;
        case kTextEncodingMacGreek :
            enc = wxFONTENCODING_MACGREEK ;
            break ;
        case kTextEncodingMacCyrillic :
            enc = wxFONTENCODING_MACCYRILLIC ;
            break ;
        case kTextEncodingMacDevanagari :
            enc = wxFONTENCODING_MACDEVANAGARI ;
            break ;
        case kTextEncodingMacGurmukhi :
            enc = wxFONTENCODING_MACGURMUKHI ;
            break ;
        case kTextEncodingMacGujarati :
            enc = wxFONTENCODING_MACGUJARATI ;
            break ;
        case kTextEncodingMacOriya :
            enc =wxFONTENCODING_MACORIYA ;
            break ;
        case kTextEncodingMacBengali :
            enc =wxFONTENCODING_MACBENGALI ;
            break ;
        case kTextEncodingMacTamil :
            enc = wxFONTENCODING_MACTAMIL ;
            break ;
        case kTextEncodingMacTelugu :
            enc = wxFONTENCODING_MACTELUGU ;
            break ;
        case kTextEncodingMacKannada :
            enc = wxFONTENCODING_MACKANNADA ;
            break ;
        case kTextEncodingMacMalayalam :
            enc = wxFONTENCODING_MACMALAJALAM ;
            break ;
        case kTextEncodingMacSinhalese :
            enc = wxFONTENCODING_MACSINHALESE ;
            break ;
        case kTextEncodingMacBurmese :
            enc = wxFONTENCODING_MACBURMESE ;
            break ;
        case kTextEncodingMacKhmer :
            enc = wxFONTENCODING_MACKHMER ;
            break ;
        case kTextEncodingMacThai :
            enc = wxFONTENCODING_MACTHAI ;
            break ;
        case kTextEncodingMacLaotian :
            enc = wxFONTENCODING_MACLAOTIAN ;
            break ;
        case kTextEncodingMacGeorgian :
            enc = wxFONTENCODING_MACGEORGIAN ;
            break ;
        case kTextEncodingMacArmenian :
            enc = wxFONTENCODING_MACARMENIAN ;
            break ;
        case kTextEncodingMacChineseSimp :
            enc = wxFONTENCODING_MACCHINESESIMP ;
            break ;
        case kTextEncodingMacTibetan :
            enc = wxFONTENCODING_MACTIBETAN ;
            break ;
        case kTextEncodingMacMongolian :
            enc = wxFONTENCODING_MACMONGOLIAN ;
            break ;
        case kTextEncodingMacEthiopic :
            enc = wxFONTENCODING_MACETHIOPIC ;
            break ;
        case kTextEncodingMacCentralEurRoman:
            enc = wxFONTENCODING_MACCENTRALEUR  ;
            break ;
        case kTextEncodingMacVietnamese:
            enc = wxFONTENCODING_MACVIATNAMESE  ;
            break ;
        case kTextEncodingMacExtArabic :
            enc = wxFONTENCODING_MACARABICEXT ;
            break ;
        case kTextEncodingMacSymbol :
            enc = wxFONTENCODING_MACSYMBOL ;
            break ;
        case kTextEncodingMacDingbats :
            enc = wxFONTENCODING_MACDINGBATS ;
            break ;
        case kTextEncodingMacTurkish :
            enc = wxFONTENCODING_MACTURKISH ;
            break ;
        case kTextEncodingMacCroatian :
            enc = wxFONTENCODING_MACCROATIAN ;
            break ;
        case kTextEncodingMacIcelandic :
            enc = wxFONTENCODING_MACICELANDIC ;
            break ;
        case kTextEncodingMacRomanian :
            enc = wxFONTENCODING_MACROMANIAN ;
            break ;
        case kTextEncodingMacCeltic :
            enc = wxFONTENCODING_MACCELTIC ;
            break ;
        case kTextEncodingMacGaelic :
            enc = wxFONTENCODING_MACGAELIC ;
            break ;
        case kTextEncodingMacKeyboardGlyphs :
            enc = wxFONTENCODING_MACKEYBOARD ;
            break ;       
	} ;
	return enc ;
}

#endif // wxUSE_BASE

#if wxUSE_GUI


//
// CFStringRefs (Carbon only)
//

#if TARGET_CARBON

#if (wxUSE_UNICODE == 1) && (SIZEOF_WCHAR_T == 4)

TECObjectRef s_TECUnicode32To16 = NULL ;
TECObjectRef s_TECUnicode16To32 = NULL ;

class wxMacUnicodeConverters
{
public :
	wxMacUnicodeConverters() ;
	~wxMacUnicodeConverters() ;
} ;

wxMacUnicodeConverters guard ;

wxMacUnicodeConverters::wxMacUnicodeConverters()
{
    OSStatus status = noErr ;
	TextEncoding kUnicode32 = CreateTextEncoding(kTextEncodingUnicodeDefault,kTextEncodingDefaultVariant,kUnicode32BitFormat) ;
	TextEncoding kUnicode16 = CreateTextEncoding(kTextEncodingUnicodeDefault,kTextEncodingDefaultVariant,kUnicode16BitFormat) ;
	
	status = TECCreateConverter(&s_TECUnicode16To32,
								kUnicode16,
								kUnicode32);
	status = TECCreateConverter(&s_TECUnicode32To16,
								kUnicode32,
								kUnicode16);
}

wxMacUnicodeConverters::~wxMacUnicodeConverters()
{
    OSStatus status = noErr ;
	status = TECDisposeConverter(s_TECUnicode32To16);
	status = TECDisposeConverter(s_TECUnicode16To32);
}
#endif
// converts this string into a carbon foundation string with optional pc 2 mac encoding
void wxMacCFStringHolder::Assign( const wxString &st , wxFontEncoding encoding )
{
	Release() ;
	wxString str = st ;
    wxMacConvertNewlines13To10( &str ) ;
#if wxUSE_UNICODE
	size_t len = str.Len() ;
	UniChar *unibuf ;
#if SIZEOF_WCHAR_T == 2
	unibuf = (UniChar*)str.wc_str() ;
#else
    OSStatus status = noErr ;
    ByteCount byteOutLen ;
    ByteCount byteInLen = len * SIZEOF_WCHAR_T ;
    ByteCount byteBufferLen = len * sizeof( UniChar ) ;
	unibuf = (UniChar*) malloc(byteBufferLen) ;
	for( int i = 0 ; i < len ; ++ i )
		unibuf[i] = (UniChar) str[i] ;
	/*	
    status = TECConvertText( s_TECUnicode32To16 , (ConstTextPtr)str.wc_str() , byteInLen, &byteInLen,
        (TextPtr)unibuf, byteBufferLen, &byteOutLen);
	 */
#endif
  	m_cfs = CFStringCreateWithCharacters( kCFAllocatorDefault,
	 unibuf , len );
#if SIZEOF_WCHAR_T == 2
	// as long as UniChar is the same as wchar_t nothing to do here
#else
	free( unibuf ) ;
#endif

#else // not wxUSE_UNICODE
    m_cfs = CFStringCreateWithCString( kCFAllocatorSystemDefault , str.c_str() ,
        wxMacGetSystemEncFromFontEnc( encoding ) ) ;
#endif
    m_release = true ;
}

wxString wxMacCFStringHolder::AsString(wxFontEncoding encoding)
{
    wxString result ;
    Size len = CFStringGetLength( m_cfs )  ;
    wxChar* buf = result.GetWriteBuf( len ) ;
#if wxUSE_UNICODE
	UniChar *unibuf ;
#if SIZEOF_WCHAR_T == 2
	unibuf = (UniChar*) buf ;
#else
	unibuf = malloc( len * sizeof( UniChar ) ) ;
#endif
    CFStringGetCharacters( m_cfs , CFRangeMake( 0 , len ) , (UniChar*) unibuf ) ;
#if SIZEOF_WCHAR_T == 2
	// as long as UniChar is the same as wchar_t nothing to do here
#else
	for( int i = 0 ; i < len ; ++ i )
		buf[i] = (wchar_t) unibuf[i] ;
	free( unibuf ) ;
#endif	
#else
    CFStringGetCString( m_cfs , buf , len+1 , wxMacGetSystemEncFromFontEnc( encoding ) ) ;
#endif
    buf[len] = 0 ;
    wxMacConvertNewlines10To13( buf ) ;
    result.UngetWriteBuf() ;
    return result ;
}

#endif //TARGET_CARBON

void wxMacConvertNewlines13To10( char * data ) 
{        
	char * buf = data ;
    while( (buf=strchr(buf,0x0d)) != NULL )
    {
        *buf = 0x0a ;
        buf++ ;
    }
}

void wxMacConvertNewlines10To13( char * data )
{        
	char * buf = data ;
    while( (buf=strchr(buf,0x0a)) != NULL )
    {
        *buf = 0x0d ;
        buf++ ;
    }
}

void wxMacConvertNewlines13To10( wxString * data ) 
{        
    size_t len = data->Length() ;

    if ( len == 0 || wxStrchr(data->c_str(),0x0d)==NULL)
        return ;
        
    wxString temp(*data) ;
    wxStringBuffer buf(*data,len ) ;
    memcpy( buf , temp.c_str() , (len+1)*sizeof(wxChar) ) ; 

	wxMacConvertNewlines13To10( buf ) ;
}

void wxMacConvertNewlines10To13( wxString * data )
{        
    size_t len = data->Length() ;

    if ( data->Length() == 0 || wxStrchr(data->c_str(),0x0a)==NULL)
        return ;

    wxString temp(*data) ;
    wxStringBuffer buf(*data,len ) ;
    memcpy( buf , temp.c_str() , (len+1)*sizeof(wxChar) ) ; 
	wxMacConvertNewlines10To13( buf ) ;
}


#if wxUSE_UNICODE
void wxMacConvertNewlines13To10( wxChar * data ) 
{        
	wxChar * buf = data ;
    while( (buf=wxStrchr(buf,0x0d)) != NULL )
    {
        *buf = 0x0a ;
        buf++ ;
    }
}

void wxMacConvertNewlines10To13( wxChar * data )
{        
	wxChar * buf =  data ;
    while( (buf=wxStrchr(buf,0x0a)) != NULL )
    {
        *buf = 0x0d ;
        buf++ ;
    }
}
#endif

// ----------------------------------------------------------------------------
// debugging support
// ----------------------------------------------------------------------------

#if defined(__WXMAC__) && !defined(__DARWIN__) && defined(__MWERKS__) && (__MWERKS__ >= 0x2400)

// MetroNub stuff doesn't seem to work in CodeWarrior 5.3 Carbon builds...

#ifndef __MetroNubUtils__
#include "MetroNubUtils.h"
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif

#if TARGET_API_MAC_CARBON

    #include <CodeFragments.h>

    extern "C" long CallUniversalProc(UniversalProcPtr theProcPtr, ProcInfoType procInfo, ...);

    ProcPtr gCallUniversalProc_Proc = NULL;

#endif

static MetroNubUserEntryBlock*    gMetroNubEntry = NULL;

static long fRunOnce = false;

/* ---------------------------------------------------------------------------
        IsMetroNubInstalled
   --------------------------------------------------------------------------- */

Boolean IsMetroNubInstalled()
{
    if (!fRunOnce)
    {
        long result, value;

        fRunOnce = true;
        gMetroNubEntry = NULL;

        if (Gestalt(gestaltSystemVersion, &value) == noErr && value < 0x1000)
        {
            /* look for MetroNub's Gestalt selector */
            if (Gestalt(kMetroNubUserSignature, &result) == noErr)
            {

            #if TARGET_API_MAC_CARBON
                if (gCallUniversalProc_Proc == NULL)
                {
                    CFragConnectionID   connectionID;
                    Ptr                 mainAddress;
                    Str255              errorString;
                    ProcPtr             symbolAddress;
                    OSErr               err;
                    CFragSymbolClass    symbolClass;

                    symbolAddress = NULL;
                    err = GetSharedLibrary("\pInterfaceLib", kPowerPCCFragArch, kFindCFrag,
                                           &connectionID, &mainAddress, errorString);

                    if (err != noErr)
                    {
                        gCallUniversalProc_Proc = NULL;
                        goto end;
                    }

                    err = FindSymbol(connectionID, "\pCallUniversalProc",
                                    (Ptr *) &gCallUniversalProc_Proc, &symbolClass);

                    if (err != noErr)
                    {
                        gCallUniversalProc_Proc = NULL;
                        goto end;
                    }
                }
            #endif

                {
                    MetroNubUserEntryBlock* block = (MetroNubUserEntryBlock *)result;

                    /* make sure the version of the API is compatible */
                    if (block->apiLowVersion <= kMetroNubUserAPIVersion &&
                        kMetroNubUserAPIVersion <= block->apiHiVersion)
                        gMetroNubEntry = block;        /* success! */
                }

            }
        }
    }

end:

#if TARGET_API_MAC_CARBON
    return (gMetroNubEntry != NULL && gCallUniversalProc_Proc != NULL);
#else
    return (gMetroNubEntry != NULL);
#endif
}

/* ---------------------------------------------------------------------------
        IsMWDebuggerRunning                                            [v1 API]
   --------------------------------------------------------------------------- */

Boolean IsMWDebuggerRunning()
{
    if (IsMetroNubInstalled())
        return CallIsDebuggerRunningProc(gMetroNubEntry->isDebuggerRunning);
    else
        return false;
}

/* ---------------------------------------------------------------------------
        AmIBeingMWDebugged                                            [v1 API]
   --------------------------------------------------------------------------- */

Boolean AmIBeingMWDebugged()
{
    if (IsMetroNubInstalled())
        return CallAmIBeingDebuggedProc(gMetroNubEntry->amIBeingDebugged);
    else
        return false;
}

extern bool WXDLLEXPORT wxIsDebuggerRunning()
{
    return IsMWDebuggerRunning() && AmIBeingMWDebugged();
}

#else

extern bool WXDLLEXPORT wxIsDebuggerRunning()
{
    return false;
}

#endif // defined(__WXMAC__) && !defined(__DARWIN__) && (__MWERKS__ >= 0x2400)

#endif // wxUSE_GUI

