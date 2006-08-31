/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/app.cpp
// Purpose:     wxApp
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/window.h"
    #include "wx/frame.h"
    #include "wx/dc.h"
    #include "wx/button.h"
    #include "wx/menu.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
    #include "wx/memory.h"
    #include "wx/gdicmn.h"
    #include "wx/module.h"
#endif

#include "wx/tooltip.h"
#include "wx/docview.h"
#include "wx/filename.h"

#include <string.h>

// mac

#ifndef __DARWIN__
    #if __option(profile)
        #include <profiler.h>
    #endif
#endif

#include "apprsrc.h"

#include "wx/mac/uma.h"
#include "wx/mac/macnotfy.h"

#ifdef __DARWIN__
#  include <CoreServices/CoreServices.h>
#  if defined(WXMAKINGDLL_CORE)
#    include <mach-o/dyld.h>
#  endif
#else
#  include <Sound.h>
#  include <Threads.h>
#  include <ToolUtils.h>
#  include <DiskInit.h>
#  include <Devices.h>
#endif

extern wxList wxPendingDelete;
extern wxList *wxWinMacWindowList;
extern wxList *wxWinMacControlList;
#if wxUSE_THREADS
extern size_t g_numberOfThreads;
#endif // wxUSE_THREADS

// statics for implementation

static bool s_inYield = false;

#if TARGET_CARBON
static bool s_inReceiveEvent = false ;
static EventTime sleepTime = kEventDurationNoWait ;
#else
static long sleepTime = 0 ;
#endif

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
    EVT_END_SESSION(wxApp::OnEndSession)
    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()

const short    kMacMinHeap = (29 * 1024) ;
// platform specific static variables

const short kwxMacMenuBarResource = 1 ;
const short kwxMacAppleMenuId = 1 ;

WXHRGN    wxApp::s_macCursorRgn = NULL;
wxWindow* wxApp::s_captureWindow = NULL ;
int       wxApp::s_lastMouseDown = 0 ;
long      wxApp::sm_lastMessageTime = 0;
long      wxApp::s_lastModifiers = 0 ;


bool      wxApp::s_macSupportPCMenuShortcuts = true ;
long      wxApp::s_macAboutMenuItemId = wxID_ABOUT ;
long      wxApp::s_macPreferencesMenuItemId = wxID_PREFERENCES ;
long      wxApp::s_macExitMenuItemId = wxID_EXIT ;
wxString  wxApp::s_macHelpMenuTitleName = wxT("&Help") ;

// Normally we're not a plugin
bool      wxApp::sm_isEmbedded = false;
//----------------------------------------------------------------------
// Core Apple Event Support
//----------------------------------------------------------------------

pascal OSErr AEHandleODoc( const AppleEvent *event , AppleEvent *reply , long refcon ) ;
pascal OSErr AEHandleOApp( const AppleEvent *event , AppleEvent *reply , long refcon ) ;
pascal OSErr AEHandlePDoc( const AppleEvent *event , AppleEvent *reply , long refcon ) ;
pascal OSErr AEHandleQuit( const AppleEvent *event , AppleEvent *reply , long refcon ) ;
pascal OSErr AEHandleRApp( const AppleEvent *event , AppleEvent *reply , long refcon ) ;

pascal OSErr AEHandleODoc( const AppleEvent *event , AppleEvent *reply , long WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEODoc( (AppleEvent*) event , reply) ;
}

pascal OSErr AEHandleOApp( const AppleEvent *event , AppleEvent *reply , long WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEOApp( (AppleEvent*) event , reply ) ;
}

pascal OSErr AEHandlePDoc( const AppleEvent *event , AppleEvent *reply , long WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEPDoc( (AppleEvent*) event , reply ) ;
}

pascal OSErr AEHandleQuit( const AppleEvent *event , AppleEvent *reply , long WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEQuit( (AppleEvent*) event , reply) ;
}

pascal OSErr AEHandleRApp( const AppleEvent *event , AppleEvent *reply , long WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAERApp( (AppleEvent*) event , reply) ;
}

// AEODoc Calls MacOpenFile on each of the files passed

short wxApp::MacHandleAEODoc(const WXEVENTREF event, WXEVENTREF WXUNUSED(reply))
{
    AEDescList docList;
    AEKeyword keywd;
    DescType returnedType;
    Size actualSize;
    long itemsInList;
    FSSpec theSpec;
    OSErr err;
    short i;
    err = AEGetParamDesc((AppleEvent *)event, keyDirectObject, typeAEList,&docList);
    if (err != noErr)
        return err;

    err = AECountItems(&docList, &itemsInList);
    if (err != noErr)
        return err;

    ProcessSerialNumber PSN ;
    PSN.highLongOfPSN = 0 ;
    PSN.lowLongOfPSN = kCurrentProcess ;
    SetFrontProcess( &PSN ) ;

    for (i = 1; i <= itemsInList; i++) {
        AEGetNthPtr(&docList, i, typeFSS, &keywd, &returnedType,
        (Ptr) & theSpec, sizeof(theSpec), &actualSize);
        wxString fName = wxMacFSSpec2MacFilename(&theSpec);
        MacOpenFile(fName);
    }
    return noErr;
}

// AEPDoc Calls MacPrintFile on each of the files passed

short wxApp::MacHandleAEPDoc(const WXEVENTREF event , WXEVENTREF WXUNUSED(reply))
{
    AEDescList docList;
    AEKeyword keywd;
    DescType returnedType;
    Size actualSize;
    long itemsInList;
    FSSpec theSpec;
    OSErr err;
    short i;
    err = AEGetParamDesc((AppleEvent *)event, keyDirectObject, typeAEList,&docList);
    if (err != noErr)
        return err;

    err = AECountItems(&docList, &itemsInList);
    if (err != noErr)
        return err;

    ProcessSerialNumber PSN ;
    PSN.highLongOfPSN = 0 ;
    PSN.lowLongOfPSN = kCurrentProcess ;
    SetFrontProcess( &PSN ) ;

    for (i = 1; i <= itemsInList; i++) {
        AEGetNthPtr(&docList, i, typeFSS, &keywd, &returnedType,
        (Ptr) & theSpec, sizeof(theSpec), &actualSize);
        wxString fName = wxMacFSSpec2MacFilename(&theSpec);
        MacPrintFile(fName);
    }
    return noErr;
}

// AEOApp calls MacNewFile

short wxApp::MacHandleAEOApp(const WXEVENTREF WXUNUSED(event) , WXEVENTREF WXUNUSED(reply))
{
    MacNewFile() ;
    return noErr ;
}

// AEQuit attempts to quit the application

short wxApp::MacHandleAEQuit(const WXEVENTREF WXUNUSED(event) , WXEVENTREF WXUNUSED(reply))
{
    wxWindow* win = GetTopWindow() ;
    if ( win )
    {
        wxCommandEvent exitEvent(wxEVT_COMMAND_MENU_SELECTED, s_macExitMenuItemId);
        if (!win->ProcessEvent(exitEvent))
            win->Close(true) ;
    }
    else
    {
        ExitMainLoop() ;
    }
    return noErr ;
}

// AEROApp calls MacReopenApp

short wxApp::MacHandleAERApp(const WXEVENTREF WXUNUSED(event) , WXEVENTREF WXUNUSED(reply))
{
    MacReopenApp() ;
    return noErr ;
}


//----------------------------------------------------------------------
// Support Routines linking the Mac...File Calls to the Document Manager
//----------------------------------------------------------------------

void wxApp::MacOpenFile(const wxString & fileName )
{
    wxDocManager* dm = wxDocManager::GetDocumentManager() ;
    if ( dm )
        dm->CreateDocument(fileName , wxDOC_SILENT ) ;
}

void wxApp::MacPrintFile(const wxString & fileName )
{
    wxDocManager* dm = wxDocManager::GetDocumentManager() ;
    if ( dm )
    {
        wxDocument *doc = dm->CreateDocument(fileName , wxDOC_SILENT ) ;
        if ( doc )
        {
            wxView* view = doc->GetFirstView() ;
            if( view )
            {
                wxPrintout *printout = view->OnCreatePrintout();
                if (printout)
                {
                    wxPrinter printer;
                    printer.Print(view->GetFrame(), printout, true);
                    delete printout;
                }
            }
            if (doc->Close())
            {
                doc->DeleteAllViews();
                dm->RemoveDocument(doc) ;
            }
        }
    }
}

void wxApp::MacNewFile()
{
}

void wxApp::MacReopenApp()
{
    // eventually check for open docs, if none, call MacNewFile
}

//----------------------------------------------------------------------
// Carbon Event Handler
//----------------------------------------------------------------------

#if TARGET_CARBON

    static const EventTypeSpec eventList[] =
    {
        { kEventClassCommand, kEventProcessCommand } ,
        { kEventClassCommand, kEventCommandUpdateStatus } ,

        { kEventClassMenu, kEventMenuOpening },
        { kEventClassMenu, kEventMenuClosed },
        { kEventClassMenu, kEventMenuTargetItem },

        { kEventClassApplication , kEventAppActivated } ,
        { kEventClassApplication , kEventAppDeactivated } ,
        // handling the quit event is not recommended by apple
        // rather using the quit apple event - which we do

        { kEventClassAppleEvent , kEventAppleEvent } ,

        { kEventClassMouse , kEventMouseDown } ,
        { kEventClassMouse , kEventMouseMoved } ,
        { kEventClassMouse , kEventMouseUp } ,
        { kEventClassMouse , kEventMouseDragged } ,
        { 'WXMC' , 'WXMC' }
    } ;

static pascal OSStatus
MenuEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar();

    if ( mbar )
    {
        wxFrame* win = mbar->GetFrame();
        if ( win )
        {

            // VZ: we could find the menu from its handle here by examining all
            //     the menus in the menu bar recursively but knowing that neither
            //     wxMSW nor wxGTK do it why bother...
#if 0
            MenuRef menuRef;

            GetEventParameter(event,
                              kEventParamDirectObject,
                              typeMenuRef, NULL,
                              sizeof(menuRef), NULL,
                              &menuRef);
#endif // 0

            wxEventType type=0;
            MenuCommand cmd=0;
            switch (GetEventKind(event))
            {
                case kEventMenuOpening:
                    type = wxEVT_MENU_OPEN;
                    break;
                case kEventMenuClosed:
                    type = wxEVT_MENU_CLOSE;
                    break;
                case kEventMenuTargetItem:
                    type = wxEVT_MENU_HIGHLIGHT;
                    GetEventParameter(event, kEventParamMenuCommand,
                                      typeMenuCommand, NULL,
                                      sizeof(cmd), NULL, &cmd);
                    if (cmd == 0) return eventNotHandledErr;
                    break;
                default:
                    wxFAIL_MSG(wxT("Unexpected menu event kind"));
                    break;
            }

            wxMenuEvent wxevent(type, cmd);
            wxevent.SetEventObject(win);

            (void)win->GetEventHandler()->ProcessEvent(wxevent);
        }
    }

    return eventNotHandledErr;
}

// due to the rather low-level event API of wxWidgets, we cannot use RunApplicationEventLoop
// but have to use ReceiveNextEvent dealing with events manually, therefore we also have
// to deal with clicks in the menu bar explicitly

pascal OSStatus wxMacWindowEventHandler( EventHandlerCallRef handler , EventRef event , void *data ) ;

static pascal OSStatus MouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    Point point ;
    UInt32 modifiers = 0;
    EventMouseButton button = 0 ;
    UInt32 click = 0 ;

    GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL,
        sizeof( Point ), NULL, &point );
    GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, NULL,
        sizeof( UInt32 ), NULL, &modifiers );
    GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL,
        sizeof( EventMouseButton ), NULL, &button );
    GetEventParameter( event, kEventParamClickCount, typeUInt32, NULL,
        sizeof( UInt32 ), NULL, &click );

    if ( button == 0 || GetEventKind( event ) == kEventMouseUp )
        modifiers += btnState ;


    switch( GetEventKind(event) )
    {
        case kEventMouseDown :
        {
            WindowRef window ;

            short windowPart = ::FindWindow(point, &window);

            if ( windowPart == inMenuBar )
            {
                MenuSelect( point ) ;
                result = noErr ;
            }
        }
        break ;
        case kEventMouseDragged :
        case kEventMouseUp :
        {
            if ( wxTheApp->s_captureWindow )
                wxMacWindowEventHandler( handler , event , (void*) wxTheApp->s_captureWindow->MacGetTopLevelWindow() ) ;
        }
        break ;
        case kEventMouseMoved :
        {
            wxTheApp->MacHandleMouseMovedEvent( point.h , point.v , modifiers , EventTimeToTicks( GetEventTime( event ) ) ) ;
            result = noErr ;
            break ;
        }
        break ;
    }

    return result ;
}

static pascal OSStatus CommandEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    HICommand command ;

    GetEventParameter( event, kEventParamDirectObject, typeHICommand, NULL,
        sizeof( HICommand ), NULL, &command );

    MenuCommand id = command.commandID ;
    if ( id == kHICommandPreferences )
        id = wxApp::s_macPreferencesMenuItemId ;

    wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar() ;
    wxMenu* menu = NULL ;
    wxMenuItem* item = NULL ;

    if ( mbar )
    {
        item = mbar->FindItem( id , &menu ) ;
        // it is not 100 % sure that an menu of id 0 is really ours, safety check
        if ( id == 0 && menu != NULL && menu->GetHMenu() != command.menu.menuRef )
        {
            item = NULL ;
            menu = NULL ;
        }
    }

    if ( item == NULL || menu == NULL || mbar == NULL )
        return result ;

       switch( GetEventKind( event ) )
       {
           case kEventProcessCommand :
           {
                if (item->IsCheckable())
                {
                    item->Check( !item->IsChecked() ) ;
                }

                menu->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) ;
                result = noErr ;
            }
               break ;
        case kEventCommandUpdateStatus:
            // eventually trigger an updateui round
            result = noErr ;
            break ;
           default :
               break ;
       }

    return result ;
}

static pascal OSStatus ApplicationEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    switch ( GetEventKind( event ) )
    {
        case kEventAppActivated :
            {
                if ( wxTheApp )
                    wxTheApp->MacResume( true ) ;
                result = noErr ;
            }
            break ;
        case kEventAppDeactivated :
            {
                if ( wxTheApp )
                    wxTheApp->MacSuspend( true ) ;
                result = noErr ;
            }
            break ;
        default :
            break ;
    }
    return result ;
}

pascal OSStatus wxAppEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    switch( GetEventClass( event ) )
    {
        case kEventClassCommand :
            result = CommandEventHandler( handler , event , data ) ;
            break ;
        case kEventClassApplication :
            result = ApplicationEventHandler( handler , event , data ) ;
            break ;
        case kEventClassMenu :
            result = MenuEventHandler( handler , event , data ) ;
            break ;
        case kEventClassMouse :
            result = MouseEventHandler( handler , event , data ) ;
            break ;
        case kEventClassAppleEvent :
            {
                EventRecord rec ;
                wxMacConvertEventToRecord( event , &rec ) ;
                result = AEProcessAppleEvent( &rec ) ;
            }
            break ;
        default :
            break ;
    }

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxAppEventHandler )

#endif

#if defined(WXMAKINGDLL_CORE) && !defined(__DARWIN__)
// we know it's there ;-)
WXIMPORT char std::__throws_bad_alloc ;
#endif

bool wxApp::Initialize(int& argc, wxChar **argv)
{
    int error = 0 ;

    // Mac-specific

    UMAInitToolbox( 4, sm_isEmbedded ) ;
    SetEventMask( everyEvent ) ;
    UMAShowWatchCursor() ;

#if defined(WXMAKINGDLL_CORE) && defined(__DARWIN__)
    // open shared library resources from here since we don't have
    //   __wxinitialize in Mach-O shared libraries
    wxStAppResource::OpenSharedLibraryResource(NULL);
#endif

#ifndef __DARWIN__
    // test the minimal configuration necessary

#  if !TARGET_CARBON
    long theSystem ;
    long theMachine;

    if (Gestalt(gestaltMachineType, &theMachine) != noErr)
    {
        error = kMacSTRWrongMachine;
    }
    else if (theMachine < gestaltMacPlus)
    {
        error = kMacSTRWrongMachine;
    }
    else if (Gestalt(gestaltSystemVersion, &theSystem) != noErr )
    {
        error = kMacSTROldSystem  ;
    }
    else if ( theSystem < 0x0860 )
    {
        error = kMacSTROldSystem  ;
    }
    else if ((long)GetApplLimit() - (long)ApplicationZone() < kMacMinHeap)
    {
        error = kMacSTRSmallSize;
    }
#  endif
    /*
    else
    {
        if ( !UMAHasAppearance() )
        {
            error = kMacSTRNoPre8Yet ;
        }
    }
    */
#endif

    // if we encountered any problems so far, give the error code and exit immediately

  if ( error )
  {
        wxStAppResource resload ;
        short itemHit;
        Str255 message;

        GetIndString(message, 128, error);
        UMAShowArrowCursor() ;
        ParamText("\pFatal Error", message, (ConstStr255Param)"\p", (ConstStr255Param)"\p");
        itemHit = Alert(128, nil);
        return false ;
  }

#ifndef __DARWIN__
#  if __option(profile)
    ProfilerInit( collectDetailed, bestTimeBase , 40000 , 50 ) ;
#  endif
#endif

#ifndef __DARWIN__
    // now avoid exceptions thrown for new (bad_alloc)
    // FIXME CS for some changes outside wxMac does not compile anymore
#if 0
    std::__throws_bad_alloc = 0 ;
#endif

#endif

    s_macCursorRgn = ::NewRgn() ;

    // Mac OS X passes a process serial number command line argument when
    // the application is launched from the Finder. This argument must be
    // removed from the command line arguments before being handled by the
    // application (otherwise applications would need to handle it)
    if ( argc > 1 )
    {
        static const wxChar *ARG_PSN = _T("-psn_");
        if ( wxStrncmp(argv[1], ARG_PSN, wxStrlen(ARG_PSN)) == 0 )
        {
            // remove this argument
            --argc;
            memmove(argv + 1, argv + 2, argc * sizeof(char *));
        }
    }

    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif


    wxWinMacWindowList = new wxList(wxKEY_INTEGER);
    wxWinMacControlList = new wxList(wxKEY_INTEGER);

    wxMacCreateNotifierTable() ;

    UMAShowArrowCursor() ;

    return true;
}

bool wxApp::OnInitGui()
{
    if( !wxAppBase::OnInitGui() )
        return false ;

#if TARGET_CARBON
    InstallStandardEventHandler( GetApplicationEventTarget() ) ;

    if (!sm_isEmbedded)
    {
        InstallApplicationEventHandler(
            GetwxAppEventHandlerUPP(),
            GetEventTypeCount(eventList), eventList, wxTheApp, (EventHandlerRef *)&(wxTheApp->m_macEventHandler));
    }
#endif

    if (!sm_isEmbedded)
    {
#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0340)
        AEInstallEventHandler( kCoreEventClass , kAEOpenDocuments ,
                               NewAEEventHandlerUPP(AEHandleODoc) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEOpenApplication ,
                               NewAEEventHandlerUPP(AEHandleOApp) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEPrintDocuments ,
                               NewAEEventHandlerUPP(AEHandlePDoc) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEReopenApplication ,
                               NewAEEventHandlerUPP(AEHandleRApp) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEQuitApplication ,
                               NewAEEventHandlerUPP(AEHandleQuit) ,
                               0 , FALSE ) ;
#else
        AEInstallEventHandler( kCoreEventClass , kAEOpenDocuments ,
                               NewAEEventHandlerProc(AEHandleODoc) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEOpenApplication ,
                               NewAEEventHandlerProc(AEHandleOApp) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEPrintDocuments ,
                               NewAEEventHandlerProc(AEHandlePDoc) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEReopenApplication ,
                               NewAEEventHandlerProc(AEHandleRApp) ,
                               0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEQuitApplication ,
                               NewAEEventHandlerProc(AEHandleQuit) ,
                               0 , FALSE ) ;
#endif
    }

    return true ;
}

void wxApp::CleanUp()
{
    wxToolTip::RemoveToolTips() ;

    // One last chance for pending objects to be cleaned up
    wxTheApp->DeletePendingObjects();

    wxMacDestroyNotifierTable() ;

    delete wxWinMacWindowList ;
    wxWinMacWindowList = NULL;

    delete wxWinMacControlList ;
    wxWinMacControlList = NULL;

#ifndef __DARWIN__
#  if __option(profile)
    ProfilerDump( (StringPtr)"\papp.prof" ) ;
    ProfilerTerm() ;
#  endif
#endif

#if defined(WXMAKINGDLL_CORE) && defined(__DARWIN__)
    // close shared library resources from here since we don't have
    //   __wxterminate in Mach-O shared libraries
    wxStAppResource::CloseSharedLibraryResource();
#endif

    UMACleanupToolbox() ;
    if (s_macCursorRgn) {
        ::DisposeRgn((RgnHandle)s_macCursorRgn);
    }

    #if 0
        TerminateAE() ;
    #endif

    wxAppBase::CleanUp();
}

//----------------------------------------------------------------------
// misc initialization stuff
//----------------------------------------------------------------------

// extern variable for shared library resource id
// need to be able to find it with NSLookupAndBindSymbol
short gSharedLibraryResource = kResFileNotOpened ;

#if defined(WXMAKINGDLL_CORE) && defined(__DARWIN__)
CFBundleRef gSharedLibraryBundle = NULL;
#endif /* WXMAKINGDLL_CORE && __DARWIN__ */

wxStAppResource::wxStAppResource()
{
    m_currentRefNum = CurResFile() ;
    if ( gSharedLibraryResource != kResFileNotOpened )
    {
        UseResFile( gSharedLibraryResource ) ;
    }
}

wxStAppResource::~wxStAppResource()
{
    if ( m_currentRefNum != kResFileNotOpened )
    {
        UseResFile( m_currentRefNum ) ;
    }
}

void wxStAppResource::OpenSharedLibraryResource(const void *initBlock)
{
    gSharedLibraryResource = kResFileNotOpened;

#ifdef WXMAKINGDLL_CORE
    if ( initBlock != NULL ) {
        const CFragInitBlock *theInitBlock = (const CFragInitBlock *)initBlock;
        FSSpec *fileSpec = NULL;

        if (theInitBlock->fragLocator.where == kDataForkCFragLocator) {
            fileSpec = theInitBlock->fragLocator.u.onDisk.fileSpec;
        }
        else if (theInitBlock->fragLocator.where == kResourceCFragLocator) {
            fileSpec = theInitBlock->fragLocator.u.inSegs.fileSpec;
        }

        if (fileSpec != NULL) {
            gSharedLibraryResource =  FSpOpenResFile(fileSpec, fsRdPerm);
        }
    }
    else {
#ifdef __DARWIN__
        // Open the shared library resource file if it is not yet open
        NSSymbol    theSymbol;
        NSModule    theModule;
        const char *theLibPath;

        gSharedLibraryBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.wxwindows.wxWidgets"));
        if (gSharedLibraryBundle != NULL) {
            // wxWidgets has been bundled into a framework
            //   load the framework resources

            gSharedLibraryResource = CFBundleOpenBundleResourceMap(gSharedLibraryBundle);
        }
        else {
            // wxWidgets is a simple dynamic shared library
            //   load the resources from the data fork of a separate resource file
            wxString theResPath;
            wxString theName;
            FSRef  theResRef;
            OSErr  theErr = noErr;

            // get the library path
            theSymbol = NSLookupAndBindSymbol("_gSharedLibraryResource");
            theModule = NSModuleForSymbol(theSymbol);
            theLibPath = NSLibraryNameForModule(theModule);

            // if we call wxLogDebug from here then, as wxTheApp hasn't been
            // created yet when we're called from wxApp::Initialize(), wxLog
            // is going to create a default stderr-based log target instead of
            // the expected normal GUI one -- don't do it, if we really want
            // to see this message just use fprintf() here
#if 0
            wxLogDebug( wxT("wxMac library installation name is '%s'"),
                        theLibPath );
#endif

            // allocate copy to replace .dylib.* extension with .rsrc
            if (theLibPath != NULL) {
#if wxUSE_UNICODE
                theResPath = wxString(theLibPath, wxConvLocal);
#else
                theResPath = wxString(theLibPath);
#endif
                // replace '_core' with '' in case of multi-lib build
                theResPath.Replace(wxT("_core"), wxEmptyString);
                // replace ".dylib" shared library extension with ".rsrc"
                theResPath.Replace(wxT(".dylib"), wxT(".rsrc"));
                // Find the begining of the filename
                theName = theResPath.AfterLast('/');

#if 0
                wxLogDebug( wxT("wxMac resources file name is '%s'"),
                            theResPath.mb_str() );
#endif

                theErr = FSPathMakeRef((UInt8 *) theResPath.mb_str(), &theResRef, false);
                if (theErr != noErr) {
                    // try in current directory (using name only)
                    theErr = FSPathMakeRef((UInt8 *) theName.mb_str(), &theResRef, false);
                }

                // open the resource file
                if (theErr == noErr) {
                    theErr = FSOpenResourceFile( &theResRef, 0, NULL, fsRdPerm,
                                                 &gSharedLibraryResource);
                }
                if (theErr != noErr) {
#ifdef __WXDEBUG__
                    wxLogDebug( wxT("unable to open wxMac resource file '%s'\n"),
                                theResPath.mb_str() );
#endif // __WXDEBUG__
                }

            }
        }
#endif /* __DARWIN__ */
    }
#endif /* WXMAKINGDLL_CORE */
}

void wxStAppResource::CloseSharedLibraryResource()
{
#ifdef WXMAKINGDLL_CORE
    // Close the shared library resource file
    if (gSharedLibraryResource != kResFileNotOpened) {
#ifdef __DARWIN__
        if (gSharedLibraryBundle != NULL) {
            CFBundleCloseBundleResourceMap(gSharedLibraryBundle,
                                           gSharedLibraryResource);
            gSharedLibraryBundle = NULL;
        }
        else
#endif /* __DARWIN__ */
        {
            CloseResFile(gSharedLibraryResource);
        }
        gSharedLibraryResource = kResFileNotOpened;
    }
#endif /* WXMAKINGDLL_CORE */
}

#if defined(WXMAKINGDLL_CORE) && !defined(__DARWIN__)

// for shared libraries we have to manually get the correct resource
// ref num upon initializing and releasing when terminating, therefore
// the __wxinitialize and __wxterminate must be used

extern "C" {
    void __sinit(void);    /*    (generated by linker)    */
    pascal OSErr __initialize(const CFragInitBlock *theInitBlock);
    pascal void __terminate(void);
}

pascal OSErr __wxinitialize(const CFragInitBlock *theInitBlock)
{
    wxStAppResource::OpenSharedLibraryResource( theInitBlock ) ;
    return __initialize( theInitBlock ) ;
}

pascal void __wxterminate(void)
{
    wxStAppResource::CloseSharedLibraryResource() ;
    __terminate() ;
}

#endif /* WXMAKINGDLL_CORE && !__DARWIN__ */

#if TARGET_CARBON

bool wxMacConvertEventToRecord( EventRef event , EventRecord *rec)
{
    bool converted = ConvertEventRefToEventRecord( event,rec) ;
    OSStatus err = noErr ;
    if ( !converted )
    {
        switch( GetEventClass( event ) )
        {
            case kEventClassKeyboard :
            {
                converted = true ;
                switch( GetEventKind(event) )
                {
                    case kEventRawKeyDown :
                        rec->what = keyDown ;
                        break ;
                    case kEventRawKeyRepeat :
                        rec->what = autoKey ;
                        break ;
                    case kEventRawKeyUp :
                        rec->what = keyUp ;
                        break ;
                    case kEventRawKeyModifiersChanged :
                        rec->what = nullEvent ;
                        break ;
                    default :
                        converted = false ;
                        break ;
                }
                if ( converted )
                {
                    UInt32 keyCode ;
                    unsigned char charCode ;
                    UInt32 modifiers ;
                    GetMouse( &rec->where) ;

                    err = GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, 4, NULL, &modifiers);
                    err = GetEventParameter(event, kEventParamKeyCode, typeUInt32, NULL, 4, NULL, &keyCode);
                    err = GetEventParameter(event, kEventParamKeyMacCharCodes, typeChar, NULL, 1, NULL, &charCode);
                    rec->modifiers = modifiers ;
                    rec->message = (keyCode << 8 ) + charCode ;
                }
            }
            break ;
            case kEventClassTextInput :
            {
                switch( GetEventKind( event ) )
                {
                    case kEventTextInputUnicodeForKeyEvent :
                        {
                            EventRef rawEvent ;
                            err = GetEventParameter( event , kEventParamTextInputSendKeyboardEvent ,typeEventRef,NULL,sizeof(rawEvent),NULL,&rawEvent ) ;
                            converted = true ;
                            {
                                UInt32 keyCode ;
                                unsigned char charCode ;
                                UInt32 modifiers ;
                                GetMouse( &rec->where) ;
                                rec->what = keyDown ;
                                err = GetEventParameter(rawEvent, kEventParamKeyModifiers, typeUInt32, NULL, 4, NULL, &modifiers);
                                err = GetEventParameter(rawEvent, kEventParamKeyCode, typeUInt32, NULL, 4, NULL, &keyCode);
                                err = GetEventParameter(rawEvent, kEventParamKeyMacCharCodes, typeChar, NULL, 1, NULL, &charCode);
                                rec->modifiers = modifiers ;
                                rec->message = (keyCode << 8 ) + charCode ;
                            }
                       }
                        break ;
                    default :
                        break ;
                }
            }
            break ;
        }
    }

    return converted ;
}

/*
pascal OSStatus wxMacApplicationEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    EventRecord rec ;
    switch ( GetEventClass( event ) )
    {
        case kEventClassKeyboard :
            if ( wxMacConvertEventToRecord( event , &rec ) )
            {
                wxTheApp->MacHandleModifierEvents( &rec ) ;
                wxTheApp->MacHandleOneEvent( &rec ) ;
                result = noErr ;
            }
            break ;
        case kEventClassTextInput :
            if ( wxMacConvertEventToRecord( event , &rec ) )
            {
                wxTheApp->MacHandleModifierEvents( &rec ) ;
                wxTheApp->MacHandleOneEvent( &rec ) ;
                result = noErr ;
            }
            break ;
        default :
            break ;
    }
    return result ;
}
*/
#endif

wxApp::wxApp()
{
    m_printMode = wxPRINT_WINDOWS;
    m_auto3D = true;

    m_macCurrentEvent = NULL ;
#if TARGET_CARBON
    m_macCurrentEventHandlerCallRef = NULL ;
#endif
}

int wxApp::MainLoop()
{
    m_keepGoing = true;

    while (m_keepGoing)
    {
        MacDoOneEvent() ;
    }

    return 0;
}

void wxApp::ExitMainLoop()
{
    m_keepGoing = false;
}

// Is a message/event pending?
bool wxApp::Pending()
{
#if TARGET_CARBON
    // without the receive event (with pull param = false ) nothing is ever reported
    EventRef theEvent;
    ReceiveNextEvent (0, NULL, kEventDurationNoWait, false, &theEvent);
    return GetNumEventsInQueue( GetMainEventQueue() ) > 0 ;
#else
    EventRecord event ;

    return EventAvail( everyEvent , &event ) ;
#endif
}

// Dispatch a message.
bool wxApp::Dispatch()
{
    MacDoOneEvent() ;

    return true;
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    wxAppBase::OnIdle(event);

    // If they are pending events, we must process them: pending events are
    // either events to the threads other than main or events posted with
    // wxPostEvent() functions
    wxMacProcessNotifierAndPendingEvents();

  if(!wxMenuBar::MacGetInstalledMenuBar() && wxMenuBar::MacGetCommonMenuBar())
    wxMenuBar::MacGetCommonMenuBar()->MacInstallMenuBar();
}

void wxApp::WakeUpIdle()
{
    wxMacWakeUp() ;
}

void wxApp::Exit()
{
    wxApp::CleanUp();
    ::ExitToShell() ;
}

void wxApp::OnEndSession(wxCloseEvent& WXUNUSED(event))
{
    if (GetTopWindow())
        GetTopWindow()->Close(true);
}

// Default behaviour: close the application with prompts. The
// user can veto the close, and therefore the end session.
void wxApp::OnQueryEndSession(wxCloseEvent& event)
{
    if (GetTopWindow())
    {
        if (!GetTopWindow()->Close(!event.CanVeto()))
            event.Veto(true);
    }
}

extern "C" void wxCYield() ;
void wxCYield()
{
    wxYield() ;
}

// Yield to other processes

bool wxApp::Yield(bool onlyIfNeeded)
{
    if (s_inYield)
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return false;
    }

    s_inYield = true;

#if wxUSE_THREADS
    YieldToAnyThread() ;
#endif
    // by definition yield should handle all non-processed events
#if TARGET_CARBON
    EventRef theEvent;

    OSStatus status = noErr ;
    do
    {
        s_inReceiveEvent = true ;
        status = ReceiveNextEvent(0, NULL,kEventDurationNoWait,true,&theEvent) ;
        s_inReceiveEvent = false ;

        if ( status == eventLoopTimedOutErr )
        {
            // make sure next time the event loop will trigger idle events
            sleepTime = kEventDurationNoWait ;
        }
        else if ( status == eventLoopQuitErr )
        {
            // according to QA1061 this may also occur when a WakeUp Process
            // is executed
        }
        else
        {
            MacHandleOneEvent( theEvent ) ;
            ReleaseEvent(theEvent);
        }
    } while( status == noErr ) ;
#else
    EventRecord event ;

    // having a larger value here leads to large performance slowdowns
    // so we cannot give background apps more processor time here
    // we do so however having a large sleep value in the main event loop
    sleepTime = 0 ;

    while ( !IsExiting() && WaitNextEvent(everyEvent, &event,sleepTime, (RgnHandle) wxApp::s_macCursorRgn))
    {
        MacHandleModifierEvents( &event ) ;
        MacHandleOneEvent( &event );
        if ( event.what != kHighLevelEvent )
            SetRectRgn( (RgnHandle) wxApp::s_macCursorRgn , event.where.h , event.where.v ,  event.where.h + 1 , event.where.v + 1 ) ;
    }
    MacHandleModifierEvents( &event ) ;
#endif

    wxMacProcessNotifierAndPendingEvents() ;
    s_inYield = false;

    return true;
}

// platform specifics

void wxApp::MacSuspend( bool convertClipboard )
{
#if !TARGET_CARBON
    // we have to deactive the top level windows manually

    wxWindowListNode* node = wxTopLevelWindows.GetFirst();
    while (node)
    {
        wxTopLevelWindow* win = (wxTopLevelWindow*) node->Data();
#if TARGET_CARBON
#if 0 //  having problems right now with that
        if (!win->HasFlag(wxSTAY_ON_TOP))
#endif
#endif
            win->MacActivate( ((EventRecord*) MacGetCurrentEvent())->when , false ) ;

        node = node->GetNext();
    }

    ::HideFloatingWindows() ;
#endif
    s_lastMouseDown = 0 ;

    if( convertClipboard )
    {
        MacConvertPrivateToPublicScrap() ;
    }
}

extern wxList wxModalDialogs;

void wxApp::MacResume( bool convertClipboard )
{
    s_lastMouseDown = 0 ;
    if( convertClipboard )
    {
        MacConvertPublicToPrivateScrap() ;
    }

#if !TARGET_CARBON
    ::ShowFloatingWindows() ;
    // raise modal dialogs in case a non modal window was selected to activate the app

    wxNode* node = wxModalDialogs.GetFirst();
    while (node)
    {
        wxDialog* dialog = (wxDialog *) node->GetData();
        dialog->Raise();

        node = node->GetNext();
    }
#endif
}

void wxApp::MacConvertPrivateToPublicScrap()
{
}

void wxApp::MacConvertPublicToPrivateScrap()
{
}

void wxApp::MacDoOneEvent()
{
#if TARGET_CARBON
    EventRef theEvent;

    s_inReceiveEvent = true ;
    OSStatus status = ReceiveNextEvent(0, NULL,sleepTime,true,&theEvent) ;
    s_inReceiveEvent = false ;
    if ( status == eventLoopTimedOutErr )
    {
        if ( wxTheApp->ProcessIdle() )
            sleepTime = kEventDurationNoWait ;
        else
        {
#if wxUSE_THREADS
            if (g_numberOfThreads)
            {
                sleepTime = kEventDurationNoWait;
            }
            else
#endif // wxUSE_THREADS
            {
                sleepTime = kEventDurationSecond;
            }
        }
    }
    else if ( status == eventLoopQuitErr )
    {
        // according to QA1061 this may also occur when a WakeUp Process
        // is executed
    }
    else
    {
        MacHandleOneEvent( theEvent ) ;
        ReleaseEvent(theEvent);
        sleepTime = kEventDurationNoWait ;
    }
#else
      EventRecord event ;

    EventMask eventMask = everyEvent ;

    if (WaitNextEvent(eventMask, &event, sleepTime, (RgnHandle) s_macCursorRgn))
    {
        MacHandleModifierEvents( &event ) ;
        MacHandleOneEvent( &event );
    }
    else
    {
        MacHandleModifierEvents( &event ) ;
        // idlers
        WindowPtr window = ::FrontWindow() ;
        if ( window )
            ::IdleControls( window ) ;

        if ( wxTheApp->ProcessIdle() )
            sleepTime = kEventDurationNoWait;
        else
        {
#if wxUSE_THREADS
            if (g_numberOfThreads)
            {
                sleepTime = kEventDurationNoWait;
            }
            else
#endif // wxUSE_THREADS
            {
                sleepTime = kEventDurationSecond;
            }
        }
    }
    if ( event.what != kHighLevelEvent )
        SetRectRgn( (RgnHandle) s_macCursorRgn , event.where.h , event.where.v ,  event.where.h + 1 , event.where.v + 1 ) ;
#endif
    // repeaters

    DeletePendingObjects() ;
    wxMacProcessNotifierAndPendingEvents() ;
}

/*virtual*/ void wxApp::MacHandleUnhandledEvent( WXEVENTREF evr )
{
    // Override to process unhandled events as you please
}

void wxApp::MacHandleOneEvent( WXEVENTREF evr )
{
#if TARGET_CARBON
    EventTargetRef theTarget;
    theTarget = GetEventDispatcherTarget();
    m_macCurrentEvent = evr ;
    OSStatus status = SendEventToEventTarget ((EventRef) evr , theTarget);
    if(status == eventNotHandledErr)
    {
        MacHandleUnhandledEvent(evr);
    }
#else
    EventRecord* ev = (EventRecord*) evr ;
    m_macCurrentEvent = ev ;

    wxApp::sm_lastMessageTime = ev->when ;

    switch (ev->what)
    {
        case mouseDown:
            MacHandleMouseDownEvent( ev ) ;
            if ( ev->modifiers & controlKey )
                s_lastMouseDown = 2;
            else
                s_lastMouseDown = 1;
            break;
        case mouseUp:
            if ( s_lastMouseDown == 2 )
            {
                ev->modifiers |= controlKey ;
            }
            else
            {
                ev->modifiers &= ~controlKey ;
            }
            MacHandleMouseUpEvent( ev ) ;
            s_lastMouseDown = 0;
            break;
        case activateEvt:
            MacHandleActivateEvent( ev ) ;
            break;
        case updateEvt:
            // In embedded mode we first let the UnhandledEvent function
            // try to handle the update event. If we handle it ourselves
            // first and then pass it on, the host's windows won't update.
            MacHandleUnhandledEvent(ev);
            MacHandleUpdateEvent( ev ) ;
            break;
        case keyDown:
        case autoKey:
            MacHandleKeyDownEvent( ev ) ;
            break;
        case keyUp:
            MacHandleKeyUpEvent( ev ) ;
            break;
        case diskEvt:
            MacHandleDiskEvent( ev ) ;
            break;
        case osEvt:
            MacHandleOSEvent( ev ) ;
            break;
        case kHighLevelEvent:
            MacHandleHighLevelEvent( ev ) ;
            break;
        default:
            break;
    }
#endif
    wxMacProcessNotifierAndPendingEvents() ;
}

#if !TARGET_CARBON
bool s_macIsInModalLoop = false ;

void wxApp::MacHandleModifierEvents( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    if ( ev->modifiers != s_lastModifiers && wxWindow::FindFocus() != NULL )
    {
        wxKeyEvent event(wxEVT_KEY_DOWN);

        event.m_shiftDown = ev->modifiers & shiftKey;
        event.m_controlDown = ev->modifiers & controlKey;
        event.m_altDown = ev->modifiers & optionKey;
        event.m_metaDown = ev->modifiers & cmdKey;

        event.m_x = ev->where.h;
        event.m_y = ev->where.v;
        event.SetTimestamp( ev->when );
        wxWindow* focus = wxWindow::FindFocus() ;
        event.SetEventObject(focus);

        if ( (ev->modifiers ^ s_lastModifiers ) & controlKey )
        {
            event.m_keyCode = WXK_CONTROL ;
            event.SetEventType( ( ev->modifiers & controlKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
            focus->GetEventHandler()->ProcessEvent( event ) ;
        }
        if ( (ev->modifiers ^ s_lastModifiers ) & shiftKey )
        {
            event.m_keyCode = WXK_SHIFT ;
            event.SetEventType( ( ev->modifiers & shiftKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
            focus->GetEventHandler()->ProcessEvent( event ) ;
        }
        if ( (ev->modifiers ^ s_lastModifiers ) & optionKey )
        {
            event.m_keyCode = WXK_ALT ;
            event.SetEventType( ( ev->modifiers & optionKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
            focus->GetEventHandler()->ProcessEvent( event ) ;
        }
        if ( ( ev->modifiers ^ s_lastModifiers ) & cmdKey )
        {
            event.m_keyCode = WXK_COMMAND ;
            event.SetEventType( ( ev->modifiers & cmdKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
            focus->GetEventHandler()->ProcessEvent( event ) ;
        }
        s_lastModifiers = ev->modifiers ;
    }
}

void wxApp::MacHandleHighLevelEvent( WXEVENTREF evr )
{
    // we must avoid reentrancy problems when processing high level events eg printing
    bool former = s_inYield ;
    s_inYield = true ;
    EventRecord* ev = (EventRecord*) evr ;
    ::AEProcessAppleEvent( ev ) ;
    s_inYield = former ;
}

void wxApp::MacHandleMouseDownEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    wxToolTip::RemoveToolTips() ;

    WindowRef window;
    WindowRef frontWindow = ::FrontNonFloatingWindow() ;
    WindowAttributes frontWindowAttributes = NULL ;
    if ( frontWindow )
        ::GetWindowAttributes( frontWindow , &frontWindowAttributes ) ;

    short windowPart = ::FindWindow(ev->where, &window);
    wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) window ) ;
    if ( wxPendingDelete.Member(win) )
        return ;

    BitMap screenBits;
    GetQDGlobalsScreenBits( &screenBits );

    switch (windowPart)
    {
        case inMenuBar :
            if ( s_macIsInModalLoop )
            {
                SysBeep ( 30 ) ;
            }
            else
            {
                UInt32 menuresult = MenuSelect(ev->where) ;
               MacHandleMenuSelect( HiWord( menuresult ) , LoWord( menuresult ) );
                s_lastMouseDown = 0;
            }
            break ;
        case inSysWindow :
            SystemClick( ev , window ) ;
            s_lastMouseDown = 0;
            break ;
        case inDrag :
            if ( window != frontWindow && s_macIsInModalLoop && !(ev->modifiers & cmdKey ) )
            {
                SysBeep ( 30 ) ;
            }
            else
            {
                DragWindow(window, ev->where, &screenBits.bounds);
                if (win)
                {
                    GrafPtr port ;
                    GetPort( &port ) ;
                    Point pt = { 0, 0 } ;
                    SetPortWindowPort(window) ;
                    LocalToGlobal( &pt ) ;
                    SetPort( port ) ;
                        win->SetSize( pt.h , pt.v , -1 ,
                            -1 , wxSIZE_USE_EXISTING);
                }
                s_lastMouseDown = 0;
            }
            break ;
        case inGoAway:
            if (TrackGoAway(window, ev->where))
            {
                if ( win )
                    win->Close() ;
            }
            s_lastMouseDown = 0;
            break;
        case inGrow:
          {
                Rect newContentRect ;
                Rect constraintRect ;
                constraintRect.top = win->GetMinHeight() ;
                if ( constraintRect.top == -1 )
                    constraintRect.top  = 0 ;
                constraintRect.left = win->GetMinWidth() ;
                if ( constraintRect.left == -1 )
                    constraintRect.left  = 0 ;
                constraintRect.right = win->GetMaxWidth() ;
                if ( constraintRect.right == -1 )
                    constraintRect.right  = 32000 ;
                constraintRect.bottom = win->GetMaxHeight() ;
                if ( constraintRect.bottom == -1 )
                    constraintRect.bottom = 32000 ;

                Boolean growResult = ResizeWindow( window , ev->where ,
                    &constraintRect , &newContentRect ) ;
                if ( growResult )
                {
                    win->SetSize( newContentRect.left , newContentRect.top ,
                        newContentRect.right - newContentRect.left ,
                        newContentRect.bottom - newContentRect.top, wxSIZE_USE_EXISTING);
                }
                s_lastMouseDown = 0;
          }
            break;
        case inZoomIn:
        case inZoomOut:
                if (TrackBox(window, ev->where, windowPart))
                {
                    // TODO setup size event
                    ZoomWindow( window , windowPart , false ) ;
                    if (win)
                    {
                        Rect tempRect ;
                        GrafPtr port ;
                        GetPort( &port ) ;
                        Point pt = { 0, 0 } ;
                        SetPortWindowPort(window) ;
                        LocalToGlobal( &pt ) ;
                        SetPort( port ) ;

                        GetWindowPortBounds(window, &tempRect ) ;
                        win->SetSize( pt.h , pt.v , tempRect.right-tempRect.left ,
                            tempRect.bottom-tempRect.top, wxSIZE_USE_EXISTING);
                    }
                }
            s_lastMouseDown = 0;
            break;
        case inCollapseBox :
                // TODO setup size event
            s_lastMouseDown = 0;
            break ;

        case inContent :
                {
                    GrafPtr port ;
                    GetPort( &port ) ;
                    SetPortWindowPort(window) ;
                    SetPort( port ) ;
                }
                if ( window != frontWindow && wxTheApp->s_captureWindow == NULL )
                {
                    if ( s_macIsInModalLoop )
                    {
                        SysBeep ( 30 ) ;
                    }
                    else if ( UMAIsWindowFloating( window ) )
                    {
                        if ( win )
                            win->MacMouseDown( ev , windowPart ) ;
                    }
                    else
                    {
                        // Activate window first
                        ::SelectWindow( window ) ;

                        // Send event later
                        if ( win )
                            win->MacMouseDown( ev , windowPart ) ;
                    }
                }
                else
                {
                    if ( win )
                        win->MacMouseDown( ev , windowPart ) ;
                }
            break ;
        default:
            break;
    }
}

void wxApp::MacHandleMouseUpEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    WindowRef window;

    short windowPart = inNoWindow ;
    if ( wxTheApp->s_captureWindow )
    {
        window = (WindowRef) s_captureWindow->MacGetRootWindow() ;
        windowPart = inContent ;
    }
    else
    {
        windowPart = ::FindWindow(ev->where, &window) ;
    }

    switch (windowPart)
    {
        case inMenuBar :
            break ;
        case inSysWindow :
            break ;
        default:
            {
                wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) window ) ;
                if ( win )
                    win->MacMouseUp( ev , windowPart ) ;
            }
            break;
    }
}

#endif

long wxMacTranslateKey(unsigned char key, unsigned char code) ;
long wxMacTranslateKey(unsigned char key, unsigned char code)
{
    long retval = key ;
    switch (key)
    {
        case kHomeCharCode :
                 retval = WXK_HOME;
          break;
        case kEnterCharCode :
                 retval = WXK_RETURN;
          break;
        case kEndCharCode :
                 retval = WXK_END;
          break;
        case kHelpCharCode :
                 retval = WXK_HELP;
          break;
        case kBackspaceCharCode :
                 retval = WXK_BACK;
          break;
        case kTabCharCode :
                 retval = WXK_TAB;
          break;
        case kPageUpCharCode :
                 retval = WXK_PAGEUP;
          break;
        case kPageDownCharCode :
                 retval = WXK_PAGEDOWN;
          break;
        case kReturnCharCode :
                 retval = WXK_RETURN;
          break;
            case kFunctionKeyCharCode :
            {
                switch( code )
                {
                    case 0x7a :
                        retval = WXK_F1 ;
                        break;
                    case 0x78 :
                        retval = WXK_F2 ;
                        break;
                    case 0x63 :
                        retval = WXK_F3 ;
                        break;
                    case 0x76 :
                        retval = WXK_F4 ;
                        break;
                    case 0x60 :
                        retval = WXK_F5 ;
                        break;
                    case 0x61 :
                        retval = WXK_F6 ;
                        break;
                    case 0x62:
                        retval = WXK_F7 ;
                        break;
                    case 0x64 :
                        retval = WXK_F8 ;
                        break;
                    case 0x65 :
                        retval = WXK_F9 ;
                        break;
                    case 0x6D :
                        retval = WXK_F10 ;
                        break;
                    case 0x67 :
                        retval = WXK_F11 ;
                        break;
                    case 0x6F :
                        retval = WXK_F12 ;
                        break;
                    case 0x69 :
                        retval = WXK_F13 ;
                        break;
                    case 0x6B :
                        retval = WXK_F14 ;
                        break;
                    case 0x71 :
                        retval = WXK_F15 ;
                        break;
                }
            }
            break ;
            case kEscapeCharCode :
                retval = WXK_ESCAPE ;
            break ;
            case kLeftArrowCharCode :
                retval = WXK_LEFT ;
            break ;
            case kRightArrowCharCode :
                retval = WXK_RIGHT ;
            break ;
            case kUpArrowCharCode :
                retval = WXK_UP ;
            break ;
            case kDownArrowCharCode :
                retval = WXK_DOWN ;
            break ;
            case kDeleteCharCode :
                retval = WXK_DELETE ;
             default:
            break ;
     } // end switch

    return retval;
}

int wxKeyCodeToMacModifier(wxKeyCode key)
{
    switch (key)
    {
    case WXK_START:
    case WXK_MENU:
        return cmdKey;

    case WXK_SHIFT:
        return shiftKey;

    case WXK_CAPITAL:
        return alphaLock;

    case WXK_ALT:
        return optionKey;

    case WXK_CONTROL:
        return controlKey;

    default:
        return 0;
    }
}

bool wxGetKeyState(wxKeyCode key) //virtual key code if < 10.2.x, else see below
{
    wxASSERT_MSG(key != WXK_LBUTTON && key != WXK_RBUTTON && key !=
        WXK_MBUTTON, wxT("can't use wxGetKeyState() for mouse buttons"));

    KeyMap keymap;
    GetKeys(keymap);
    return !!(BitTst(keymap, (sizeof(KeyMap)*8) - key));
}

#if !TARGET_CARBON
void wxApp::MacHandleKeyDownEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    wxToolTip::RemoveToolTips() ;

    UInt32 menuresult = UMAMenuEvent(ev) ;
    if ( HiWord( menuresult ) )
    {
        if ( !s_macIsInModalLoop )
            MacHandleMenuSelect( HiWord( menuresult ) , LoWord( menuresult ) ) ;
    }
    else
    {
         wxWindow* focus = wxWindow::FindFocus() ;

        if ( MacSendKeyDownEvent( focus , ev->message , ev->modifiers , ev->when , ev->where.h , ev->where.v ) == false )
        {
#if 0
            // we must handle control keys the other way round, otherwise text content is updated too late
            // has not been handled -> perform default
            wxControl* control = wxDynamicCast( focus , wxControl ) ;
            if ( control &&  control->GetMacControl() != NULL )
            {
                short keycode ;
                short keychar ;
                keychar = short(ev->message & charCodeMask);
                keycode = short(ev->message & keyCodeMask) >> 8 ;
                ::HandleControlKey( (ControlHandle) control->GetMacControl() , keycode , keychar , ev->modifiers ) ;
            }
#endif
        }
    }
}

void wxApp::MacHandleKeyUpEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    wxToolTip::RemoveToolTips() ;

    UInt32 menuresult = UMAMenuEvent(ev) ;
    if ( HiWord( menuresult ) )
    {
    }
    else
    {
        MacSendKeyUpEvent( wxWindow::FindFocus() , ev->message , ev->modifiers , ev->when , ev->where.h , ev->where.v ) ;
    }
}

#endif

bool wxApp::MacSendKeyDownEvent( wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey )
{
    if ( !focus )
        return false ;

    short keycode ;
    short keychar ;
    keychar = short(keymessage & charCodeMask);
    keycode = short(keymessage & keyCodeMask) >> 8 ;

    if ( modifiers & ( controlKey|shiftKey|optionKey ) )
    {
        // control interferes with some built-in keys like pgdown, return etc. therefore we remove the controlKey modifier
        // and look at the character after
        UInt32 state = 0;
        UInt32 keyInfo = KeyTranslate((Ptr)GetScriptManagerVariable(smKCHRCache), ( modifiers & (~(controlKey|shiftKey|optionKey))) | keycode, &state);
        keychar = short(keyInfo & charCodeMask);
        keycode = short(keyInfo & keyCodeMask) >> 8 ;
    }
    long keyval = wxMacTranslateKey(keychar, keycode) ;
    long realkeyval = keyval ;
    if ( keyval == keychar )
    {
        // we are not on a special character combo -> pass the real os event-value to EVT_CHAR, but not to EVT_KEY (make upper first)
        realkeyval = short(keymessage & charCodeMask) ;
        keyval = wxToupper( keyval ) ;
    }

    wxKeyEvent event(wxEVT_KEY_DOWN);
    bool handled = false ;
    event.m_shiftDown = modifiers & shiftKey;
    event.m_controlDown = modifiers & controlKey;
    event.m_altDown = modifiers & optionKey;
    event.m_metaDown = modifiers & cmdKey;
    event.m_keyCode = keyval ;

    event.m_x = wherex;
    event.m_y = wherey;
    event.SetTimestamp(when);
    event.SetEventObject(focus);
    handled = focus->GetEventHandler()->ProcessEvent( event ) ;
    if ( handled && event.GetSkipped() )
        handled = false ;
    if ( !handled )
    {
#if wxUSE_ACCEL
        if (!handled)
        {
            wxWindow *ancestor = focus;
            while (ancestor)
            {
                int command = ancestor->GetAcceleratorTable()->GetCommand( event );
                if (command != -1)
                {
                    wxCommandEvent command_event( wxEVT_COMMAND_MENU_SELECTED, command );
                    handled = ancestor->GetEventHandler()->ProcessEvent( command_event );
                    break;
                }
                if (ancestor->IsTopLevel())
                    break;
                ancestor = ancestor->GetParent();
            }
        }
#endif // wxUSE_ACCEL
    }
    if (!handled)
    {
        event.Skip( false ) ;
        event.SetEventType( wxEVT_CHAR ) ;
        // raw value again
        event.m_keyCode = realkeyval ;

        handled = focus->GetEventHandler()->ProcessEvent( event ) ;
        if ( handled && event.GetSkipped() )
            handled = false ;
    }
    if ( !handled &&
         (keyval == WXK_TAB) &&
// CS: copied the change below from wxGTK
// VZ: testing for wxTE_PROCESS_TAB shouldn't be done here the control may
//     have this style, yet choose not to process this particular TAB in which
//     case TAB must still work as a navigational character
#if 0
         (!focus->HasFlag(wxTE_PROCESS_TAB)) &&
#endif
         (focus->GetParent()) &&
         (focus->GetParent()->HasFlag( wxTAB_TRAVERSAL)) )
    {
        wxNavigationKeyEvent new_event;
        new_event.SetEventObject( focus );
        new_event.SetDirection( !event.ShiftDown() );
        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
        new_event.SetWindowChange( event.ControlDown() );
        new_event.SetCurrentFocus( focus );
        handled = focus->GetEventHandler()->ProcessEvent( new_event );
        if ( handled && new_event.GetSkipped() )
            handled = false ;
    }
    // backdoor handler for default return and command escape
    if ( !handled && (!focus->IsKindOf(CLASSINFO(wxControl) ) || !focus->MacCanFocus() ) )
    {
        // if window is not having a focus still testing for default enter or cancel
        // TODO add the UMA version for ActiveNonFloatingWindow
        wxWindow* focus = wxFindWinFromMacWindow( (WXWindow) FrontWindow() ) ;
        if ( focus )
        {
            if ( keyval == WXK_RETURN )
            {
                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
                if ( tlw && tlw->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return true ;
                    }
                }
            }
            /* generate wxID_CANCEL if command-. or <esc> has been pressed (typically in dialogs) */
            else if (keyval == WXK_ESCAPE || (keyval == '.' && modifiers & cmdKey ) )
            {
                wxCommandEvent new_event(wxEVT_COMMAND_BUTTON_CLICKED,wxID_CANCEL);
                new_event.SetEventObject( focus );
                handled = focus->GetEventHandler()->ProcessEvent( new_event );
            }
        }
    }
    return handled ;
}

bool wxApp::MacSendKeyUpEvent( wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey )
{
    if ( !focus )
        return false ;

    short keycode ;
    short keychar ;
    keychar = short(keymessage & charCodeMask);
    keycode = short(keymessage & keyCodeMask) >> 8 ;
    if ( modifiers & ( controlKey|shiftKey|optionKey ) )
    {
        // control interferes with some built-in keys like pgdown, return etc. therefore we remove the controlKey modifier
        // and look at the character after
        UInt32 state = 0;
        UInt32 keyInfo = KeyTranslate((Ptr)GetScriptManagerVariable(smKCHRCache), ( modifiers & (~(controlKey|shiftKey|optionKey))) | keycode, &state);
        keychar = short(keyInfo & charCodeMask);
        keycode = short(keyInfo & keyCodeMask) >> 8 ;
    }
    long keyval = wxMacTranslateKey(keychar, keycode) ;

    if ( keyval == keychar )
    {
        keyval = wxToupper( keyval ) ;
    }
    bool handled = false ;

    wxKeyEvent event(wxEVT_KEY_UP);
    event.m_shiftDown = modifiers & shiftKey;
    event.m_controlDown = modifiers & controlKey;
    event.m_altDown = modifiers & optionKey;
    event.m_metaDown = modifiers & cmdKey;
    event.m_keyCode = keyval ;

    event.m_x = wherex;
    event.m_y = wherey;
    event.SetTimestamp(when);
    event.SetEventObject(focus);
    handled = focus->GetEventHandler()->ProcessEvent( event ) ;

    return handled ;
}

#if !TARGET_CARBON
void wxApp::MacHandleActivateEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    WindowRef window = (WindowRef) ev->message ;
    if ( window )
    {
        bool activate = (ev->modifiers & activeFlag ) ;
        WindowClass wclass ;
        ::GetWindowClass ( window , &wclass ) ;
        if ( wclass == kFloatingWindowClass )
        {
            // if it is a floater we activate/deactivate the front non-floating window instead
            window = ::FrontNonFloatingWindow() ;
        }
        wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) window ) ;
        if ( win )
            win->MacActivate( ev->when , activate ) ;
    }
}

void wxApp::MacHandleUpdateEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    WindowRef window = (WindowRef) ev->message ;
    wxTopLevelWindowMac * win = wxFindWinFromMacWindow( (WXWindow) window ) ;
    if ( win )
    {
        if ( !wxPendingDelete.Member(win) )
            win->MacUpdate( ev->when ) ;
    }
    else
    {
        // since there is no way of telling this foreign window to update itself
        // we have to invalidate the update region otherwise we keep getting the same
        // event over and over again
        BeginUpdate( window ) ;
        EndUpdate( window ) ;
    }
}

void wxApp::MacHandleDiskEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    if ( HiWord( ev->message ) != noErr )
  {
        OSErr err ;
        Point point ;
         SetPt( &point , 100 , 100 ) ;

          err = DIBadMount( point , ev->message ) ;
        wxASSERT( err == noErr ) ;
    }
}

void wxApp::MacHandleOSEvent( WXEVENTREF evr )
{
    EventRecord* ev = (EventRecord*) evr ;
    switch( ( ev->message & osEvtMessageMask ) >> 24 )
    {
        case suspendResumeMessage :
            {
                bool isResuming = ev->message & resumeFlag ;
                bool convertClipboard = ev->message & convertClipboardFlag ;

                bool doesActivate = UMAGetProcessModeDoesActivateOnFGSwitch() ;
                if ( isResuming )
                {
                    WindowRef oldFrontWindow = NULL ;
                    WindowRef newFrontWindow = NULL ;

                    // in case we don't take care of activating ourselves, we have to synchronize
                    // our idea of the active window with the process manager's - which it already activated

                    if ( !doesActivate )
                        oldFrontWindow = ::FrontNonFloatingWindow() ;

                    MacResume( convertClipboard ) ;

                    newFrontWindow = ::FrontNonFloatingWindow() ;

                    if ( oldFrontWindow )
                    {
                        wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) oldFrontWindow ) ;
                        if ( win )
                            win->MacActivate( ev->when , false ) ;
                    }
                    if ( newFrontWindow )
                    {
                        wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) newFrontWindow ) ;
                        if ( win )
                            win->MacActivate( ev->when , true ) ;
                    }
                }
                else
                {
                    MacSuspend( convertClipboard ) ;
                }
            }
            break ;
        case mouseMovedMessage :
            {
                WindowRef window;

                wxWindow* currentMouseWindow = NULL ;

                if (s_captureWindow )
                {
                    currentMouseWindow = s_captureWindow ;
                }
                else
                {
                    wxWindow::MacGetWindowFromPoint( wxPoint( ev->where.h , ev->where.v ) ,
                                                             &currentMouseWindow ) ;
                }

                if ( currentMouseWindow != wxWindow::s_lastMouseWindow )
                {
                    wxMouseEvent event ;

                    bool isDown = !(ev->modifiers & btnState) ; // 1 is for up
                    bool controlDown = ev->modifiers & controlKey ; // for simulating right mouse

                    event.m_leftDown = isDown && !controlDown;
                    event.m_middleDown = false;
                    event.m_rightDown = isDown && controlDown;
                    event.m_shiftDown = ev->modifiers & shiftKey;
                    event.m_controlDown = ev->modifiers & controlKey;
                    event.m_altDown = ev->modifiers & optionKey;
                    event.m_metaDown = ev->modifiers & cmdKey;
                    event.m_x = ev->where.h;
                    event.m_y = ev->where.v;
                    event.SetTimestamp( ev->when );
                    event.SetEventObject(this);

                    if ( wxWindow::s_lastMouseWindow )
                    {
                        wxMouseEvent eventleave(event);
                        eventleave.SetEventType( wxEVT_LEAVE_WINDOW );
                        wxWindow::s_lastMouseWindow->ScreenToClient( &eventleave.m_x, &eventleave.m_y );
                        eventleave.SetEventObject( wxWindow::s_lastMouseWindow ) ;

                        wxWindow::s_lastMouseWindow->GetEventHandler()->ProcessEvent(eventleave);
                    }
                    if ( currentMouseWindow )
                    {
                        wxMouseEvent evententer(event);
                        evententer.SetEventType( wxEVT_ENTER_WINDOW );
                        currentMouseWindow->ScreenToClient( &evententer.m_x, &evententer.m_y );
                        evententer.SetEventObject( currentMouseWindow ) ;
                        currentMouseWindow->GetEventHandler()->ProcessEvent(evententer);
                    }
                    wxWindow::s_lastMouseWindow = currentMouseWindow ;
                }

                short windowPart = inNoWindow ;

                if ( s_captureWindow )
                {
                    window = (WindowRef) s_captureWindow->MacGetRootWindow() ;
                    windowPart = inContent ;
                }
                else
                {
                    windowPart = ::FindWindow(ev->where, &window);
                }

                switch (windowPart)
                {
                    case inContent :
                        {
                            wxTopLevelWindowMac* win = wxFindWinFromMacWindow( (WXWindow) window ) ;
                            if ( win )
                                win->MacMouseMoved( ev , windowPart ) ;
                            else
                            {
                                if ( wxIsBusy() )
                                {
                                }
                                else
                                    UMAShowArrowCursor();
                             }
                        }
                        break;
                    default :
                        {
                            if ( wxIsBusy() )
                            {
                            }
                            else
                                UMAShowArrowCursor();
                        }
                        break ;
                }
            }
            break ;

    }
}
#else

void wxApp::MacHandleMouseMovedEvent(wxInt32 x , wxInt32 y ,wxUint32 modifiers , long timestamp)
{
    WindowRef window;

    wxWindow* currentMouseWindow = NULL ;

    if (s_captureWindow )
    {
        currentMouseWindow = s_captureWindow ;
    }
    else
    {
        wxWindow::MacGetWindowFromPoint( wxPoint( x, y ) , &currentMouseWindow ) ;
    }

    if ( currentMouseWindow != wxWindow::s_lastMouseWindow )
    {
        wxMouseEvent event ;

        bool isDown = !(modifiers & btnState) ; // 1 is for up
        bool controlDown = modifiers & controlKey ; // for simulating right mouse

        event.m_leftDown = isDown && !controlDown;

        event.m_middleDown = false;
        event.m_rightDown = isDown && controlDown;

        event.m_shiftDown = modifiers & shiftKey;
        event.m_controlDown = modifiers & controlKey;
        event.m_altDown = modifiers & optionKey;
        event.m_metaDown = modifiers & cmdKey;

        event.m_x = x;
        event.m_y = y;
        event.SetTimestamp(timestamp);

        if ( wxWindow::s_lastMouseWindow )
        {
            wxMouseEvent eventleave(event);
            eventleave.SetEventType( wxEVT_LEAVE_WINDOW );
            wxWindow::s_lastMouseWindow->ScreenToClient( &eventleave.m_x, &eventleave.m_y );
            eventleave.SetEventObject( wxWindow::s_lastMouseWindow ) ;

#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( wxWindow::s_lastMouseWindow , eventleave);
#endif // wxUSE_TOOLTIPS
            wxWindow::s_lastMouseWindow->GetEventHandler()->ProcessEvent(eventleave);
        }
        if ( currentMouseWindow )
        {
            wxMouseEvent evententer(event);
            evententer.SetEventType( wxEVT_ENTER_WINDOW );
            currentMouseWindow->ScreenToClient( &evententer.m_x, &evententer.m_y );
            evententer.SetEventObject( currentMouseWindow ) ;
#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( currentMouseWindow , evententer);
#endif // wxUSE_TOOLTIPS
            currentMouseWindow->GetEventHandler()->ProcessEvent(evententer);
        }
        wxWindow::s_lastMouseWindow = currentMouseWindow ;
    }

    short windowPart = inNoWindow ;

    if ( s_captureWindow )
    {
        window = (WindowRef) s_captureWindow->MacGetRootWindow() ;
        windowPart = inContent ;
    }
    else
    {
        Point pt= { y , x } ;
        windowPart = ::FindWindow(pt , &window);
    }

    switch (windowPart)
    {
        case inContent :
            {
                wxTopLevelWindowMac* win = wxFindWinFromMacWindow( window ) ;
                if ( win )
                    win->MacFireMouseEvent( nullEvent , x , y , modifiers , timestamp ) ;
                else
                {
                    if ( wxIsBusy() )
                    {
                    }
                    else
                        UMAShowArrowCursor();
                 }
            }
            break;
        default :
            {
                if ( wxIsBusy() )
                {
                }
                else
                    UMAShowArrowCursor();
            }
            break ;
    }
}
#endif

void wxApp::MacHandleMenuCommand( wxUint32 id )
{
    wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar() ;
    wxFrame* frame = mbar->GetFrame();
    wxCHECK_RET( mbar != NULL && frame != NULL, wxT("error in menu item callback") );
    if ( frame )
    {
        frame->ProcessCommand(id);
    }
}

#if !TARGET_CARBON
void wxApp::MacHandleMenuSelect( int macMenuId , int macMenuItemNum )
{
    if (macMenuId == 0)
         return; // no menu item selected

    if (macMenuId == kwxMacAppleMenuId && macMenuItemNum > 1)
    {
        #if ! TARGET_CARBON
        Str255        deskAccessoryName ;
        GrafPtr        savedPort ;

        GetMenuItemText(GetMenuHandle(kwxMacAppleMenuId), macMenuItemNum, deskAccessoryName);
        GetPort(&savedPort);
        OpenDeskAcc(deskAccessoryName);
        SetPort(savedPort);
        #endif
    }
    else
    {
        MenuCommand id ;
        GetMenuItemCommandID( GetMenuHandle(macMenuId) , macMenuItemNum , &id ) ;
        MacHandleMenuCommand( id ) ;
    }
    HiliteMenu(0);
}
#endif
