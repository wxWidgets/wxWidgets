/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/nonownedwnd.cpp
// Purpose:     implementation of wxNonOwnedWindow
// Author:      Stefan Csomor
// Created:     2008-03-24
// RCS-ID:      $Id: nonownedwnd.cpp 50329 2007-11-29 17:00:58Z VS $
// Copyright:   (c) Stefan Csomor 2008
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/hashmap.h"
#include "wx/evtloop.h"
#include "wx/tooltip.h"
#include "wx/nonownedwnd.h"

#include "wx/mac/private.h"
#include "wx/settings.h"
#include "wx/frame.h"

#if wxUSE_SYSTEM_OPTIONS
    #include "wx/sysopt.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// unified title and toolbar constant - not in Tiger headers, so we duplicate it here
#define kWindowUnifiedTitleAndToolbarAttribute (1 << 7)

// trace mask for activation tracing messages
#define TRACE_ACTIVATE "activation"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

static pascal long wxShapedMacWindowDef(short varCode, WindowRef window, SInt16 message, SInt32 param);

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

// ---------------------------------------------------------------------------
// Carbon Events
// ---------------------------------------------------------------------------

static const EventTypeSpec eventList[] =
{
    // TODO: remove control related event like key and mouse (except for WindowLeave events)

    { kEventClassKeyboard, kEventRawKeyDown } ,
    { kEventClassKeyboard, kEventRawKeyRepeat } ,
    { kEventClassKeyboard, kEventRawKeyUp } ,
    { kEventClassKeyboard, kEventRawKeyModifiersChanged } ,

    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,
    { kEventClassTextInput, kEventTextInputUpdateActiveInputArea } ,

    { kEventClassWindow , kEventWindowShown } ,
    { kEventClassWindow , kEventWindowActivated } ,
    { kEventClassWindow , kEventWindowDeactivated } ,
    { kEventClassWindow , kEventWindowBoundsChanging } ,
    { kEventClassWindow , kEventWindowBoundsChanged } ,
    { kEventClassWindow , kEventWindowClose } ,
    { kEventClassWindow , kEventWindowGetRegion } ,

    // we have to catch these events on the toplevel window level,
    // as controls don't get the raw mouse events anymore

    { kEventClassMouse , kEventMouseDown } ,
    { kEventClassMouse , kEventMouseUp } ,
    { kEventClassMouse , kEventMouseWheelMoved } ,
    { kEventClassMouse , kEventMouseMoved } ,
    { kEventClassMouse , kEventMouseDragged } ,
} ;

static pascal OSStatus KeyboardEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    // call DoFindFocus instead of FindFocus, because for Composite Windows(like WxGenericListCtrl)
    // FindFocus does not return the actual focus window, but the enclosing window
    wxWindow* focus = wxWindow::DoFindFocus();
    if ( focus == NULL )
        focus = (wxNonOwnedWindow*) data ;

    unsigned char charCode ;
    wxChar uniChar[2] ;
    uniChar[0] = 0;
    uniChar[1] = 0;

    UInt32 keyCode ;
    UInt32 modifiers ;
    Point point ;
    UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;

#if wxUSE_UNICODE
    ByteCount dataSize = 0 ;
    if ( GetEventParameter( event, kEventParamKeyUnicodes, typeUnicodeText, NULL, 0 , &dataSize, NULL ) == noErr )
    {
        UniChar buf[2] ;
        int numChars = dataSize / sizeof( UniChar) + 1;

        UniChar* charBuf = buf ;

        if ( numChars * 2 > 4 )
            charBuf = new UniChar[ numChars ] ;
        GetEventParameter( event, kEventParamKeyUnicodes, typeUnicodeText, NULL, dataSize , NULL , charBuf ) ;
        charBuf[ numChars - 1 ] = 0;

#if SIZEOF_WCHAR_T == 2
        uniChar = charBuf[0] ;
#else
        wxMBConvUTF16 converter ;
        converter.MB2WC( uniChar , (const char*)charBuf , 2 ) ;
#endif

        if ( numChars * 2 > 4 )
            delete[] charBuf ;
    }
#endif

    GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &charCode );
    GetEventParameter( event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &keyCode );
    GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers );
    GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &point );

    UInt32 message = (keyCode << 8) + charCode;
    switch ( GetEventKind( event ) )
    {
        case kEventRawKeyRepeat :
        case kEventRawKeyDown :
            {
                WXEVENTREF formerEvent = wxTheApp->MacGetCurrentEvent() ;
                WXEVENTHANDLERCALLREF formerHandler = wxTheApp->MacGetCurrentEventHandlerCallRef() ;
                wxTheApp->MacSetCurrentEvent( event , handler ) ;
                if ( /* focus && */ wxTheApp->MacSendKeyDownEvent(
                    focus , message , modifiers , when , point.h , point.v , uniChar[0] ) )
                {
                    result = noErr ;
                }
                wxTheApp->MacSetCurrentEvent( formerEvent , formerHandler ) ;
            }
            break ;

        case kEventRawKeyUp :
            if ( /* focus && */ wxTheApp->MacSendKeyUpEvent(
                focus , message , modifiers , when , point.h , point.v , uniChar[0] ) )
            {
                result = noErr ;
            }
            break ;

        case kEventRawKeyModifiersChanged :
            {
                wxKeyEvent event(wxEVT_KEY_DOWN);

                event.m_shiftDown = modifiers & shiftKey;
                event.m_controlDown = modifiers & controlKey;
                event.m_altDown = modifiers & optionKey;
                event.m_metaDown = modifiers & cmdKey;
                event.m_x = point.h;
                event.m_y = point.v;

#if wxUSE_UNICODE
                event.m_uniChar = uniChar[0] ;
#endif

                event.SetTimestamp(when);
                event.SetEventObject(focus);

                if ( /* focus && */ (modifiers ^ wxApp::s_lastModifiers ) & controlKey )
                {
                    event.m_keyCode = WXK_CONTROL ;
                    event.SetEventType( ( modifiers & controlKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( /* focus && */ (modifiers ^ wxApp::s_lastModifiers ) & shiftKey )
                {
                    event.m_keyCode = WXK_SHIFT ;
                    event.SetEventType( ( modifiers & shiftKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( /* focus && */ (modifiers ^ wxApp::s_lastModifiers ) & optionKey )
                {
                    event.m_keyCode = WXK_ALT ;
                    event.SetEventType( ( modifiers & optionKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( /* focus && */ (modifiers ^ wxApp::s_lastModifiers ) & cmdKey )
                {
                    event.m_keyCode = WXK_COMMAND ;
                    event.SetEventType( ( modifiers & cmdKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }

                wxApp::s_lastModifiers = modifiers ;
            }
            break ;

        default:
            break;
    }

    return result ;
}

// we don't interfere with foreign controls on our toplevel windows, therefore we always give back eventNotHandledErr
// for windows that we didn't create (like eg Scrollbars in a databrowser), or for controls where we did not handle the
// mouse down at all
//
// This handler can also be called from app level where data (ie target window) may be null or a non wx window

wxWindow* g_MacLastWindow = NULL ;

EventMouseButton g_lastButton = 0 ;
bool g_lastButtonWasFakeRight = false ;

void SetupMouseEvent( wxMouseEvent &wxevent , wxMacCarbonEvent &cEvent )
{
    UInt32 modifiers = cEvent.GetParameter<UInt32>(kEventParamKeyModifiers, typeUInt32) ;
    Point screenMouseLocation = cEvent.GetParameter<Point>(kEventParamMouseLocation) ;

    // these parameters are not given for all events
    EventMouseButton button = 0 ;
    UInt32 clickCount = 0 ;
    UInt32 mouseChord = 0;

    cEvent.GetParameter<EventMouseButton>( kEventParamMouseButton, typeMouseButton , &button ) ;
    cEvent.GetParameter<UInt32>( kEventParamClickCount, typeUInt32 , &clickCount ) ;
    // the chord is the state of the buttons pressed currently
    cEvent.GetParameter<UInt32>( kEventParamMouseChord, typeUInt32 , &mouseChord ) ;

    wxevent.m_x = screenMouseLocation.h;
    wxevent.m_y = screenMouseLocation.v;
    wxevent.m_shiftDown = modifiers & shiftKey;
    wxevent.m_controlDown = modifiers & controlKey;
    wxevent.m_altDown = modifiers & optionKey;
    wxevent.m_metaDown = modifiers & cmdKey;
    wxevent.SetTimestamp( cEvent.GetTicks() ) ;

    // a control click is interpreted as a right click
    bool thisButtonIsFakeRight = false ;
    if ( button == kEventMouseButtonPrimary && (modifiers & controlKey) )
    {
        button = kEventMouseButtonSecondary ;
        thisButtonIsFakeRight = true ;
    }

    // otherwise we report double clicks by connecting a left click with a ctrl-left click
    if ( clickCount > 1 && button != g_lastButton )
        clickCount = 1 ;

    // we must make sure that our synthetic 'right' button corresponds in
    // mouse down, moved and mouse up, and does not deliver a right down and left up

    if ( cEvent.GetKind() == kEventMouseDown )
    {
        g_lastButton = button ;
        g_lastButtonWasFakeRight = thisButtonIsFakeRight ;
    }

    if ( button == 0 )
    {
        g_lastButton = 0 ;
        g_lastButtonWasFakeRight = false ;
    }
    else if ( g_lastButton == kEventMouseButtonSecondary && g_lastButtonWasFakeRight )
        button = g_lastButton ;

    // Adjust the chord mask to remove the primary button and add the
    // secondary button.  It is possible that the secondary button is
    // already pressed, e.g. on a mouse connected to a laptop, but this
    // possibility is ignored here:
    if( thisButtonIsFakeRight && ( mouseChord & 1U ) )
        mouseChord = ((mouseChord & ~1U) | 2U);

    if(mouseChord & 1U)
                wxevent.m_leftDown = true ;
    if(mouseChord & 2U)
                wxevent.m_rightDown = true ;
    if(mouseChord & 4U)
                wxevent.m_middleDown = true ;

    // translate into wx types
    switch ( cEvent.GetKind() )
    {
        case kEventMouseDown :
            switch ( button )
            {
                case kEventMouseButtonPrimary :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_LEFT_DCLICK : wxEVT_LEFT_DOWN )  ;
                    break ;

                case kEventMouseButtonSecondary :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_RIGHT_DCLICK : wxEVT_RIGHT_DOWN ) ;
                    break ;

                case kEventMouseButtonTertiary :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_MIDDLE_DCLICK : wxEVT_MIDDLE_DOWN ) ;
                    break ;

                default:
                    break ;
            }
            break ;

        case kEventMouseUp :
            switch ( button )
            {
                case kEventMouseButtonPrimary :
                    wxevent.SetEventType( wxEVT_LEFT_UP )  ;
                    break ;

                case kEventMouseButtonSecondary :
                    wxevent.SetEventType( wxEVT_RIGHT_UP ) ;
                    break ;

                case kEventMouseButtonTertiary :
                    wxevent.SetEventType( wxEVT_MIDDLE_UP ) ;
                    break ;

                default:
                    break ;
            }
            break ;

     case kEventMouseWheelMoved :
        {
            wxevent.SetEventType( wxEVT_MOUSEWHEEL ) ;

            EventMouseWheelAxis axis = cEvent.GetParameter<EventMouseWheelAxis>(kEventParamMouseWheelAxis, typeMouseWheelAxis) ;
            SInt32 delta = cEvent.GetParameter<SInt32>(kEventParamMouseWheelDelta, typeSInt32) ;

            wxevent.m_wheelRotation = delta;
            wxevent.m_wheelDelta = 1;
            wxevent.m_linesPerAction = 1;
        }
        break ;

        case kEventMouseEntered :
        case kEventMouseExited :
        case kEventMouseDragged :
        case kEventMouseMoved :
            wxevent.SetEventType( wxEVT_MOTION ) ;
            break;
        default :
            break ;
    }
}

#define NEW_CAPTURE_HANDLING 1

pascal OSStatus
wxMacTopLevelMouseEventHandler(EventHandlerCallRef WXUNUSED(handler),
                               EventRef event,
                               void *data)
{
    wxNonOwnedWindow* toplevelWindow = (wxNonOwnedWindow*) data ;

    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    Point screenMouseLocation = cEvent.GetParameter<Point>(kEventParamMouseLocation) ;
    Point windowMouseLocation = screenMouseLocation ;

    WindowRef window = NULL;
    short windowPart = ::FindWindow(screenMouseLocation, &window);

    wxWindow* currentMouseWindow = NULL ;
    ControlRef control = NULL ;

#if NEW_CAPTURE_HANDLING
    if ( wxApp::s_captureWindow )
    {
        window = (WindowRef) wxApp::s_captureWindow->MacGetTopLevelWindowRef() ;
        windowPart = inContent ;
    }
#endif

    if ( window )
    {
        wxMacGlobalToLocal( window,  &windowMouseLocation ) ;

        if ( wxApp::s_captureWindow
#if !NEW_CAPTURE_HANDLING
             && wxApp::s_captureWindow->MacGetTopLevelWindowRef() == (WXWindow) window && windowPart == inContent
#endif
             )
        {
            currentMouseWindow = wxApp::s_captureWindow ;
        }
        else if ( (IsWindowActive(window) && windowPart == inContent) )
        {
            ControlPartCode part ;
            control = FindControlUnderMouse( windowMouseLocation , window , &part ) ;
            // if there is no control below the mouse position, send the event to the toplevel window itself
            if ( control == 0 )
            {
                currentMouseWindow = (wxWindow*) data ;
            }
            else
            {
                currentMouseWindow = (wxWindow*) wxFindControlFromMacControl( control ) ;
#ifndef __WXUNIVERSAL__
                if ( currentMouseWindow == NULL && cEvent.GetKind() == kEventMouseMoved )
                {
#if wxUSE_TOOLBAR
                    // for wxToolBar to function we have to send certaint events to it
                    // instead of its children (wxToolBarTools)
                    ControlRef parent ;
                    GetSuperControl(control, &parent );
                    wxWindow *wxParent = (wxWindow*) wxFindControlFromMacControl( parent ) ;
                    if ( wxParent && wxParent->IsKindOf( CLASSINFO( wxToolBar ) ) )
                        currentMouseWindow = wxParent ;
#endif
                }
#endif
            }

            // disabled windows must not get any input messages
            if ( currentMouseWindow && !currentMouseWindow->MacIsReallyEnabled() )
                currentMouseWindow = NULL;
        }
    }

    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    SetupMouseEvent( wxevent , cEvent ) ;

    // handle all enter / leave events

    if ( currentMouseWindow != g_MacLastWindow )
    {
        if ( g_MacLastWindow )
        {
            wxMouseEvent eventleave(wxevent);
            eventleave.SetEventType( wxEVT_LEAVE_WINDOW );
            g_MacLastWindow->ScreenToClient( &eventleave.m_x, &eventleave.m_y );
            eventleave.SetEventObject( g_MacLastWindow ) ;
            wxevent.SetId( g_MacLastWindow->GetId() ) ;

#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( g_MacLastWindow , eventleave);
#endif

            g_MacLastWindow->GetEventHandler()->ProcessEvent( eventleave );
        }

        if ( currentMouseWindow )
        {
            wxMouseEvent evententer(wxevent);
            evententer.SetEventType( wxEVT_ENTER_WINDOW );
            currentMouseWindow->ScreenToClient( &evententer.m_x, &evententer.m_y );
            evententer.SetEventObject( currentMouseWindow ) ;
            wxevent.SetId( currentMouseWindow->GetId() ) ;

#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( currentMouseWindow , evententer );
#endif

            currentMouseWindow->GetEventHandler()->ProcessEvent( evententer );
        }

        g_MacLastWindow = currentMouseWindow ;
    }

    if ( windowPart == inMenuBar )
    {
        // special case menu bar, as we are having a low-level runloop we must do it ourselves
        if ( cEvent.GetKind() == kEventMouseDown )
        {
            ::MenuSelect( screenMouseLocation ) ;
            ::HiliteMenu(0);
            result = noErr ;
        }
    }
    else if ( currentMouseWindow )
    {
        wxWindow *currentMouseWindowParent = currentMouseWindow->GetParent();

        currentMouseWindow->ScreenToClient( &wxevent.m_x , &wxevent.m_y ) ;

        wxevent.SetEventObject( currentMouseWindow ) ;
        wxevent.SetId( currentMouseWindow->GetId() ) ;

        // make tooltips current

#if wxUSE_TOOLTIPS
        if ( wxevent.GetEventType() == wxEVT_MOTION )
            wxToolTip::RelayEvent( currentMouseWindow , wxevent );
#endif

        if ( currentMouseWindow->GetEventHandler()->ProcessEvent( wxevent ) )
        {
/* 
            // this code is dangerous in case the delete in the mouse down occured further up in the chain, trying alternative 
             
            if ((currentMouseWindowParent != NULL) && 
                (currentMouseWindowParent->GetChildren().Find(currentMouseWindow) == NULL)) 
            */ 
            // deleted in the meantime 
            if ( g_MacLastWindow == NULL ) 
                currentMouseWindow = NULL;

            result = noErr;
        }
        else
        {
            // if the user code did _not_ handle the event, then perform the
            // default processing
            if ( wxevent.GetEventType() == wxEVT_LEFT_DOWN )
            {
                // ... that is set focus to this window
                if (currentMouseWindow->AcceptsFocus() && wxWindow::FindFocus()!=currentMouseWindow)
                    currentMouseWindow->SetFocus();
            }
        }

        if ( cEvent.GetKind() == kEventMouseUp && wxApp::s_captureWindow )
        {
            wxApp::s_captureWindow = NULL ;
            // update cursor ?
         }

        // update cursor

        wxWindow* cursorTarget = currentMouseWindow ;
        wxPoint cursorPoint( wxevent.m_x , wxevent.m_y ) ;

        extern wxCursor gGlobalCursor;

        if (!gGlobalCursor.IsOk())
        {
            while ( cursorTarget && !cursorTarget->MacSetupCursor( cursorPoint ) )
            {
                cursorTarget = cursorTarget->GetParent() ;
                if ( cursorTarget )
                    cursorPoint += cursorTarget->GetPosition();
            }
        }

    }
    else // currentMouseWindow == NULL
    {
        // don't mess with controls we don't know about
        // for some reason returning eventNotHandledErr does not lead to the correct behaviour
        // so we try sending them the correct control directly
        if ( cEvent.GetKind() == kEventMouseDown && toplevelWindow && control )
        {
            EventModifiers modifiers = cEvent.GetParameter<EventModifiers>(kEventParamKeyModifiers, typeUInt32) ;
            Point clickLocation = windowMouseLocation ;

            HIPoint hiPoint ;
            hiPoint.x = clickLocation.h ;
            hiPoint.y = clickLocation.v ;
            HIViewConvertPoint( &hiPoint , (ControlRef) toplevelWindow->GetHandle() , control  ) ;
            clickLocation.h = (int)hiPoint.x ;
            clickLocation.v = (int)hiPoint.y ;

            HandleControlClick( control , clickLocation , modifiers , (ControlActionUPP ) -1 ) ;
            result = noErr ;
        }
    }

    return result ;
}

static pascal OSStatus
wxNonOwnedWindowEventHandler(EventHandlerCallRef WXUNUSED(handler),
                                EventRef event,
                                void *data)
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    // WindowRef windowRef = cEvent.GetParameter<WindowRef>(kEventParamDirectObject) ;
    wxNonOwnedWindow* toplevelWindow = (wxNonOwnedWindow*) data ;

    switch ( GetEventKind( event ) )
    {
        case kEventWindowActivated :
        {
            toplevelWindow->MacActivate( cEvent.GetTicks() , true) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, true , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent( wxevent );
            // we still sending an eventNotHandledErr in order to allow for default processing
        }
            break ;

        case kEventWindowDeactivated :
        {
            toplevelWindow->MacActivate(cEvent.GetTicks() , false) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, false , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent( wxevent );
            // we still sending an eventNotHandledErr in order to allow for default processing
        }
            break ;

        case kEventWindowShown :
            toplevelWindow->Refresh() ;
            result = noErr ;
            break ;

        case kEventWindowClose :
            toplevelWindow->Close() ;
            result = noErr ;
            break ;

        case kEventWindowBoundsChanged :
        {
            UInt32 attributes = cEvent.GetParameter<UInt32>(kEventParamAttributes, typeUInt32) ;
            Rect newRect = cEvent.GetParameter<Rect>(kEventParamCurrentBounds) ;
            wxRect r( newRect.left , newRect.top , newRect.right - newRect.left , newRect.bottom - newRect.top ) ;
            if ( attributes & kWindowBoundsChangeSizeChanged )
            {
#ifndef __WXUNIVERSAL__
                // according to the other ports we handle this within the OS level
                // resize event, not within a wxSizeEvent
                wxFrame *frame = wxDynamicCast( toplevelWindow , wxFrame ) ;
                if ( frame )
                {
                    frame->PositionBars();
                }
#endif
                wxSizeEvent event( r.GetSize() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;

                toplevelWindow->GetEventHandler()->ProcessEvent( event ) ;
                toplevelWindow->wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
            }

            if ( attributes & kWindowBoundsChangeOriginChanged )
            {
                wxMoveEvent event( r.GetLeftTop() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;
                toplevelWindow->GetEventHandler()->ProcessEvent( event ) ;
            }

            result = noErr ;
        }
            break ;

        case kEventWindowBoundsChanging :
        {
            UInt32 attributes = cEvent.GetParameter<UInt32>(kEventParamAttributes,typeUInt32) ;
            Rect newRect = cEvent.GetParameter<Rect>(kEventParamCurrentBounds) ;

            if ( (attributes & kWindowBoundsChangeSizeChanged) || (attributes & kWindowBoundsChangeOriginChanged) )
            {
                // all (Mac) rects are in content area coordinates, all wxRects in structure coordinates
                int left , top , right , bottom ;
                toplevelWindow->MacGetContentAreaInset( left , top , right , bottom ) ;

                wxRect r(
                    newRect.left - left,
                    newRect.top - top,
                    newRect.right - newRect.left + left + right,
                    newRect.bottom - newRect.top + top + bottom ) ;

                // this is a EVT_SIZING not a EVT_SIZE type !
                wxSizeEvent wxevent( r , toplevelWindow->GetId() ) ;
                wxevent.SetEventObject( toplevelWindow ) ;
                wxRect adjustR = r ;
                if ( toplevelWindow->GetEventHandler()->ProcessEvent( wxevent ) )
                    adjustR = wxevent.GetRect() ;

                if ( toplevelWindow->GetMaxWidth() != -1 && adjustR.GetWidth() > toplevelWindow->GetMaxWidth() )
                    adjustR.SetWidth( toplevelWindow->GetMaxWidth() ) ;
                if ( toplevelWindow->GetMaxHeight() != -1 && adjustR.GetHeight() > toplevelWindow->GetMaxHeight() )
                    adjustR.SetHeight( toplevelWindow->GetMaxHeight() ) ;
                if ( toplevelWindow->GetMinWidth() != -1 && adjustR.GetWidth() < toplevelWindow->GetMinWidth() )
                    adjustR.SetWidth( toplevelWindow->GetMinWidth() ) ;
                if ( toplevelWindow->GetMinHeight() != -1 && adjustR.GetHeight() < toplevelWindow->GetMinHeight() )
                    adjustR.SetHeight( toplevelWindow->GetMinHeight() ) ;
                const Rect adjustedRect = { adjustR.y + top  , adjustR.x + left , adjustR.y + adjustR.height - bottom , adjustR.x + adjustR.width - right } ;
                if ( !EqualRect( &newRect , &adjustedRect ) )
                    cEvent.SetParameter<Rect>( kEventParamCurrentBounds , &adjustedRect ) ;
                toplevelWindow->wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
            }

            result = noErr ;
        }
            break ;

        case kEventWindowGetRegion :
            {
            }
            break ;

        default :
            break ;
    }

    return result ;
}

// mix this in from window.cpp
pascal OSStatus wxMacUnicodeTextEventHandler( EventHandlerCallRef handler , EventRef event , void *data ) ;

pascal OSStatus wxNonOwnedEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassTextInput :
            result = wxMacUnicodeTextEventHandler( handler, event , data ) ;
            break ;

        case kEventClassKeyboard :
            result = KeyboardEventHandler( handler, event , data ) ;
            break ;

        case kEventClassWindow :
            result = wxNonOwnedWindowEventHandler( handler, event , data ) ;
            break ;

        case kEventClassMouse :
            result = wxMacTopLevelMouseEventHandler( handler, event , data ) ;
            break ;

        default :
            break ;
    }

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxNonOwnedEventHandler )

// ---------------------------------------------------------------------------
// wxWindowMac utility functions
// ---------------------------------------------------------------------------

// Find an item given the Macintosh Window Reference

WX_DECLARE_HASH_MAP(WindowRef, wxNonOwnedWindow*, wxPointerHash, wxPointerEqual, MacWindowMap);

static MacWindowMap wxWinMacWindowList;

wxNonOwnedWindow *wxFindWinFromMacWindow(WindowRef inWindowRef)
{
    MacWindowMap::iterator node = wxWinMacWindowList.find(inWindowRef);

    return (node == wxWinMacWindowList.end()) ? NULL : node->second;
}

void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxNonOwnedWindow *win) ;
void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxNonOwnedWindow *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // nothing else :-)
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    wxWinMacWindowList[inWindowRef] = win;
}

void wxRemoveMacWindowAssociation(wxNonOwnedWindow *win) ;
void wxRemoveMacWindowAssociation(wxNonOwnedWindow *win)
{
    MacWindowMap::iterator it;
    for ( it = wxWinMacWindowList.begin(); it != wxWinMacWindowList.end(); ++it )
    {
        if ( it->second == win )
        {
            wxWinMacWindowList.erase(it);
            break;
        }
    }
}

// ----------------------------------------------------------------------------
// wxNonOwnedWindow creation
// ----------------------------------------------------------------------------

wxNonOwnedWindow *wxNonOwnedWindow::s_macDeactivateWindow = NULL;

void wxNonOwnedWindow::Init()
{
    m_macWindow = NULL ;
    m_macEventHandler = NULL ;
}

wxMacDeferredWindowDeleter::wxMacDeferredWindowDeleter( WindowRef windowRef )
{
    m_macWindow = windowRef ;
}

wxMacDeferredWindowDeleter::~wxMacDeferredWindowDeleter()
{
    DisposeWindow( (WindowRef) m_macWindow ) ;
}

bool wxNonOwnedWindow::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;

    SetName( name );

    m_windowId = id == -1 ? NewControlId() : id;

    DoMacCreateRealWindow( parent, pos , size , style , name ) ;

    SetBackgroundColour(wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ));

    if (GetExtraStyle() & wxFRAME_EX_METAL)
        MacSetMetalAppearance(true);

    if ( parent )
        parent->AddChild(this);

    return true;
}

wxNonOwnedWindow::~wxNonOwnedWindow()
{
    if ( m_macWindow )
    {
#if wxUSE_TOOLTIPS
        wxToolTip::NotifyWindowDelete(m_macWindow) ;
#endif
        wxPendingDelete.Append( new wxMacDeferredWindowDeleter( (WindowRef) m_macWindow ) ) ;
    }

    if ( m_macEventHandler )
    {
        ::RemoveEventHandler((EventHandlerRef) m_macEventHandler);
        m_macEventHandler = NULL ;
    }

    wxRemoveMacWindowAssociation( this ) ;

    // avoid dangling refs
    if ( s_macDeactivateWindow == this )
        s_macDeactivateWindow = NULL;
}

// ----------------------------------------------------------------------------
// wxNonOwnedWindow misc
// ----------------------------------------------------------------------------

wxPoint wxNonOwnedWindow::GetClientAreaOrigin() const
{
    return wxPoint(0, 0) ;
}

bool wxNonOwnedWindow::SetBackgroundColour(const wxColour& c )
{
    wxColour col = c;
        
    if ( !wxWindow::SetBackgroundColour(col) && m_hasBgCol )
        return false ;
    
    if ( GetBackgroundStyle() != wxBG_STYLE_CUSTOM )
    {
        if ( col == wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) )
        {
            SetThemeWindowBackground( (WindowRef) m_macWindow,  kThemeBrushDocumentWindowBackground, false ) ;
            SetBackgroundStyle(wxBG_STYLE_SYSTEM);
        }
        else if ( col == wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) )
        {
            SetThemeWindowBackground( (WindowRef) m_macWindow,  kThemeBrushDialogBackgroundActive, false ) ;
            SetBackgroundStyle(wxBG_STYLE_SYSTEM);
        }
    }
    return true;
}    

void wxNonOwnedWindowInstallTopLevelWindowEventHandler(WindowRef window, EventHandlerRef* handler, void *ref)
{
    InstallWindowEventHandler(window, GetwxNonOwnedEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, ref, handler );
}

void wxNonOwnedWindow::MacInstallTopLevelWindowEventHandler()
{
    if ( m_macEventHandler != NULL )
    {
        verify_noerr( ::RemoveEventHandler( (EventHandlerRef) m_macEventHandler ) ) ;
    }
    wxNonOwnedWindowInstallTopLevelWindowEventHandler(MAC_WXHWND(m_macWindow),(EventHandlerRef *)&m_macEventHandler,this);
}

void  wxNonOwnedWindow::MacCreateRealWindow(
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name )
{
    DoMacCreateRealWindow( NULL, pos, size, style, name );
}

void  wxNonOwnedWindow::DoMacCreateRealWindow(
    wxWindow* parent,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name )
{
    OSStatus err = noErr ;
    SetName(name);
    m_windowStyle = style;
    m_isShown = false;

    // create frame.
    int x = (int)pos.x;
    int y = (int)pos.y;

    Rect theBoundsRect;
    wxRect display = wxGetClientDisplayRect() ;

    if ( x == wxDefaultPosition.x )
        x = display.x ;

    if ( y == wxDefaultPosition.y )
        y = display.y ;

    int w = WidthDefault(size.x);
    int h = HeightDefault(size.y);

    ::SetRect(&theBoundsRect, x, y , x + w, y + h);

    // translate the window attributes in the appropriate window class and attributes
    WindowClass wclass = 0;
    WindowAttributes attr = kWindowNoAttributes ;
    WindowGroupRef group = NULL ;
    bool activationScopeSet = false;
    WindowActivationScope activationScope = kWindowActivationScopeNone;

    if ( HasFlag( wxFRAME_TOOL_WINDOW) )
    {
        if (
            HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) ||
            HasFlag( wxSYSTEM_MENU ) || HasFlag( wxCAPTION ) ||
            HasFlag(wxTINY_CAPTION_HORIZ) ||  HasFlag(wxTINY_CAPTION_VERT)
            )
        {
            if ( HasFlag( wxSTAY_ON_TOP ) )
                wclass = kUtilityWindowClass;
            else
                wclass = kFloatingWindowClass ;

            if ( HasFlag(wxTINY_CAPTION_VERT) )
                attr |= kWindowSideTitlebarAttribute ;
        }
        else
        {
            if ( HasFlag( wxNO_BORDER ) )
            {
                wclass = kSimpleWindowClass ;
            }
            else
            {
                wclass = kPlainWindowClass ;
            }
            activationScopeSet = true;
            activationScope = kWindowActivationScopeNone;
        }
    }
    else if ( HasFlag( wxPOPUP_WINDOW ) )
    {
        if ( HasFlag( wxBORDER_NONE ) )
        {
            wclass = kHelpWindowClass ;   // has no border
            attr |= kWindowNoShadowAttribute;
        }
        else
        {
            wclass = kPlainWindowClass ;  // has a single line border, it will have to do for now
        }
        group = GetWindowGroupOfClass(kFloatingWindowClass) ;
        // make sure we don't deactivate something
        activationScopeSet = true;
        activationScope = kWindowActivationScopeNone;
    }
    else if ( HasFlag( wxCAPTION ) )
    {
        wclass = kDocumentWindowClass ;
        attr |= kWindowInWindowMenuAttribute ;
    }
    else if ( HasFlag( wxFRAME_DRAWER ) )
    {
        wclass = kDrawerWindowClass;
    }
    else
    {
        if ( HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) ||
            HasFlag( wxCLOSE_BOX ) || HasFlag( wxSYSTEM_MENU ) )
        {
            wclass = kDocumentWindowClass ;
        }
        else if ( HasFlag( wxNO_BORDER ) )
        {
            wclass = kSimpleWindowClass ;
        }
        else
        {
            wclass = kPlainWindowClass ;
        }
    }

    if ( wclass != kPlainWindowClass )
    {
        if ( HasFlag( wxMINIMIZE_BOX ) )
            attr |= kWindowCollapseBoxAttribute ;

        if ( HasFlag( wxMAXIMIZE_BOX ) )
            attr |= kWindowFullZoomAttribute ;

        if ( HasFlag( wxRESIZE_BORDER ) )
            attr |= kWindowResizableAttribute ;

        if ( HasFlag( wxCLOSE_BOX) )
            attr |= kWindowCloseBoxAttribute ;
    }
    attr |= kWindowLiveResizeAttribute;

    if ( HasFlag(wxSTAY_ON_TOP) )
        group = GetWindowGroupOfClass(kUtilityWindowClass) ;

    if ( HasFlag( wxFRAME_FLOAT_ON_PARENT ) )
        group = GetWindowGroupOfClass(kFloatingWindowClass) ;

    if ( group == NULL && parent != NULL )
    {
        WindowRef parenttlw = (WindowRef) parent->MacGetTopLevelWindowRef();
        if( parenttlw )
            group = GetWindowGroupParent( GetWindowGroup( parenttlw ) );
    }

    attr |= kWindowCompositingAttribute;
#if 0 // wxMAC_USE_CORE_GRAPHICS ; TODO : decide on overall handling of high dpi screens (pixel vs userscale)
    attr |= kWindowFrameworkScaledAttribute;
#endif

    if ( HasFlag(wxFRAME_SHAPED) )
    {
        WindowDefSpec customWindowDefSpec;
        customWindowDefSpec.defType = kWindowDefProcPtr;
        customWindowDefSpec.u.defProc =
#ifdef __LP64__
            (WindowDefUPP) wxShapedMacWindowDef;
#else
            NewWindowDefUPP(wxShapedMacWindowDef);
#endif
        err = ::CreateCustomWindow( &customWindowDefSpec, wclass,
                              attr, &theBoundsRect,
                              (WindowRef*) &m_macWindow);
    }
    else
    {
        err = ::CreateNewWindow( wclass , attr , &theBoundsRect , (WindowRef*)&m_macWindow ) ;
    }

    if ( err == noErr && m_macWindow != NULL && group != NULL )
        SetWindowGroup( (WindowRef) m_macWindow , group ) ;

    wxCHECK_RET( err == noErr, wxT("Mac OS error when trying to create new window") );

    // setup a separate group for each window, so that overlays can be handled easily

    WindowGroupRef overlaygroup = NULL;
    verify_noerr( CreateWindowGroup( kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse, &overlaygroup ));
    verify_noerr( SetWindowGroupParent( overlaygroup, GetWindowGroup( (WindowRef) m_macWindow )));
    verify_noerr( SetWindowGroup( (WindowRef) m_macWindow , overlaygroup ));

    if ( activationScopeSet )
    {
        verify_noerr( SetWindowActivationScope( (WindowRef) m_macWindow , activationScope ));
    }

    // the create commands are only for content rect,
    // so we have to set the size again as structure bounds
    SetWindowBounds(  (WindowRef) m_macWindow , kWindowStructureRgn , &theBoundsRect ) ;

    wxAssociateWinWithMacWindow( (WindowRef) m_macWindow , this ) ;
    m_peer = new wxMacControl(this , true /*isRootControl*/) ;

    // There is a bug in 10.2.X for ::GetRootControl returning the window view instead of
    // the content view, so we have to retrieve it explicitly
    HIViewFindByID( HIViewGetRoot( (WindowRef) m_macWindow ) , kHIViewWindowContentID ,
        m_peer->GetControlRefAddr() ) ;
    if ( !m_peer->Ok() )
    {
        // compatibility mode fallback
        GetRootControl( (WindowRef) m_macWindow , m_peer->GetControlRefAddr() ) ;
    }

    // the root control level handler
    MacInstallEventHandler( (WXWidget) m_peer->GetControlRef() ) ;

    // Causes the inner part of the window not to be metal
    // if the style is used before window creation.
#if 0 // TARGET_API_MAC_OSX
    if ( m_macUsesCompositing && m_macWindow != NULL )
    {
        if ( GetExtraStyle() & wxFRAME_EX_METAL )
            MacSetMetalAppearance( true ) ;
    }
#endif

    if ( m_macWindow != NULL )
    {
        MacSetUnifiedAppearance( true ) ;
    }

    HIViewRef growBoxRef = 0 ;
    err = HIViewFindByID( HIViewGetRoot( (WindowRef)m_macWindow ), kHIViewWindowGrowBoxID, &growBoxRef  );
    if ( err == noErr && growBoxRef != 0 )
        HIGrowBoxViewSetTransparent( growBoxRef, true ) ;

    // the frame window event handler
    InstallStandardEventHandler( GetWindowEventTarget(MAC_WXHWND(m_macWindow)) ) ;
    MacInstallTopLevelWindowEventHandler() ;

    DoSetWindowVariant( m_windowVariant ) ;

    m_macFocus = NULL ;

    if ( HasFlag(wxFRAME_SHAPED) )
    {
        // default shape matches the window size
        wxRegion rgn( 0, 0, w, h );
        SetShape( rgn );
    }

    wxWindowCreateEvent event(this);
    GetEventHandler()->ProcessEvent( event );
}

// Raise the window to the top of the Z order
void wxNonOwnedWindow::Raise()
{
    ::SelectWindow( (WindowRef)m_macWindow ) ;
}

// Lower the window to the bottom of the Z order
void wxNonOwnedWindow::Lower()
{
    ::SendBehind( (WindowRef)m_macWindow , NULL ) ;
}

void wxNonOwnedWindow::MacDelayedDeactivation(long timestamp)
{
    if (s_macDeactivateWindow)
    {
    /*
        wxLogTrace(TRACE_ACTIVATE,
                   wxT("Doing delayed deactivation of %p"),
                   s_macDeactivateWindow);
    */
        s_macDeactivateWindow->MacActivate(timestamp, false);
    }
}

void wxNonOwnedWindow::MacActivate( long timestamp , bool WXUNUSED(inIsActivating) )
{
    /*
    wxLogTrace(TRACE_ACTIVATE, wxT("TopLevel=%p::MacActivate"), this);
    */
    
    if (s_macDeactivateWindow == this)
        s_macDeactivateWindow = NULL;

    MacDelayedDeactivation(timestamp);
}

void wxNonOwnedWindow::ShowNoActivate()
{
    bool plainTransition = true;

#if wxUSE_SYSTEM_OPTIONS
    if ( wxSystemOptions::HasOption(wxMAC_WINDOW_PLAIN_TRANSITION) )
        plainTransition = ( wxSystemOptions::GetOptionInt( wxMAC_WINDOW_PLAIN_TRANSITION ) == 1 ) ;
#endif

    if ( plainTransition )
       ::ShowWindow( (WindowRef)m_macWindow );
    else
       ::TransitionWindow( (WindowRef)m_macWindow, kWindowZoomTransitionEffect, kWindowShowTransitionAction, NULL );

    // because apps expect a size event to occur at this moment
    wxSizeEvent event(GetSize() , m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent( event );

}

bool wxNonOwnedWindow::Show(bool show)
{
    bool plainTransition = true;

#if wxUSE_SYSTEM_OPTIONS
    if ( wxSystemOptions::HasOption(wxMAC_WINDOW_PLAIN_TRANSITION) )
        plainTransition = ( wxSystemOptions::GetOptionInt( wxMAC_WINDOW_PLAIN_TRANSITION ) == 1 ) ;
#endif

    if ( !wxWindow::Show(show) )
        return false;

    if (show)
    {
        ShowNoActivate();
        ::SelectWindow( (WindowRef)m_macWindow ) ;
    }
    else
    {
        if ( plainTransition )
           ::HideWindow( (WindowRef)m_macWindow );
        else
           ::TransitionWindow( (WindowRef)m_macWindow, kWindowZoomTransitionEffect, kWindowHideTransitionAction, NULL );
    }

    return true ;
}

/*
bool wxNonOwnedWindow::ShowWithEffect(wxShowEffect effect,
                            unsigned timeout,
                            wxDirection dir)
{
    // TODO factor common code
    if ( !wxWindow::Show(true) )
        return false;
 
    WindowTransitionEffect transition = 0 ;
    switch( effect )
    {
        case wxSHOW_EFFECT_ROLL :
        case wxSHOW_EFFECT_SLIDE :
            transition = kWindowGenieTransitionEffect;
            break;
        case wxSHOW_EFFECT_BLEND :
            transition = kWindowFadeTransitionEffect;
            break;
        case wxSHOW_EFFECT_EXPAND :
        default :
            // having sheets would be fine, but this might lead to a repositioning
#if 0
            if ( GetParent() )
                transition = kWindowSheetTransitionEffect;
            else
#endif
                transition = kWindowZoomTransitionEffect;
            break;
    }
    
    TransitionWindowOptions options;
    options.version = 0;
    options.duration = timeout / 1000.0;
    options.window = transition == kWindowSheetTransitionEffect ? (WindowRef) GetParent()->MacGetTopLevelWindowRef() :0;
    options.userData = 0;
    
    wxSize size = wxGetDisplaySize();
    Rect bounds;
    GetWindowBounds( (WindowRef)m_macWindow, kWindowStructureRgn, &bounds );
    CGRect hiBounds = CGRectMake( bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top );

    if ( dir & wxRIGHT )
    {
        hiBounds.origin.x = size.x;
        hiBounds.size.width = 0;
    }
    if ( dir & wxUP )
    {
        hiBounds.origin.y = 0;
        hiBounds.size.height = 0;
    }
    if ( dir & wxDOWN )
    {
        hiBounds.origin.y = size.y;
        hiBounds.size.height = 0;
    }
    if ( dir & wxLEFT )
    {
        hiBounds.origin.x = 0;
        hiBounds.size.width = 0;
    }
    
    ::TransitionWindowWithOptions( (WindowRef)m_macWindow, transition, kWindowShowTransitionAction, transition == kWindowGenieTransitionEffect ? &hiBounds : NULL  ,  
                                  false, &options );

    ::SelectWindow( (WindowRef)m_macWindow ) ;
    
    // because apps expect a size event to occur at this moment
    wxSizeEvent event(GetSize() , m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent( event );
    
    return true;
}

bool wxNonOwnedWindow::HideWithEffect(wxShowEffect effect,
                            unsigned timeout ,
                            wxDirection dir )
{
    if ( !wxWindow::Show(false) )
        return false;
    
    WindowTransitionEffect transition = 0 ;
    switch( effect )
    {
        case wxSHOW_EFFECT_ROLL :
        case wxSHOW_EFFECT_SLIDE :
            transition = kWindowGenieTransitionEffect;
            break;
        case wxSHOW_EFFECT_BLEND :
            transition = kWindowFadeTransitionEffect;
            break;
        case wxSHOW_EFFECT_EXPAND :
        default:
#if 0
            if ( GetParent() )
                transition = kWindowSheetTransitionEffect;
            else
#endif
                transition = kWindowZoomTransitionEffect;
            break;
    }
    TransitionWindowOptions options;
    options.version = 0;
    options.duration = timeout / 1000.0;
    options.window = transition == kWindowSheetTransitionEffect ? (WindowRef) GetParent()->MacGetTopLevelWindowRef() :0;
    options.userData = 0;
    
    wxSize size = wxGetDisplaySize();
    Rect bounds;
    GetWindowBounds( (WindowRef)m_macWindow, kWindowStructureRgn, &bounds );
    CGRect hiBounds = CGRectMake( bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top );
    
    if ( dir & wxRIGHT )
    {
        hiBounds.origin.x = size.x;
        hiBounds.size.width = 0;
    }
    if ( dir & wxUP )
    {
        hiBounds.origin.y = 0;
        hiBounds.size.height = 0;
    }
    if ( dir & wxDOWN )
    {
        hiBounds.origin.y = size.y;
        hiBounds.size.height = 0;
    }
    if ( dir & wxLEFT )
    {
        hiBounds.origin.x = 0;
        hiBounds.size.width = 0;
    }
    ::TransitionWindowWithOptions( (WindowRef)m_macWindow, transition, kWindowHideTransitionAction, transition == kWindowGenieTransitionEffect ? &hiBounds : NULL  ,  
                                  false, &options );

    return true;
}
*/

bool wxNonOwnedWindow::SetTransparent(wxByte alpha)
{
    OSStatus result = SetWindowAlpha((WindowRef)m_macWindow, (CGFloat)((alpha)/255.0));
    return result == noErr;
}


bool wxNonOwnedWindow::CanSetTransparent()
{
    return true;
}


void wxNonOwnedWindow::SetExtraStyle(long exStyle)
{
    if ( GetExtraStyle() == exStyle )
        return ;

    wxWindow::SetExtraStyle( exStyle ) ;

    if ( m_macWindow != NULL )
    {
        bool metal = GetExtraStyle() & wxFRAME_EX_METAL ;

        if ( MacGetMetalAppearance() != metal )
        {
            if ( MacGetUnifiedAppearance() )
                MacSetUnifiedAppearance( !metal ) ;

            MacSetMetalAppearance( metal ) ;
        }
    }
}

bool wxNonOwnedWindow::SetBackgroundStyle(wxBackgroundStyle style)
{
    if ( !wxWindow::SetBackgroundStyle(style) )
        return false ;

    WindowRef windowRef = HIViewGetWindow( (HIViewRef)GetHandle() );

/*
    if ( GetBackgroundStyle() == wxBG_STYLE_TRANSPARENT )
    {
        OSStatus err = HIWindowChangeFeatures( windowRef, 0, kWindowIsOpaque );
        verify_noerr( err );
        err = ReshapeCustomWindow( windowRef );
        verify_noerr( err );
    }
*/
    return true ;
}

// TODO: switch to structure bounds -
// we are still using coordinates of the content view
//
void wxNonOwnedWindow::MacGetContentAreaInset( int &left , int &top , int &right , int &bottom )
{
    Rect content, structure ;

    GetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &structure ) ;
    GetWindowBounds( (WindowRef) m_macWindow, kWindowContentRgn , &content ) ;

    left = content.left - structure.left ;
    top = content.top  - structure.top ;
    right = structure.right - content.right ;
    bottom = structure.bottom - content.bottom ;
}

void wxNonOwnedWindow::DoMoveWindow(int x, int y, int width, int height)
{
    m_cachedClippedRectValid = false ;
    Rect bounds = { y , x , y + height , x + width } ;
    verify_noerr(SetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;
    wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
}

void wxNonOwnedWindow::DoGetPosition( int *x, int *y ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;

    if (x)
       *x = bounds.left ;
    if (y)
       *y = bounds.top ;
}

void wxNonOwnedWindow::DoGetSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

void wxNonOwnedWindow::DoGetClientSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowContentRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

void wxNonOwnedWindow::MacSetMetalAppearance( bool set )
{
    if ( MacGetUnifiedAppearance() )
        MacSetUnifiedAppearance( false ) ;

    MacChangeWindowAttributes( set ? kWindowMetalAttribute : kWindowNoAttributes ,
        set ? kWindowNoAttributes : kWindowMetalAttribute ) ;
}

bool wxNonOwnedWindow::MacGetMetalAppearance() const
{
    return MacGetWindowAttributes() & kWindowMetalAttribute ;
}

void wxNonOwnedWindow::MacSetUnifiedAppearance( bool set )
{
    if ( MacGetMetalAppearance() )
        MacSetMetalAppearance( false ) ;

    MacChangeWindowAttributes( set ? kWindowUnifiedTitleAndToolbarAttribute : kWindowNoAttributes ,
        set ? kWindowNoAttributes : kWindowUnifiedTitleAndToolbarAttribute) ;

    // For some reason, Tiger uses white as the background color for this appearance,
    // while most apps using it use the typical striped background. Restore that behavior
    // for wx.
    // TODO: Determine if we need this on Leopard as well. (should be harmless either way,
    // though)
    SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) ) ;
}

bool wxNonOwnedWindow::MacGetUnifiedAppearance() const
{
    return MacGetWindowAttributes() & kWindowUnifiedTitleAndToolbarAttribute ;
}

void wxNonOwnedWindow::MacChangeWindowAttributes( wxUint32 attributesToSet , wxUint32 attributesToClear )
{
    ChangeWindowAttributes( (WindowRef)m_macWindow, attributesToSet, attributesToClear ) ;
}

wxUint32 wxNonOwnedWindow::MacGetWindowAttributes() const
{
    UInt32 attr = 0 ;
    GetWindowAttributes( (WindowRef) m_macWindow, &attr ) ;

    return attr ;
}

void wxNonOwnedWindow::MacPerformUpdates()
{
    // for composited windows this also triggers a redraw of all
    // invalid views in the window
    HIWindowFlush((WindowRef) m_macWindow) ;
}

// ---------------------------------------------------------------------------
// Shape implementation
// ---------------------------------------------------------------------------


bool wxNonOwnedWindow::SetShape(const wxRegion& region)
{
    wxCHECK_MSG( HasFlag(wxFRAME_SHAPED), false,
                 _T("Shaped windows must be created with the wxFRAME_SHAPED style."));

    // The empty region signifies that the shape
    // should be removed from the window.
    if ( region.IsEmpty() )
    {
        wxSize sz = GetClientSize();
        wxRegion rgn(0, 0, sz.x, sz.y);
        if ( rgn.IsEmpty() )
            return false ;
        else
            return SetShape(rgn);
    }

    // Make a copy of the region
    RgnHandle  shapeRegion = NewRgn();
    CopyRgn( (RgnHandle)region.GetWXHRGN(), shapeRegion );

    // Dispose of any shape region we may already have
    RgnHandle oldRgn = (RgnHandle)GetWRefCon( (WindowRef)MacGetWindowRef() );
    if ( oldRgn )
        DisposeRgn(oldRgn);

    // Save the region so we can use it later
    SetWRefCon((WindowRef)MacGetWindowRef(), (URefCon)shapeRegion);

    // inform the window manager that the window has changed shape
    ReshapeCustomWindow((WindowRef)MacGetWindowRef());

    return true;
}

// ---------------------------------------------------------------------------
// Support functions for shaped windows, based on Apple's CustomWindow sample at
// http://developer.apple.com/samplecode/Sample_Code/Human_Interface_Toolbox/Mac_OS_High_Level_Toolbox/CustomWindow.htm
// ---------------------------------------------------------------------------

static void wxShapedMacWindowGetPos(WindowRef window, Rect* inRect)
{
    GetWindowPortBounds(window, inRect);
    Point pt = { inRect->top ,inRect->left };
    wxMacLocalToGlobal( window, &pt ) ;
    inRect->bottom += pt.v - inRect->top;
    inRect->right += pt.h - inRect->left;
    inRect->top = pt.v;
    inRect->left = pt.h;
}

static SInt32 wxShapedMacWindowGetFeatures(WindowRef WXUNUSED(window), SInt32 param)
{
    /*------------------------------------------------------
        Define which options your custom window supports.
    --------------------------------------------------------*/
    //just enable everything for our demo
    *(OptionBits*)param =
        //kWindowCanGrow |
        //kWindowCanZoom |
        kWindowCanCollapse |
        //kWindowCanGetWindowRegion |
        //kWindowHasTitleBar |
        //kWindowSupportsDragHilite |
        kWindowCanDrawInCurrentPort |
        //kWindowCanMeasureTitle |
        kWindowWantsDisposeAtProcessDeath |
        kWindowSupportsGetGrowImageRegion |
        kWindowDefSupportsColorGrafPort;

    return 1;
}

// The content region is left as a rectangle matching the window size, this is
// so the origin in the paint event, and etc. still matches what the
// programmer expects.
static void wxShapedMacWindowContentRegion(WindowRef window, RgnHandle rgn)
{
    SetEmptyRgn(rgn);
    wxNonOwnedWindow* win = wxFindWinFromMacWindow(window);
    if (win)
    {
        Rect r ;
        wxShapedMacWindowGetPos( window, &r ) ;
        RectRgn( rgn , &r ) ;
    }
}

// The structure region is set to the shape given to the SetShape method.
static void wxShapedMacWindowStructureRegion(WindowRef window, RgnHandle rgn)
{
    RgnHandle cachedRegion = (RgnHandle) GetWRefCon(window);

    SetEmptyRgn(rgn);
    if (cachedRegion)
    {
        Rect windowRect;
        wxShapedMacWindowGetPos(window, &windowRect);    // how big is the window
        CopyRgn(cachedRegion, rgn);                      // make a copy of our cached region
        OffsetRgn(rgn, windowRect.left, windowRect.top); // position it over window
        //MapRgn(rgn, &mMaskSize, &windowRect);          //scale it to our actual window size
    }
}

static SInt32 wxShapedMacWindowGetRegion(WindowRef window, SInt32 param)
{
    GetWindowRegionPtr rgnRec = (GetWindowRegionPtr)param;

    if (rgnRec == NULL)
        return paramErr;

    switch (rgnRec->regionCode)
    {
        case kWindowStructureRgn:
            wxShapedMacWindowStructureRegion(window, rgnRec->winRgn);
            break;

        case kWindowContentRgn:
            wxShapedMacWindowContentRegion(window, rgnRec->winRgn);
            break;

        default:
            SetEmptyRgn(rgnRec->winRgn);
            break;
    }

    return noErr;
}

// Determine the region of the window which was hit
//
static SInt32 wxShapedMacWindowHitTest(WindowRef window, SInt32 param)
{
    Point hitPoint;
    static RgnHandle tempRgn = NULL;

    if (tempRgn == NULL)
        tempRgn = NewRgn();

    // get the point clicked
    SetPt( &hitPoint, LoWord(param), HiWord(param) );

     // Mac OS 8.5 or later
    wxShapedMacWindowStructureRegion(window, tempRgn);
    if (PtInRgn( hitPoint, tempRgn )) //in window content region?
        return wInContent;

    // no significant area was hit
    return wNoHit;
}

static pascal long wxShapedMacWindowDef(short WXUNUSED(varCode), WindowRef window, SInt16 message, SInt32 param)
{
    switch (message)
    {
        case kWindowMsgHitTest:
            return wxShapedMacWindowHitTest(window, param);

        case kWindowMsgGetFeatures:
            return wxShapedMacWindowGetFeatures(window, param);

        // kWindowMsgGetRegion is sent during CreateCustomWindow and ReshapeCustomWindow
        case kWindowMsgGetRegion:
            return wxShapedMacWindowGetRegion(window, param);

        default:
            break;
    }

    return 0;
}


