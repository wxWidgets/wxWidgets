///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for Mac
// Author:      Stefan Csomor
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2004 Stefan Csomor
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/strconv.h"
    #include "wx/control.h"
#endif //WX_PRECOMP

#include "wx/mac/uma.h"
#include "wx/mac/aga.h"
#include "wx/tooltip.h"
#include "wx/dnd.h"

#if wxUSE_SYSTEM_OPTIONS
    #include "wx/sysopt.h"
#endif

#ifndef __DARWIN__
#include <ToolUtils.h>
#endif

// for targeting OSX
#include "wx/mac/private.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// trace mask for activation tracing messages
static const wxChar *TRACE_ACTIVATE = _T("activation");

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList       wxModelessWindows;

static pascal long wxShapedMacWindowDef(short varCode, WindowRef window, SInt16 message, SInt32 param);

// ============================================================================
// wxTopLevelWindowMac implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxTopLevelWindowMac, wxTopLevelWindowBase)
END_EVENT_TABLE()


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
        focus = (wxTopLevelWindowMac*) data ;

    unsigned char charCode ;
    wxChar uniChar[2] ;
    uniChar[0] = 0;
    uniChar[1] = 0;

    UInt32 keyCode ;
    UInt32 modifiers ;
    Point point ;
    UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;

#if wxUSE_UNICODE
    UInt32 dataSize = 0 ;
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

static EventMouseButton lastButton = 0 ;

static void SetupMouseEvent( wxMouseEvent &wxevent , wxMacCarbonEvent &cEvent )
{
    UInt32 modifiers = cEvent.GetParameter<UInt32>(kEventParamKeyModifiers, typeUInt32) ;
    Point screenMouseLocation = cEvent.GetParameter<Point>(kEventParamMouseLocation) ;

    // this parameter are not given for all events
    EventMouseButton button = 0 ;
    UInt32 clickCount = 0 ;
    cEvent.GetParameter<EventMouseButton>( kEventParamMouseButton, typeMouseButton , &button ) ;
    cEvent.GetParameter<UInt32>( kEventParamClickCount, typeUInt32 , &clickCount ) ;

    wxevent.m_x = screenMouseLocation.h;
    wxevent.m_y = screenMouseLocation.v;
    wxevent.m_shiftDown = modifiers & shiftKey;
    wxevent.m_controlDown = modifiers & controlKey;
    wxevent.m_altDown = modifiers & optionKey;
    wxevent.m_metaDown = modifiers & cmdKey;
    wxevent.SetTimestamp( cEvent.GetTicks() ) ;

   // a control click is interpreted as a right click
    if ( button == kEventMouseButtonPrimary && (modifiers & controlKey) )
        button = kEventMouseButtonSecondary ;

    // otherwise we report double clicks by connecting a left click with a ctrl-left click
    if ( clickCount > 1 && button != lastButton )
        clickCount = 1 ;

    // we must make sure that our synthetic 'right' button corresponds in
    // mouse down, moved and mouse up, and does not deliver a right down and left up

    if ( cEvent.GetKind() == kEventMouseDown )
        lastButton = button ;

    if ( button == 0 )
        lastButton = 0 ;
    else if ( lastButton )
        button = lastButton ;

    // determine the correct down state, wx does not want a 'down' for a mouseUp event,
    // while mac delivers this button
    if ( button != 0 && cEvent.GetKind() != kEventMouseUp )
    {
        switch ( button )
        {
            case kEventMouseButtonPrimary :
                wxevent.m_leftDown = true ;
                break ;

            case kEventMouseButtonSecondary :
                wxevent.m_rightDown = true ;
                break ;

            case kEventMouseButtonTertiary :
                wxevent.m_middleDown = true ;
                break ;

            default:
                break ;
        }
    }

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

            // EventMouseWheelAxis axis = cEvent.GetParameter<EventMouseWheelAxis>(kEventParamMouseWheelAxis, typeMouseWheelAxis) ;
            SInt32 delta = cEvent.GetParameter<SInt32>(kEventParamMouseWheelDelta, typeLongInteger) ;

            wxevent.m_wheelRotation = delta;
            wxevent.m_wheelDelta = 1;
            wxevent.m_linesPerAction = 1;
        }
        break ;

        default :
            wxevent.SetEventType( wxEVT_MOTION ) ;
            break ;
    }
}

ControlRef wxMacFindSubControl( wxTopLevelWindowMac* toplevelWindow, const Point& location , ControlRef superControl , ControlPartCode *outPart )
{
    if ( superControl )
    {
        UInt16 childrenCount = 0 ;
        ControlHandle sibling ;
        Rect r ;
        OSStatus err = CountSubControls( superControl , &childrenCount ) ;
        if ( err == errControlIsNotEmbedder )
            return NULL ;

        wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") ) ;

        for ( UInt16 i = childrenCount ; i >=1  ; --i )
        {
            err = GetIndexedSubControl( superControl , i , & sibling ) ;
            if ( err == errControlIsNotEmbedder )
                return NULL ;

            wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") ) ;
            if ( IsControlVisible( sibling ) )
            {
                UMAGetControlBoundsInWindowCoords( sibling , &r ) ;
                if ( MacPtInRect( location , &r ) )
                {
                    ControlHandle child = wxMacFindSubControl( toplevelWindow , location , sibling , outPart ) ;
                    if ( child )
                    {
                        return child ;
                    }
                    else
                    {
                        Point testLocation = location ;

                        if ( toplevelWindow )
                        {
                            testLocation.h -= r.left ;
                            testLocation.v -= r.top ;
                        }

                        *outPart = TestControl( sibling , testLocation ) ;

                        return sibling ;
                    }
                }
            }
        }
    }

    return NULL ;
}

ControlRef wxMacFindControlUnderMouse( wxTopLevelWindowMac* toplevelWindow , const Point& location , WindowRef window , ControlPartCode *outPart )
{
#if TARGET_API_MAC_OSX
    if ( UMAGetSystemVersion() >= 0x1030 )
        return FindControlUnderMouse( location , window , outPart ) ;
#endif

    ControlRef rootControl = NULL ;
    verify_noerr( GetRootControl( window , &rootControl ) ) ;

    return wxMacFindSubControl( toplevelWindow , location , rootControl , outPart ) ;
}

#define NEW_CAPTURE_HANDLING 1

pascal OSStatus wxMacTopLevelMouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    wxTopLevelWindowMac* toplevelWindow = (wxTopLevelWindowMac*) data ;

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
        QDGlobalToLocalPoint( UMAGetWindowPort(window ) ,  &windowMouseLocation ) ;

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
            control = wxMacFindControlUnderMouse( toplevelWindow , windowMouseLocation , window , &part ) ;
            // if there is no control below the mouse position, send the event to the toplevel window itself
            if ( control == 0 )
            {
                currentMouseWindow = (wxWindow*) data ;
            }
            else
            {
                currentMouseWindow = wxFindControlFromMacControl( control ) ;
                if ( currentMouseWindow == NULL && cEvent.GetKind() == kEventMouseMoved )
                {
#if wxUSE_TOOLBAR
                    // for wxToolBar to function we have to send certaint events to it
                    // instead of its children (wxToolBarTools)
                    ControlRef parent ;
                    GetSuperControl(control, &parent );
                    wxWindow *wxParent = wxFindControlFromMacControl( parent ) ;
                    if ( wxParent && wxParent->IsKindOf( CLASSINFO( wxToolBar ) ) )
                        currentMouseWindow = wxParent ;
#endif
                }
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

            g_MacLastWindow->GetEventHandler()->ProcessEvent(eventleave);
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

            currentMouseWindow->GetEventHandler()->ProcessEvent(evententer);
        }

        g_MacLastWindow = currentMouseWindow ;
    }

    if ( windowPart == inMenuBar )
    {
        // special case menu bar, as we are having a low-level runloop we must do it ourselves
        if ( cEvent.GetKind() == kEventMouseDown )
        {
            ::MenuSelect( screenMouseLocation ) ;
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

        if ( currentMouseWindow->GetEventHandler()->ProcessEvent(wxevent) )
        {
            if ((currentMouseWindowParent != NULL) &&
                (currentMouseWindowParent->GetChildren().Find(currentMouseWindow) == NULL))
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

            ControlPartCode dummyPart ;
            // if built-in find control is finding the wrong control (ie static box instead of overlaid
            // button, we cannot let the standard handler do its job, but must handle manually

            if ( ( cEvent.GetKind() == kEventMouseDown )
#ifdef __WXMAC_OSX__
                &&
                (FindControlUnderMouse(windowMouseLocation , window , &dummyPart) !=
                wxMacFindControlUnderMouse( toplevelWindow , windowMouseLocation , window , &dummyPart ) )
#endif
                )
            {
                if ( currentMouseWindow->MacIsReallyEnabled() )
                {
                    EventModifiers modifiers = cEvent.GetParameter<EventModifiers>(kEventParamKeyModifiers, typeUInt32) ;
                    Point clickLocation = windowMouseLocation ;

                    currentMouseWindow->MacRootWindowToWindow( &clickLocation.h , &clickLocation.v ) ;

                    HandleControlClick( (ControlRef) currentMouseWindow->GetHandle() , clickLocation ,
                        modifiers , (ControlActionUPP ) -1 ) ;

                    if ((currentMouseWindowParent != NULL) &&
                        (currentMouseWindowParent->GetChildren().Find(currentMouseWindow) == NULL))
                    {
                        currentMouseWindow = NULL;
                    }
                }

                result = noErr ;
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

        while ( cursorTarget && !cursorTarget->MacSetupCursor( cursorPoint ) )
        {
            cursorTarget = cursorTarget->GetParent() ;
            if ( cursorTarget )
                cursorPoint += cursorTarget->GetPosition();
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
#if TARGET_API_MAC_OSX
            HIPoint hiPoint ;
            hiPoint.x = clickLocation.h ;
            hiPoint.y = clickLocation.v ;
            HIViewConvertPoint( &hiPoint , (ControlRef) toplevelWindow->GetHandle() , control  ) ;
            clickLocation.h = (int)hiPoint.x ;
            clickLocation.v = (int)hiPoint.y ;
#endif // TARGET_API_MAC_OSX

            HandleControlClick( control , clickLocation , modifiers , (ControlActionUPP ) -1 ) ;
            result = noErr ;
        }
    }

    return result ;
}

static pascal OSStatus wxMacTopLevelWindowEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    // WindowRef windowRef = cEvent.GetParameter<WindowRef>(kEventParamDirectObject) ;
    wxTopLevelWindowMac* toplevelWindow = (wxTopLevelWindowMac*) data ;

    switch ( GetEventKind( event ) )
    {
        case kEventWindowActivated :
        {
            toplevelWindow->MacActivate( cEvent.GetTicks() , true) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, true , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent(wxevent);
            // we still sending an eventNotHandledErr in order to allow for default processing
        }
            break ;

        case kEventWindowDeactivated :
        {
            toplevelWindow->MacActivate(cEvent.GetTicks() , false) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, false , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent(wxevent);
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
                // according to the other ports we handle this within the OS level
                // resize event, not within a wxSizeEvent
                wxFrame *frame = wxDynamicCast( toplevelWindow , wxFrame ) ;
                if ( frame )
                {
                    frame->PositionBars();
                }

                wxSizeEvent event( r.GetSize() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;

                toplevelWindow->GetEventHandler()->ProcessEvent(event) ;
                toplevelWindow->wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
            }

            if ( attributes & kWindowBoundsChangeOriginChanged )
            {
                wxMoveEvent event( r.GetLeftTop() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;
                toplevelWindow->GetEventHandler()->ProcessEvent(event) ;
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
                if ( toplevelWindow->GetEventHandler()->ProcessEvent(wxevent) )
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

        default :
            break ;
    }

    return result ;
}

// mix this in from window.cpp
pascal OSStatus wxMacUnicodeTextEventHandler( EventHandlerCallRef handler , EventRef event , void *data ) ;

pascal OSStatus wxMacTopLevelEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
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
            result = wxMacTopLevelWindowEventHandler( handler, event , data ) ;
            break ;

        case kEventClassMouse :
            result = wxMacTopLevelMouseEventHandler( handler, event , data ) ;
            break ;

        default :
            break ;
    }

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacTopLevelEventHandler )

// ---------------------------------------------------------------------------
// wxWindowMac utility functions
// ---------------------------------------------------------------------------

// Find an item given the Macintosh Window Reference

WX_DECLARE_HASH_MAP(WindowRef, wxTopLevelWindowMac*, wxPointerHash, wxPointerEqual, MacWindowMap);

static MacWindowMap wxWinMacWindowList;

wxTopLevelWindowMac *wxFindWinFromMacWindow(WindowRef inWindowRef)
{
    MacWindowMap::iterator node = wxWinMacWindowList.find(inWindowRef);

    return (node == wxWinMacWindowList.end()) ? NULL : node->second;
}

void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxTopLevelWindowMac *win) ;
void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxTopLevelWindowMac *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // nothing else :-)
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    wxWinMacWindowList[inWindowRef] = win;
}

void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win) ;
void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win)
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
// wxTopLevelWindowMac creation
// ----------------------------------------------------------------------------

wxTopLevelWindowMac *wxTopLevelWindowMac::s_macDeactivateWindow = NULL;

typedef struct
{
    wxPoint m_position ;
    wxSize m_size ;
    bool m_wasResizable ;
} FullScreenData ;

void wxTopLevelWindowMac::Init()
{
    m_iconized =
    m_maximizeOnShow = false;
    m_macWindow = NULL ;

    m_macEventHandler = NULL ;
    m_macFullScreenData = NULL ;
}

class wxMacDeferredWindowDeleter : public wxObject
{
public :
    wxMacDeferredWindowDeleter( WindowRef windowRef )
    {
        m_macWindow = windowRef ;
    }

    virtual ~wxMacDeferredWindowDeleter()
    {
        UMADisposeWindow( (WindowRef) m_macWindow ) ;
    }

protected :
    WindowRef m_macWindow ;
} ;

bool wxTopLevelWindowMac::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
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
    wxWindow::SetLabel( title ) ;

    MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    if (GetExtraStyle() & wxFRAME_EX_METAL)
        MacSetMetalAppearance(true);

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    return true;
}

wxTopLevelWindowMac::~wxTopLevelWindowMac()
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

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
    delete data ;
    m_macFullScreenData = NULL ;
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::Maximize(bool maximize)
{
    Point idealSize = { 0 , 0 } ;
    if ( maximize )
    {
        Rect rect ;
        GetAvailableWindowPositioningBounds(GetMainDevice(),&rect) ;
        idealSize.h = rect.right - rect.left ;
        idealSize.v = rect.bottom - rect.top ;
    }
    ZoomWindowIdeal( (WindowRef)m_macWindow , maximize ? inZoomOut : inZoomIn , &idealSize ) ;
}

bool wxTopLevelWindowMac::IsMaximized() const
{
    return IsWindowInStandardState( (WindowRef)m_macWindow , NULL , NULL ) ;
}

void wxTopLevelWindowMac::Iconize(bool iconize)
{
    if ( IsWindowCollapsable( (WindowRef)m_macWindow) )
        CollapseWindow( (WindowRef)m_macWindow , iconize ) ;
}

bool wxTopLevelWindowMac::IsIconized() const
{
    return IsWindowCollapsed((WindowRef)m_macWindow ) ;
}

void wxTopLevelWindowMac::Restore()
{
    if ( IsMaximized() )
        Maximize(false);
    else if ( IsIconized() )
        Iconize(false);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac misc
// ----------------------------------------------------------------------------

wxPoint wxTopLevelWindowMac::GetClientAreaOrigin() const
{
    return wxPoint(0, 0) ;
}

void wxTopLevelWindowMac::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);
}

void  wxTopLevelWindowMac::MacSetBackgroundBrush( const wxBrush &brush )
{
    wxTopLevelWindowBase::MacSetBackgroundBrush( brush ) ;

    if ( m_macBackgroundBrush.Ok() && m_macBackgroundBrush.GetStyle() != wxTRANSPARENT && m_macBackgroundBrush.MacGetBrushKind() == kwxMacBrushTheme )
    {
        SetThemeWindowBackground( (WindowRef) m_macWindow , m_macBackgroundBrush.MacGetTheme() , false ) ;
    }
}

void wxTopLevelWindowMac::MacInstallTopLevelWindowEventHandler()
{
    if ( m_macEventHandler != NULL )
    {
        verify_noerr( ::RemoveEventHandler( (EventHandlerRef) m_macEventHandler ) ) ;
    }

    InstallWindowEventHandler(
        MAC_WXHWND(m_macWindow), GetwxMacTopLevelEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this, (EventHandlerRef *)&m_macEventHandler );
}

void  wxTopLevelWindowMac::MacCreateRealWindow(
    const wxString& title,
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

    if ( HasFlag( wxFRAME_TOOL_WINDOW) )
    {
        if (
            HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) ||
            HasFlag( wxSYSTEM_MENU ) || HasFlag( wxCAPTION ) ||
            HasFlag(wxTINY_CAPTION_HORIZ) ||  HasFlag(wxTINY_CAPTION_VERT)
            )
        {
            wclass = kFloatingWindowClass ;

            if ( HasFlag(wxTINY_CAPTION_VERT) )
                attr |= kWindowSideTitlebarAttribute ;
        }
        else
        {
            wclass = kPlainWindowClass ;
        }
    }
    else if ( HasFlag( wxCAPTION ) )
    {
        wclass = kDocumentWindowClass ;
        attr |= kWindowInWindowMenuAttribute ;
    }
#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )
    else if ( HasFlag( wxFRAME_DRAWER ) )
    {
        wclass = kDrawerWindowClass;
    }
#endif  //10.2 and up
    else
    {
        if ( HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) ||
            HasFlag( wxCLOSE_BOX ) || HasFlag( wxSYSTEM_MENU ) )
        {
            wclass = kDocumentWindowClass ;
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

    // turn on live resizing (OS X only)
    if (UMAGetSystemVersion() >= 0x1000)
        attr |= kWindowLiveResizeAttribute;

    if ( HasFlag(wxSTAY_ON_TOP) )
        group = GetWindowGroupOfClass(kUtilityWindowClass) ;

    attr |= kWindowCompositingAttribute;
#if 0 // wxMAC_USE_CORE_GRAPHICS ; TODO : decide on overall handling of high dpi screens (pixel vs userscale)
    attr |= kWindowFrameworkScaledAttribute;
#endif

    if ( HasFlag(wxFRAME_SHAPED) )
    {
        WindowDefSpec customWindowDefSpec;
        customWindowDefSpec.defType = kWindowDefProcPtr;
        customWindowDefSpec.u.defProc = NewWindowDefUPP(wxShapedMacWindowDef);

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
    verify_noerr( CreateWindowGroup( kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse, &group ));
    verify_noerr( SetWindowGroupParent( group, GetWindowGroup( (WindowRef) m_macWindow )));
    verify_noerr( SetWindowGroup( (WindowRef) m_macWindow , group ));
  
    // the create commands are only for content rect,
    // so we have to set the size again as structure bounds
    SetWindowBounds(  (WindowRef) m_macWindow , kWindowStructureRgn , &theBoundsRect ) ;

    wxAssociateWinWithMacWindow( (WindowRef) m_macWindow , this ) ;
    UMASetWTitle( (WindowRef) m_macWindow , title , m_font.GetEncoding() ) ;
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
    GetEventHandler()->ProcessEvent(event);
}

void wxTopLevelWindowMac::ClearBackground()
{
    wxWindow::ClearBackground() ;
}

// Raise the window to the top of the Z order
void wxTopLevelWindowMac::Raise()
{
    ::SelectWindow( (WindowRef)m_macWindow ) ;
}

// Lower the window to the bottom of the Z order
void wxTopLevelWindowMac::Lower()
{
    ::SendBehind( (WindowRef)m_macWindow , NULL ) ;
}

void wxTopLevelWindowMac::MacDelayedDeactivation(long timestamp)
{
    if (s_macDeactivateWindow)
    {
        wxLogTrace(TRACE_ACTIVATE,
                   wxT("Doing delayed deactivation of %p"),
                   s_macDeactivateWindow);

        s_macDeactivateWindow->MacActivate(timestamp, false);
    }
}

void wxTopLevelWindowMac::MacActivate( long timestamp , bool inIsActivating )
{
    wxLogTrace(TRACE_ACTIVATE, wxT("TopLevel=%p::MacActivate"), this);

    if (s_macDeactivateWindow == this)
        s_macDeactivateWindow = NULL;

    MacDelayedDeactivation(timestamp);
    MacPropagateHiliteChanged() ;
}

void wxTopLevelWindowMac::SetTitle(const wxString& title)
{
    wxWindow::SetLabel( title ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title , m_font.GetEncoding() ) ;
}

wxString wxTopLevelWindowMac::GetTitle() const
{
    return wxWindow::GetLabel();
}

bool wxTopLevelWindowMac::Show(bool show)
{
    if ( !wxTopLevelWindowBase::Show(show) )
        return false;

    bool plainTransition = false;

#if wxUSE_SYSTEM_OPTIONS
    // code contributed by Ryan Wilcox December 18, 2003
    plainTransition = UMAGetSystemVersion() >= 0x1000 ;
    if ( wxSystemOptions::HasOption(wxMAC_WINDOW_PLAIN_TRANSITION) )
        plainTransition = ( wxSystemOptions::GetOptionInt( wxMAC_WINDOW_PLAIN_TRANSITION ) == 1 ) ;
#endif

    if (show)
    {
        if ( plainTransition )
           ::ShowWindow( (WindowRef)m_macWindow );
        else
           ::TransitionWindow( (WindowRef)m_macWindow, kWindowZoomTransitionEffect, kWindowShowTransitionAction, NULL );

        ::SelectWindow( (WindowRef)m_macWindow ) ;

        // because apps expect a size event to occur at this moment
        wxSizeEvent event(GetSize() , m_windowId);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        if ( plainTransition )
           ::HideWindow( (WindowRef)m_macWindow );
        else
           ::TransitionWindow( (WindowRef)m_macWindow, kWindowZoomTransitionEffect, kWindowHideTransitionAction, NULL );
    }

    MacPropagateVisibilityChanged() ;

    return true ;
}

bool wxTopLevelWindowMac::ShowFullScreen(bool show, long style)
{
    if ( show )
    {
        FullScreenData *data = (FullScreenData *)m_macFullScreenData ;
        delete data ;
        data = new FullScreenData() ;

        m_macFullScreenData = data ;
        data->m_position = GetPosition() ;
        data->m_size = GetSize() ;
        data->m_wasResizable = MacGetWindowAttributes() & kWindowResizableAttribute ;

        if ( style & wxFULLSCREEN_NOMENUBAR )
            HideMenuBar() ;

        wxRect client = wxGetClientDisplayRect() ;

        int left , top , right , bottom ;
        int x, y, w, h ;

        x = client.x ;
        y = client.y ;
        w = client.width ;
        h = client.height ;

        MacGetContentAreaInset( left , top , right , bottom ) ;

        if ( style & wxFULLSCREEN_NOCAPTION )
        {
            y -= top ;
            h += top ;
        }

        if ( style & wxFULLSCREEN_NOBORDER )
        {
            x -= left ;
            w += left + right ;
            h += bottom ;
        }

        if ( style & wxFULLSCREEN_NOTOOLBAR )
        {
            // TODO
        }

        if ( style & wxFULLSCREEN_NOSTATUSBAR )
        {
            // TODO
        }

        SetSize( x , y , w, h ) ;
        if ( data->m_wasResizable )
            MacChangeWindowAttributes( kWindowNoAttributes , kWindowResizableAttribute ) ;
    }
    else
    {
        ShowMenuBar() ;
        FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
        if ( data->m_wasResizable )
            MacChangeWindowAttributes( kWindowResizableAttribute ,  kWindowNoAttributes ) ;
        SetPosition( data->m_position ) ;
        SetSize( data->m_size ) ;

        delete data ;
        m_macFullScreenData = NULL ;
    }

    return false;
}

bool wxTopLevelWindowMac::IsFullScreen() const
{
    return m_macFullScreenData != NULL ;
}


bool wxTopLevelWindowMac::SetTransparent(wxByte alpha)
{
    OSStatus result = SetWindowAlpha((WindowRef)m_macWindow, float(alpha)/255.0);
    return result == noErr;
}


bool wxTopLevelWindowMac::CanSetTransparent()
{
    return true;
}


void wxTopLevelWindowMac::SetExtraStyle(long exStyle)
{
    if ( GetExtraStyle() == exStyle )
        return ;

    wxTopLevelWindowBase::SetExtraStyle( exStyle ) ;

#if TARGET_API_MAC_OSX
    if ( m_macWindow != NULL )
    {
        bool metal = GetExtraStyle() & wxFRAME_EX_METAL ;
        if ( MacGetMetalAppearance() != metal )
            MacSetMetalAppearance( metal ) ;
    }
#endif
}

// TODO: switch to structure bounds -
// we are still using coordinates of the content view
//
void wxTopLevelWindowMac::MacGetContentAreaInset( int &left , int &top , int &right , int &bottom )
{
    Rect content, structure ;

    GetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &structure ) ;
    GetWindowBounds( (WindowRef) m_macWindow, kWindowContentRgn , &content ) ;

    left = content.left - structure.left ;
    top = content.top  - structure.top ;
    right = structure.right - content.right ;
    bottom = structure.bottom - content.bottom ;
}

void wxTopLevelWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    m_cachedClippedRectValid = false ;
    Rect bounds = { y , x , y + height , x + width } ;
    verify_noerr(SetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;
    wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
}

void wxTopLevelWindowMac::DoGetPosition( int *x, int *y ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;

    if (x)
       *x = bounds.left ;
    if (y)
       *y = bounds.top ;
}

void wxTopLevelWindowMac::DoGetSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

void wxTopLevelWindowMac::DoGetClientSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowContentRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

void wxTopLevelWindowMac::MacSetMetalAppearance( bool set )
{
#if TARGET_API_MAC_OSX
    MacChangeWindowAttributes( set ? kWindowMetalAttribute : kWindowNoAttributes ,
        set ? kWindowNoAttributes : kWindowMetalAttribute ) ;
#endif
}

bool wxTopLevelWindowMac::MacGetMetalAppearance() const
{
#if TARGET_API_MAC_OSX
    return MacGetWindowAttributes() & kWindowMetalAttribute ;
#else
    return false ;
#endif
}

void wxTopLevelWindowMac::MacChangeWindowAttributes( wxUint32 attributesToSet , wxUint32 attributesToClear )
{
    ChangeWindowAttributes( (WindowRef)m_macWindow, attributesToSet, attributesToClear ) ;
}

wxUint32 wxTopLevelWindowMac::MacGetWindowAttributes() const
{
    UInt32 attr = 0 ;
    GetWindowAttributes( (WindowRef) m_macWindow, &attr ) ;

    return attr ;
}

void wxTopLevelWindowMac::MacPerformUpdates()
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    // for composited windows this also triggers a redraw of all
    // invalid views in the window
    if ( UMAGetSystemVersion() >= 0x1030 )
        HIWindowFlush((WindowRef) m_macWindow) ;
    else
#endif
    {
        // the only way to trigger the redrawing on earlier systems is to call
        // ReceiveNextEvent

        EventRef currentEvent = (EventRef) wxTheApp->MacGetCurrentEvent() ;
        UInt32 currentEventClass = 0 ;
        if ( currentEvent != NULL )
        {
            currentEventClass = ::GetEventClass( currentEvent ) ;
            ::GetEventKind( currentEvent ) ;
        }

        if ( currentEventClass != kEventClassMenu )
        {
            // when tracking a menu, strange redraw errors occur if we flush now, so leave..
            EventRef theEvent;
            ReceiveNextEvent( 0 , NULL , kEventDurationNoWait , false , &theEvent ) ;
        }
    }
}

// Attracts the users attention to this window if the application is
// inactive (should be called when a background event occurs)

static pascal void wxMacNMResponse( NMRecPtr ptr )
{
    NMRemove( ptr ) ;
    DisposePtr( (Ptr)ptr ) ;
}

void wxTopLevelWindowMac::RequestUserAttention(int flags )
{
    NMRecPtr notificationRequest = (NMRecPtr) NewPtr( sizeof( NMRec) ) ;
    static wxMacNMUPP nmupp( wxMacNMResponse );

    memset( notificationRequest , 0 , sizeof(*notificationRequest) ) ;
    notificationRequest->qType = nmType ;
    notificationRequest->nmMark = 1 ;
    notificationRequest->nmIcon = 0 ;
    notificationRequest->nmSound = 0 ;
    notificationRequest->nmStr = NULL ;
    notificationRequest->nmResp = nmupp ;

    verify_noerr( NMInstall( notificationRequest ) ) ;
}

// ---------------------------------------------------------------------------
// Shape implementation
// ---------------------------------------------------------------------------


bool wxTopLevelWindowMac::SetShape(const wxRegion& region)
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
    SetWRefCon((WindowRef)MacGetWindowRef(), (SInt32)shapeRegion);

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
    Point pt = { inRect->left, inRect->top };

    QDLocalToGlobalPoint( GetWindowPort(window), &pt ) ;
    inRect->top = pt.v;
    inRect->left = pt.h;
    inRect->bottom += pt.v;
    inRect->right += pt.h;
}

static SInt32 wxShapedMacWindowGetFeatures(WindowRef window, SInt32 param)
{
    /*------------------------------------------------------
        Define which options your custom window supports.
    --------------------------------------------------------*/
    //just enable everything for our demo
    *(OptionBits*)param =
        //kWindowCanGrow |
        //kWindowCanZoom |
        //kWindowCanCollapse |
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
    wxTopLevelWindowMac* win = wxFindWinFromMacWindow(window);
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

static pascal long wxShapedMacWindowDef(short varCode, WindowRef window, SInt16 message, SInt32 param)
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
