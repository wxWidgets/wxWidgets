///////////////////////////////////////////////////////////////////////////////
// Name:        mac/toplevel.cpp
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

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/frame.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
#endif //WX_PRECOMP

#include "wx/mac/uma.h"
#include "wx/mac/aga.h"
#include "wx/app.h"
#include "wx/tooltip.h"
#include "wx/dnd.h"
#if wxUSE_SYSTEM_OPTIONS
    #include "wx/sysopt.h"
#endif

#include <ToolUtils.h>

//For targeting OSX
#include "wx/mac/private.h"

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

extern long wxMacTranslateKey(unsigned char key, unsigned char code) ;

static const EventTypeSpec eventList[] =
{
    // TODO remove control related event like key and mouse (except for WindowLeave events)
#if 1
    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,

    { kEventClassKeyboard, kEventRawKeyDown } ,
    { kEventClassKeyboard, kEventRawKeyRepeat } ,
    { kEventClassKeyboard, kEventRawKeyUp } ,
    { kEventClassKeyboard, kEventRawKeyModifiersChanged } ,
#endif

    { kEventClassWindow , kEventWindowShown } ,
    { kEventClassWindow , kEventWindowActivated } ,
    { kEventClassWindow , kEventWindowDeactivated } ,
    { kEventClassWindow , kEventWindowBoundsChanging } ,
    { kEventClassWindow , kEventWindowBoundsChanged } ,
    { kEventClassWindow , kEventWindowClose } ,

    // we have to catch these events on the toplevel window level, as controls don't get the
    // raw mouse events anymore
        
    { kEventClassMouse , kEventMouseDown } ,
    { kEventClassMouse , kEventMouseUp } ,
    { kEventClassMouse , kEventMouseWheelMoved } ,
    { kEventClassMouse , kEventMouseMoved } ,
    { kEventClassMouse , kEventMouseDragged } ,
} ;

static pascal OSStatus TextInputEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxWindow* focus = wxWindow::FindFocus() ;
    char charCode ;
    UInt32 keyCode ;
    UInt32 modifiers ;
    Point point ;

    EventRef rawEvent ;

    GetEventParameter( event , kEventParamTextInputSendKeyboardEvent ,typeEventRef,NULL,sizeof(rawEvent),NULL,&rawEvent ) ;

    GetEventParameter( rawEvent, kEventParamKeyMacCharCodes, typeChar, NULL,sizeof(char), NULL,&charCode );
    GetEventParameter( rawEvent, kEventParamKeyCode, typeUInt32, NULL,  sizeof(UInt32), NULL, &keyCode );
       GetEventParameter( rawEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
    GetEventParameter( rawEvent, kEventParamMouseLocation, typeQDPoint, NULL,
        sizeof( Point ), NULL, &point );

    switch ( GetEventKind( event ) )
    {
        case kEventTextInputUnicodeForKeyEvent :
            // this is only called when no default handler has jumped in, eg a wxControl on a floater window does not
            // get its own kEventTextInputUnicodeForKeyEvent, so we route back the
            wxControl* control = wxDynamicCast( focus , wxControl ) ;
            if ( control )
            {
                ControlRef macControl = (ControlRef) control->GetHandle() ;
                if ( macControl )
                {
                    ::HandleControlKey( macControl , keyCode , charCode , modifiers ) ;
                    result = noErr ;
                }
            }
            /*
            // this may lead to double events sent to a window in case all handlers have skipped the key down event
            UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;
            UInt32 message = (keyCode << 8) + charCode;

            if ( (focus != NULL) && wxTheApp->MacSendKeyDownEvent(
                focus , message , modifiers , when , point.h , point.v ) )
            {
                result = noErr ;
            }
            */
            break ;
    }

    return result ;
}

static pascal OSStatus KeyboardEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxWindow* focus = wxWindow::FindFocus() ;
    if ( focus == NULL )
        return result ;
        
    char charCode ;
    UInt32 keyCode ;
    UInt32 modifiers ;
    Point point ;
    UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;

    GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL,sizeof(char), NULL,&charCode );
    GetEventParameter( event, kEventParamKeyCode, typeUInt32, NULL,  sizeof(UInt32), NULL, &keyCode );
       GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
    GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL,
        sizeof( Point ), NULL, &point );

    UInt32 message = (keyCode << 8) + charCode;
    switch( GetEventKind( event ) )
    {
        case kEventRawKeyRepeat :
        case kEventRawKeyDown :
            {
                WXEVENTREF formerEvent = wxTheApp->MacGetCurrentEvent() ;
                WXEVENTHANDLERCALLREF formerHandler = wxTheApp->MacGetCurrentEventHandlerCallRef() ;
                wxTheApp->MacSetCurrentEvent( event , handler ) ;
                if ( (focus != NULL) && wxTheApp->MacSendKeyDownEvent(
                    focus , message , modifiers , when , point.h , point.v ) )
                {
                    result = noErr ;
                }
                wxTheApp->MacSetCurrentEvent( formerEvent , formerHandler ) ;
            }
            break ;
        case kEventRawKeyUp :
            if ( (focus != NULL) && wxTheApp->MacSendKeyUpEvent(
                focus , message , modifiers , when , point.h , point.v ) )
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
                event.m_timeStamp = when;
                wxWindow* focus = wxWindow::FindFocus() ;
                event.SetEventObject(focus);

                if ( focus && (modifiers ^ wxTheApp->s_lastModifiers ) & controlKey )
                {
                    event.m_keyCode = WXK_CONTROL ;
                    event.SetEventType( ( modifiers & controlKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( focus && (modifiers ^ wxTheApp->s_lastModifiers ) & shiftKey )
                {
                    event.m_keyCode = WXK_SHIFT ;
                    event.SetEventType( ( modifiers & shiftKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( focus && (modifiers ^ wxTheApp->s_lastModifiers ) & optionKey )
                {
                    event.m_keyCode = WXK_ALT ;
                    event.SetEventType( ( modifiers & optionKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                if ( focus && (modifiers ^ wxTheApp->s_lastModifiers ) & cmdKey )
                {
                    event.m_keyCode = WXK_COMMAND ;
                    event.SetEventType( ( modifiers & cmdKey ) ? wxEVT_KEY_DOWN : wxEVT_KEY_UP ) ;
                    focus->GetEventHandler()->ProcessEvent( event ) ;
                }
                wxTheApp->s_lastModifiers = modifiers ;
            }
             break ;
    }

    return result ;
}

// we don't interfere with foreign controls on our toplevel windows, therefore we always give back eventNotHandledErr
// for windows that we didn't create (like eg Scrollbars in a databrowser) , or for controls where we did not handle the
// mouse down at all

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
    cEvent.GetParameter<EventMouseButton>(kEventParamMouseButton, typeMouseButton , &button) ;
    cEvent.GetParameter<UInt32>(kEventParamClickCount, typeUInt32 , &clickCount ) ;

    wxevent.m_x = screenMouseLocation.h;
    wxevent.m_y = screenMouseLocation.v;
    wxevent.m_shiftDown = modifiers & shiftKey;
    wxevent.m_controlDown = modifiers & controlKey;
    wxevent.m_altDown = modifiers & optionKey;
    wxevent.m_metaDown = modifiers & cmdKey;
    wxevent.SetTimestamp( cEvent.GetTicks() ) ;
   // a control click is interpreted as a right click 
    if ( button == kEventMouseButtonPrimary && (modifiers & controlKey) )
    {
        button = kEventMouseButtonSecondary ;
    }
    
    // we must make sure that our synthetic 'right' button corresponds in
    // mouse down, moved and mouse up, and does not deliver a right down and left up
    
    if ( cEvent.GetKind() == kEventMouseDown )
        lastButton = button ;
        
    if ( button == 0 )
        lastButton = 0 ;    
    else if ( lastButton )
        button = lastButton ;

    // determinate the correct down state, wx does not want a 'down' for a mouseUp event, while mac delivers
    // this button
    if ( button != 0 && cEvent.GetKind() != kEventMouseUp )
    {
        switch( button )
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
        }
    }
    // translate into wx types
    switch ( cEvent.GetKind() )
    {
        case kEventMouseDown :
            switch( button )
            {
                case kEventMouseButtonPrimary :
                    wxevent.SetEventType(clickCount > 1 ? wxEVT_LEFT_DCLICK : wxEVT_LEFT_DOWN )  ;
                    break ;
                case kEventMouseButtonSecondary :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_RIGHT_DCLICK : wxEVT_RIGHT_DOWN ) ;
                    break ;
                case kEventMouseButtonTertiary :
                    wxevent.SetEventType(clickCount > 1 ? wxEVT_MIDDLE_DCLICK : wxEVT_MIDDLE_DOWN ) ;
                    break ;
            }
            break ;
        case kEventMouseUp :
            switch( button )
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
            }
            break ;
     case kEventMouseWheelMoved :
         {
            wxevent.SetEventType(wxEVT_MOUSEWHEEL ) ;

            // EventMouseWheelAxis axis = cEvent.GetParameter<EventMouseWheelAxis>(kEventParamMouseWheelAxis, typeMouseWheelAxis) ;
            SInt32 delta = cEvent.GetParameter<SInt32>(kEventParamMouseWheelDelta, typeLongInteger) ;

            wxevent.m_wheelRotation = delta;
            wxevent.m_wheelDelta = 1;
            wxevent.m_linesPerAction = 1;
            break ;
         }
        default :
            wxevent.SetEventType(wxEVT_MOTION ) ;
            break ;
    }       
}

ControlRef wxMacFindSubControl( Point location , ControlRef superControl , ControlPartCode *outPart )
{
    if ( superControl )
    {
        UInt16 childrenCount = 0 ;
        OSStatus err = CountSubControls( superControl , &childrenCount ) ; 
        if ( err == errControlIsNotEmbedder )
            return NULL ;
        wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") ) ;
            
        for ( UInt16 i = childrenCount ; i >=1  ; --i )
        {
            ControlHandle sibling ;
            err = GetIndexedSubControl( superControl , i , & sibling ) ;
            if ( err == errControlIsNotEmbedder )
                return NULL ;
            
            wxASSERT_MSG( err == noErr , wxT("Unexpected error when accessing subcontrols") ) ;
            if ( IsControlVisible( sibling ) )
            {
                Rect r ;
                UMAGetControlBoundsInWindowCoords( sibling , &r ) ;
                if ( MacPtInRect( location , &r ) )
                {
                    ControlHandle child = wxMacFindSubControl( location , sibling , outPart ) ;
                    if ( child )
                        return child ;
                    else
                    {
                        Point testLocation = location ;
#if TARGET_API_MAC_OSX
                        testLocation.h -= r.left ;
                        testLocation.v -= r.top ;
#endif
                        *outPart = TestControl( sibling , testLocation ) ;
                        return sibling ;
                    }
                }
            }
        }
    }
    return NULL ;
}

ControlRef wxMacFindControlUnderMouse( Point location , WindowRef window , ControlPartCode *outPart )
{
#if TARGET_API_MAC_OSX
    if ( UMAGetSystemVersion() >= 0x1030 )
        return FindControlUnderMouse( location , window , outPart ) ;
#endif
    ControlRef rootControl = NULL ;
    verify_noerr( GetRootControl( window , &rootControl ) ) ;
    return wxMacFindSubControl( location , rootControl , outPart ) ;

}
pascal OSStatus wxMacTopLevelMouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;
    
    Point screenMouseLocation = cEvent.GetParameter<Point>(kEventParamMouseLocation) ;
    Point windowMouseLocation = screenMouseLocation ;

    WindowRef window ;
    short windowPart = ::FindWindow(screenMouseLocation, &window);

    wxWindow* currentMouseWindow = NULL ;
    ControlRef control = NULL ;
    
    if ( window )
    {
        QDGlobalToLocalPoint( UMAGetWindowPort(window ) ,  &windowMouseLocation ) ;

        if ( wxTheApp->s_captureWindow && wxTheApp->s_captureWindow->MacGetTopLevelWindowRef() == (WXWindow) window && windowPart == inContent )
        {
            currentMouseWindow = wxTheApp->s_captureWindow ;
        }
        else if ( (IsWindowActive(window) && windowPart == inContent) )
        {
            ControlPartCode part ;
            control = wxMacFindControlUnderMouse( windowMouseLocation , window , &part ) ;
            // if there is no control below the mouse position, send the event to the toplevel window itself
            if ( control == 0 )
                currentMouseWindow = (wxWindow*) data ;
            else
            {
                currentMouseWindow = wxFindControlFromMacControl( control ) ;
                if ( currentMouseWindow == NULL && cEvent.GetKind() == kEventMouseMoved )
                {
                	// for wxToolBar to function we have to send certaint events to it
                	// instead of its children (wxToolBarTools)	
                    ControlRef parent ;
                    GetSuperControl(control, &parent );
                    wxWindow *wxParent = wxFindControlFromMacControl( parent ) ;
                    if ( wxParent && wxParent->IsKindOf( CLASSINFO( wxToolBar ) ) )
                        currentMouseWindow = wxParent ;
                }
            }
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

#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( g_MacLastWindow , eventleave);
#endif // wxUSE_TOOLTIPS
            g_MacLastWindow->GetEventHandler()->ProcessEvent(eventleave);
        }
        if ( currentMouseWindow )
        {
            wxMouseEvent evententer(wxevent);
            evententer.SetEventType( wxEVT_ENTER_WINDOW );
            currentMouseWindow->ScreenToClient( &evententer.m_x, &evententer.m_y );
            evententer.SetEventObject( currentMouseWindow ) ;
#if wxUSE_TOOLTIPS
            wxToolTip::RelayEvent( currentMouseWindow , evententer);
#endif // wxUSE_TOOLTIPS
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
    } // if ( windowPart == inMenuBar )
    else if ( currentMouseWindow )
    {
        currentMouseWindow->ScreenToClient( &wxevent.m_x , &wxevent.m_y ) ;
        
        wxevent.SetEventObject( currentMouseWindow ) ;

        // update cursor
        
        wxWindow* cursorTarget = currentMouseWindow ;
        wxPoint cursorPoint( wxevent.m_x , wxevent.m_y ) ;

        while( cursorTarget && !cursorTarget->MacSetupCursor( cursorPoint ) )
        {
            cursorTarget = cursorTarget->GetParent() ;
            if ( cursorTarget )
                cursorPoint += cursorTarget->GetPosition() ;
        }

        // make tooltips current
        
    #if wxUSE_TOOLTIPS
        if ( wxevent.GetEventType() == wxEVT_MOTION
            || wxevent.GetEventType() == wxEVT_ENTER_WINDOW
            || wxevent.GetEventType() == wxEVT_LEAVE_WINDOW )
            wxToolTip::RelayEvent( currentMouseWindow , wxevent);
    #endif // wxUSE_TOOLTIPS                
        if ( currentMouseWindow->GetEventHandler()->ProcessEvent(wxevent) )
            result = noErr;
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

            if ( ( cEvent.GetKind() == kEventMouseDown ) && 
                (FindControlUnderMouse(windowMouseLocation , window , &dummyPart) != 
                wxMacFindControlUnderMouse( windowMouseLocation , window , &dummyPart ) ) )
            {
                if ( currentMouseWindow->MacIsReallyEnabled() ) 
                {
                    EventModifiers modifiers = cEvent.GetParameter<EventModifiers>(kEventParamKeyModifiers, typeUInt32) ;
                    Point clickLocation = windowMouseLocation ;
    #if TARGET_API_MAC_OSX
                    currentMouseWindow->MacRootWindowToWindow( &clickLocation.h , &clickLocation.v ) ;
    #endif
                    HandleControlClick( (ControlRef) currentMouseWindow->GetHandle() , clickLocation ,
                        modifiers , (ControlActionUPP ) -1 ) ;
                }
                result = noErr ;
            }
        }
        if ( cEvent.GetKind() == kEventMouseUp && wxTheApp->s_captureWindow )
        {
            wxTheApp->s_captureWindow = NULL ;
            // update cursor ?
         }
    } // else if ( currentMouseWindow )
    else
    {
        // don't mess with controls we don't know about
        // for some reason returning eventNotHandledErr does not lead to the correct behaviour
        // so we try sending them the correct control directly
        wxTopLevelWindowMac* toplevelWindow = (wxTopLevelWindowMac*) data ;
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
#endif
            HandleControlClick( control , clickLocation ,
                modifiers , (ControlActionUPP ) -1 ) ;
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

    switch( GetEventKind( event ) )
    {
        case kEventWindowActivated :
        {
            toplevelWindow->MacActivate( cEvent.GetTicks() , true) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, true , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent(wxevent);
            // we still sending an eventNotHandledErr in order to allow for default processing
            break ;
        }
        case kEventWindowDeactivated :
        {
            toplevelWindow->MacActivate(cEvent.GetTicks() , false) ;
            wxActivateEvent wxevent(wxEVT_ACTIVATE, false , toplevelWindow->GetId());
            wxevent.SetTimestamp( cEvent.GetTicks() ) ;
            wxevent.SetEventObject(toplevelWindow);
            toplevelWindow->GetEventHandler()->ProcessEvent(wxevent);
            // we still sending an eventNotHandledErr in order to allow for default processing
            break ;
        }
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
            UInt32 attributes = cEvent.GetParameter<UInt32>(kEventParamAttributes,typeUInt32) ;
            Rect newRect = cEvent.GetParameter<Rect>(kEventParamCurrentBounds) ;
            wxRect r( newRect.left , newRect.top , newRect.right - newRect.left , newRect.bottom - newRect.top ) ;
            if ( attributes & kWindowBoundsChangeSizeChanged )
            {
                // according to the other ports we handle this within the OS level
                // resize event, not within a wxSizeEvent
                wxFrame *frame = wxDynamicCast( toplevelWindow , wxFrame ) ;
                if ( frame )
                {
        #if wxUSE_STATUSBAR
                    frame->PositionStatusBar();
        #endif            
        #if wxUSE_TOOLBAR
                    frame->PositionToolBar();
        #endif            
                }

                wxSizeEvent event( r.GetSize() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;

                toplevelWindow->GetEventHandler()->ProcessEvent(event) ;
            }
            if ( attributes & kWindowBoundsChangeOriginChanged )
            {
                wxMoveEvent event( r.GetLeftTop() , toplevelWindow->GetId() ) ;
                event.SetEventObject( toplevelWindow ) ;
                toplevelWindow->GetEventHandler()->ProcessEvent(event) ;
            }
            result = noErr ;
            break ;
        }
        case kEventWindowBoundsChanging :
        {
            UInt32 attributes = cEvent.GetParameter<UInt32>(kEventParamAttributes,typeUInt32) ;
            Rect newRect = cEvent.GetParameter<Rect>(kEventParamCurrentBounds) ;

            if ( (attributes & kWindowBoundsChangeSizeChanged) || (attributes & kWindowBoundsChangeOriginChanged) )
            {
                // all (Mac) rects are in content area coordinates, all wxRects in structure coordinates
                int left , top , right , bottom ;
                toplevelWindow->MacGetContentAreaInset( left , top , right , bottom ) ;
                wxRect r( newRect.left - left  , newRect.top  - top  , 
                    newRect.right - newRect.left + left + right  , newRect.bottom - newRect.top + top + bottom ) ;
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
            }

            result = noErr ;
            break ;
        }
        default :
            break ;
    }
    return result ;
}

pascal OSStatus wxMacTopLevelEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassKeyboard :
            result = KeyboardEventHandler( handler, event , data ) ;
            break ;
        case kEventClassTextInput :
            result = TextInputEventHandler( handler, event , data ) ;
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

wxList wxWinMacWindowList(wxKEY_INTEGER);
wxTopLevelWindowMac *wxFindWinFromMacWindow(WindowRef inWindowRef)
{
    wxNode *node = wxWinMacWindowList.Find((long)inWindowRef);
    if (!node)
        return NULL;
    return (wxTopLevelWindowMac *)node->GetData();
}

void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxTopLevelWindowMac *win) ;
void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxTopLevelWindowMac *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    if ( !wxWinMacWindowList.Find((long)inWindowRef) )
        wxWinMacWindowList.Append((long)inWindowRef, win);
}

void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win) ;
void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win)
{
    wxWinMacWindowList.DeleteObject(win);
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac creation
// ----------------------------------------------------------------------------

wxTopLevelWindowMac *wxTopLevelWindowMac::s_macDeactivateWindow = NULL;

typedef struct 
{
    wxPoint m_position ;
    wxSize m_size ;  
} FullScreenData ;

void wxTopLevelWindowMac::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;
    m_macWindow = NULL ;
#if TARGET_API_MAC_OSX 
    m_macUsesCompositing = TRUE;
#else
    m_macUsesCompositing = FALSE;
#endif
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

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;
    wxWindow::SetTitle( title ) ;

    MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    return TRUE;
}

wxTopLevelWindowMac::~wxTopLevelWindowMac()
{
    if ( m_macWindow )
    {
        wxToolTip::NotifyWindowDelete(m_macWindow) ;
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
    wxMacPortStateHelper help( (GrafPtr) GetWindowPort( (WindowRef) m_macWindow) ) ;
    wxMacWindowClipper clip (this);
    ZoomWindow( (WindowRef)m_macWindow , maximize ? inZoomOut : inZoomIn , false ) ;
}

bool wxTopLevelWindowMac::IsMaximized() const
{
    return IsWindowInStandardState(  (WindowRef)m_macWindow , NULL , NULL ) ;
}

void wxTopLevelWindowMac::Iconize(bool iconize)
{
    if ( IsWindowCollapsable((WindowRef)m_macWindow) )
        CollapseWindow((WindowRef)m_macWindow , iconize ) ;
}

bool wxTopLevelWindowMac::IsIconized() const
{
    return IsWindowCollapsed((WindowRef)m_macWindow ) ;
}

void wxTopLevelWindowMac::Restore()
{
    // not available on mac
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac misc
// ----------------------------------------------------------------------------

wxPoint wxTopLevelWindowMac::GetClientAreaOrigin() const
{
    return wxPoint(0,0) ;
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
    InstallWindowEventHandler(MAC_WXHWND(m_macWindow), GetwxMacTopLevelEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this, (EventHandlerRef *)&m_macEventHandler);
}

void  wxTopLevelWindowMac::MacCreateRealWindow( const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name )
{
    OSStatus err = noErr ;
    SetName(name);
    m_windowStyle = style;
    m_isShown = FALSE;

    // create frame.

    Rect theBoundsRect;

    int x = (int)pos.x;
    int y = (int)pos.y;
    
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
            {
                attr |= kWindowSideTitlebarAttribute ;
            }
        }
        else
        {
            wclass = kPlainWindowClass ;
        }
    }
    else if ( HasFlag( wxCAPTION ) )
    {
        wclass = kDocumentWindowClass ;
    }
#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )
    else if ( HasFlag( wxFRAME_DRAWER ) )
    {
        wclass = kDrawerWindowClass;
        // Should this be left for compositing check below?
        // CreateNewWindow will fail without it, should wxDrawerWindow turn
        // on compositing before calling MacCreateRealWindow?
        attr |= kWindowCompositingAttribute;// | kWindowStandardHandlerAttribute;
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

    if ( HasFlag( wxMINIMIZE_BOX ) )
    {
        attr |= kWindowCollapseBoxAttribute ;
    }
    if ( HasFlag( wxMAXIMIZE_BOX ) )
    {
        attr |= kWindowFullZoomAttribute ;
    }
    if ( HasFlag( wxRESIZE_BORDER ) )
    {
        attr |= kWindowResizableAttribute ;
    }
    if ( HasFlag( wxCLOSE_BOX) )
    {
        attr |= kWindowCloseBoxAttribute ;
    }

    if (UMAGetSystemVersion() >= 0x1000)
    {
        // turn on live resizing (OS X only)
        attr |= kWindowLiveResizeAttribute;
    }

    if (HasFlag(wxSTAY_ON_TOP))
        wclass = kUtilityWindowClass;

#if TARGET_API_MAC_OSX 
    attr |= kWindowCompositingAttribute;
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

    wxCHECK_RET( err == noErr, wxT("Mac OS error when trying to create new window") );

    // the create commands are only for content rect, so we have to set the size again as
    // structure bounds 
    SetWindowBounds(  (WindowRef) m_macWindow , kWindowStructureRgn , &theBoundsRect ) ;

    wxAssociateWinWithMacWindow( (WindowRef) m_macWindow , this ) ;
    UMASetWTitle( (WindowRef) m_macWindow , title , m_font.GetEncoding() ) ;
    m_peer = new wxMacControl() ;
#if TARGET_API_MAC_OSX
    // There is a bug in 10.2.X for ::GetRootControl returning the window view instead of 
    // the content view, so we have to retrieve it explicitely
    HIViewFindByID( HIViewGetRoot( (WindowRef) m_macWindow ) , kHIViewWindowContentID , 
        m_peer->GetControlRefAddr() ) ;
    if ( !m_peer->Ok() )
    {
        // compatibility mode fallback
        GetRootControl( (WindowRef) m_macWindow , m_peer->GetControlRefAddr() ) ;
    }
#else
    ::CreateRootControl( (WindowRef)m_macWindow , m_peer->GetControlRefAddr() ) ;
#endif
    // the root control level handleer
    MacInstallEventHandler( (WXWidget) m_peer->GetControlRef() ) ;

    // the frame window event handler
    InstallStandardEventHandler( GetWindowEventTarget(MAC_WXHWND(m_macWindow)) ) ;
    MacInstallTopLevelWindowEventHandler() ;

    m_macFocus = NULL ;

    if ( HasFlag(wxFRAME_SHAPED) )
    {
        // default shape matches the window size
        wxRegion rgn(0, 0, w, h);
        SetShape(rgn);
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
    if(s_macDeactivateWindow)
    {
        wxLogDebug(wxT("Doing delayed deactivation of %p"),s_macDeactivateWindow);
        s_macDeactivateWindow->MacActivate(timestamp, false);
    }
}

void wxTopLevelWindowMac::MacActivate( long timestamp , bool inIsActivating )
{
    // wxLogDebug(wxT("TopLevel=%p::MacActivate"),this);

    if(s_macDeactivateWindow==this)
        s_macDeactivateWindow=NULL;
    MacDelayedDeactivation(timestamp);
    MacPropagateHiliteChanged() ;
}

void wxTopLevelWindowMac::SetTitle(const wxString& title)
{
    wxWindow::SetTitle( title ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title , m_font.GetEncoding() ) ;
}

bool wxTopLevelWindowMac::Show(bool show)
{
    if ( !wxTopLevelWindowBase::Show(show) )
        return FALSE;

    if (show)
    {
        #if wxUSE_SYSTEM_OPTIONS	//code contributed by Ryan Wilcox December 18, 2003
        if ( (wxSystemOptions::HasOption(wxMAC_WINDOW_PLAIN_TRANSITION) ) && ( wxSystemOptions::GetOptionInt( wxMAC_WINDOW_PLAIN_TRANSITION ) == 1) )
        {
           ::ShowWindow( (WindowRef)m_macWindow );
        }
        else
        #endif
        {
           ::TransitionWindow((WindowRef)m_macWindow,kWindowZoomTransitionEffect,kWindowShowTransitionAction,nil);
        }
        ::SelectWindow( (WindowRef)m_macWindow ) ;
        // as apps expect a size event to occur at this moment
        wxSizeEvent event( GetSize() , m_windowId);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        #if wxUSE_SYSTEM_OPTIONS
        if ( (wxSystemOptions::HasOption(wxMAC_WINDOW_PLAIN_TRANSITION) ) && ( wxSystemOptions::GetOptionInt( wxMAC_WINDOW_PLAIN_TRANSITION ) == 1) )
        {
           ::HideWindow((WindowRef) m_macWindow );
        }
        else
        #endif
        {
           ::TransitionWindow((WindowRef)m_macWindow,kWindowZoomTransitionEffect,kWindowHideTransitionAction,nil);
        }
    }

    MacPropagateVisibilityChanged() ;

    return TRUE ;
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
        
        if ( style & wxFULLSCREEN_NOMENUBAR )
        {
                HideMenuBar() ;
        }
        int left , top , right , bottom ;
        wxRect client = wxGetClientDisplayRect() ;

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
    }
    else
    {
        ShowMenuBar() ;
        FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
        SetPosition( data->m_position ) ;
        SetSize( data->m_size ) ;
        delete data ;
        m_macFullScreenData = NULL ;
    }
    return FALSE; 
}

bool wxTopLevelWindowMac::IsFullScreen() const 
{ 
    return m_macFullScreenData != NULL ; 
}

// we are still using coordinates of the content view, todo switch to structure bounds

void wxTopLevelWindowMac::MacGetContentAreaInset( int &left , int &top , int &right , int &bottom )
{
    Rect content ;
    Rect structure ;
    GetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &structure ) ;
    GetWindowBounds( (WindowRef) m_macWindow, kWindowContentRgn , &content ) ;

    left = content.left - structure.left ;
    top = content.top  - structure.top ;
    right = structure.right - content.right ;
    bottom = structure.bottom - content.bottom ;
}

void wxTopLevelWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    Rect bounds = { y , x , y + height , x + width } ;
    verify_noerr(SetWindowBounds( (WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;
}

void wxTopLevelWindowMac::DoGetPosition( int *x, int *y ) const
{
    Rect bounds ;
    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;
    if(x)   *x = bounds.left ;
    if(y)   *y = bounds.top ;
}
void wxTopLevelWindowMac::DoGetSize( int *width, int *height ) const
{
    Rect bounds ;
    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowStructureRgn , &bounds )) ;
    if(width)   *width = bounds.right - bounds.left ;
    if(height)   *height = bounds.bottom - bounds.top ;
}

void wxTopLevelWindowMac::DoGetClientSize( int *width, int *height ) const
{
    Rect bounds ;
    verify_noerr(GetWindowBounds((WindowRef) m_macWindow, kWindowContentRgn , &bounds )) ;
    if(width)   *width = bounds.right - bounds.left ;
    if(height)   *height = bounds.bottom - bounds.top ;
}

void wxTopLevelWindowMac::MacSetMetalAppearance( bool set ) 
{
#if TARGET_API_MAC_OSX
    UInt32 attr = 0 ;
    GetWindowAttributes((WindowRef) m_macWindow , &attr ) ;
    wxASSERT_MSG( attr & kWindowCompositingAttribute ,
        wxT("Cannot set metal appearance on a non-compositing window") ) ;
                
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
    ChangeWindowAttributes ( (WindowRef) m_macWindow , attributesToSet, attributesToClear ) ;
}

wxUint32 wxTopLevelWindowMac::MacGetWindowAttributes() const 
{
    UInt32 attr = 0 ;
    GetWindowAttributes((WindowRef) m_macWindow , &attr ) ;
    return attr ;
}

// ---------------------------------------------------------------------------
// Shape implementation
// ---------------------------------------------------------------------------


bool wxTopLevelWindowMac::SetShape(const wxRegion& region)
{
    wxCHECK_MSG( HasFlag(wxFRAME_SHAPED), FALSE,
                 _T("Shaped windows must be created with the wxFRAME_SHAPED style."));

    // The empty region signifies that the shape should be removed from the
    // window.
    if ( region.IsEmpty() )
    {
        wxSize sz = GetClientSize();
        wxRegion rgn(0, 0, sz.x, sz.y);
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

    // Tell the window manager that the window has changed shape
    ReshapeCustomWindow((WindowRef)MacGetWindowRef());
    return TRUE;
}

// ---------------------------------------------------------------------------
// Support functions for shaped windows, based on Apple's CustomWindow sample at
// http://developer.apple.com/samplecode/Sample_Code/Human_Interface_Toolbox/Mac_OS_High_Level_Toolbox/CustomWindow.htm
// ---------------------------------------------------------------------------

static void wxShapedMacWindowGetPos(WindowRef window, Rect* inRect)
{
    GetWindowPortBounds(window, inRect);
    Point pt = {inRect->left, inRect->top};
    QDLocalToGlobalPoint( GetWindowPort(window) , &pt ) ;
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
    *(OptionBits*)param=//kWindowCanGrow|
                        //kWindowCanZoom|
                        //kWindowCanCollapse|
                        //kWindowCanGetWindowRegion|
                        //kWindowHasTitleBar|
                        //kWindowSupportsDragHilite|
                        kWindowCanDrawInCurrentPort|
                        //kWindowCanMeasureTitle|
                        kWindowWantsDisposeAtProcessDeath|
                        kWindowSupportsGetGrowImageRegion|
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
        wxShapedMacWindowGetPos(window, &r ) ;
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
        wxShapedMacWindowGetPos(window, &windowRect);	//how big is the window
        CopyRgn(cachedRegion, rgn);		//make a copy of our cached region
        OffsetRgn(rgn, windowRect.left, windowRect.top); // position it over window
        //MapRgn(rgn, &mMaskSize, &windowRect);	//scale it to our actual window size
    }
}



static SInt32 wxShapedMacWindowGetRegion(WindowRef window, SInt32 param)
{
    GetWindowRegionPtr rgnRec=(GetWindowRegionPtr)param;

    switch(rgnRec->regionCode)
    {
        case kWindowStructureRgn:
            wxShapedMacWindowStructureRegion(window, rgnRec->winRgn);
            break;
        case kWindowContentRgn:
            wxShapedMacWindowContentRegion(window, rgnRec->winRgn);
            break;
        default:
            SetEmptyRgn(rgnRec->winRgn);
    }  //switch

    return noErr;
}


static SInt32 wxShapedMacWindowHitTest(WindowRef window,SInt32 param)
{
    /*------------------------------------------------------
        Determine the region of the window which was hit
    --------------------------------------------------------*/
    Point hitPoint;
    static RgnHandle tempRgn=nil;

    if(!tempRgn)
    	tempRgn=NewRgn();

    SetPt(&hitPoint,LoWord(param),HiWord(param));//get the point clicked

     //Mac OS 8.5 or later
    wxShapedMacWindowStructureRegion(window, tempRgn);
    if (PtInRgn(hitPoint, tempRgn)) //in window content region?
        return wInContent;

    return wNoHit;//no significant area was hit.
}


static pascal long wxShapedMacWindowDef(short varCode, WindowRef window, SInt16 message, SInt32 param)
{
    switch(message)
    {
        case kWindowMsgHitTest:
            return wxShapedMacWindowHitTest(window,param);

        case kWindowMsgGetFeatures:
            return wxShapedMacWindowGetFeatures(window,param);

        // kWindowMsgGetRegion is sent during CreateCustomWindow and ReshapeCustomWindow
        case kWindowMsgGetRegion:
            return wxShapedMacWindowGetRegion(window,param);
    }

    return 0;
}

// ---------------------------------------------------------------------------

