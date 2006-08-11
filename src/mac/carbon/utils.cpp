/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/utils.cpp
// Purpose:     Various utilities
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #if wxUSE_GUI
        #include "wx/toplevel.h"
        #include "wx/font.h"
    #endif
#endif

#include "wx/apptrait.h"

#if wxUSE_GUI
    #include "wx/mac/uma.h"
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

#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <ATSUnicode.h>
    #include <TextCommon.h>
    #include <TextEncodingConverter.h>
#endif
#endif // wxUSE_GUI

#include "wx/mac/private.h"

#if defined(__MWERKS__) && wxUSE_UNICODE
#if __MWERKS__ < 0x4100 || !defined(__DARWIN__)
    #include <wtime.h>
#endif
#endif

// ---------------------------------------------------------------------------
// code used in both base and GUI compilation
// ---------------------------------------------------------------------------

// our OS version is the same in non GUI and GUI cases
static int DoGetOSVersion(int *majorVsn, int *minorVsn)
{
    long theSystem;

    // are there x-platform conventions ?

    Gestalt(gestaltSystemVersion, &theSystem);
    if (minorVsn != NULL)
        *minorVsn = (theSystem & 0xFF);

    if (majorVsn != NULL)
        *majorVsn = (theSystem >> 8);

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

#if defined(__WXDEBUG__) && defined(__WXMAC__) && !defined(__DARWIN__) && defined(__MWERKS__) && (__MWERKS__ >= 0x2400)

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


Boolean IsMetroNubInstalled()
{
    if (!fRunOnce)
    {
        long result, value;

        fRunOnce = true;
        gMetroNubEntry = NULL;

        if (Gestalt(gestaltSystemVersion, &value) == noErr && value < 0x1000)
        {
            // look for MetroNub's Gestalt selector
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

                    // make sure the version of the API is compatible
                    if (block->apiLowVersion <= kMetroNubUserAPIVersion &&
                        kMetroNubUserAPIVersion <= block->apiHiVersion)
                    {
                        // success!
                        gMetroNubEntry = block;
                    }
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

Boolean IsMWDebuggerRunning()
{
    if (IsMetroNubInstalled())
        return CallIsDebuggerRunningProc(gMetroNubEntry->isDebuggerRunning);

    return false;
}

Boolean AmIBeingMWDebugged()
{
    if (IsMetroNubInstalled())
        return CallAmIBeingDebuggedProc(gMetroNubEntry->amIBeingDebugged);

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

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
    return wxGetUserName( buf , maxSize );
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
    *pstr = wxMacFindFolder( (short) kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder );
    return pstr->c_str();
}

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
    // Gets Chooser name of user by examining a System resource.
    buf[0] = 0;

    const short kComputerNameID = -16413;

    short oldResFile = CurResFile();
    UseResFile(0);
    StringHandle chooserName = (StringHandle)::GetString(kComputerNameID);
    UseResFile(oldResFile);

    if (chooserName && *chooserName)
    {
        HLock( (Handle) chooserName );
        wxString name = wxMacMakeStringFromPascal( *chooserName );
        HUnlock( (Handle) chooserName );
        ReleaseResource( (Handle) chooserName );
        wxStrncpy( buf , name , maxSize - 1 );
    }

    return true;
}

// Get user name e.g. Stefan Csomor
bool wxGetUserName(wxChar *buf, int maxSize)
{
    // Gets Chooser name of user by examining a System resource.
    buf[0] = 0;

    const short kChooserNameID = -16096;

    short oldResFile = CurResFile();
    UseResFile(0);
    StringHandle chooserName = (StringHandle)::GetString(kChooserNameID);
    UseResFile(oldResFile);

    if (chooserName && *chooserName)
    {
        HLock( (Handle) chooserName );
        wxString name = wxMacMakeStringFromPascal( *chooserName );
        HUnlock( (Handle) chooserName );
        ReleaseResource( (Handle) chooserName );
        wxStrncpy( buf , name , maxSize - 1 );
    }

    return true;
}

int wxKill(long pid, wxSignal sig , wxKillError *rc, int flags)
{
    // TODO
    return 0;
}

WXDLLEXPORT bool wxGetEnv(const wxString& var, wxString *value)
{
    // TODO : under classic there is no environement support, under X yes
    return false;
}

// set the env var name to the given value, return true on success
WXDLLEXPORT bool wxSetEnv(const wxString& var, const wxChar *value)
{
    // TODO : under classic there is no environement support, under X yes
    return false;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    // TODO
    return false;
}

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags)
{
    // TODO
    return false;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
wxMemorySize wxGetFreeMemory()
{
    return (wxMemorySize)FreeMem();
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
    return false;
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
    return false;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = (float)strtod(s, NULL);
        delete[] s;
    }

    return succ;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = strtol(s, NULL, 10);
        delete[] s;
    }

    return succ;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = (int)strtol(s, NULL, 10);
        delete[] s;
    }

    return succ;
}
#endif // wxUSE_RESOURCES

int gs_wxBusyCursorCount = 0;
extern wxCursor    gMacCurrentCursor;
wxCursor        gMacStoredActiveCursor;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(const wxCursor *cursor)
{
    if (gs_wxBusyCursorCount++ == 0)
    {
        gMacStoredActiveCursor = gMacCurrentCursor;
        cursor->MacInstall();
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
        gMacStoredActiveCursor.MacInstall();
        gMacStoredActiveCursor = wxNullCursor;
    }
}

// true if we're between the above two calls
bool wxIsBusy()
{
    return (gs_wxBusyCursorCount > 0);
}

#endif // wxUSE_GUI

#if wxUSE_BASE

wxString wxMacFindFolderNoSeparator( short        vol,
              OSType       folderType,
              Boolean      createFolder)
{
    FSRef fsRef;
    wxString strDir;

    if ( FSFindFolder( vol, folderType, createFolder, &fsRef) == noErr)
    {
        strDir = wxMacFSRefToPath( &fsRef );
    }

    return strDir;
}

wxString wxMacFindFolder( short        vol,
              OSType       folderType,
              Boolean      createFolder)
{
    return wxMacFindFolderNoSeparator(vol, folderType, createFolder) + wxFILE_SEP_PATH;
}

#endif // wxUSE_BASE

#if wxUSE_GUI

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    // TODO
    return false;
}

void wxGetMousePosition( int* x, int* y )
{
    Point pt;

    GetMouse( &pt );
    LocalToGlobal( &pt );
    *x = pt.h;
    *y = pt.v;
};

// Return true if we have a colour display
bool wxColourDisplay()
{
    return true;
}

// Returns depth of screen
int wxDisplayDepth()
{
    Rect globRect;
    SetRect(&globRect, -32760, -32760, 32760, 32760);
    GDHandle    theMaxDevice;

    int theDepth = 8;
    theMaxDevice = GetMaxDevice(&globRect);
    if (theMaxDevice != NULL)
        theDepth = (**(**theMaxDevice).gdPMap).pixelSize;

    return theDepth;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    BitMap screenBits;
    GetQDGlobalsScreenBits( &screenBits );

    if (width != NULL)
        *width = screenBits.bounds.right - screenBits.bounds.left;

    if (height != NULL)
        *height = screenBits.bounds.bottom - screenBits.bounds.top;
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxDisplaySize(width, height);
    // on mac 72 is fixed (at least now;-)
    float cvPt2Mm = 25.4 / 72;

    if (width != NULL)
        *width = int( *width * cvPt2Mm );

    if (height != NULL)
        *height = int( *height * cvPt2Mm );
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    Rect r;

    GetAvailableWindowPositioningBounds( GetMainDevice() , &r );
    if ( x )
        *x = r.left;
    if ( y )
        *y = r.top;
    if ( width )
        *width = r.right - r.left;
    if ( height )
        *height = r.bottom - r.top;
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
    return wxT("MacOS"); //TODO:define further
#endif
}

#ifndef __DARWIN__
wxChar *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

bool wxGetDiskSpace(const wxString& path, wxDiskspaceSize_t *pTotal, wxDiskspaceSize_t *pFree)
{
    if ( path.empty() )
        return false;

    wxString p = path;
    if (p[0u] == ':' )
      p = wxGetCwd() + p;

    int pos = p.Find(':');
    if ( pos != wxNOT_FOUND )
      p = p.Mid(1,pos);

    p = p + wxT(":");

    OSErr err = noErr;

    FSRef fsRef;
    err = wxMacPathToFSRef( p , &fsRef );
    if ( noErr == err )
    {
        FSVolumeRefNum vRefNum;
        err = FSGetVRefNum( &fsRef , &vRefNum );
        if ( noErr == err )
        {
            UInt64 freeBytes , totalBytes;
            err = FSGetVInfo( vRefNum , NULL , &freeBytes , &totalBytes );
            if ( noErr == err )
            {
                if ( pTotal )
                    *pTotal = wxDiskspaceSize_t( totalBytes );
                if ( pFree )
                    *pFree = wxDiskspaceSize_t( freeBytes );
            }
        }
    }

    return err == noErr;
}
#endif // !__DARWIN__

//---------------------------------------------------------------------------
// wxMac Specific utility functions
//---------------------------------------------------------------------------

void wxMacStringToPascal( const wxString&from , StringPtr to )
{
    wxCharBuffer buf = from.mb_str( wxConvLocal );
    int len = strlen(buf);

    if ( len > 255 )
        len = 255;
    to[0] = len;
    memcpy( (char*) &to[1] , buf , len );
}

wxString wxMacMakeStringFromPascal( ConstStringPtr from )
{
    return wxString( (char*) &from[1] , wxConvLocal , from[0] );
}

// ----------------------------------------------------------------------------
// Common Event Support
// ----------------------------------------------------------------------------

extern ProcessSerialNumber gAppProcess;

void wxMacWakeUp()
{
    ProcessSerialNumber psn;
    Boolean isSame;
    psn.highLongOfPSN = 0;
    psn.lowLongOfPSN = kCurrentProcess;
    SameProcess( &gAppProcess , &psn , &isSame );
    if ( isSame )
    {
#if TARGET_CARBON
        OSStatus err = noErr;

#if 0
        // lead sometimes to race conditions, although all calls used should be thread safe ...
        static wxMacCarbonEvent s_wakeupEvent;
        if ( !s_wakeupEvent.IsValid() )
        {
           err = s_wakeupEvent.Create( 'WXMC', 'WXMC', GetCurrentEventTime(),
                        kEventAttributeNone );
        }
        if ( err == noErr )
        {

            if ( IsEventInQueue( GetMainEventQueue() , s_wakeupEvent ) )
                return;
            s_wakeupEvent.SetCurrentTime();
            err = PostEventToQueue(GetMainEventQueue(), s_wakeupEvent,
                                  kEventPriorityHigh );
        }
#else
        wxMacCarbonEvent wakeupEvent;
        wakeupEvent.Create( 'WXMC', 'WXMC', GetCurrentEventTime(),
                            kEventAttributeNone );
        err = PostEventToQueue(GetMainEventQueue(), wakeupEvent,
                               kEventPriorityHigh );
#endif
#else
        PostEvent( nullEvent , 0 );
#endif
    }
    else
    {
        WakeUpProcess( &gAppProcess );
    }
}

#endif // wxUSE_BASE

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// Native Struct Conversions
// ----------------------------------------------------------------------------

void wxMacRectToNative( const wxRect *wx , Rect *n )
{
    n->left = wx->x;
    n->top = wx->y;
    n->right = wx->x + wx->width;
    n->bottom = wx->y + wx->height;
}

void wxMacNativeToRect( const Rect *n , wxRect* wx )
{
    wx->x = n->left;
    wx->y = n->top;
    wx->width = n->right - n->left;
    wx->height = n->bottom - n->top;
}

void wxMacPointToNative( const wxPoint* wx , Point *n )
{
    n->h = wx->x;
    n->v = wx->y;
}

void wxMacNativeToPoint( const Point *n , wxPoint* wx )
{
    wx->x = n->h;
    wx->y = n->v;
}

// ----------------------------------------------------------------------------
// Carbon Event Support
// ----------------------------------------------------------------------------

OSStatus wxMacCarbonEvent::GetParameter(EventParamName inName, EventParamType inDesiredType, UInt32 inBufferSize, void * outData)
{
    return ::GetEventParameter( m_eventRef , inName , inDesiredType , NULL , inBufferSize , NULL , outData );
}

OSStatus wxMacCarbonEvent::SetParameter(EventParamName inName, EventParamType inType, UInt32 inBufferSize, const void * inData)
{
    return ::SetEventParameter( m_eventRef , inName , inType , inBufferSize , inData );
}

// ----------------------------------------------------------------------------
// Control Access Support
// ----------------------------------------------------------------------------

wxMacControl::wxMacControl(wxWindow* peer , bool isRootControl )
{
    Init();
    m_peer = peer;
    m_isRootControl = isRootControl;
}

wxMacControl::wxMacControl( wxWindow* peer , ControlRef control )
{
    Init();
    m_peer = peer;
    m_controlRef = control;
}

wxMacControl::wxMacControl( wxWindow* peer , WXWidget control )
{
    Init();
    m_peer = peer;
    m_controlRef = (ControlRef) control;
}

wxMacControl::~wxMacControl()
{
}

void wxMacControl::Init()
{
    m_peer = NULL;
    m_controlRef = NULL;
    m_needsFocusRect = false;
    m_isRootControl = false;
}

void wxMacControl::Dispose()
{
    ::DisposeControl( m_controlRef );
    m_controlRef = NULL;
}

void wxMacControl::SetReference( SInt32 data )
{
    SetControlReference( m_controlRef , data );
}

OSStatus wxMacControl::GetData(ControlPartCode inPartCode , ResType inTag , Size inBufferSize , void * inOutBuffer , Size * outActualSize ) const
{
    return ::GetControlData( m_controlRef , inPartCode , inTag , inBufferSize , inOutBuffer , outActualSize );
}

OSStatus wxMacControl::GetDataSize(ControlPartCode inPartCode , ResType inTag , Size * outActualSize ) const
{
    return ::GetControlDataSize( m_controlRef , inPartCode , inTag , outActualSize );
}

OSStatus wxMacControl::SetData(ControlPartCode inPartCode , ResType inTag , Size inSize , const void * inData)
{
    return ::SetControlData( m_controlRef , inPartCode , inTag , inSize , inData );
}

OSStatus wxMacControl::SendEvent( EventRef event , OptionBits inOptions )
{
#if TARGET_API_MAC_OSX
    return SendEventToEventTargetWithOptions( event,
        HIObjectGetEventTarget( (HIObjectRef) m_controlRef ), inOptions );
#else
    #pragma unused(inOptions)
    return SendEventToEventTarget(event,GetControlEventTarget( m_controlRef ) );
#endif
}

OSStatus wxMacControl::SendHICommand( HICommand &command , OptionBits inOptions )
{
    wxMacCarbonEvent event( kEventClassCommand , kEventCommandProcess );

    event.SetParameter<HICommand>(kEventParamDirectObject,command);

    return SendEvent( event , inOptions );
}

OSStatus wxMacControl::SendHICommand( UInt32 commandID , OptionBits inOptions  )
{
    HICommand command;

    memset( &command, 0 , sizeof(command) );
    command.commandID = commandID;
    return SendHICommand( command , inOptions );
}

void wxMacControl::Flash( ControlPartCode part , UInt32 ticks )
{
    unsigned long finalTicks;

    HiliteControl( m_controlRef , part );
    Delay( ticks , &finalTicks );
    HiliteControl( m_controlRef , kControlNoPart );
}

SInt32 wxMacControl::GetValue() const
{
    return ::GetControl32BitValue( m_controlRef );
}

SInt32 wxMacControl::GetMaximum() const
{
    return ::GetControl32BitMaximum( m_controlRef );
}

SInt32 wxMacControl::GetMinimum() const
{
    return ::GetControl32BitMinimum( m_controlRef );
}

void wxMacControl::SetValue( SInt32 v )
{
    ::SetControl32BitValue( m_controlRef , v );
}

void wxMacControl::SetMinimum( SInt32 v )
{
    ::SetControl32BitMinimum( m_controlRef , v );
}

void wxMacControl::SetMaximum( SInt32 v )
{
    ::SetControl32BitMaximum( m_controlRef , v );
}

void wxMacControl::SetValueAndRange( SInt32 value , SInt32 minimum , SInt32 maximum )
{
    ::SetControl32BitMinimum( m_controlRef , minimum );
    ::SetControl32BitMaximum( m_controlRef , maximum );
    ::SetControl32BitValue( m_controlRef , value );
}

OSStatus wxMacControl::SetFocus( ControlFocusPart focusPart )
{
    return SetKeyboardFocus( GetControlOwner( m_controlRef ), m_controlRef, focusPart );
}

bool wxMacControl::HasFocus() const
{
    ControlRef control;
    GetKeyboardFocus( GetUserFocusWindow() , &control );
    return control == m_controlRef;
}

void wxMacControl::SetNeedsFocusRect( bool needs )
{
    m_needsFocusRect = needs;
}

bool wxMacControl::NeedsFocusRect() const
{
    return m_needsFocusRect;
}

void wxMacControl::VisibilityChanged(bool shown)
{
}

void wxMacControl::SuperChangedPosition()
{
}

void wxMacControl::SetFont( const wxFont & font , const wxColour& foreground , long windowStyle )
{
    m_font = font;
    ControlFontStyleRec fontStyle;
    if ( font.MacGetThemeFontID() != kThemeCurrentPortFont )
    {
        switch ( font.MacGetThemeFontID() )
        {
            case kThemeSmallSystemFont :
                fontStyle.font = kControlFontSmallSystemFont;
                break;

            case 109 : // mini font
                fontStyle.font = -5;
                break;

            case kThemeSystemFont :
                fontStyle.font = kControlFontBigSystemFont;
                break;

            default :
                fontStyle.font = kControlFontBigSystemFont;
                break;
        }

        fontStyle.flags = kControlUseFontMask;
    }
    else
    {
        fontStyle.font = font.MacGetFontNum();
        fontStyle.style = font.MacGetFontStyle();
        fontStyle.size = font.MacGetFontSize();
        fontStyle.flags = kControlUseFontMask | kControlUseFaceMask | kControlUseSizeMask;
    }

    fontStyle.just = teJustLeft;
    fontStyle.flags |= kControlUseJustMask;
    if ( ( windowStyle & wxALIGN_MASK ) & wxALIGN_CENTER_HORIZONTAL )
        fontStyle.just = teJustCenter;
    else if ( ( windowStyle & wxALIGN_MASK ) & wxALIGN_RIGHT )
        fontStyle.just = teJustRight;


    // we only should do this in case of a non-standard color, as otherwise 'disabled' controls
    // won't get grayed out by the system anymore

    if ( foreground != *wxBLACK )
    {
        fontStyle.foreColor = MAC_WXCOLORREF( foreground.GetPixel() );
        fontStyle.flags |= kControlUseForeColorMask;
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
    ::SetControl32BitMinimum( m_controlRef , minimum );
    ::SetControl32BitMaximum( m_controlRef , maximum );
}

short wxMacControl::HandleKey( SInt16 keyCode,  SInt16 charCode, EventModifiers modifiers )
{
    return HandleControlKey( m_controlRef , keyCode , charCode , modifiers );
}

void wxMacControl::SetActionProc( ControlActionUPP   actionProc )
{
    SetControlAction( m_controlRef , actionProc );
}

void wxMacControl::SetViewSize( SInt32 viewSize )
{
    SetControlViewSize(m_controlRef , viewSize );
}

SInt32 wxMacControl::GetViewSize() const
{
    return GetControlViewSize( m_controlRef );
}

bool wxMacControl::IsVisible() const
{
    return IsControlVisible( m_controlRef );
}

void wxMacControl::SetVisibility( bool visible , bool redraw )
{
    SetControlVisibility( m_controlRef , visible , redraw );
}

bool wxMacControl::IsEnabled() const
{
#if TARGET_API_MAC_OSX
    return IsControlEnabled( m_controlRef );
#else
    return IsControlActive( m_controlRef );
#endif
}

bool wxMacControl::IsActive() const
{
    return IsControlActive( m_controlRef );
}

void wxMacControl::Enable( bool enable )
{
    if ( enable )
        EnableControl( m_controlRef );
    else
        DisableControl( m_controlRef );
}

void wxMacControl::SetDrawingEnabled( bool enable )
{
    HIViewSetDrawingEnabled( m_controlRef , enable );
}

bool wxMacControl::GetNeedsDisplay() const
{
    return HIViewGetNeedsDisplay( m_controlRef );
}

void wxMacControl::SetNeedsDisplay( RgnHandle where )
{
    if ( !IsVisible() )
        return;

    HIViewSetNeedsDisplayInRegion( m_controlRef , where , true );
}

void wxMacControl::SetNeedsDisplay( Rect* where )
{
    if ( !IsVisible() )
        return;

    if ( where != NULL )
    {
        RgnHandle update = NewRgn();
        RectRgn( update , where );
        HIViewSetNeedsDisplayInRegion( m_controlRef , update , true );
        DisposeRgn( update );
    }
    else
        HIViewSetNeedsDisplay( m_controlRef , true );
}

void  wxMacControl::Convert( wxPoint *pt , wxMacControl *from , wxMacControl *to )
{
    HIPoint hiPoint;

    hiPoint.x = pt->x;
    hiPoint.y = pt->y;
    HIViewConvertPoint( &hiPoint , from->m_controlRef , to->m_controlRef  );
    pt->x = (int)hiPoint.x;
    pt->y = (int)hiPoint.y;
}

void wxMacControl::SetRect( Rect *r )
{
        //A HIRect is actually a CGRect on OSX - which consists of two structures -
        //CGPoint and CGSize, which have two floats each
        HIRect hir = { { r->left , r->top }, { r->right - r->left , r->bottom - r->top } };
        HIViewSetFrame ( m_controlRef , &hir );
        // eventuall we might have to do a SetVisibility( false , true );
        // before and a SetVisibility( true , true ); after
 }

void wxMacControl::GetRect( Rect *r )
{
    GetControlBounds( m_controlRef , r );
}

void wxMacControl::GetRectInWindowCoords( Rect *r )
{
    UMAGetControlBoundsInWindowCoords( m_controlRef , r );
}

void wxMacControl::GetBestRect( Rect *r )
{
    short   baselineoffset;

    GetBestControlRect( m_controlRef , r , &baselineoffset );
}

void wxMacControl::SetLabel( const wxString &title )
{
    wxFontEncoding encoding;

    if ( m_font.Ok() )
        encoding = m_font.GetEncoding();
    else
        encoding = wxFont::GetDefaultEncoding();

    UMASetControlTitle( m_controlRef , title , encoding );
}

void wxMacControl::GetFeatures( UInt32 * features )
{
    GetControlFeatures( m_controlRef , features );
}

OSStatus wxMacControl::GetRegion( ControlPartCode partCode , RgnHandle region )
{
    OSStatus err = GetControlRegion( m_controlRef , partCode , region );
    return err;
}

OSStatus wxMacControl::SetZOrder( bool above , wxMacControl* other )
{
#if TARGET_API_MAC_OSX
    return HIViewSetZOrder( m_controlRef,above ? kHIViewZOrderAbove : kHIViewZOrderBelow,
       (other != NULL) ? other->m_controlRef : NULL);
#else
    return 0;
#endif
}

#if TARGET_API_MAC_OSX
// SetNeedsDisplay would not invalidate the children
static void InvalidateControlAndChildren( HIViewRef control )
{
    HIViewSetNeedsDisplay( control , true );
    UInt16 childrenCount = 0;
    OSStatus err = CountSubControls( control , &childrenCount );
    if ( err == errControlIsNotEmbedder )
        return;

    wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") );

    for ( UInt16 i = childrenCount; i >=1; --i )
    {
        HIViewRef child;

        err = GetIndexedSubControl( control , i , & child );
        if ( err == errControlIsNotEmbedder )
            return;

        InvalidateControlAndChildren( child );
    }
}
#endif

void wxMacControl::InvalidateWithChildren()
{
#if TARGET_API_MAC_OSX
    InvalidateControlAndChildren( m_controlRef );
#endif
}

void wxMacControl::ScrollRect( wxRect *r , int dx , int dy )
{
    wxASSERT( r != NULL );

    HIRect scrollarea = CGRectMake( r->x , r->y , r->width , r->height);
    HIViewScrollRect ( m_controlRef , &scrollarea , dx ,dy );
}

OSType wxMacCreator = 'WXMC';
OSType wxMacControlProperty = 'MCCT';

void wxMacControl::SetReferenceInNativeControl()
{
    void * data = this;
    verify_noerr( SetControlProperty ( m_controlRef ,
        wxMacCreator,wxMacControlProperty, sizeof(data), &data ) );
}

wxMacControl* wxMacControl::GetReferenceFromNativeControl(ControlRef control)
{
    wxMacControl* ctl = NULL;
    UInt32 actualSize;
    if ( GetControlProperty( control ,wxMacCreator,wxMacControlProperty, sizeof(ctl) ,
        &actualSize , &ctl ) == noErr )
    {
        return ctl;
    }
    return NULL;
}

// ============================================================================
// DataBrowser Wrapper
// ============================================================================
//
// basing on DataBrowserItemIDs
//

pascal void wxMacDataBrowserControl::DataBrowserItemNotificationProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData )
{
    wxMacDataBrowserControl* ctl = dynamic_cast<wxMacDataBrowserControl*>( wxMacControl::GetReferenceFromNativeControl( browser ) );
    if ( ctl != 0 )
    {
        ctl->ItemNotification(itemID, message, itemData);
    }
}

pascal OSStatus wxMacDataBrowserControl::DataBrowserGetSetItemDataProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    Boolean changeValue )
{
    OSStatus err = errDataBrowserPropertyNotSupported;

    wxMacDataBrowserControl* ctl = dynamic_cast<wxMacDataBrowserControl*>( wxMacControl::GetReferenceFromNativeControl( browser ) );
    if ( ctl != 0 )
    {
        err = ctl->GetSetItemData(itemID, property, itemData, changeValue);
    }
    return err;
}

pascal Boolean wxMacDataBrowserControl::DataBrowserCompareProc(
    ControlRef browser,
    DataBrowserItemID itemOneID,
    DataBrowserItemID itemTwoID,
    DataBrowserPropertyID sortProperty)
{
    wxMacDataBrowserControl* ctl = dynamic_cast<wxMacDataBrowserControl*>( wxMacControl::GetReferenceFromNativeControl( browser ) );
    if ( ctl != 0 )
    {
        return ctl->CompareItems(itemOneID, itemTwoID, sortProperty);
    }
    return false;
}

DataBrowserItemDataUPP gDataBrowserItemDataUPP = NULL;
DataBrowserItemNotificationUPP gDataBrowserItemNotificationUPP = NULL;
DataBrowserItemCompareUPP gDataBrowserItemCompareUPP = NULL;

wxMacDataBrowserControl::wxMacDataBrowserControl( wxWindow* peer, const wxPoint& pos, const wxSize& size, long style) : wxMacControl( peer )
{
    Rect bounds = wxMacGetBoundsForControl( peer, pos, size );
    OSStatus err = ::CreateDataBrowserControl(
        MAC_WXHWND(peer->MacGetTopLevelWindowRef()),
        &bounds, kDataBrowserListView, &m_controlRef );
    SetReferenceInNativeControl();
    verify_noerr( err );
    if ( gDataBrowserItemCompareUPP == NULL )
        gDataBrowserItemCompareUPP = NewDataBrowserItemCompareUPP(DataBrowserCompareProc);
    if ( gDataBrowserItemDataUPP == NULL )
        gDataBrowserItemDataUPP = NewDataBrowserItemDataUPP(DataBrowserGetSetItemDataProc);
    if ( gDataBrowserItemNotificationUPP == NULL )
    {
        gDataBrowserItemNotificationUPP =
#if TARGET_API_MAC_OSX
            (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationProc);
#else
            NewDataBrowserItemNotificationUPP(DataBrowserItemNotificationProc);
#endif
    }

    DataBrowserCallbacks callbacks;
    InitializeDataBrowserCallbacks( &callbacks, kDataBrowserLatestCallbacks );

    callbacks.u.v1.itemDataCallback = gDataBrowserItemDataUPP;
    callbacks.u.v1.itemCompareCallback = gDataBrowserItemCompareUPP;
    callbacks.u.v1.itemNotificationCallback = gDataBrowserItemNotificationUPP;
    SetCallbacks( &callbacks );

}

OSStatus wxMacDataBrowserControl::GetItemCount( DataBrowserItemID container,
    Boolean recurse,
    DataBrowserItemState state,
    UInt32 *numItems) const
{
    return GetDataBrowserItemCount( m_controlRef, container, recurse, state, numItems );
}

OSStatus wxMacDataBrowserControl::GetItems( DataBrowserItemID container,
    Boolean recurse,
    DataBrowserItemState state,
    Handle items) const
{
    return GetDataBrowserItems( m_controlRef, container, recurse, state, items );
}

OSStatus wxMacDataBrowserControl::SetSelectionFlags( DataBrowserSelectionFlags options )
{
    return SetDataBrowserSelectionFlags( m_controlRef, options );
}

OSStatus wxMacDataBrowserControl::AddColumn( DataBrowserListViewColumnDesc *columnDesc,
        DataBrowserTableViewColumnIndex position )
{
    return AddDataBrowserListViewColumn( m_controlRef, columnDesc, position );
}

OSStatus wxMacDataBrowserControl::AutoSizeColumns()
{
    return AutoSizeDataBrowserListViewColumns(m_controlRef);
}

OSStatus wxMacDataBrowserControl::SetHasScrollBars( bool horiz, bool vert )
{
    return SetDataBrowserHasScrollBars( m_controlRef, horiz, vert );
}

OSStatus wxMacDataBrowserControl::SetHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle )
{
    return SetDataBrowserTableViewHiliteStyle( m_controlRef, hiliteStyle );
}

OSStatus wxMacDataBrowserControl::SetHeaderButtonHeight(UInt16 height)
{
    return SetDataBrowserListViewHeaderBtnHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserControl::GetHeaderButtonHeight(UInt16 *height)
{
    return GetDataBrowserListViewHeaderBtnHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserControl::SetCallbacks(const DataBrowserCallbacks *callbacks)
{
    return SetDataBrowserCallbacks( m_controlRef, callbacks );
}

OSStatus wxMacDataBrowserControl::UpdateItems(
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty,
    DataBrowserPropertyID propertyID ) const
{
    return UpdateDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty, propertyID );
}

bool wxMacDataBrowserControl::IsItemSelected( DataBrowserItemID item ) const
{
    return IsDataBrowserItemSelected( m_controlRef, item );
}

OSStatus wxMacDataBrowserControl::AddItems(
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty )
{
    return AddDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty );
}

OSStatus wxMacDataBrowserControl::RemoveItems(
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty )
{
    return RemoveDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty );
}

OSStatus wxMacDataBrowserControl::RevealItem(
    DataBrowserItemID item,
    DataBrowserPropertyID propertyID,
    DataBrowserRevealOptions options ) const
{
    return RevealDataBrowserItem( m_controlRef, item, propertyID, options );
}

OSStatus wxMacDataBrowserControl::SetSelectedItems(
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserSetOption operation )
{
    return SetDataBrowserSelectedItems( m_controlRef, numItems, items, operation );
}

OSStatus wxMacDataBrowserControl::GetSelectionAnchor( DataBrowserItemID *first, DataBrowserItemID *last ) const
{
    return GetDataBrowserSelectionAnchor( m_controlRef, first, last );
}

OSStatus wxMacDataBrowserControl::GetItemID( DataBrowserTableViewRowIndex row, DataBrowserItemID * item ) const
{
    return GetDataBrowserTableViewItemID( m_controlRef, row, item );
}

OSStatus wxMacDataBrowserControl::GetItemRow( DataBrowserItemID item, DataBrowserTableViewRowIndex * row ) const
{
    return GetDataBrowserTableViewItemRow( m_controlRef, item, row );
}

OSStatus wxMacDataBrowserControl::SetDefaultRowHeight( UInt16 height )
{
    return SetDataBrowserTableViewRowHeight( m_controlRef , height );
}

OSStatus wxMacDataBrowserControl::GetDefaultRowHeight( UInt16 * height ) const
{
    return GetDataBrowserTableViewRowHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserControl::SetRowHeight( DataBrowserItemID item , UInt16 height)
{
    return SetDataBrowserTableViewItemRowHeight( m_controlRef, item , height );
}

OSStatus wxMacDataBrowserControl::GetRowHeight( DataBrowserItemID item , UInt16 *height) const
{
    return GetDataBrowserTableViewItemRowHeight( m_controlRef, item , height);
}

OSStatus wxMacDataBrowserControl::GetColumnWidth( DataBrowserPropertyID column , UInt16 *width ) const
{
    return GetDataBrowserTableViewNamedColumnWidth( m_controlRef , column , width );
}

OSStatus wxMacDataBrowserControl::SetColumnWidth( DataBrowserPropertyID column , UInt16 width )
{
    return SetDataBrowserTableViewNamedColumnWidth( m_controlRef , column , width );
}

OSStatus wxMacDataBrowserControl::GetDefaultColumnWidth( UInt16 *width ) const
{
    return GetDataBrowserTableViewColumnWidth( m_controlRef , width );
}

OSStatus wxMacDataBrowserControl::SetDefaultColumnWidth( UInt16 width )
{
    return SetDataBrowserTableViewColumnWidth( m_controlRef , width );
}

OSStatus wxMacDataBrowserControl::GetColumnCount(UInt32* numColumns) const
{
    return GetDataBrowserTableViewColumnCount( m_controlRef, numColumns);
}

OSStatus wxMacDataBrowserControl::GetColumnPosition( DataBrowserPropertyID column,
    UInt32 *position) const
{
    return GetDataBrowserTableViewColumnPosition( m_controlRef , column , position);
}

OSStatus wxMacDataBrowserControl::SetColumnPosition( DataBrowserPropertyID column, UInt32 position)
{
    return SetDataBrowserTableViewColumnPosition( m_controlRef , column , position);
}

OSStatus wxMacDataBrowserControl::GetScrollPosition( UInt32 *top , UInt32 *left ) const
{
    return GetDataBrowserScrollPosition( m_controlRef , top , left );
}

OSStatus wxMacDataBrowserControl::SetScrollPosition( UInt32 top , UInt32 left )
{
    return SetDataBrowserScrollPosition( m_controlRef , top , left );
}

OSStatus wxMacDataBrowserControl::GetSortProperty( DataBrowserPropertyID *column ) const
{
    return GetDataBrowserSortProperty( m_controlRef , column );
}

OSStatus wxMacDataBrowserControl::SetSortProperty( DataBrowserPropertyID column )
{
    return SetDataBrowserSortProperty( m_controlRef , column );
}

OSStatus wxMacDataBrowserControl::GetSortOrder( DataBrowserSortOrder *order ) const
{
    return GetDataBrowserSortOrder( m_controlRef , order );
}

OSStatus wxMacDataBrowserControl::SetSortOrder( DataBrowserSortOrder order )
{
    return SetDataBrowserSortOrder( m_controlRef , order );
}

OSStatus wxMacDataBrowserControl::GetPropertyFlags( DataBrowserPropertyID property,
    DataBrowserPropertyFlags *flags ) const
{
    return GetDataBrowserPropertyFlags( m_controlRef , property , flags );
}

OSStatus wxMacDataBrowserControl::SetPropertyFlags( DataBrowserPropertyID property,
    DataBrowserPropertyFlags flags )
{
    return SetDataBrowserPropertyFlags( m_controlRef , property , flags );
}

OSStatus wxMacDataBrowserControl::GetHeaderDesc( DataBrowserPropertyID property,
    DataBrowserListViewHeaderDesc *desc ) const
{
    return GetDataBrowserListViewHeaderDesc( m_controlRef , property , desc );
}

OSStatus wxMacDataBrowserControl::SetHeaderDesc( DataBrowserPropertyID property,
    DataBrowserListViewHeaderDesc *desc )
{
    return SetDataBrowserListViewHeaderDesc( m_controlRef , property , desc );
}

OSStatus wxMacDataBrowserControl::SetDisclosureColumn( DataBrowserPropertyID property ,
    Boolean expandableRows )
{
    return SetDataBrowserListViewDisclosureColumn( m_controlRef, property, expandableRows);
}

// ============================================================================
// Higher-level Databrowser
// ============================================================================
//
// basing on data item objects
//

wxMacDataItem::wxMacDataItem()
{
}

wxMacDataItem::~wxMacDataItem()
{
}

bool wxMacDataItem::IsLessThan(wxMacDataItemBrowserControl *owner ,
    const wxMacDataItem*,
    DataBrowserPropertyID property) const
{
    return false;
}

OSStatus wxMacDataItem::GetSetData(wxMacDataItemBrowserControl *owner ,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    bool changeValue )
{
    return errDataBrowserPropertyNotSupported;
}

void wxMacDataItem::Notification(wxMacDataItemBrowserControl *owner ,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData ) const
{
}


wxMacDataItemBrowserControl::wxMacDataItemBrowserControl( wxWindow* peer , const wxPoint& pos, const wxSize& size, long style) :
    wxMacDataBrowserControl( peer, pos, size, style )
{
    m_suppressSelection = false;
}

wxMacDataItemBrowserSelectionSuppressor::wxMacDataItemBrowserSelectionSuppressor(wxMacDataItemBrowserControl *browser)
{
    m_former = browser->SuppressSelection(true);
    m_browser = browser;
}

wxMacDataItemBrowserSelectionSuppressor::~wxMacDataItemBrowserSelectionSuppressor()
{
    m_browser->SuppressSelection(m_former);
}

bool  wxMacDataItemBrowserControl::SuppressSelection( bool suppress )
{
    bool former = m_suppressSelection;
    m_suppressSelection = suppress;

    return former;
}

Boolean wxMacDataItemBrowserControl::CompareItems(DataBrowserItemID itemOneID,
    DataBrowserItemID itemTwoID,
    DataBrowserPropertyID sortProperty)
{
    wxMacDataItem* itemOne = (wxMacDataItem*) itemOneID;
    wxMacDataItem* itemTwo = (wxMacDataItem*) itemTwoID;
    return CompareItems( itemOne , itemTwo , sortProperty );
}

Boolean wxMacDataItemBrowserControl::CompareItems(const wxMacDataItem*  itemOne,
    const wxMacDataItem*  itemTwo,
    DataBrowserPropertyID sortProperty)
{
    Boolean retval = false;
    if ( itemOne != NULL )
        retval = itemOne->IsLessThan( this , itemTwo , sortProperty);
    return retval;
}

OSStatus wxMacDataItemBrowserControl::GetSetItemData(
    DataBrowserItemID itemID,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    Boolean changeValue )
{
    wxMacDataItem* item = (wxMacDataItem*) itemID;
    return GetSetItemData(item, property, itemData , changeValue );
}

OSStatus wxMacDataItemBrowserControl::GetSetItemData(
    wxMacDataItem* item,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    Boolean changeValue )
{
    OSStatus err = errDataBrowserPropertyNotSupported;
    switch( property )
    {
        case kDataBrowserContainerIsClosableProperty :
        case kDataBrowserContainerIsSortableProperty :
        case kDataBrowserContainerIsOpenableProperty :
            // right now default behaviour on these
            break;
        default :
            if ( item != NULL )
                err = item->GetSetData( this, property , itemData , changeValue );
            break;

    }
    return err;
}

void wxMacDataItemBrowserControl::ItemNotification(
    DataBrowserItemID itemID,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData)
{
    wxMacDataItem* item = (wxMacDataItem*) itemID;
    ItemNotification( item , message, itemData);
}

void wxMacDataItemBrowserControl::ItemNotification(
    const wxMacDataItem* item,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData)
{
    if (item != NULL)
        item->Notification( this, message, itemData);
}

unsigned int wxMacDataItemBrowserControl::GetItemCount(const wxMacDataItem* container,
        bool recurse , DataBrowserItemState state) const
{
    UInt32 numItems = 0;
    verify_noerr( wxMacDataBrowserControl::GetItemCount( (DataBrowserItemID)container,
        recurse, state, &numItems ) );
    return numItems;
}

void wxMacDataItemBrowserControl::GetItems(const wxMacDataItem* container,
    bool recurse , DataBrowserItemState state, wxArrayMacDataItemPtr &items) const
{
    Handle handle = NewHandle(0);
    verify_noerr( wxMacDataBrowserControl::GetItems( (DataBrowserItemID)container ,
        recurse , state, handle) );

    int itemCount = GetHandleSize(handle)/sizeof(DataBrowserItemID);
    HLock( handle );
    wxMacDataItemPtr* itemsArray = (wxMacDataItemPtr*) *handle;
    for ( int i = 0; i < itemCount; ++i)
    {
        items.Add(itemsArray[i]);
    }
    HUnlock( handle );
    DisposeHandle( handle );
}

unsigned int wxMacDataItemBrowserControl::GetLineFromItem(const wxMacDataItem* item) const
{
    DataBrowserTableViewRowIndex row;
    OSStatus err = GetItemRow( (DataBrowserItemID) item , &row);
    wxASSERT( err == noErr);
    return row;
}

wxMacDataItem*  wxMacDataItemBrowserControl::GetItemFromLine(unsigned int n) const
{
    DataBrowserItemID id;
    OSStatus err =  GetItemID( (DataBrowserTableViewRowIndex) n , &id);
    wxASSERT( err == noErr);
    return (wxMacDataItem*) id;
}

void wxMacDataItemBrowserControl::UpdateItem(const wxMacDataItem *container,
        const wxMacDataItem *item , DataBrowserPropertyID property) const
{
    verify_noerr( wxMacDataBrowserControl::UpdateItems((DataBrowserItemID)container, 1,
        (DataBrowserItemID*) &item, kDataBrowserItemNoProperty /* notSorted */, property ) );
}

void wxMacDataItemBrowserControl::UpdateItems(const wxMacDataItem *container,
        wxArrayMacDataItemPtr &itemArray , DataBrowserPropertyID property) const
{
    unsigned int noItems = itemArray.GetCount();
    DataBrowserItemID *items = new DataBrowserItemID[noItems];
    for ( unsigned int i = 0; i < noItems; ++i )
        items[i] = (DataBrowserItemID) itemArray[i];

    verify_noerr( wxMacDataBrowserControl::UpdateItems((DataBrowserItemID)container, noItems,
        items, kDataBrowserItemNoProperty /* notSorted */, property ) );
    delete [] items;
}

void wxMacDataItemBrowserControl::AddItem(wxMacDataItem *container, wxMacDataItem *item)
{
    verify_noerr( wxMacDataBrowserControl::AddItems( (DataBrowserItemID)container, 1,
        (DataBrowserItemID*) &item, kDataBrowserItemNoProperty ) );
}

void wxMacDataItemBrowserControl::AddItems(wxMacDataItem *container, wxArrayMacDataItemPtr &itemArray )
{
    unsigned int noItems = itemArray.GetCount();
    DataBrowserItemID *items = new DataBrowserItemID[noItems];
    for ( unsigned int i = 0; i < noItems; ++i )
        items[i] = (DataBrowserItemID) itemArray[i];

    verify_noerr( wxMacDataBrowserControl::AddItems( (DataBrowserItemID)container, noItems,
        (DataBrowserItemID*) items, kDataBrowserItemNoProperty ) );
    delete [] items;
}

void wxMacDataItemBrowserControl::RemoveItem(wxMacDataItem *container, wxMacDataItem* item)
{
    OSStatus err = wxMacDataBrowserControl::RemoveItems( (DataBrowserItemID)container, 1,
        (UInt32*) &item, kDataBrowserItemNoProperty );
    verify_noerr( err );
}

void wxMacDataItemBrowserControl::RemoveItems(wxMacDataItem *container, wxArrayMacDataItemPtr &itemArray)
{
    unsigned int noItems = itemArray.GetCount();
    DataBrowserItemID *items = new DataBrowserItemID[noItems];
    for ( unsigned int i = 0; i < noItems; ++i )
        items[i] = (DataBrowserItemID) itemArray[i];

    OSStatus err = wxMacDataBrowserControl::RemoveItems( (DataBrowserItemID)container, noItems,
        (UInt32*) items, kDataBrowserItemNoProperty );
    verify_noerr( err );
    delete [] items;
}

void wxMacDataItemBrowserControl::RemoveAllItems(wxMacDataItem *container)
{
    OSStatus err = wxMacDataBrowserControl::RemoveItems( (DataBrowserItemID)container, 0 , NULL , kDataBrowserItemNoProperty );
    verify_noerr( err );
}

void wxMacDataItemBrowserControl::SetSelectedItem(wxMacDataItem* item , DataBrowserSetOption option)
{
    verify_noerr(wxMacDataBrowserControl::SetSelectedItems( 1, (DataBrowserItemID*) &item, option ));
}

void wxMacDataItemBrowserControl::SetSelectedAllItems(DataBrowserSetOption option)
{
    verify_noerr(wxMacDataBrowserControl::SetSelectedItems( 0 , NULL , option ));
}

void wxMacDataItemBrowserControl::SetSelectedItems(wxArrayMacDataItemPtr &itemArray , DataBrowserSetOption option)
{
    unsigned int noItems = itemArray.GetCount();
    DataBrowserItemID *items = new DataBrowserItemID[noItems];
    for ( unsigned int i = 0; i < noItems; ++i )
        items[i] = (DataBrowserItemID) itemArray[i];

    verify_noerr(wxMacDataBrowserControl::SetSelectedItems( noItems, (DataBrowserItemID*) items, option ));
    delete [] items;
}

Boolean wxMacDataItemBrowserControl::IsItemSelected( const wxMacDataItem* item) const
{
    return wxMacDataBrowserControl::IsItemSelected( (DataBrowserItemID) item);
}

void wxMacDataItemBrowserControl::RevealItem( wxMacDataItem* item, DataBrowserRevealOptions options)
{
    verify_noerr(wxMacDataBrowserControl::RevealItem( (DataBrowserItemID) item, kDataBrowserNoItem , options ) );
}

void wxMacDataItemBrowserControl::GetSelectionAnchor( wxMacDataItemPtr* first , wxMacDataItemPtr* last) const
{
    verify_noerr(wxMacDataBrowserControl::GetSelectionAnchor( (DataBrowserItemID*) first, (DataBrowserItemID*) last) );
}



//
// Tab Control
//

OSStatus wxMacControl::SetTabEnabled( SInt16 tabNo , bool enable )
{
    return ::SetTabEnabled( m_controlRef , tabNo , enable );
}

//
// Quartz Support
//

#ifdef __WXMAC_OSX__
// snippets from Sketch Sample from Apple :

#define kGenericRGBProfilePathStr "/System/Library/ColorSync/Profiles/Generic RGB Profile.icc"

/*
    This function locates, opens, and returns the profile reference for the calibrated
    Generic RGB color space. It is up to the caller to call CMCloseProfile when done
    with the profile reference this function returns.
*/
CMProfileRef wxMacOpenGenericProfile()
{
    static CMProfileRef cachedRGBProfileRef = NULL;

    // we only create the profile reference once
    if (cachedRGBProfileRef == NULL)
    {
        CMProfileLocation loc;

        loc.locType = cmPathBasedProfile;
        strcpy(loc.u.pathLoc.path, kGenericRGBProfilePathStr);

        verify_noerr( CMOpenProfile(&cachedRGBProfileRef, &loc) );
    }

    // clone the profile reference so that the caller has their own reference, not our cached one
    if (cachedRGBProfileRef)
        CMCloneProfileRef(cachedRGBProfileRef);

    return cachedRGBProfileRef;
}

/*
    Return the generic RGB color space. This is a 'get' function and the caller should
    not release the returned value unless the caller retains it first. Usually callers
    of this routine will immediately use the returned colorspace with CoreGraphics
    so they typically do not need to retain it themselves.

    This function creates the generic RGB color space once and hangs onto it so it can
    return it whenever this function is called.
*/

CGColorSpaceRef wxMacGetGenericRGBColorSpace()
{
    static wxMacCFRefHolder<CGColorSpaceRef> genericRGBColorSpace;

    if (genericRGBColorSpace == NULL)
    {
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            genericRGBColorSpace.Set( CGColorSpaceCreateWithName( CFSTR("kCGColorSpaceGenericRGB") ) );
        }
        else
        {
            CMProfileRef genericRGBProfile = wxMacOpenGenericProfile();

            if (genericRGBProfile)
            {
                genericRGBColorSpace.Set( CGColorSpaceCreateWithPlatformColorSpace(genericRGBProfile) );

                wxASSERT_MSG( genericRGBColorSpace != NULL, wxT("couldn't create the generic RGB color space") );

                // we opened the profile so it is up to us to close it
                CMCloseProfile(genericRGBProfile);
            }
        }
    }

    return genericRGBColorSpace;
}
#endif

wxMacPortSaver::wxMacPortSaver( GrafPtr port )
{
    ::GetPort( &m_port );
    ::SetPort( port );
}

wxMacPortSaver::~wxMacPortSaver()
{
    ::SetPort( m_port );
}

#endif // wxUSE_GUI
