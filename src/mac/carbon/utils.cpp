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

#include "MoreFilesX.h"

#ifndef __DARWIN__
#include <Threads.h>
#include <Sound.h>
#endif

#if wxUSE_GUI
#if TARGET_API_MAC_OSX
#include <CoreServices/CoreServices.h>
#else
#include <DriverServices.h>
#include <Multiprocessing.h>
#endif

#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#endif // wxUSE_GUI

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

#ifndef __DARWIN__

void wxMicroSleep(unsigned long microseconds)
{
	AbsoluteTime wakeup = AddDurationToAbsolute( microseconds * durationMicrosecond , UpTime());
	MPDelayUntil( & wakeup);
}

void wxMilliSleep(unsigned long milliseconds)
{
	AbsoluteTime wakeup = AddDurationToAbsolute( milliseconds, UpTime());
	MPDelayUntil( & wakeup);
}

void wxSleep(int nSecs)
{
    wxMilliSleep(1000*nSecs);
}

#endif

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
    FSRef fsRef ;    
    wxString strDir ;

    if ( FSFindFolder( vol, folderType, createFolder, &fsRef) == noErr)
        strDir = wxMacFSRefToPath( &fsRef ) + wxFILE_SEP_PATH ;

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
    Rect r ;
    GetAvailableWindowPositioningBounds( GetMainDevice() , &r ) ;
    if ( x )
        *x = r.left ;
    if ( y )
        *y = r.top ;
    if ( width )
        *width = r.right - r.left ;
    if ( height )
        *height = r.bottom - r.top ;
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

    OSErr err = noErr ;
    
    FSRef fsRef ;
    err = wxMacPathToFSRef( p , &fsRef ) ;
    if ( noErr == err )
    {
        FSVolumeRefNum vRefNum ;
        err = FSGetVRefNum( &fsRef , &vRefNum ) ;
        if ( noErr == err )
        {
            UInt64 freeBytes , totalBytes ;
            err = FSGetVInfo( vRefNum , NULL , &freeBytes , &totalBytes ) ;
            if ( noErr == err )
            {
                if ( pTotal ) 
                    *pTotal = wxLongLong( totalBytes ) ;
                if ( pFree )
                    *pFree = wxLongLong( freeBytes ) ;
            }
        }
    }
    
    return err == noErr ;
}
#endif // !__DARWIN__

//---------------------------------------------------------------------------
// wxMac Specific utility functions
//---------------------------------------------------------------------------

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
		default :
			// to make gcc happy
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


//
// CFStringRefs (Carbon only)
//

#if TARGET_CARBON

// converts this string into a carbon foundation string with optional pc 2 mac encoding
void wxMacCFStringHolder::Assign( const wxString &st , wxFontEncoding encoding )
{
	Release() ;

	wxString str = st ;
    wxMacConvertNewlines13To10( &str ) ;
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
  	m_cfs = CFStringCreateWithCharacters( kCFAllocatorDefault,
	 (UniChar*)str.wc_str() , str.Len() );
#else
	wxMBConvUTF16BE converter ;
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 ) ;
    UniChar *unibuf = new UniChar[ unicharlen / sizeof(UniChar) + 1 ] ;
    converter.WC2MB( (char*)unibuf , str.wc_str() , unicharlen ) ;
    m_cfs = CFStringCreateWithCharacters( kCFAllocatorDefault ,
        unibuf , unicharlen / sizeof(UniChar) ) ;
    delete[] unibuf ;
#endif
#else // not wxUSE_UNICODE
    m_cfs = CFStringCreateWithCString( kCFAllocatorSystemDefault , str.c_str() ,
        wxMacGetSystemEncFromFontEnc( encoding ) ) ;
#endif
    m_release = true ;
}

wxString wxMacCFStringHolder::AsString(wxFontEncoding encoding)
{
    Size cflen = CFStringGetLength( m_cfs )  ;
    size_t noChars ;
    wxChar* buf = NULL ;
    
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    buf = new wxChar[ cflen + 1 ] ; 
    CFStringGetCharacters( m_cfs , CFRangeMake( 0 , cflen ) , (UniChar*) buf ) ;
    noChars = cflen ;
#else
    UniChar* unibuf = new UniChar[ cflen + 1 ] ;
    CFStringGetCharacters( m_cfs , CFRangeMake( 0 , cflen ) , (UniChar*) unibuf ) ;
    unibuf[cflen] = 0 ;
	wxMBConvUTF16BE converter ;
	noChars = converter.MB2WC( NULL , (const char*)unibuf , 0 ) ;
    buf = new wxChar[ noChars + 1 ] ;
    converter.MB2WC( buf , (const char*)unibuf , noChars ) ;
    delete[] unibuf ;
#endif
#else
    CFIndex cStrLen ;
    CFStringGetBytes( m_cfs , CFRangeMake(0, cflen) , wxMacGetSystemEncFromFontEnc( encoding ) ,
        '?' , false , NULL , 0 , &cStrLen ) ;
    buf = new wxChar[ cStrLen + 1 ] ; 
    CFStringGetBytes( m_cfs , CFRangeMake(0, cflen) , wxMacGetSystemEncFromFontEnc( encoding ) ,
        '?' , false , (unsigned char*) buf , cStrLen , &cStrLen) ;
    noChars = cStrLen ;
#endif

    buf[noChars] = 0 ;
    wxMacConvertNewlines10To13( buf ) ;
    wxString result(buf) ;
    delete[] buf ;
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
// Common Event Support
// ----------------------------------------------------------------------------


extern ProcessSerialNumber gAppProcess ;

void wxMacWakeUp()
{
    ProcessSerialNumber psn ;
    Boolean isSame ;
    psn.highLongOfPSN = 0 ;
    psn.lowLongOfPSN = kCurrentProcess ;
    SameProcess( &gAppProcess , &psn , &isSame ) ;
    if ( isSame )
    {
#if TARGET_CARBON
        EventRef dummyEvent ;
        OSStatus err = MacCreateEvent(nil, 'WXMC', 'WXMC', GetCurrentEventTime(),
                        kEventAttributeNone, &dummyEvent);
        if (err == noErr) 
        {
            err = PostEventToQueue(GetMainEventQueue(), dummyEvent,
                                  kEventPriorityHigh);
        } 
#else
        PostEvent( nullEvent , 0 ) ;
#endif
    }
    else
    {
        WakeUpProcess( &gAppProcess ) ;
    }
}

#endif // wxUSE_BASE

#if wxUSE_GUI


// ----------------------------------------------------------------------------
// Carbon Event Support
// ----------------------------------------------------------------------------


OSStatus wxMacCarbonEvent::GetParameter(EventParamName inName, EventParamType inDesiredType, UInt32 inBufferSize, void * outData)
{
    return ::GetEventParameter( m_eventRef , inName , inDesiredType , NULL , inBufferSize , NULL , outData ) ;   
}

OSStatus wxMacCarbonEvent::SetParameter(EventParamName inName, EventParamType inType, UInt32 inBufferSize, const void * inData)
{
    return ::SetEventParameter( m_eventRef , inName , inType , inBufferSize , inData ) ;   
}

// ----------------------------------------------------------------------------
// Control Access Support
// ----------------------------------------------------------------------------

void wxMacControl::Dispose()
{
    ::DisposeControl( m_controlRef ) ;
    m_controlRef = NULL ;
}

void wxMacControl::SetReference( SInt32 data ) 
{
    SetControlReference( m_controlRef , data ) ;
}

OSStatus wxMacControl::GetData(ControlPartCode inPartCode , ResType inTag , Size inBufferSize , void * inOutBuffer , Size * outActualSize ) const
{
    return ::GetControlData( m_controlRef , inPartCode , inTag , inBufferSize , inOutBuffer , outActualSize ) ;   
}

OSStatus wxMacControl::GetDataSize(ControlPartCode inPartCode , ResType inTag , Size * outActualSize ) const
{
    return ::GetControlDataSize( m_controlRef , inPartCode , inTag , outActualSize ) ;   
}

OSStatus wxMacControl::SetData(ControlPartCode inPartCode , ResType inTag , Size inSize , const void * inData)
{
    return ::SetControlData( m_controlRef , inPartCode , inTag , inSize , inData ) ;   
}

OSStatus wxMacControl::SendEvent(  EventRef event , OptionBits inOptions ) 
{
#if TARGET_API_MAC_OSX
    return SendEventToEventTargetWithOptions( event, 
        HIObjectGetEventTarget(  (HIObjectRef) m_controlRef ), inOptions );        
#else
    #pragma unused(inOptions) 
    return SendEventToEventTarget(event,GetControlEventTarget( m_controlRef ) ) ;
#endif
}

OSStatus wxMacControl::SendHICommand( HICommand &command , OptionBits inOptions ) 
{
    wxMacCarbonEvent event( kEventClassCommand , kEventCommandProcess ) ;
    event.SetParameter<HICommand>(kEventParamDirectObject,command) ;
    return SendEvent( event , inOptions ) ;     
}

OSStatus wxMacControl::SendHICommand( UInt32 commandID , OptionBits inOptions  ) 
{
    HICommand command ;
    memset( &command, 0 , sizeof(command) ) ;
    command.commandID = commandID ;
    return SendHICommand( command , inOptions ) ;
}

void wxMacControl::Flash( ControlPartCode part , UInt32 ticks ) 
{
    HiliteControl( m_controlRef , part ) ;
    unsigned long finalTicks ;
    Delay( ticks , &finalTicks ) ;
    HiliteControl( m_controlRef , kControlNoPart ) ;
}

SInt32 wxMacControl::GetValue() const
{ 
    return ::GetControl32BitValue( m_controlRef ) ; 
}

SInt32 wxMacControl::GetMaximum() const
{ 
    return ::GetControl32BitMaximum( m_controlRef ) ; 
}

SInt32 wxMacControl::GetMinimum() const
{ 
    return ::GetControl32BitMinimum( m_controlRef ) ; 
}

void wxMacControl::SetValue( SInt32 v ) 
{ 
    ::SetControl32BitValue( m_controlRef , v ) ; 
}

void wxMacControl::SetMinimum( SInt32 v ) 
{ 
    ::SetControl32BitMinimum( m_controlRef , v ) ; 
}

void wxMacControl::SetMaximum( SInt32 v ) 
{ 
    ::SetControl32BitMaximum( m_controlRef , v ) ;
}

void wxMacControl::SetValueAndRange( SInt32 value , SInt32 minimum , SInt32 maximum )
{
    ::SetControl32BitMinimum( m_controlRef , minimum ) ;
    ::SetControl32BitMaximum( m_controlRef , maximum ) ; 
    ::SetControl32BitValue( m_controlRef , value ) ;
}

OSStatus wxMacControl::SetFocus( ControlFocusPart focusPart ) 
{
    return SetKeyboardFocus(  GetControlOwner( m_controlRef )  , 
        m_controlRef , focusPart ) ;
}

bool wxMacControl::HasFocus() const 
{
    ControlRef control ;
    GetKeyboardFocus( GetUserFocusWindow() , &control ) ;
    return control == m_controlRef ;
}

bool wxMacControl::NeedsFocusRect() const 
{
    return false ;
}

void wxMacControl::VisibilityChanged(bool shown) 
{
}

void wxMacControl::SetFont( const wxFont & font , const wxColour& foreground , long windowStyle ) 
{
    m_font = font ;
	ControlFontStyleRec	fontStyle;
	if ( font.MacGetThemeFontID() != kThemeCurrentPortFont )
	{
	    switch( font.MacGetThemeFontID() )
	    {
	        case kThemeSmallSystemFont :    fontStyle.font = kControlFontSmallSystemFont ;  break ;
	        case 109 /*mini font */ :       fontStyle.font = -5 ;                           break ;
	        case kThemeSystemFont :         fontStyle.font = kControlFontBigSystemFont ;    break ;
	        default :                       fontStyle.font = kControlFontBigSystemFont ;    break ;
	    }
	    fontStyle.flags = kControlUseFontMask ; 
	}
	else
	{
	    fontStyle.font = font.MacGetFontNum() ;
	    fontStyle.style = font.MacGetFontStyle() ;
	    fontStyle.size = font.MacGetFontSize() ;
	    fontStyle.flags = kControlUseFontMask | kControlUseFaceMask | kControlUseSizeMask ;
	}

    fontStyle.just = teJustLeft ;
    fontStyle.flags |= kControlUseJustMask ;
    if ( ( windowStyle & wxALIGN_MASK ) & wxALIGN_CENTER_HORIZONTAL )
        fontStyle.just = teJustCenter ;
    else if ( ( windowStyle & wxALIGN_MASK ) & wxALIGN_RIGHT )
        fontStyle.just = teJustRight ;

    
    // we only should do this in case of a non-standard color, as otherwise 'disabled' controls
    // won't get grayed out by the system anymore
    
    if ( foreground != *wxBLACK )
    {
        fontStyle.foreColor = MAC_WXCOLORREF(foreground.GetPixel() ) ;
        fontStyle.flags |= kControlUseForeColorMask ;
    }
	
	::SetControlFontStyle( m_controlRef , &fontStyle );
}

void wxMacControl::SetBackground( const wxBrush &WXUNUSED(brush) ) 
{
    // TODO 
    // setting up a color proc is not recommended anymore
}

void wxMacControl::SetRange( SInt32 minimum , SInt32 maximum )
{
    ::SetControl32BitMinimum( m_controlRef , minimum ) ;
    ::SetControl32BitMaximum( m_controlRef , maximum ) ; 
}

short wxMacControl::HandleKey(  SInt16 keyCode,  SInt16 charCode, EventModifiers modifiers ) 
{
    return HandleControlKey( m_controlRef , keyCode , charCode , modifiers ) ;
}

void wxMacControl::SetActionProc( ControlActionUPP   actionProc )
{
    SetControlAction( m_controlRef , actionProc ) ;
}

void wxMacControl::SetViewSize( SInt32 viewSize )
{
    SetControlViewSize(m_controlRef , viewSize ) ;
}

SInt32 wxMacControl::GetViewSize() const
{
    return GetControlViewSize( m_controlRef ) ;
}

bool wxMacControl::IsVisible() const 
{
    return IsControlVisible( m_controlRef ) ;
}

void wxMacControl::SetVisibility( bool visible , bool redraw ) 
{
    SetControlVisibility( m_controlRef , visible , redraw ) ;
}

bool wxMacControl::IsEnabled() const 
{
#if TARGET_API_MAC_OSX
    return IsControlEnabled( m_controlRef ) ;
#else
    return IsControlActive( m_controlRef ) ;
#endif
}

bool wxMacControl::IsActive() const 
{
    return IsControlActive( m_controlRef ) ;
}

void wxMacControl::Enable( bool enable ) 
{
#if TARGET_API_MAC_OSX
    if ( enable )
        EnableControl( m_controlRef ) ;
    else
        DisableControl( m_controlRef ) ;
#else
    if ( enable )
        ActivateControl( m_controlRef ) ;
    else
        DeactivateControl( m_controlRef ) ;
#endif
}

void wxMacControl::SetDrawingEnabled( bool enable ) 
{
#if TARGET_API_MAC_OSX
    HIViewSetDrawingEnabled( m_controlRef , enable ) ;
#endif
}

bool wxMacControl::GetNeedsDisplay() const 
{
#if TARGET_API_MAC_OSX
    return HIViewGetNeedsDisplay( m_controlRef ) ;
#else
    return false ;
#endif
}

void wxMacControl::SetNeedsDisplay( bool needsDisplay , RgnHandle where ) 
{
#if TARGET_API_MAC_OSX
    if ( where != NULL )
        HIViewSetNeedsDisplayInRegion( m_controlRef , where , needsDisplay ) ;
    else
        HIViewSetNeedsDisplay( m_controlRef , needsDisplay ) ;
#endif
}

void  wxMacControl::Convert( wxPoint *pt , wxMacControl *from , wxMacControl *to ) 
{
#if TARGET_API_MAC_OSX
    HIPoint hiPoint ;
    hiPoint.x = pt->x ;
    hiPoint.y = pt->y ;
    HIViewConvertPoint( &hiPoint , from->m_controlRef , to->m_controlRef  ) ;
    pt->x = (int)hiPoint.x ;
    pt->y = (int)hiPoint.y ;
#endif
}

void wxMacControl::SetRect( Rect *r ) 
{
#if TARGET_API_MAC_OSX
	//A HIRect is actually a CGRect on OSX - which consists of two structures -
	//CGPoint and CGSize, which have two floats each
    HIRect hir = { { r->left , r->top }, { r->right - r->left , r->bottom - r->top } } ;
    HIViewSetFrame ( m_controlRef , &hir ) ;
#else
    SetControlBounds( m_controlRef , r ) ;
#endif

}

void wxMacControl::GetRect( Rect *r ) 
{
    GetControlBounds( m_controlRef , r ) ;
}

void wxMacControl::GetRectInWindowCoords( Rect *r ) 
{
    UMAGetControlBoundsInWindowCoords( m_controlRef , r ) ;
}

void wxMacControl::GetBestRect( Rect *r ) 
{
    short   baselineoffset ;
    GetBestControlRect( m_controlRef , r , &baselineoffset ) ;
}

void wxMacControl::SetTitle( const wxString &title ) 
{
    wxFontEncoding encoding;

    if ( m_font.Ok() )
        encoding = m_font.GetEncoding();
    else
        encoding = wxFont::GetDefaultEncoding();
    
    UMASetControlTitle(  m_controlRef , title , encoding ) ;
}

void wxMacControl::GetFeatures( UInt32 * features )
{
    GetControlFeatures( m_controlRef , features ) ;
}

OSStatus wxMacControl::GetRegion( ControlPartCode partCode , RgnHandle region ) 
{
    return GetControlRegion( m_controlRef , partCode , region ) ;
}

OSStatus wxMacControl::SetZOrder( bool above , wxMacControl* other ) 
{
#if TARGET_API_MAC_OSX
    return HIViewSetZOrder( m_controlRef,above ? kHIViewZOrderAbove : kHIViewZOrderBelow, 
       (other != NULL) ? other->m_controlRef : NULL) ;
#else
    return 0 ;
#endif
}


#if TARGET_API_MAC_OSX
// SetNeedsDisplay would not invalidate the children
static void InvalidateControlAndChildren( HIViewRef control )
{
    HIViewSetNeedsDisplay( control , true ) ;
    UInt16 childrenCount = 0 ;
    OSStatus err = CountSubControls( control , &childrenCount ) ; 
    if ( err == errControlIsNotEmbedder )
        return ;
    wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") ) ;

    for ( UInt16 i = childrenCount ; i >=1  ; --i )
    {
        HIViewRef child ;
        err = GetIndexedSubControl( control , i , & child ) ;
        if ( err == errControlIsNotEmbedder )
            return ;
        InvalidateControlAndChildren( child ) ;
    }
}
#endif

void wxMacControl::InvalidateWithChildren() 
{
#if TARGET_API_MAC_OSX
    InvalidateControlAndChildren( m_controlRef ) ;
#endif
}

void wxMacControl::ScrollRect( const wxRect &r , int dx , int dy ) 
{
#if TARGET_API_MAC_OSX
    HIRect scrollarea = CGRectMake( r.x , r.y , r.width , r.height) ;
    HIViewScrollRect ( m_controlRef , &scrollarea , dx ,dy ) ;
#endif
}


// SetNeedsDisplay would not invalidate the children

//
// Databrowser
//

OSStatus wxMacControl::SetSelectionFlags( DataBrowserSelectionFlags options ) 
{
    return SetDataBrowserSelectionFlags( m_controlRef , options ) ;
}

OSStatus wxMacControl::AddListViewColumn( DataBrowserListViewColumnDesc *columnDesc,
        DataBrowserTableViewColumnIndex position ) 
{
    return AddDataBrowserListViewColumn( m_controlRef , columnDesc, position ) ;
}

OSStatus wxMacControl::AutoSizeListViewColumns()
{
    return AutoSizeDataBrowserListViewColumns(m_controlRef) ;
}

OSStatus wxMacControl::SetHasScrollBars( bool horiz , bool vert ) 
{
    return SetDataBrowserHasScrollBars( m_controlRef , horiz , vert ) ;
}

OSStatus wxMacControl::SetTableViewHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle )
{
    return SetDataBrowserTableViewHiliteStyle( m_controlRef , hiliteStyle ) ;
}

OSStatus wxMacControl::SetListViewHeaderBtnHeight(UInt16 height) 
{
    return SetDataBrowserListViewHeaderBtnHeight( m_controlRef ,height ) ;
}

OSStatus wxMacControl::SetCallbacks(const DataBrowserCallbacks *  callbacks) 
{
    return SetDataBrowserCallbacks( m_controlRef , callbacks ) ;
}

OSStatus wxMacControl::UpdateItems( DataBrowserItemID container, UInt32 numItems,
        const DataBrowserItemID* items,                
        DataBrowserPropertyID preSortProperty,
        DataBrowserPropertyID propertyID ) 
{
    return UpdateDataBrowserItems( m_controlRef , container, numItems, items, preSortProperty, propertyID ) ;
}

bool wxMacControl::IsItemSelected( DataBrowserItemID item ) 
{
    return IsDataBrowserItemSelected( m_controlRef , item ) ;
}

OSStatus wxMacControl::AddItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID* items,                
            DataBrowserPropertyID preSortProperty ) 
{
    return AddDataBrowserItems( m_controlRef , container, numItems, items, preSortProperty ) ;
}

OSStatus wxMacControl::RemoveItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID* items,                
            DataBrowserPropertyID preSortProperty ) 
{
    return RemoveDataBrowserItems( m_controlRef , container, numItems, items, preSortProperty ) ;
}

OSStatus wxMacControl::RevealItem( DataBrowserItemID item,
            DataBrowserPropertyID propertyID,
            DataBrowserRevealOptions options ) 
{
    return RevealDataBrowserItem( m_controlRef , item , propertyID , options ) ;
}

OSStatus wxMacControl::SetSelectedItems(UInt32 numItems,
            const DataBrowserItemID * items,
            DataBrowserSetOption operation ) 
{
    return SetDataBrowserSelectedItems( m_controlRef , numItems , items, operation ) ;
}

//
// Tab Control
//
 
OSStatus wxMacControl::SetTabEnabled( SInt16 tabNo , bool enable ) 
{
    return ::SetTabEnabled( m_controlRef , tabNo , enable ) ;
}
 
#endif // wxUSE_GUI

