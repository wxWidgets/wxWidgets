/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/app.cpp
// Purpose:     wxApp
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

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
#include "wx/link.h"

#include <string.h>

// mac

#ifndef __DARWIN__
    #if __option(profile)
        #include <profiler.h>
    #endif
#endif

// #include "apprsrc.h"

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

// Keep linker from discarding wxStockGDIMac
wxFORCE_LINK_MODULE(gdiobj)

// statics for implementation
static bool s_inYield = false;
static bool s_inReceiveEvent = false ;
static EventTime sleepTime = kEventDurationNoWait ;


IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
    EVT_END_SESSION(wxApp::OnEndSession)
    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()


// platform specific static variables
static const short kwxMacAppleMenuId = 1 ;

wxWindow* wxApp::s_captureWindow = NULL ;
long      wxApp::s_lastModifiers = 0 ;

long      wxApp::s_macAboutMenuItemId = wxID_ABOUT ;
long      wxApp::s_macPreferencesMenuItemId = wxID_PREFERENCES ;
long      wxApp::s_macExitMenuItemId = wxID_EXIT ;
wxString  wxApp::s_macHelpMenuTitleName = wxT("&Help") ;

bool      wxApp::sm_isEmbedded = false; // Normally we're not a plugin

//----------------------------------------------------------------------
// Core Apple Event Support
//----------------------------------------------------------------------

pascal OSErr AEHandleODoc( const AppleEvent *event , AppleEvent *reply , SRefCon refcon ) ;
pascal OSErr AEHandleOApp( const AppleEvent *event , AppleEvent *reply , SRefCon refcon ) ;
pascal OSErr AEHandlePDoc( const AppleEvent *event , AppleEvent *reply , SRefCon refcon ) ;
pascal OSErr AEHandleQuit( const AppleEvent *event , AppleEvent *reply , SRefCon refcon ) ;
pascal OSErr AEHandleRApp( const AppleEvent *event , AppleEvent *reply , SRefCon refcon ) ;

pascal OSErr AEHandleODoc( const AppleEvent *event , AppleEvent *reply , SRefCon WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEODoc( (AppleEvent*) event , reply) ;
}

pascal OSErr AEHandleOApp( const AppleEvent *event , AppleEvent *reply , SRefCon WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEOApp( (AppleEvent*) event , reply ) ;
}

pascal OSErr AEHandlePDoc( const AppleEvent *event , AppleEvent *reply , SRefCon WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEPDoc( (AppleEvent*) event , reply ) ;
}

pascal OSErr AEHandleQuit( const AppleEvent *event , AppleEvent *reply , SRefCon WXUNUSED(refcon) )
{
    return wxTheApp->MacHandleAEQuit( (AppleEvent*) event , reply) ;
}

pascal OSErr AEHandleRApp( const AppleEvent *event , AppleEvent *reply , SRefCon WXUNUSED(refcon) )
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

    wxString fName ;
    FSRef theRef ;

    for (i = 1; i <= itemsInList; i++)
    {
        AEGetNthPtr(
            &docList, i, typeFSRef, &keywd, &returnedType,
            (Ptr)&theRef, sizeof(theRef), &actualSize);
        fName = wxMacFSRefToPath( &theRef ) ;

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

    wxString fName ;
    FSRef theRef ;

    for (i = 1; i <= itemsInList; i++)
    {
        AEGetNthPtr(
            &docList, i, typeFSRef, &keywd, &returnedType,
            (Ptr)&theRef, sizeof(theRef), &actualSize);
        fName = wxMacFSRefToPath( &theRef ) ;

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
#if wxUSE_DOC_VIEW_ARCHITECTURE
    wxDocManager* dm = wxDocManager::GetDocumentManager() ;
    if ( dm )
        dm->CreateDocument(fileName , wxDOC_SILENT ) ;
#endif
}


void wxApp::MacPrintFile(const wxString & fileName )
{
#if wxUSE_DOC_VIEW_ARCHITECTURE

#if wxUSE_PRINTING_ARCHITECTURE
    wxDocManager* dm = wxDocManager::GetDocumentManager() ;
    if ( dm )
    {
        wxDocument *doc = dm->CreateDocument(fileName , wxDOC_SILENT ) ;
        if ( doc )
        {
            wxView* view = doc->GetFirstView() ;
            if ( view )
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
#endif //print

#endif //docview
}



void wxApp::MacNewFile()
{
}

void wxApp::MacReopenApp()
{
    // HIG says :
    // if there is no open window -> create a new one
    // if all windows are hidden -> show the first
    // if some windows are not hidden -> do nothing

    wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
    if ( node == NULL )
    {
        MacNewFile() ;
    }
    else
    {
        wxTopLevelWindow* firstIconized = NULL ;
        while (node)
        {
            wxTopLevelWindow* win = (wxTopLevelWindow*) node->GetData();
            if ( !win->IsIconized() )
            {
                firstIconized = NULL ;
                break ;
            }
            else
            {
                if ( firstIconized == NULL )
                    firstIconized = win ;
            }

            node = node->GetNext();
        }

        if ( firstIconized )
            firstIconized->Iconize( false ) ;
    }
}

//----------------------------------------------------------------------
// Macintosh CommandID support - converting between native and wx IDs
//----------------------------------------------------------------------

// if no native match they just return the passed-in id

struct IdPair
{
    UInt32 macId ;
    int wxId ;
} ;

IdPair gCommandIds [] =
{
    { kHICommandCut ,           wxID_CUT } ,
    { kHICommandCopy ,          wxID_COPY } ,
    { kHICommandPaste ,         wxID_PASTE } ,
    { kHICommandSelectAll ,     wxID_SELECTALL } ,
    { kHICommandClear ,         wxID_CLEAR } ,
    { kHICommandUndo ,          wxID_UNDO } ,
    { kHICommandRedo ,          wxID_REDO } ,
} ;

int wxMacCommandToId( UInt32 macCommandId )
{
    int wxid = 0 ;

    switch ( macCommandId )
    {
        case kHICommandPreferences :
            wxid = wxApp::s_macPreferencesMenuItemId ;
            break ;

        case kHICommandQuit :
            wxid = wxApp::s_macExitMenuItemId ;
            break ;

        case kHICommandAbout :
            wxid = wxApp::s_macAboutMenuItemId ;
            break ;

        default :
            {
                for ( size_t i = 0 ; i < WXSIZEOF(gCommandIds) ; ++i )
                {
                    if ( gCommandIds[i].macId == macCommandId )
                    {
                        wxid = gCommandIds[i].wxId ;
                        break ;
                    }
                }
            }
            break ;
    }

    if ( wxid == 0 )
        wxid = (int) macCommandId ;

    return wxid ;
}

UInt32 wxIdToMacCommand( int wxId )
{
    UInt32 macId = 0 ;

    if ( wxId == wxApp::s_macPreferencesMenuItemId )
        macId = kHICommandPreferences ;
    else if (wxId == wxApp::s_macExitMenuItemId)
        macId = kHICommandQuit ;
    else if (wxId == wxApp::s_macAboutMenuItemId)
        macId = kHICommandAbout ;
    else
    {
        for ( size_t i = 0 ; i < WXSIZEOF(gCommandIds) ; ++i )
        {
            if ( gCommandIds[i].wxId == wxId )
            {
                macId = gCommandIds[i].macId ;
                break ;
            }
        }
    }

    if ( macId == 0 )
        macId = (int) wxId ;

    return macId ;
}

wxMenu* wxFindMenuFromMacCommand( const HICommand &command , wxMenuItem* &item )
{
    wxMenu* itemMenu = NULL ;
    int id = 0 ;

    // for 'standard' commands which don't have a wx-menu
    if ( command.commandID == kHICommandPreferences || command.commandID == kHICommandQuit || command.commandID == kHICommandAbout )
    {
        id = wxMacCommandToId( command.commandID ) ;

        wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar() ;
        if ( mbar )
            item = mbar->FindItem( id , &itemMenu ) ;
    }
    else if ( command.commandID != 0 && command.menu.menuRef != 0 && command.menu.menuItemIndex != 0 )
    {
        id = wxMacCommandToId( command.commandID ) ;
        // make sure it is one of our own menus, or of the 'synthetic' apple and help menus , otherwise don't touch
        MenuItemIndex firstUserHelpMenuItem ;
        static MenuHandle helpMenuHandle = NULL ;
        if ( helpMenuHandle == NULL )
        {
            if ( UMAGetHelpMenuDontCreate( &helpMenuHandle , &firstUserHelpMenuItem) != noErr )
                helpMenuHandle = NULL ;
        }

        // is it part of the application or the Help menu, then look for the id directly
        if ( ( GetMenuHandle( kwxMacAppleMenuId ) != NULL && command.menu.menuRef == GetMenuHandle( kwxMacAppleMenuId ) ) ||
             ( helpMenuHandle != NULL && command.menu.menuRef == helpMenuHandle ) )
        {
            wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar() ;
            if ( mbar )
                item = mbar->FindItem( id , &itemMenu ) ;
        }
        else
        {
            URefCon refCon ;

            GetMenuItemRefCon( command.menu.menuRef , command.menu.menuItemIndex , &refCon ) ;
            itemMenu = wxFindMenuFromMacMenu( command.menu.menuRef ) ;
            if ( itemMenu != NULL )
                item = (wxMenuItem*) refCon ;
        }
    }

    return itemMenu ;
}

//----------------------------------------------------------------------
// Carbon Event Handler
//----------------------------------------------------------------------

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
wxMacAppMenuEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    wxMacCarbonEvent cEvent( event ) ;
    MenuRef menuRef = cEvent.GetParameter<MenuRef>(kEventParamDirectObject) ;
    wxMenu* menu = wxFindMenuFromMacMenu( menuRef ) ;

    if ( menu )
    {
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
                cmd = cEvent.GetParameter<MenuCommand>(kEventParamMenuCommand,typeMenuCommand) ;
                if (cmd != 0)
                    type = wxEVT_MENU_HIGHLIGHT;
                break;

            default:
                wxFAIL_MSG(wxT("Unexpected menu event kind"));
                break;
        }

        if ( type )
        {
            wxMenuEvent wxevent(type, cmd, menu);
            wxevent.SetEventObject(menu);

            wxEvtHandler* handler = menu->GetEventHandler();
            if (handler && handler->ProcessEvent(wxevent))
            {
                // handled
            }
            else
            {
                wxWindow *win = menu->GetInvokingWindow();
                if (win)
                    win->GetEventHandler()->ProcessEvent(wxevent);
                }
            }
    }

    return eventNotHandledErr;
}

static pascal OSStatus wxMacAppCommandEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    HICommand command ;

    wxMacCarbonEvent cEvent( event ) ;
    cEvent.GetParameter<HICommand>(kEventParamDirectObject,typeHICommand,&command) ;

    wxMenuItem* item = NULL ;
    wxMenu* itemMenu = wxFindMenuFromMacCommand( command , item ) ;
    int id = wxMacCommandToId( command.commandID ) ;

    if ( item )
    {
        wxASSERT( itemMenu != NULL ) ;

        switch ( cEvent.GetKind() )
        {
            case kEventProcessCommand :
            {
                if (item->IsCheckable())
                    item->Check( !item->IsChecked() ) ;

                if ( itemMenu->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) )
                    result = noErr ;
            }
            break ;

        case kEventCommandUpdateStatus:
            {
                wxUpdateUIEvent event(id);
                event.SetEventObject( itemMenu );

                bool processed = false;

                // Try the menu's event handler
                {
                    wxEvtHandler *handler = itemMenu->GetEventHandler();
                    if ( handler )
                        processed = handler->ProcessEvent(event);
                }

                // Try the window the menu was popped up from
                // (and up through the hierarchy)
                if ( !processed )
                {
                    const wxMenuBase *menu = itemMenu;
                    while ( menu )
                    {
                        wxWindow *win = menu->GetInvokingWindow();
                        if ( win )
                        {
                            processed = win->GetEventHandler()->ProcessEvent(event);
                            break;
                        }

                        menu = menu->GetParent();
                    }
                }

                if ( processed )
                {
                    // if anything changed, update the changed attribute
                    if (event.GetSetText())
                        itemMenu->SetLabel(id, event.GetText());
                    if (event.GetSetChecked())
                        itemMenu->Check(id, event.GetChecked());
                    if (event.GetSetEnabled())
                        itemMenu->Enable(id, event.GetEnabled());

                    result = noErr ;
                }
            }
            break ;

        default :
            break ;
        }
    }
    return result ;
}

static pascal OSStatus wxMacAppApplicationEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    switch ( GetEventKind( event ) )
    {
        case kEventAppActivated :
            if ( wxTheApp )
                wxTheApp->SetActive( true , NULL ) ;
            result = noErr ;
            break ;

        case kEventAppDeactivated :
            if ( wxTheApp )
                wxTheApp->SetActive( false , NULL ) ;
            result = noErr ;
            break ;

        default :
            break ;
    }

    return result ;
}

pascal OSStatus wxMacAppEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    EventRef formerEvent = (EventRef) wxTheApp->MacGetCurrentEvent() ;
    EventHandlerCallRef formerEventHandlerCallRef = (EventHandlerCallRef) wxTheApp->MacGetCurrentEventHandlerCallRef() ;
    wxTheApp->MacSetCurrentEvent( event , handler ) ;

    OSStatus result = eventNotHandledErr ;
    switch ( GetEventClass( event ) )
    {
        case kEventClassCommand :
            result = wxMacAppCommandEventHandler( handler , event , data ) ;
            break ;

        case kEventClassApplication :
            result = wxMacAppApplicationEventHandler( handler , event , data ) ;
            break ;

        case kEventClassMenu :
            result = wxMacAppMenuEventHandler( handler , event , data ) ;
            break ;

        case kEventClassMouse :
            {
                wxMacCarbonEvent cEvent( event ) ;

                WindowRef window ;
                Point screenMouseLocation = cEvent.GetParameter<Point>(kEventParamMouseLocation) ;
                ::FindWindow(screenMouseLocation, &window);
                // only send this event in case it had not already been sent to a tlw, as we get
                // double events otherwise (in case event.skip) was called
                if ( window == NULL )
                    result = wxMacTopLevelMouseEventHandler( handler , event , NULL ) ;
            }
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

    wxTheApp->MacSetCurrentEvent( formerEvent, formerEventHandlerCallRef ) ;

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacAppEventHandler )

#ifdef __WXDEBUG__

pascal static void wxMacAssertOutputHandler(OSType componentSignature, UInt32 options,
    const char *assertionString, const char *exceptionLabelString,
    const char *errorString, const char *fileName, long lineNumber, void *value, ConstStr255Param outputMsg)
{
    // flow into assert handling
    wxString fileNameStr ;
    wxString assertionStr ;
    wxString exceptionStr ;
    wxString errorStr ;

#if wxUSE_UNICODE
    fileNameStr = wxString(fileName, wxConvLocal);
    assertionStr = wxString(assertionString, wxConvLocal);
    exceptionStr = wxString((exceptionLabelString!=0) ? exceptionLabelString : "", wxConvLocal) ;
    errorStr = wxString((errorString!=0) ? errorString : "", wxConvLocal) ;
#else
    fileNameStr = fileName;
    assertionStr = assertionString;
    exceptionStr = (exceptionLabelString!=0) ? exceptionLabelString : "" ;
    errorStr = (errorString!=0) ? errorString : "" ;
#endif

#if 1
    // flow into log
    wxLogDebug( wxT("AssertMacros: %s %s %s file: %s, line: %ld (value %p)\n"),
        assertionStr.c_str() ,
        exceptionStr.c_str() ,
        errorStr.c_str(),
        fileNameStr.c_str(), lineNumber ,
        value ) ;
#else

    wxOnAssert(fileNameStr, lineNumber , assertionStr ,
        wxString::Format( wxT("%s %s value (%p)") , exceptionStr, errorStr , value ) ) ;
#endif
}

#endif //__WXDEBUG__

#ifdef __WXMAC_OSX__
extern "C"
{
   // m_macEventPosted run loop source callback:
   void macPostedEventCallback(void *unused);
}

void macPostedEventCallback(void *unused)
{
    wxTheApp->ProcessPendingEvents();
}
#endif

bool wxApp::Initialize(int& argc, wxChar **argv)
{
    // Mac-specific

#ifdef __WXDEBUG__
    InstallDebugAssertOutputHandler( NewDebugAssertOutputHandlerUPP( wxMacAssertOutputHandler ) );
#endif

    UMAInitToolbox( 4, sm_isEmbedded ) ;
    SetEventMask( everyEvent ) ;
    UMAShowWatchCursor() ;

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

#if TARGET_API_MAC_OSX
    // these might be the startup dirs, set them to the 'usual' dir containing the app bundle
    wxString startupCwd = wxGetCwd() ;
    if ( startupCwd == wxT("/") || startupCwd.Right(15) == wxT("/Contents/MacOS") )
    {
        CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle() ) ;
        CFURLRef urlParent = CFURLCreateCopyDeletingLastPathComponent( kCFAllocatorDefault , url ) ;
        CFRelease( url ) ;
        CFStringRef path = CFURLCopyFileSystemPath ( urlParent , kCFURLPOSIXPathStyle ) ;
        CFRelease( urlParent ) ;
        wxString cwd = wxMacCFStringHolder(path).AsString(wxLocale::GetSystemEncoding());
        wxSetWorkingDirectory( cwd ) ;
    }
#endif

    wxMacCreateNotifierTable() ;

#ifdef __WXMAC_OSX__
    /* connect posted events to common-mode run loop so that wxPostEvent events
       are handled even while we're in the menu or on a scrollbar */
    CFRunLoopSourceContext event_posted_context = {0};
    event_posted_context.perform = macPostedEventCallback;
    m_macEventPosted = CFRunLoopSourceCreate(NULL,0,&event_posted_context);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), m_macEventPosted, kCFRunLoopCommonModes);
	// run loop takes ownership
	CFRelease(m_macEventPosted);
#endif

    UMAShowArrowCursor() ;

    return true;
}

AEEventHandlerUPP sODocHandler = NULL ;
AEEventHandlerUPP sOAppHandler = NULL ;
AEEventHandlerUPP sPDocHandler = NULL ;
AEEventHandlerUPP sRAppHandler = NULL ;
AEEventHandlerUPP sQuitHandler = NULL ;

bool wxApp::OnInitGui()
{
    if ( !wxAppBase::OnInitGui() )
        return false ;

    InstallStandardEventHandler( GetApplicationEventTarget() ) ;

    if (!sm_isEmbedded)
    {
        InstallApplicationEventHandler(
            GetwxMacAppEventHandlerUPP(),
            GetEventTypeCount(eventList), eventList, wxTheApp, (EventHandlerRef *)&(wxTheApp->m_macEventHandler));
    }

    if (!sm_isEmbedded)
    {
        sODocHandler = NewAEEventHandlerUPP(AEHandleODoc) ;
        sOAppHandler = NewAEEventHandlerUPP(AEHandleOApp) ;
        sPDocHandler = NewAEEventHandlerUPP(AEHandlePDoc) ;
        sRAppHandler = NewAEEventHandlerUPP(AEHandleRApp) ;
        sQuitHandler = NewAEEventHandlerUPP(AEHandleQuit) ;

        AEInstallEventHandler( kCoreEventClass , kAEOpenDocuments ,
                               sODocHandler , 0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEOpenApplication ,
                               sOAppHandler , 0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEPrintDocuments ,
                               sPDocHandler , 0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEReopenApplication ,
                               sRAppHandler , 0 , FALSE ) ;
        AEInstallEventHandler( kCoreEventClass , kAEQuitApplication ,
                               sQuitHandler , 0 , FALSE ) ;
    }

    return true ;
}

void wxApp::CleanUp()
{
#if wxUSE_TOOLTIPS
    wxToolTip::RemoveToolTips() ;
#endif

#ifdef __WXMAC_OSX__
    if (m_macEventPosted)
	{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m_macEventPosted, kCFRunLoopCommonModes);
		m_macEventPosted = NULL;
	}
#endif

    // One last chance for pending objects to be cleaned up
    wxTheApp->DeletePendingObjects();

    wxMacDestroyNotifierTable() ;

#ifndef __DARWIN__
#  if __option(profile)
    ProfilerDump( (StringPtr)"\papp.prof" ) ;
    ProfilerTerm() ;
#  endif
#endif

    UMACleanupToolbox() ;

    if (!sm_isEmbedded)
        RemoveEventHandler( (EventHandlerRef)(wxTheApp->m_macEventHandler) );

    if (!sm_isEmbedded)
    {
        AERemoveEventHandler( kCoreEventClass , kAEOpenDocuments ,
                               sODocHandler , FALSE ) ;
        AERemoveEventHandler( kCoreEventClass , kAEOpenApplication ,
                               sOAppHandler , FALSE ) ;
        AERemoveEventHandler( kCoreEventClass , kAEPrintDocuments ,
                               sPDocHandler , FALSE ) ;
        AERemoveEventHandler( kCoreEventClass , kAEReopenApplication ,
                               sRAppHandler , FALSE ) ;
        AERemoveEventHandler( kCoreEventClass , kAEQuitApplication ,
                               sQuitHandler , FALSE ) ;

        DisposeAEEventHandlerUPP( sODocHandler ) ;
        DisposeAEEventHandlerUPP( sOAppHandler ) ;
        DisposeAEEventHandlerUPP( sPDocHandler ) ;
        DisposeAEEventHandlerUPP( sRAppHandler ) ;
        DisposeAEEventHandlerUPP( sQuitHandler ) ;
    }

    wxAppBase::CleanUp();
}

//----------------------------------------------------------------------
// misc initialization stuff
//----------------------------------------------------------------------

#if defined(WXMAKINGDLL_CORE) && !defined(__DARWIN__)

// for shared libraries we have to manually get the correct resource
// ref num upon initializing and releasing when terminating, therefore
// the __wxinitialize and __wxterminate must be used

extern "C"
{
    void __sinit(void);    // (generated by linker)
    pascal OSErr __initialize(const CFragInitBlock *theInitBlock);
    pascal void __terminate(void);
}

pascal OSErr __wxinitialize(const CFragInitBlock *theInitBlock)
{
    return __initialize( theInitBlock ) ;
}

pascal void __wxterminate(void)
{
    __terminate() ;
}

#endif /* WXMAKINGDLL_CORE && !__DARWIN__ */

bool wxMacConvertEventToRecord( EventRef event , EventRecord *rec)
{
    OSStatus err = noErr ;
    bool converted = ConvertEventRefToEventRecord( event, rec) ;

    if ( !converted )
    {
        switch ( GetEventClass( event ) )
        {
            case kEventClassKeyboard :
            {
                converted = true ;
                switch ( GetEventKind(event) )
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
#ifndef __LP64__
                    GetMouse( &rec->where) ;
#endif
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
                switch ( GetEventKind( event ) )
                {
                    case kEventTextInputUnicodeForKeyEvent :
                        {
                            EventRef rawEvent ;
                            err = GetEventParameter(
                                event, kEventParamTextInputSendKeyboardEvent, typeEventRef, NULL,
                                sizeof(rawEvent), NULL, &rawEvent ) ;
                            converted = true ;

                            {
                                UInt32 keyCode, modifiers;
                                unsigned char charCode ;
#ifndef __LP64__

                                GetMouse( &rec->where) ;
#endif
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

            default :
                break ;
        }
    }

    return converted ;
}

wxApp::wxApp()
{
    m_printMode = wxPRINT_WINDOWS;

    m_macCurrentEvent = NULL ;
    m_macCurrentEventHandlerCallRef = NULL ;

#ifdef __WXMAC_OSX__
    m_macEventPosted = NULL ;
#endif
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    wxAppBase::OnIdle(event);

    // If they are pending events, we must process them: pending events are
    // either events to the threads other than main or events posted with
    // wxPostEvent() functions
    wxMacProcessNotifierAndPendingEvents();

  if (!wxMenuBar::MacGetInstalledMenuBar() && wxMenuBar::MacGetCommonMenuBar())
    wxMenuBar::MacGetCommonMenuBar()->MacInstallMenuBar();
}

void wxApp::WakeUpIdle()
{
#ifdef __WXMAC_OSX__
    if (m_macEventPosted)
    {
        CFRunLoopSourceSignal(m_macEventPosted);
    }
#endif

    wxMacWakeUp() ;
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

    // by definition yield should handle all non-processed events

    EventRef theEvent;

    OSStatus status = noErr ;

    while ( status == noErr )
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
    }

    wxMacProcessNotifierAndPendingEvents() ;
    s_inYield = false;

    return true;
}

void wxApp::MacDoOneEvent()
{
    EventRef theEvent;

    s_inReceiveEvent = true ;
    OSStatus status = ReceiveNextEvent(0, NULL, sleepTime, true, &theEvent) ;
    s_inReceiveEvent = false ;

    switch (status)
    {
        case eventLoopTimedOutErr :
            if ( wxTheApp->ProcessIdle() )
                sleepTime = kEventDurationNoWait ;
            else
                sleepTime = kEventDurationSecond;
            break;

        case eventLoopQuitErr :
            // according to QA1061 this may also occur
            // when a WakeUp Process is executed
            break;

        default:
            MacHandleOneEvent( theEvent ) ;
            ReleaseEvent( theEvent );
            sleepTime = kEventDurationNoWait ;
            break;
    }
    // repeaters

    DeletePendingObjects() ;
    wxMacProcessNotifierAndPendingEvents() ;
}

// virtual
void wxApp::MacHandleUnhandledEvent( WXEVENTREF evr )
{
    // Override to process unhandled events as you please
}

CFMutableArrayRef GetAutoReleaseArray()
{
    static CFMutableArrayRef array = 0;
    if ( array == 0)
        array= CFArrayCreateMutable(kCFAllocatorDefault,0,&kCFTypeArrayCallBacks);
    return array;
}

void wxApp::MacHandleOneEvent( WXEVENTREF evr )
{
    EventTargetRef theTarget;
    theTarget = GetEventDispatcherTarget();
    m_macCurrentEvent = evr ;

    OSStatus status = SendEventToEventTarget((EventRef) evr , theTarget);
    if (status == eventNotHandledErr)
        MacHandleUnhandledEvent(evr);

    wxMacProcessNotifierAndPendingEvents() ;

#if wxUSE_THREADS
    wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

    CFArrayRemoveAllValues( GetAutoReleaseArray() );
}

void wxApp::MacAddToAutorelease( void* cfrefobj )
{
    CFArrayAppendValue( GetAutoReleaseArray(), cfrefobj );
}

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
            switch ( code )
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

                default:
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
            break ;

        default:
            break ;
     } // end switch

    return retval;
}

int wxMacKeyCodeToModifier(wxKeyCode key)
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

#ifndef __DARWIN__
bool wxGetKeyState(wxKeyCode key) //virtual key code if < 10.2.x, else see below
{
    wxASSERT_MSG(key != WXK_LBUTTON && key != WXK_RBUTTON && key !=
        WXK_MBUTTON, wxT("can't use wxGetKeyState() for mouse buttons"));

//if OS X > 10.2 (i.e. 10.2.x)
//a known apple bug prevents the system from determining led
//states with GetKeys... can only determine caps lock led
     return !!(GetCurrentKeyModifiers() & wxMacKeyCodeToModifier(key));
//else
//  KeyMapByteArray keymap;
//  GetKeys((BigEndianLong*)keymap);
//  return !!(BitTst(keymap, (sizeof(KeyMapByteArray)*8) - iKey));
}
#endif


wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    wxPoint pt = wxGetMousePosition();
    ms.SetX(pt.x);
    ms.SetY(pt.y);

#if TARGET_API_MAC_OSX
    UInt32 buttons = GetCurrentButtonState();
    ms.SetLeftDown( (buttons & 0x01) != 0 );
    ms.SetMiddleDown( (buttons & 0x04) != 0 );
    ms.SetRightDown( (buttons & 0x02) != 0 );
#else
    ms.SetLeftDown( Button() );
    ms.SetMiddleDown( 0 );
    ms.SetRightDown( 0 );
#endif

    UInt32 modifiers = GetCurrentKeyModifiers();
    ms.SetControlDown(modifiers & controlKey);
    ms.SetShiftDown(modifiers & shiftKey);
    ms.SetAltDown(modifiers & optionKey);
    ms.SetMetaDown(modifiers & cmdKey);

    return ms;
}

// TODO : once the new key/char handling is tested, move all the code to wxWindow

bool wxApp::MacSendKeyDownEvent( wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey , wxChar uniChar )
{
    if ( !focus )
        return false ;

    bool handled;
    wxKeyEvent event(wxEVT_KEY_DOWN) ;
    MacCreateKeyEvent( event, focus , keymessage , modifiers , when , wherex , wherey , uniChar ) ;

    handled = focus->GetEventHandler()->ProcessEvent( event ) ;
    if ( handled && event.GetSkipped() )
        handled = false ;

#if wxUSE_ACCEL
    if ( !handled )
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

    return handled ;
}

bool wxApp::MacSendKeyUpEvent( wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey , wxChar uniChar )
{
    if ( !focus )
        return false ;

    bool handled;
    wxKeyEvent event( wxEVT_KEY_UP ) ;
    MacCreateKeyEvent( event, focus , keymessage , modifiers , when , wherex , wherey , uniChar ) ;
    handled = focus->GetEventHandler()->ProcessEvent( event ) ;

    return handled ;
}

bool wxApp::MacSendCharEvent( wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey , wxChar uniChar )
{
    if ( !focus )
        return false ;

    wxKeyEvent event(wxEVT_CHAR) ;
    MacCreateKeyEvent( event, focus , keymessage , modifiers , when , wherex , wherey , uniChar ) ;
    long keyval = event.m_keyCode ;

    bool handled = false ;

    wxTopLevelWindowMac *tlw = focus->MacGetTopLevelWindow() ;

    if (tlw)
    {
        event.SetEventType( wxEVT_CHAR_HOOK );
        handled = tlw->GetEventHandler()->ProcessEvent( event );
        if ( handled && event.GetSkipped() )
            handled = false ;
    }

    if ( !handled )
    {
        event.SetEventType( wxEVT_CHAR );
        event.Skip( false ) ;
        handled = focus->GetEventHandler()->ProcessEvent( event ) ;
    }

    if ( !handled && (keyval == WXK_TAB) )
    {
        wxWindow* iter = focus->GetParent() ;
        while ( iter && !handled )
        {
            if ( iter->HasFlag( wxTAB_TRAVERSAL ) )
            {
                wxNavigationKeyEvent new_event;
                new_event.SetEventObject( focus );
                new_event.SetDirection( !event.ShiftDown() );
                /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
                new_event.SetWindowChange( event.ControlDown() );
                new_event.SetCurrentFocus( focus );
                handled = focus->GetParent()->GetEventHandler()->ProcessEvent( new_event );
                if ( handled && new_event.GetSkipped() )
                    handled = false ;
            }

            iter = iter->GetParent() ;
        }
    }

    // backdoor handler for default return and command escape
    if ( !handled && (!focus->IsKindOf(CLASSINFO(wxControl) ) || !focus->MacCanFocus() ) )
    {
        // if window is not having a focus still testing for default enter or cancel
        // TODO: add the UMA version for ActiveNonFloatingWindow
        wxWindow* focus = wxFindWinFromMacWindow( FrontWindow() ) ;
        if ( focus )
        {
            if ( keyval == WXK_RETURN || keyval == WXK_NUMPAD_ENTER )
            {
                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(focus), wxTopLevelWindow);
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
            else if (keyval == WXK_ESCAPE || (keyval == '.' && modifiers & cmdKey ) )
            {
                // generate wxID_CANCEL if command-. or <esc> has been pressed (typically in dialogs)
                wxCommandEvent new_event(wxEVT_COMMAND_BUTTON_CLICKED,wxID_CANCEL);
                new_event.SetEventObject( focus );
                handled = focus->GetEventHandler()->ProcessEvent( new_event );
            }
        }
    }
    return handled ;
}

// This method handles common code for SendKeyDown, SendKeyUp, and SendChar events.
void wxApp::MacCreateKeyEvent( wxKeyEvent& event, wxWindow* focus , long keymessage , long modifiers , long when , short wherex , short wherey , wxChar uniChar )
{
    short keycode, keychar ;

    keychar = short(keymessage & charCodeMask);
    keycode = short(keymessage & keyCodeMask) >> 8 ;
    if ( !(event.GetEventType() == wxEVT_CHAR) && (modifiers & (controlKey | shiftKey | optionKey) ) )
    {
        // control interferes with some built-in keys like pgdown, return etc. therefore we remove the controlKey modifier
        // and look at the character after
        UInt32 state = 0;
        UInt32 keyInfo = KeyTranslate((Ptr)GetScriptManagerVariable(smKCHRCache), ( modifiers & (~(controlKey | shiftKey | optionKey))) | keycode, &state);
        keychar = short(keyInfo & charCodeMask);
    }

    long keyval = wxMacTranslateKey(keychar, keycode) ;
    if ( keyval == keychar && ( event.GetEventType() == wxEVT_KEY_UP || event.GetEventType() == wxEVT_KEY_DOWN ) )
        keyval = wxToupper( keyval ) ;

    // Check for NUMPAD keys
    if (keyval >= '0' && keyval <= '9' && keycode >= 82 && keycode <= 92)
    {
        keyval = (keyval - '0') + WXK_NUMPAD0;
    }
    else if (keycode >= 67 && keycode <= 81)
    {
        switch (keycode)
        {
        case 76 :
            keyval = WXK_NUMPAD_ENTER;
            break;

        case 81:
            keyval = WXK_NUMPAD_EQUAL;
            break;

        case 67:
            keyval = WXK_NUMPAD_MULTIPLY;
            break;

        case 75:
            keyval = WXK_NUMPAD_DIVIDE;
            break;

        case 78:
            keyval = WXK_NUMPAD_SUBTRACT;
            break;

        case 69:
            keyval = WXK_NUMPAD_ADD;
            break;

        case 65:
            keyval = WXK_NUMPAD_DECIMAL;
            break;

        default:
            break;
        } // end switch
    }

    event.m_shiftDown = modifiers & shiftKey;
    event.m_controlDown = modifiers & controlKey;
    event.m_altDown = modifiers & optionKey;
    event.m_metaDown = modifiers & cmdKey;
    event.m_keyCode = keyval ;
#if wxUSE_UNICODE
    event.m_uniChar = uniChar ;
#endif

    event.m_rawCode = keymessage;
    event.m_rawFlags = modifiers;
    event.m_x = wherex;
    event.m_y = wherey;
    event.SetTimestamp(when);
    event.SetEventObject(focus);
}
