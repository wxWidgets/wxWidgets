///////////////////////////////////////////////////////////////////////////////
// Name:        mac/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
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
#endif //WX_PRECOMP

#include "wx/mac/uma.h"
#include "wx/mac/aga.h"
#include "wx/app.h"
#include "wx/tooltip.h"
#include "wx/dnd.h"

#include "ToolUtils.h"


#define wxMAC_DEBUG_REDRAW 0
#ifndef wxMAC_DEBUG_REDRAW
#define wxMAC_DEBUG_REDRAW 0
#endif

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList       wxModelessWindows;

// double click testing
static   Point     gs_lastWhere;
static   long      gs_lastWhen = 0;


#if TARGET_CARBON
static pascal long wxShapedMacWindowDef(short varCode, WindowRef window, SInt16 message, SInt32 param);
#endif

// ============================================================================
// wxTopLevelWindowMac implementation
// ============================================================================

// ---------------------------------------------------------------------------
// Carbon Events
// ---------------------------------------------------------------------------

#if TARGET_CARBON

extern long wxMacTranslateKey(unsigned char key, unsigned char code) ;

static const EventTypeSpec eventList[] =
{
    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,

    { kEventClassKeyboard, kEventRawKeyDown } ,
    { kEventClassKeyboard, kEventRawKeyRepeat } ,
    { kEventClassKeyboard, kEventRawKeyUp } ,
    { kEventClassKeyboard, kEventRawKeyModifiersChanged } ,

    { kEventClassWindow , kEventWindowUpdate } ,
    { kEventClassWindow , kEventWindowActivated } ,
    { kEventClassWindow , kEventWindowDeactivated } ,
    { kEventClassWindow , kEventWindowBoundsChanging } ,
    { kEventClassWindow , kEventWindowBoundsChanged } ,
    { kEventClassWindow , kEventWindowClose } ,

    { kEventClassMouse , kEventMouseDown } ,
    { kEventClassMouse , kEventMouseUp } ,
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
    UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;

    EventRef rawEvent ;

    GetEventParameter( event , kEventParamTextInputSendKeyboardEvent ,typeEventRef,NULL,sizeof(rawEvent),NULL,&rawEvent ) ;

    GetEventParameter( rawEvent, kEventParamKeyMacCharCodes, typeChar, NULL,sizeof(char), NULL,&charCode );
    GetEventParameter( rawEvent, kEventParamKeyCode, typeUInt32, NULL,  sizeof(UInt32), NULL, &keyCode );
       GetEventParameter( rawEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
    GetEventParameter( rawEvent, kEventParamMouseLocation, typeQDPoint, NULL,
        sizeof( Point ), NULL, &point );

    UInt32 message = (keyCode << 8) + charCode;

    switch ( GetEventKind( event ) )
    {
        case kEventTextInputUnicodeForKeyEvent :
            if ( (focus != NULL) && wxTheApp->MacSendKeyDownEvent(
                focus , message , modifiers , when , point.h , point.v ) )
            {
                result = noErr ;
            }
            break ;
    }

    return result ;
}

static pascal OSStatus KeyboardEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxWindow* focus = wxWindow::FindFocus() ;
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
            if ( (focus != NULL) && wxTheApp->MacSendKeyDownEvent(
                focus , message , modifiers , when , point.h , point.v ) )
            {
                result = noErr ;
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
                wxTheApp->s_lastModifiers = modifiers ;
            }
             break ;
    }

    return result ;
}

static pascal OSStatus MouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxTopLevelWindowMac* toplevelWindow = (wxTopLevelWindowMac*) data ;
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

    WindowRef window ;
    short windowPart = ::FindWindow(point, &window);

    if ( IsWindowActive(window) && windowPart == inContent )
    {
        switch ( GetEventKind( event ) )
        {
            case kEventMouseDown :
                toplevelWindow->MacFireMouseEvent( mouseDown , point.h , point.v , modifiers , EventTimeToTicks( GetEventTime( event ) ) ) ;
                result = noErr ;
                break ;
            case kEventMouseUp :
                toplevelWindow->MacFireMouseEvent( mouseUp , point.h , point.v , modifiers , EventTimeToTicks( GetEventTime( event ) ) ) ;
                result = noErr ;
                break ;
            case kEventMouseMoved :
                toplevelWindow->MacFireMouseEvent( nullEvent , point.h , point.v , modifiers , EventTimeToTicks( GetEventTime( event ) ) ) ;
                result = noErr ;
                break ;
            case kEventMouseDragged :
                toplevelWindow->MacFireMouseEvent( nullEvent , point.h , point.v , modifiers , EventTimeToTicks( GetEventTime( event ) ) ) ;
                result = noErr ;
                break ;
            default :
                break ;
        }
    }

    return result ;


}
static pascal OSStatus WindowEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    OSStatus err = noErr ;

    UInt32        attributes;
    WindowRef windowRef ;
    wxTopLevelWindowMac* toplevelWindow = (wxTopLevelWindowMac*) data ;

    GetEventParameter( event, kEventParamDirectObject, typeWindowRef, NULL,
        sizeof( WindowRef ), NULL, &windowRef );

    switch( GetEventKind( event ) )
    {
        case kEventWindowUpdate :
            if ( !wxPendingDelete.Member(toplevelWindow) )
                toplevelWindow->MacUpdate( EventTimeToTicks( GetEventTime( event ) ) ) ;
            result = noErr ;
            break ;
        case kEventWindowActivated :
                toplevelWindow->MacActivate( EventTimeToTicks( GetEventTime( event ) ) , true) ;
            result = noErr ;
            break ;
        case kEventWindowDeactivated :
                toplevelWindow->MacActivate( EventTimeToTicks( GetEventTime( event ) ) , false) ;
            result = noErr ;
            break ;
        case kEventWindowClose :
                toplevelWindow->Close() ;
            result = noErr ;
            break ;
        case kEventWindowBoundsChanged :
            err = GetEventParameter( event, kEventParamAttributes, typeUInt32,
                        NULL, sizeof( UInt32 ), NULL, &attributes );
            if ( err == noErr )
            {
                Rect newContentRect ;

                GetEventParameter( event, kEventParamCurrentBounds, typeQDRectangle, NULL,
                    sizeof( newContentRect ), NULL, &newContentRect );

                toplevelWindow->SetSize( newContentRect.left , newContentRect.top ,
                    newContentRect.right - newContentRect.left ,
                    newContentRect.bottom - newContentRect.top, wxSIZE_USE_EXISTING);

                result = noErr;
            }
            break ;
        default :
            break ;
    }
    return result ;
}

pascal OSStatus wxMacWindowEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
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
            result = WindowEventHandler( handler, event , data ) ;
            break ;
        case kEventClassMouse :
            result = MouseEventHandler( handler, event , data ) ;
            break ;
        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacWindowEventHandler )

// Patch 531199 defined a window event handler, as follows.
// TODO: merge the moving/sizing event handling with the event
// handler above.
#if 0
static pascal OSStatus
WindowHandler( EventHandlerCallRef inHandler, EventRef inEvent, void* userData )
{
    Rect		bounds;
    SInt16		height, width;
    UInt32		attributes;
    OSStatus	result = eventNotHandledErr;
    
    GetEventParameter( inEvent, kEventParamAttributes, typeUInt32, NULL, sizeof( UInt32 ), NULL, &attributes );
    
    if ((attributes & (kWindowBoundsChangeSizeChanged | kWindowBoundsChangeOriginChanged)) != 0)
    {
        // Extract the current bounds. This is the paramter you get to modify to
        // alter the window position or size during a window resizing.
        GetEventParameter( inEvent, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof( bounds ), NULL, &bounds );
        
        wxRect rect;
        rect.SetLeft(bounds.left);
        rect.SetTop(bounds.top);
        rect.SetRight(bounds.right);
        rect.SetBottom(bounds.bottom);
        
        bool rc;
        wxWindowMac *pWindow = (wxWindowMac*)userData;
        if ((attributes & kWindowBoundsChangeSizeChanged) != 0) {
            wxSizeEvent event(rect, pWindow->GetId());
            event.SetEventObject(pWindow);
            rc = pWindow->GetEventHandler()->ProcessEvent(event);
            rect = event.GetRect();
        }
        else {
            wxMoveEvent event(rect, pWindow->GetId());
            event.SetEventObject(pWindow);
            rc = pWindow->GetEventHandler()->ProcessEvent(event);
            rect = event.GetRect();
        }
        
        if (rc) {
            bounds.left = rect.GetLeft();
            bounds.top = rect.GetTop();
            bounds.right = rect.GetRight();
            bounds.bottom = rect.GetBottom();
        }
        
        // Set the current bounds parameter to our adjusted bounds. Return
        // noErr to indicate we handled this event.
        SetEventParameter( inEvent, kEventParamCurrentBounds, typeQDRectangle, sizeof( bounds ), &bounds );
        result = noErr;
    }
    return result;
}
#endif
    // WindowHandler

#endif

// ---------------------------------------------------------------------------
// wxWindowMac utility functions
// ---------------------------------------------------------------------------

// Find an item given the Macintosh Window Reference

wxList *wxWinMacWindowList = NULL;
wxTopLevelWindowMac *wxFindWinFromMacWindow(WXWindow inWindowRef)
{
    wxNode *node = wxWinMacWindowList->Find((long)inWindowRef);
    if (!node)
        return NULL;
    return (wxTopLevelWindowMac *)node->GetData();
}

void wxAssociateWinWithMacWindow(WXWindow inWindowRef, wxTopLevelWindowMac *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    if ( !wxWinMacWindowList->Find((long)inWindowRef) )
        wxWinMacWindowList->Append((long)inWindowRef, win);
}

void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win)
{
    wxWinMacWindowList->DeleteObject(win);
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac creation
// ----------------------------------------------------------------------------

WXHWND wxTopLevelWindowMac::s_macWindowInUpdate = NULL;

void wxTopLevelWindowMac::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;
    m_macNoEraseUpdateRgn = NewRgn() ;
    m_macNeedsErasing = false ;
    m_macWindow = NULL ;
#if TARGET_CARBON
    m_macEventHandler = NULL ;
 #endif
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

#if TARGET_CARBON
    if ( m_macEventHandler )
    {
        ::RemoveEventHandler((EventHandlerRef) m_macEventHandler);
        m_macEventHandler = NULL ;
    }
#endif

    wxRemoveMacWindowAssociation( this ) ;

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    DisposeRgn( (RgnHandle) m_macNoEraseUpdateRgn ) ;
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::Maximize(bool maximize)
{
    // not available on mac
}

bool wxTopLevelWindowMac::IsMaximized() const
{
    return false ;
}

void wxTopLevelWindowMac::Iconize(bool iconize)
{
    // not available on mac
}

bool wxTopLevelWindowMac::IsIconized() const
{
    // mac dialogs cannot be iconized
    return FALSE;
}

void wxTopLevelWindowMac::Restore()
{
    // not available on mac
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);
}

void  wxTopLevelWindowMac::MacCreateRealWindow( const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name )
{
    SetName(name);
    m_windowStyle = style;
    m_isShown = FALSE;

    // create frame.

    Rect theBoundsRect;

    m_x = (int)pos.x;
    m_y = (int)pos.y;
    if ( m_y < 50 )
        m_y = 50 ;
    if ( m_x < 20 )
        m_x = 20 ;

    m_width = size.x;
    if (m_width == -1)
        m_width = 20;
    m_height = size.y;
    if (m_height == -1)
        m_height = 20;

    ::SetRect(&theBoundsRect, m_x, m_y , m_x + m_width, m_y + m_height);

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
#if TARGET_CARBON
            wclass = kPlainWindowClass ;
#else
            wclass = kFloatingWindowClass ;
#endif
        }
    }
    else if ( HasFlag( wxCAPTION ) )
    {
        wclass = kDocumentWindowClass ;
    }
    else
    {
        if ( HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) ||
            HasFlag( wxSYSTEM_MENU ) )
        {
            wclass = kDocumentWindowClass ;
        }
        else
        {
#if TARGET_CARBON
            wclass = kPlainWindowClass ;
#else
            wclass = kModalWindowClass ;
#endif
        }
    }

    if ( HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) )
    {
        attr |= kWindowFullZoomAttribute ;
        attr |= kWindowCollapseBoxAttribute ;
    }
    if ( HasFlag( wxRESIZE_BORDER ) )
    {
        attr |= kWindowResizableAttribute ;
    }
    if ( HasFlag( wxSYSTEM_MENU ) )
    {
        attr |= kWindowCloseBoxAttribute ;
    }

#if TARGET_CARBON
#if 0 //  having problems right now with that
    if (HasFlag(wxSTAY_ON_TOP))
    	wclass = kUtilityWindowClass;
#endif
#endif

#if TARGET_CARBON
    if ( HasFlag(wxFRAME_SHAPED) )
    {
        WindowDefSpec customWindowDefSpec;
        customWindowDefSpec.defType = kWindowDefProcPtr;
        customWindowDefSpec.u.defProc = NewWindowDefUPP(wxShapedMacWindowDef);

        ::CreateCustomWindow( &customWindowDefSpec, wclass,
                              attr, &theBoundsRect,
                              (WindowRef*) &m_macWindow);
    }
    else
#endif
    {
        ::CreateNewWindow( wclass , attr , &theBoundsRect , (WindowRef*)&m_macWindow ) ;
    }

    wxAssociateWinWithMacWindow( m_macWindow , this ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title ) ;
    ::CreateRootControl( (WindowRef)m_macWindow , (ControlHandle*)&m_macRootControl ) ;
#if TARGET_CARBON
    InstallStandardEventHandler( GetWindowEventTarget(MAC_WXHWND(m_macWindow)) ) ;
    InstallWindowEventHandler(MAC_WXHWND(m_macWindow), GetwxMacWindowEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this, &((EventHandlerRef)m_macEventHandler));

    // Patch 531199 also defined a window event handler, as follows:
#if 0
    // install a window event handler to send wxEVT_MOVING and wxEVT_SIZING events
    EventTypeSpec events[] = { { kEventClassWindow, kEventWindowBoundsChanging } };
    EventHandlerUPP	handlerProc = NewEventHandlerUPP( WindowHandler );
    EventHandlerRef eventHandlerRef;
    InstallWindowEventHandler( m_macWindowData->m_macWindow, handlerProc, GetEventTypeCount(events),
        events, (void*)this, &eventHandlerRef);
#endif

#endif
    m_macFocus = NULL ;


#if TARGET_CARBON
    if ( HasFlag(wxFRAME_SHAPED) )
    {
        // default shape matches the window size
        wxRegion rgn(0, 0, m_width, m_height);
        SetShape(rgn);
    }
#endif
}

void wxTopLevelWindowMac::MacGetPortParams(WXPOINTPTR localOrigin, WXRECTPTR clipRect, WXHWND *window  , wxWindowMac** rootwin)
{
    ((Point*)localOrigin)->h = 0;
    ((Point*)localOrigin)->v = 0;
    ((Rect*)clipRect)->left = 0;
    ((Rect*)clipRect)->top = 0;
    ((Rect*)clipRect)->right = m_width;
    ((Rect*)clipRect)->bottom = m_height;
    *window = m_macWindow ;
    *rootwin = this ;
}

void wxTopLevelWindowMac::Clear()
{
    wxWindow::Clear() ;
}

WXWidget wxTopLevelWindowMac::MacGetContainerForEmbedding()
{
    return m_macRootControl ;
}


void wxTopLevelWindowMac::MacUpdate( long timestamp)
{
    wxMacPortStateHelper help( (GrafPtr) GetWindowPort( (WindowRef) m_macWindow) ) ;

    RgnHandle       visRgn = NewRgn() ;
    GetPortVisibleRegion( GetWindowPort( (WindowRef)m_macWindow ), visRgn );
    BeginUpdate( (WindowRef)m_macWindow ) ;

    RgnHandle       updateRgn = NewRgn();
    RgnHandle       diffRgn = NewRgn() ;

    if ( updateRgn && diffRgn )
    {
#if 1
        // macos internal control redraws clean up areas we'd like to redraw ourselves
        // therefore we pick the boundary rect and make sure we can redraw it
        // this has to be intersected by the visRgn in order to avoid drawing over its own
        // boundaries
        RgnHandle trueUpdateRgn = NewRgn() ;
        Rect trueUpdateRgnBoundary ;
        GetPortVisibleRegion( GetWindowPort( (WindowRef)m_macWindow ), trueUpdateRgn );
        GetRegionBounds( trueUpdateRgn , &trueUpdateRgnBoundary ) ;
        RectRgn( updateRgn , &trueUpdateRgnBoundary ) ;
        SectRgn( updateRgn , visRgn , updateRgn ) ;
        if ( trueUpdateRgn )
            DisposeRgn( trueUpdateRgn ) ;
        SetPortVisibleRegion(  GetWindowPort( (WindowRef)m_macWindow ), updateRgn ) ;
#else
        GetPortVisibleRegion( GetWindowPort( (WindowRef)m_macWindow ), updateRgn );
#endif
        DiffRgn( updateRgn , (RgnHandle) m_macNoEraseUpdateRgn , diffRgn ) ;
        if ( !EmptyRgn( updateRgn ) )
        {
            MacRedraw( updateRgn , timestamp , m_macNeedsErasing || !EmptyRgn( diffRgn )  ) ;
        }
    }
    if ( updateRgn )
        DisposeRgn( updateRgn );
    if ( diffRgn )
        DisposeRgn( diffRgn );
    if ( visRgn )
        DisposeRgn( visRgn ) ;
        
    EndUpdate( (WindowRef)m_macWindow ) ;
    SetEmptyRgn( (RgnHandle) m_macNoEraseUpdateRgn ) ;
    m_macNeedsErasing = false ;
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

void wxTopLevelWindowMac::MacFireMouseEvent(
    wxUint16 kind , wxInt32 x , wxInt32 y ,wxUint32 modifiers , long timestamp )
{
    wxMouseEvent event(wxEVT_LEFT_DOWN);
    bool isDown = !(modifiers & btnState) ; // 1 is for up
    bool controlDown = modifiers & controlKey ; // for simulating right mouse

    event.m_leftDown = isDown && !controlDown;

    event.m_middleDown = FALSE;
    event.m_rightDown = isDown && controlDown;

    if ( kind == mouseDown )
    {
        if ( controlDown )
            event.SetEventType(wxEVT_RIGHT_DOWN ) ;
        else
            event.SetEventType(wxEVT_LEFT_DOWN ) ;
    }
    else if ( kind == mouseUp )
    {
        if ( controlDown )
            event.SetEventType(wxEVT_RIGHT_UP ) ;
        else
            event.SetEventType(wxEVT_LEFT_UP ) ;
    }
    else
    {
        event.SetEventType(wxEVT_MOTION ) ;
    }

    event.m_shiftDown = modifiers & shiftKey;
    event.m_controlDown = modifiers & controlKey;
    event.m_altDown = modifiers & optionKey;
    event.m_metaDown = modifiers & cmdKey;

    Point       localwhere ;
    localwhere.h = x ;
    localwhere.v = y ;

    GrafPtr     port ;
    ::GetPort( &port ) ;
    ::SetPort( UMAGetWindowPort( (WindowRef)m_macWindow ) ) ;
    ::GlobalToLocal( &localwhere ) ;
    ::SetPort( port ) ;

    if ( kind == mouseDown )
    {
        if ( timestamp - gs_lastWhen <= GetDblTime() )
        {
            if ( abs( localwhere.h - gs_lastWhere.h ) < 3 && abs( localwhere.v - gs_lastWhere.v ) < 3 )
            {
                // This is not right if the second mouse down
                // event occured in a differen window. We
                // correct this in MacDispatchMouseEvent.
                if ( controlDown )
                    event.SetEventType(wxEVT_RIGHT_DCLICK ) ;
                else
                    event.SetEventType(wxEVT_LEFT_DCLICK ) ;
            }
            gs_lastWhen = 0 ;
        }
        else
        {
            gs_lastWhen = timestamp ;
        }
        gs_lastWhere = localwhere ;
    }

    event.m_x = localwhere.h;
    event.m_y = localwhere.v;
    event.m_x += m_x;
    event.m_y += m_y;

    event.m_timeStamp = timestamp;
    event.SetEventObject(this);
    if ( wxTheApp->s_captureWindow )
    {
        int x = event.m_x ;
        int y = event.m_y ;
        wxTheApp->s_captureWindow->ScreenToClient( &x , &y ) ;
        event.m_x = x ;
        event.m_y = y ;
        event.SetEventObject( wxTheApp->s_captureWindow ) ;
        wxTheApp->s_captureWindow->GetEventHandler()->ProcessEvent( event ) ;

        if ( kind == mouseUp )
        {
            wxTheApp->s_captureWindow = NULL ;
            if ( !wxIsBusy() )
            {
                m_cursor.MacInstall() ;
            }
        }
    }
    else
    {
        MacDispatchMouseEvent( event ) ;
    }
}

#if !TARGET_CARBON

void wxTopLevelWindowMac::MacMouseDown( WXEVENTREF ev , short part)
{
    MacFireMouseEvent( mouseDown , ((EventRecord*)ev)->where.h , ((EventRecord*)ev)->where.v ,
        ((EventRecord*)ev)->modifiers , ((EventRecord*)ev)->when ) ;
}

void wxTopLevelWindowMac::MacMouseUp( WXEVENTREF ev , short part)
{
    switch (part)
    {
        case inContent:
            {
                MacFireMouseEvent( mouseUp , ((EventRecord*)ev)->where.h , ((EventRecord*)ev)->where.v ,
                    ((EventRecord*)ev)->modifiers , ((EventRecord*)ev)->when ) ;
            }
            break ;
    }
}

void wxTopLevelWindowMac::MacMouseMoved( WXEVENTREF ev , short part)
{
    switch (part)
    {
        case inContent:
            {
                MacFireMouseEvent( nullEvent /*moved*/ , ((EventRecord*)ev)->where.h , ((EventRecord*)ev)->where.v ,
                    ((EventRecord*)ev)->modifiers , ((EventRecord*)ev)->when ) ;
            }
            break ;
    }
}

#endif

void wxTopLevelWindowMac::MacActivate( long timestamp , bool inIsActivating )
{
    wxActivateEvent event(wxEVT_ACTIVATE, inIsActivating , m_windowId);
    event.m_timeStamp = timestamp ;
    event.SetEventObject(this);

    GetEventHandler()->ProcessEvent(event);

    UMAHighlightAndActivateWindow( (WindowRef)m_macWindow , inIsActivating ) ;

    // Early versions of MacOS X don't refresh backgrounds properly,
    // so refresh the whole window on activation and deactivation.
    long osVersion = UMAGetSystemVersion();
    if (osVersion >= 0x1000 && osVersion < 0x1020 )
    {
        Refresh(TRUE);
    }
    else
    {
        // for the moment we have to resolve some redrawing issues like this
        // the OS is stealing some redrawing areas as soon as it draws a control
        Refresh(TRUE);
    }
}

#if !TARGET_CARBON

void wxTopLevelWindowMac::MacKeyDown( WXEVENTREF ev )
{
}

#endif

void wxTopLevelWindowMac::SetTitle(const wxString& title)
{
    wxWindow::SetTitle( title ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title ) ;
}

bool wxTopLevelWindowMac::Show(bool show)
{
    if ( !wxWindow::Show(show) )
        return FALSE;

    if (show)
    {
      ::TransitionWindow((WindowRef)m_macWindow,kWindowZoomTransitionEffect,kWindowShowTransitionAction,nil);
      ::SelectWindow( (WindowRef)m_macWindow ) ;
      // no need to generate events here, they will get them triggered by macos
      // actually they should be , but apparently they are not
      wxSize size(m_width, m_height);
      wxSizeEvent event(size, m_windowId);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
    else
    {
      ::TransitionWindow((WindowRef)m_macWindow,kWindowZoomTransitionEffect,kWindowHideTransitionAction,nil);
    }

    if ( !show )
    {
    }
    else
    {
        Refresh() ;
    }

    return TRUE;
}

void wxTopLevelWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    int former_x = m_x ;
    int former_y = m_y ;
    int former_w = m_width ;
    int former_h = m_height ;

    int actualWidth = width;
    int actualHeight = height;
    int actualX = x;
    int actualY = y;

    if ((m_minWidth != -1) && (actualWidth < m_minWidth))
        actualWidth = m_minWidth;
    if ((m_minHeight != -1) && (actualHeight < m_minHeight))
        actualHeight = m_minHeight;
    if ((m_maxWidth != -1) && (actualWidth > m_maxWidth))
        actualWidth = m_maxWidth;
    if ((m_maxHeight != -1) && (actualHeight > m_maxHeight))
        actualHeight = m_maxHeight;

    bool doMove = false ;
    bool doResize = false ;

    if ( actualX != former_x || actualY != former_y )
    {
        doMove = true ;
    }
    if ( actualWidth != former_w || actualHeight != former_h )
    {
        doResize = true ;
    }

    if ( doMove || doResize )
    {
        m_x = actualX ;
        m_y = actualY ;
        m_width = actualWidth ;
        m_height = actualHeight ;

        if ( doMove )
            ::MoveWindow((WindowRef)m_macWindow, m_x, m_y  , false); // don't make frontmost

        if ( doResize )
            ::SizeWindow((WindowRef)m_macWindow, m_width, m_height  , true);

        // the OS takes care of invalidating and erasing the new area so we only have to
        // take care of refreshing for full repaints

        if ( doResize && !HasFlag(wxNO_FULL_REPAINT_ON_RESIZE) )
            Refresh() ;


        if ( IsKindOf( CLASSINFO( wxFrame ) ) )
        {
            wxFrame* frame = (wxFrame*) this ;
            frame->PositionStatusBar();
            frame->PositionToolBar();
        }
        if ( doMove )
            wxWindowMac::MacTopLevelWindowChangedPosition() ; // like this only children will be notified

        MacRepositionScrollBars() ;
        if ( doMove )
        {
            wxPoint point(m_x, m_y);
            wxMoveEvent event(point, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event) ;
        }
        if ( doResize )
        {
             MacRepositionScrollBars() ;
             wxSize size(m_width, m_height);
             wxSizeEvent event(size, m_windowId);
             event.SetEventObject(this);
             GetEventHandler()->ProcessEvent(event);
        }
    }

}

/*
 * Invalidation Mechanism
 *
 * The update mechanism reflects exactely the windows mechanism
 * the rect gets added to the window invalidate region, if the eraseBackground flag
 * has been true for any part of the update rgn the background is erased in the entire region
 * not just in the specified rect.
 *
 * In order to achive this, we also have an internal m_macNoEraseUpdateRgn, all rects that have
 * the eraseBackground flag set to false are also added to this rgn. upon receiving an update event
 * the update rgn is compared to the m_macNoEraseUpdateRgn and in case they differ, every window
 * will get the eraseBackground event first
 */

void wxTopLevelWindowMac::MacInvalidate( const WXRECTPTR rect, bool eraseBackground )
{
    GrafPtr formerPort ;
    GetPort( &formerPort ) ;
    SetPortWindowPort( (WindowRef)m_macWindow ) ;

    m_macNeedsErasing |= eraseBackground ;

    // if we already know that we will have to erase, there's no need to track the rest
    if ( !m_macNeedsErasing)
    {
        // we end only here if eraseBackground is false
        // if we already have a difference between m_macNoEraseUpdateRgn and UpdateRgn
        // we will have to erase anyway

        RgnHandle       updateRgn = NewRgn();
        RgnHandle       diffRgn = NewRgn() ;
        if ( updateRgn && diffRgn )
        {
            GetWindowUpdateRgn( (WindowRef)m_macWindow , updateRgn );
            Point pt = {0,0} ;
            LocalToGlobal( &pt ) ;
            OffsetRgn( updateRgn , -pt.h , -pt.v ) ;
            DiffRgn( updateRgn , (RgnHandle) m_macNoEraseUpdateRgn , diffRgn ) ;
            if ( !EmptyRgn( diffRgn ) )
            {
                m_macNeedsErasing = true ;
            }
        }
        if ( updateRgn )
            DisposeRgn( updateRgn );
        if ( diffRgn )
            DisposeRgn( diffRgn );

        if ( !m_macNeedsErasing )
        {
            RgnHandle rectRgn = NewRgn() ;
            SetRectRgn( rectRgn , ((Rect*)rect)->left , ((Rect*)rect)->top , ((Rect*)rect)->right , ((Rect*)rect)->bottom ) ;
            UnionRgn( (RgnHandle) m_macNoEraseUpdateRgn , rectRgn , (RgnHandle) m_macNoEraseUpdateRgn ) ;
            DisposeRgn( rectRgn ) ;
        }
    }
    InvalWindowRect( (WindowRef)m_macWindow , (Rect*)rect ) ;
    // turn this on to debug the refreshing cycle
#if wxMAC_DEBUG_REDRAW
    PaintRect( rect ) ;
#endif
    SetPort( formerPort ) ;
}


bool wxTopLevelWindowMac::SetShape(const wxRegion& region)
{
    wxCHECK_MSG( HasFlag(wxFRAME_SHAPED), FALSE,
                 _T("Shaped windows must be created with the wxFRAME_SHAPED style."));

#if TARGET_CARBON
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
#else
    return FALSE;
#endif
}

// ---------------------------------------------------------------------------
// Support functions for shaped windows, based on Apple's CustomWindow sample at
// http://developer.apple.com/samplecode/Sample_Code/Human_Interface_Toolbox/Mac_OS_High_Level_Toolbox/CustomWindow.htm
// ---------------------------------------------------------------------------

#if TARGET_CARBON

static void wxShapedMacWindowGetPos(WindowRef window, Rect* inRect)
{
    GetWindowPortBounds(window, inRect);
    Point pt = {inRect->left, inRect->top};
    SetPort((GrafPtr) GetWindowPort(window));
    LocalToGlobal(&pt);
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
                        kWindowSupportsSetGrowImageRegion|
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
        wxRect r = win->GetRect();
        SetRectRgn(rgn, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
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

#endif
// ---------------------------------------------------------------------------

