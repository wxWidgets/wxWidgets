/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/window.cpp
// Purpose:     wxWindowMac
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/window.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/panel.h"
    #include "wx/frame.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/button.h"
    #include "wx/menu.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/scrolbar.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
    #include "wx/toolbar.h"
    #include "wx/layout.h"
    #include "wx/statusbr.h"
    #include "wx/menuitem.h"
#endif

#include "wx/tooltip.h"
#include "wx/spinctrl.h"
#include "wx/geometry.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif

#if wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include "wx/mac/uma.h"

#define MAC_SCROLLBAR_SIZE 15
#define MAC_SMALL_SCROLLBAR_SIZE 11

#ifndef __DARWIN__
#include <Windows.h>
#include <ToolUtils.h>
#include <Scrap.h>
#include <MacTextEditor.h>
#endif

#if TARGET_API_MAC_OSX
#ifndef __HIVIEW__
    #include <HIToolbox/HIView.h>
#endif
#endif

#include <string.h>

#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowMac, wxWindowBase)
#else
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif

BEGIN_EVENT_TABLE(wxWindowMac, wxWindowBase)
    EVT_NC_PAINT(wxWindowMac::OnNcPaint)
    EVT_ERASE_BACKGROUND(wxWindowMac::OnEraseBackground)
#if TARGET_API_MAC_OSX
    EVT_PAINT(wxWindowMac::OnPaint)
#endif
    EVT_SET_FOCUS(wxWindowMac::OnSetFocus)
    EVT_KILL_FOCUS(wxWindowMac::OnSetFocus)
    EVT_MOUSE_EVENTS(wxWindowMac::OnMouseEvent)
END_EVENT_TABLE()

#define wxMAC_DEBUG_REDRAW 0
#ifndef wxMAC_DEBUG_REDRAW
#define wxMAC_DEBUG_REDRAW 0
#endif

// ---------------------------------------------------------------------------
// Utility Routines to move between different coordinate systems
// ---------------------------------------------------------------------------

/*
 * Right now we have the following setup :
 * a border that is not part of the native control is always outside the
 * control's border (otherwise we loose all native intelligence, future ways
 * may be to have a second embedding control responsible for drawing borders
 * and backgrounds eventually)
 * so all this border calculations have to be taken into account when calling
 * native methods or getting native oriented data
 * so we have three coordinate systems here
 * wx client coordinates
 * wx window coordinates (including window frames)
 * native coordinates
 */

//
// originating from native control
//


void wxMacNativeToWindow( const wxWindow* window , RgnHandle handle )
{
    OffsetRgn( handle , window->MacGetLeftBorderSize() , window->MacGetTopBorderSize() ) ;
}

void wxMacNativeToWindow( const wxWindow* window , Rect *rect )
{
    OffsetRect( rect , window->MacGetLeftBorderSize() , window->MacGetTopBorderSize() ) ;
}

//
// directed towards native control
//

void wxMacWindowToNative( const wxWindow* window , RgnHandle handle )
{
    OffsetRgn( handle , -window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize() );
}

void wxMacWindowToNative( const wxWindow* window , Rect *rect )
{
    OffsetRect( rect , -window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize() ) ;
}

// ---------------------------------------------------------------------------
// Carbon Events
// ---------------------------------------------------------------------------

#if TARGET_API_MAC_OSX

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_3
enum
{
    kEventControlVisibilityChanged = 157
};
#endif

#endif

static const EventTypeSpec eventList[] =
{
    { kEventClassCommand, kEventProcessCommand } ,
    { kEventClassCommand, kEventCommandUpdateStatus } ,

    { kEventClassControl , kEventControlHit } ,

    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,
    { kEventClassTextInput, kEventTextInputUpdateActiveInputArea } ,

    { kEventClassControl , kEventControlDraw } ,
#if TARGET_API_MAC_OSX
    { kEventClassControl , kEventControlVisibilityChanged } ,
    { kEventClassControl , kEventControlEnabledStateChanged } ,
    { kEventClassControl , kEventControlHiliteChanged } ,
#endif
    { kEventClassControl , kEventControlSetFocusPart } ,

    { kEventClassService , kEventServiceGetTypes },
    { kEventClassService , kEventServiceCopy },
    { kEventClassService , kEventServicePaste },

//    { kEventClassControl , kEventControlInvalidateForSizeChange } , // 10.3 only
//    { kEventClassControl , kEventControlBoundsChanged } ,
} ;

static pascal OSStatus wxMacWindowControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    wxWindowMac* thisWindow = (wxWindowMac*) data ;

    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    switch ( GetEventKind( event ) )
    {
#if TARGET_API_MAC_OSX
        case kEventControlDraw :
            {
                RgnHandle updateRgn = NULL ;
                RgnHandle allocatedRgn = NULL ;
                wxRegion visRegion = thisWindow->MacGetVisibleRegion() ;

                if ( cEvent.GetParameter<RgnHandle>(kEventParamRgnHandle, &updateRgn) != noErr )
                {
                    updateRgn = (RgnHandle) visRegion.GetWXHRGN() ;
                }
                else
                {
                    if ( thisWindow->MacGetLeftBorderSize() != 0 || thisWindow->MacGetTopBorderSize() != 0 )
                    {
                        // as this update region is in native window locals we must adapt it to wx window local
                        allocatedRgn = NewRgn() ;
                        CopyRgn( updateRgn , allocatedRgn ) ;

                        // hide the given region by the new region that must be shifted
                        wxMacNativeToWindow( thisWindow , allocatedRgn ) ;
                        updateRgn = allocatedRgn ;
                    }
                }

                Rect rgnBounds ;
                GetRegionBounds( updateRgn , &rgnBounds ) ;

#if wxMAC_DEBUG_REDRAW
                if ( thisWindow->MacIsUserPane() )
                {
                    static float color = 0.5 ;
                    static channel = 0 ;
                    HIRect bounds;
                    CGContextRef cgContext = cEvent.GetParameter<CGContextRef>(kEventParamCGContextRef) ;

                    HIViewGetBounds( controlRef, &bounds );
                    CGContextSetRGBFillColor( cgContext, channel == 0 ? color : 0.5 ,
                        channel == 1 ? color : 0.5 , channel == 2 ? color : 0.5 , 1 );
                    CGContextFillRect( cgContext, bounds );
                    color += 0.1 ;
                    if ( color > 0.9 )
                    {
                        color = 0.5 ;
                        channel++ ;
                        if ( channel == 3 )
                            channel = 0 ;
                    }
                }
#endif

                {
#if wxMAC_USE_CORE_GRAPHICS
                    bool created = false ;
                    CGContextRef cgContext = NULL ;
                    OSStatus err = cEvent.GetParameter<CGContextRef>(kEventParamCGContextRef, &cgContext) ;
                    wxASSERT_MSG( err == noErr , wxT("Unable to retrieve CGContextRef") ) ;
                    thisWindow->MacSetCGContextRef( cgContext ) ;

                    {
                        wxMacCGContextStateSaver sg( cgContext ) ;
                        // make sure the context is having its origin at the wx-window coordinates of the 
                        // view (read at the top of this file about the differences)
						if ( thisWindow->MacGetLeftBorderSize() != 0 || thisWindow->MacGetTopBorderSize() != 0 )
							CGContextTranslateCTM( cgContext , -thisWindow->MacGetLeftBorderSize() , -thisWindow->MacGetTopBorderSize() );
                        float alpha = 1.0 ;
                        {
                            wxWindow* iter = thisWindow ;
                            while ( iter ) 
                            {
                                alpha *= (float) iter->GetTransparent()/255.0 ;
                                if ( iter->IsTopLevel() )
                                    iter = NULL ;
                                else
                                    iter = iter->GetParent() ;
                            }
                        }
                        CGContextSetAlpha( cgContext , alpha ) ;
#endif
                        if ( thisWindow->MacDoRedraw( updateRgn , cEvent.GetTicks() ) )
                            result = noErr ;

#if wxMAC_USE_CORE_GRAPHICS
                        thisWindow->MacSetCGContextRef( NULL ) ;
                    }

                    if ( created )
                        CGContextRelease( cgContext ) ;
#endif
                }

                if ( allocatedRgn )
                    DisposeRgn( allocatedRgn ) ;
            }
            break ;

        case kEventControlVisibilityChanged :
            thisWindow->MacVisibilityChanged() ;
            break ;

        case kEventControlEnabledStateChanged :
            thisWindow->MacEnabledStateChanged() ;
            break ;

        case kEventControlHiliteChanged :
            thisWindow->MacHiliteChanged() ;
            break ;
#endif

        // we emulate this event under Carbon CFM
        case kEventControlSetFocusPart :
            {
                Boolean focusEverything = false ;
                ControlPartCode controlPart = cEvent.GetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode );

#ifdef __WXMAC_OSX__
                if ( cEvent.GetParameter<Boolean>(kEventParamControlFocusEverything , &focusEverything ) == noErr )
                {
                }
#endif

                if ( controlPart == kControlFocusNoPart )
                {
#if wxUSE_CARET
                    if ( thisWindow->GetCaret() )
                        thisWindow->GetCaret()->OnKillFocus();
#endif

                    static bool inKillFocusEvent = false ;

                    if ( !inKillFocusEvent )
                    {
                        inKillFocusEvent = true ;
                        wxFocusEvent event( wxEVT_KILL_FOCUS, thisWindow->GetId());
                        event.SetEventObject(thisWindow);
                        thisWindow->GetEventHandler()->ProcessEvent(event) ;
                        inKillFocusEvent = false ;
                    }
                }
                else
                {
                    // panel wants to track the window which was the last to have focus in it
                    wxChildFocusEvent eventFocus(thisWindow);
                    thisWindow->GetEventHandler()->ProcessEvent(eventFocus);

#if wxUSE_CARET
                    if ( thisWindow->GetCaret() )
                        thisWindow->GetCaret()->OnSetFocus();
#endif

                    wxFocusEvent event(wxEVT_SET_FOCUS, thisWindow->GetId());
                    event.SetEventObject(thisWindow);
                    thisWindow->GetEventHandler()->ProcessEvent(event) ;
                }

                if ( thisWindow->MacIsUserPane() )
                    result = noErr ;
            }
            break ;

        case kEventControlHit :
            result = thisWindow->MacControlHit( handler , event ) ;
            break ;

        default :
            break ;
    }

    return result ;
}

static pascal OSStatus wxMacWindowServiceEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    wxWindowMac* thisWindow = (wxWindowMac*) data ;
    wxTextCtrl* textCtrl = wxDynamicCast( thisWindow , wxTextCtrl ) ;
    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    switch ( GetEventKind( event ) )
    {
        case kEventServiceGetTypes :
            if ( textCtrl )
            {
                long from, to ;
                textCtrl->GetSelection( &from , &to ) ;

                CFMutableArrayRef copyTypes = 0 , pasteTypes = 0;
                if ( from != to )
                    copyTypes = cEvent.GetParameter< CFMutableArrayRef >( kEventParamServiceCopyTypes , typeCFMutableArrayRef ) ;
                if ( textCtrl->IsEditable() )
                    pasteTypes = cEvent.GetParameter< CFMutableArrayRef >( kEventParamServicePasteTypes , typeCFMutableArrayRef ) ;

                static const OSType textDataTypes[] = { kTXNTextData /* , 'utxt', 'PICT', 'MooV', 'AIFF' */  };
                for ( size_t i = 0 ; i < WXSIZEOF(textDataTypes) ; ++i )
                {
                    CFStringRef typestring = CreateTypeStringWithOSType(textDataTypes[i]);
                    if ( typestring )
                    {
                        if ( copyTypes )
                            CFArrayAppendValue(copyTypes, typestring) ;
                        if ( pasteTypes )
                            CFArrayAppendValue(pasteTypes, typestring) ;

                        CFRelease( typestring ) ;
                    }
                }

                result = noErr ;
            }
            break ;

        case kEventServiceCopy :
            if ( textCtrl )
            {
                long from, to ;

                textCtrl->GetSelection( &from , &to ) ;
                wxString val = textCtrl->GetValue() ;
                val = val.Mid( from , to - from ) ;
                ScrapRef scrapRef = cEvent.GetParameter< ScrapRef > ( kEventParamScrapRef , typeScrapRef ) ;
                verify_noerr( ClearScrap( &scrapRef ) ) ;
                verify_noerr( PutScrapFlavor( scrapRef , kTXNTextData , 0 , val.length() , val.c_str() ) ) ;
                result = noErr ;
            }
            break ;

        case kEventServicePaste :
            if ( textCtrl )
            {
                ScrapRef scrapRef = cEvent.GetParameter< ScrapRef > ( kEventParamScrapRef , typeScrapRef ) ;
                Size textSize, pastedSize ;
                verify_noerr( GetScrapFlavorSize(scrapRef, kTXNTextData, &textSize) ) ;
                textSize++ ;
                char *content = new char[textSize] ;
                GetScrapFlavorData(scrapRef, kTXNTextData, &pastedSize, content );
                content[textSize - 1] = 0 ;

#if wxUSE_UNICODE
                textCtrl->WriteText( wxString( content , wxConvLocal ) );
#else
                textCtrl->WriteText( wxString( content ) ) ;
#endif

                delete[] content ;
                result = noErr ;
            }
            break ;

        default:
            break ;
    }

    return result ;
}

pascal OSStatus wxMacUnicodeTextEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    wxWindowMac* focus = (wxWindowMac*) data ;

    wchar_t* uniChars = NULL ;
    UInt32 when = EventTimeToTicks( GetEventTime( event ) ) ;

    UniChar* charBuf = NULL;
    ByteCount dataSize = 0 ;
    int numChars = 0 ;
    UniChar buf[2] ;
    if ( GetEventParameter( event, kEventParamTextInputSendText, typeUnicodeText, NULL, 0 , &dataSize, NULL ) == noErr )
    {
        numChars = dataSize / sizeof( UniChar) + 1;
        charBuf = buf ;

        if ( (size_t) numChars * 2 > sizeof(buf) )
            charBuf = new UniChar[ numChars ] ;
        else
            charBuf = buf ;

        uniChars = new wchar_t[ numChars ] ;
        GetEventParameter( event, kEventParamTextInputSendText, typeUnicodeText, NULL, dataSize , NULL , charBuf ) ;
        charBuf[ numChars - 1 ] = 0;
#if SIZEOF_WCHAR_T == 2
        uniChars = (wchar_t*) charBuf ;
        memcpy( uniChars , charBuf , numChars * 2 ) ;
#else
        // the resulting string will never have more chars than the utf16 version, so this is safe
        wxMBConvUTF16 converter ;
        numChars = converter.MB2WC( uniChars , (const char*)charBuf , numChars ) ;
#endif
    }

    switch ( GetEventKind( event ) )
    {
        case kEventTextInputUpdateActiveInputArea :
            {
                // An IME input event may return several characters, but we need to send one char at a time to
                // EVT_CHAR
                for (int pos=0 ; pos < numChars ; pos++)
                {
                    WXEVENTREF formerEvent = wxTheApp->MacGetCurrentEvent() ;
                    WXEVENTHANDLERCALLREF formerHandler = wxTheApp->MacGetCurrentEventHandlerCallRef() ;
                    wxTheApp->MacSetCurrentEvent( event , handler ) ;

                    UInt32 message = (0  << 8) + ((char)uniChars[pos] );
                    if ( wxTheApp->MacSendCharEvent(
                                                    focus , message , 0 , when , 0 , 0 , uniChars[pos] ) )
                    {
                        result = noErr ;
                    }

                    wxTheApp->MacSetCurrentEvent( formerEvent , formerHandler ) ;
                }
            }
            break ;
        case kEventTextInputUnicodeForKeyEvent :
            {
                UInt32 keyCode, modifiers ;
                Point point ;
                EventRef rawEvent ;
                unsigned char charCode ;

                GetEventParameter( event, kEventParamTextInputSendKeyboardEvent, typeEventRef, NULL, sizeof(rawEvent), NULL, &rawEvent ) ;
                GetEventParameter( rawEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &charCode );
                GetEventParameter( rawEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &keyCode );
                GetEventParameter( rawEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers );
                GetEventParameter( rawEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &point );

                UInt32 message = (keyCode << 8) + charCode;

                // An IME input event may return several characters, but we need to send one char at a time to
                // EVT_CHAR
                for (int pos=0 ; pos < numChars ; pos++)
                {
                    WXEVENTREF formerEvent = wxTheApp->MacGetCurrentEvent() ;
                    WXEVENTHANDLERCALLREF formerHandler = wxTheApp->MacGetCurrentEventHandlerCallRef() ;
                    wxTheApp->MacSetCurrentEvent( event , handler ) ;

                    if ( wxTheApp->MacSendCharEvent(
                        focus , message , modifiers , when , point.h , point.v , uniChars[pos] ) )
                    {
                        result = noErr ;
                    }

                    wxTheApp->MacSetCurrentEvent( formerEvent , formerHandler ) ;
                }
            }
            break;
        default:
            break ;
    }

    delete [] uniChars ;
    if ( charBuf != buf )
        delete [] charBuf ;

    return result ;
}

static pascal OSStatus wxMacWindowCommandEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    wxWindowMac* focus = (wxWindowMac*) data ;

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
                    else
                    {
                        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED , id);
                        event.SetEventObject(focus);
                        event.SetInt(item->IsCheckable() ? item->IsChecked() : -1);

                        if ( focus->GetEventHandler()->ProcessEvent(event) )
                            result = noErr ;
                    }
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

                    if ( !processed )
                    {
                        processed = focus->GetEventHandler()->ProcessEvent(event);
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

pascal OSStatus wxMacWindowEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    EventRef formerEvent = (EventRef) wxTheApp->MacGetCurrentEvent() ;
    EventHandlerCallRef formerEventHandlerCallRef = (EventHandlerCallRef) wxTheApp->MacGetCurrentEventHandlerCallRef() ;
    wxTheApp->MacSetCurrentEvent( event , handler ) ;
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassCommand :
            result = wxMacWindowCommandEventHandler( handler , event , data ) ;
            break ;

        case kEventClassControl :
            result = wxMacWindowControlEventHandler( handler, event, data ) ;
            break ;

        case kEventClassService :
            result = wxMacWindowServiceEventHandler( handler, event , data ) ;
            break ;

        case kEventClassTextInput :
            result = wxMacUnicodeTextEventHandler( handler , event , data ) ;
            break ;

        default :
            break ;
    }

    wxTheApp->MacSetCurrentEvent( formerEvent, formerEventHandlerCallRef ) ;

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacWindowEventHandler )

#if !TARGET_API_MAC_OSX

// ---------------------------------------------------------------------------
// UserPane events for non OSX builds
// ---------------------------------------------------------------------------

static pascal void wxMacControlUserPaneDrawProc(ControlRef control, SInt16 part)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        win->MacControlUserPaneDrawProc(part) ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneDrawUPP , wxMacControlUserPaneDrawProc ) ;

static pascal ControlPartCode wxMacControlUserPaneHitTestProc(ControlRef control, Point where)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        return win->MacControlUserPaneHitTestProc(where.h , where.v) ;
    else
        return kControlNoPart ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneHitTestUPP , wxMacControlUserPaneHitTestProc ) ;

static pascal ControlPartCode wxMacControlUserPaneTrackingProc(ControlRef control, Point startPt, ControlActionUPP actionProc)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        return win->MacControlUserPaneTrackingProc( startPt.h , startPt.v , (void*) actionProc) ;
    else
        return kControlNoPart ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneTrackingUPP , wxMacControlUserPaneTrackingProc ) ;

static pascal void wxMacControlUserPaneIdleProc(ControlRef control)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        win->MacControlUserPaneIdleProc() ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneIdleUPP , wxMacControlUserPaneIdleProc ) ;

static pascal ControlPartCode wxMacControlUserPaneKeyDownProc(ControlRef control, SInt16 keyCode, SInt16 charCode, SInt16 modifiers)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        return win->MacControlUserPaneKeyDownProc(keyCode,charCode,modifiers) ;
    else
        return kControlNoPart ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneKeyDownUPP , wxMacControlUserPaneKeyDownProc ) ;

static pascal void wxMacControlUserPaneActivateProc(ControlRef control, Boolean activating)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        win->MacControlUserPaneActivateProc(activating) ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneActivateUPP , wxMacControlUserPaneActivateProc ) ;

static pascal ControlPartCode wxMacControlUserPaneFocusProc(ControlRef control, ControlFocusPart action)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        return win->MacControlUserPaneFocusProc(action) ;
    else
        return kControlNoPart ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneFocusUPP , wxMacControlUserPaneFocusProc ) ;

static pascal void wxMacControlUserPaneBackgroundProc(ControlRef control, ControlBackgroundPtr info)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    if ( win )
        win->MacControlUserPaneBackgroundProc(info) ;
}
wxMAC_DEFINE_PROC_GETTER( ControlUserPaneBackgroundUPP , wxMacControlUserPaneBackgroundProc ) ;

void wxWindowMac::MacControlUserPaneDrawProc(wxInt16 part)
{
    int x = 0 , y = 0;
    RgnHandle rgn = NewRgn() ;
    GetClip( rgn ) ;
    MacWindowToRootWindow( &x, &y ) ;
    OffsetRgn( rgn , -x , -y ) ;
    wxMacWindowStateSaver sv( this ) ;
    SectRgn( rgn , (RgnHandle) MacGetVisibleRegion().GetWXHRGN() , rgn ) ;
    MacDoRedraw( rgn , 0 ) ;
    DisposeRgn( rgn ) ;
}

wxInt16 wxWindowMac::MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y)
{
    return kControlNoPart ;
}

wxInt16 wxWindowMac::MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc)
{
    return kControlNoPart ;
}

void wxWindowMac::MacControlUserPaneIdleProc()
{
}

wxInt16 wxWindowMac::MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers)
{
    return kControlNoPart ;
}

void wxWindowMac::MacControlUserPaneActivateProc(bool activating)
{
}

wxInt16 wxWindowMac::MacControlUserPaneFocusProc(wxInt16 action)
{
    if ( AcceptsFocus() )
        return 1 ;
    else
        return kControlNoPart ;
}

void wxWindowMac::MacControlUserPaneBackgroundProc(void* info)
{
}

#endif

// ---------------------------------------------------------------------------
// Scrollbar Tracking for all
// ---------------------------------------------------------------------------

pascal void wxMacLiveScrollbarActionProc( ControlRef control , ControlPartCode partCode ) ;
pascal void wxMacLiveScrollbarActionProc( ControlRef control , ControlPartCode partCode )
{
    if ( partCode != 0)
    {
        wxWindow*  wx = wxFindControlFromMacControl( control ) ;
        if ( wx )
            wx->MacHandleControlClick( (WXWidget) control , partCode , true /* stillDown */ ) ;
    }
}
wxMAC_DEFINE_PROC_GETTER( ControlActionUPP , wxMacLiveScrollbarActionProc ) ;

// ===========================================================================
// implementation
// ===========================================================================

WX_DECLARE_HASH_MAP(ControlRef, wxWindow*, wxPointerHash, wxPointerEqual, MacControlMap);

static MacControlMap wxWinMacControlList;

wxWindow *wxFindControlFromMacControl(ControlRef inControl )
{
    MacControlMap::iterator node = wxWinMacControlList.find(inControl);

    return (node == wxWinMacControlList.end()) ? NULL : node->second;
}

void wxAssociateControlWithMacControl(ControlRef inControl, wxWindow *control)
{
    // adding NULL ControlRef is (first) surely a result of an error and
    // (secondly) breaks native event processing
    wxCHECK_RET( inControl != (ControlRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    wxWinMacControlList[inControl] = control;
}

void wxRemoveMacControlAssociation(wxWindow *control)
{
   // iterate over all the elements in the class
    // is the iterator stable ? as we might have two associations pointing to the same wxWindow
    // we should go on...

    bool found = true ;
    while ( found )
    {
        found = false ;
        MacControlMap::iterator it;
        for ( it = wxWinMacControlList.begin(); it != wxWinMacControlList.end(); ++it )
        {
            if ( it->second == control )
            {
                wxWinMacControlList.erase(it);
                found = true ;
                break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
 // constructors and such
// ----------------------------------------------------------------------------

wxWindowMac::wxWindowMac()
{
    Init();
}

wxWindowMac::wxWindowMac(wxWindowMac *parent,
            wxWindowID id,
            const wxPoint& pos ,
            const wxSize& size ,
            long style ,
            const wxString& name )
{
    Init();
    Create(parent, id, pos, size, style, name);
}

void wxWindowMac::Init()
{
    m_peer = NULL ;
    m_frozenness = 0 ;
    m_macAlpha = 255 ;

#if WXWIN_COMPATIBILITY_2_4
    m_backgroundTransparent = false;
#endif

#if wxMAC_USE_CORE_GRAPHICS
    m_cgContextRef = NULL ;
#endif

    // as all windows are created with WS_VISIBLE style...
    m_isShown = true;

    m_hScrollBar = NULL ;
    m_vScrollBar = NULL ;
    m_macBackgroundBrush = wxNullBrush ;

    m_macIsUserPane = true;
    m_clipChildren = false ;
    m_cachedClippedRectValid = false ;

    // we need a valid font for the encodings
    wxWindowBase::SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
}

wxWindowMac::~wxWindowMac()
{
    SendDestroyEvent();

    m_isBeingDeleted = true;

    MacInvalidateBorders() ;

#ifndef __WXUNIVERSAL__
    // VS: make sure there's no wxFrame with last focus set to us:
    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        wxFrame *frame = wxDynamicCast(win, wxFrame);
        if ( frame )
        {
            if ( frame->GetLastFocus() == this )
                frame->SetLastFocus((wxWindow*)NULL);
            break;
        }
    }
#endif

    // destroy children before destroying this window itself
    DestroyChildren();

    // wxRemoveMacControlAssociation( this ) ;
    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
    if ( tlw )
    {
        if ( tlw->GetDefaultItem() == (wxButton*) this)
            tlw->SetDefaultItem(NULL);
    }

    if ( m_peer && m_peer->Ok() )
    {
        // in case the callback might be called during destruction
        wxRemoveMacControlAssociation( this) ;
        ::RemoveEventHandler( (EventHandlerRef ) m_macControlEventHandler ) ;
        // we currently are not using this hook
        // ::SetControlColorProc( *m_peer , NULL ) ;
        m_peer->Dispose() ;
    }

    if ( g_MacLastWindow == this )
        g_MacLastWindow = NULL ;

    wxFrame* frame = wxDynamicCast( wxGetTopLevelParent( this ) , wxFrame ) ;
    if ( frame )
    {
        if ( frame->GetLastFocus() == this )
            frame->SetLastFocus( NULL ) ;
    }

    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
        delete m_dropTarget;
        m_dropTarget = NULL;
    }
#endif

    delete m_peer ;
}

WXWidget wxWindowMac::GetHandle() const
{
    return (WXWidget) m_peer->GetControlRef() ;
}

void wxWindowMac::MacInstallEventHandler( WXWidget control )
{
    wxAssociateControlWithMacControl( (ControlRef) control , this ) ;
    InstallControlEventHandler( (ControlRef)control , GetwxMacWindowEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this,
        (EventHandlerRef *)&m_macControlEventHandler);

#if !TARGET_API_MAC_OSX
    if ( (ControlRef) control == m_peer->GetControlRef() )
    {
        m_peer->SetData<ControlUserPaneDrawUPP>(kControlEntireControl, kControlUserPaneDrawProcTag, GetwxMacControlUserPaneDrawProc()) ;
        m_peer->SetData<ControlUserPaneHitTestUPP>(kControlEntireControl, kControlUserPaneHitTestProcTag, GetwxMacControlUserPaneHitTestProc()) ;
        m_peer->SetData<ControlUserPaneTrackingUPP>(kControlEntireControl, kControlUserPaneTrackingProcTag, GetwxMacControlUserPaneTrackingProc()) ;
        m_peer->SetData<ControlUserPaneIdleUPP>(kControlEntireControl, kControlUserPaneIdleProcTag, GetwxMacControlUserPaneIdleProc()) ;
        m_peer->SetData<ControlUserPaneKeyDownUPP>(kControlEntireControl, kControlUserPaneKeyDownProcTag, GetwxMacControlUserPaneKeyDownProc()) ;
        m_peer->SetData<ControlUserPaneActivateUPP>(kControlEntireControl, kControlUserPaneActivateProcTag, GetwxMacControlUserPaneActivateProc()) ;
        m_peer->SetData<ControlUserPaneFocusUPP>(kControlEntireControl, kControlUserPaneFocusProcTag, GetwxMacControlUserPaneFocusProc()) ;
        m_peer->SetData<ControlUserPaneBackgroundUPP>(kControlEntireControl, kControlUserPaneBackgroundProcTag, GetwxMacControlUserPaneBackgroundProc()) ;
    }
#endif
}

// Constructor
bool wxWindowMac::Create(wxWindowMac *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
{
    wxCHECK_MSG( parent, false, wxT("can't create wxWindowMac without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_windowVariant = parent->GetWindowVariant() ;

    if ( m_macIsUserPane )
    {
        Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

        UInt32 features = 0
            | kControlSupportsEmbedding
            | kControlSupportsLiveFeedback
            | kControlGetsFocusOnClick
//            | kControlHasSpecialBackground
//            | kControlSupportsCalcBestRect
            | kControlHandlesTracking
            | kControlSupportsFocus
            | kControlWantsActivate
            | kControlWantsIdle ;

        m_peer = new wxMacControl(this) ;
        OSStatus err =::CreateUserPaneControl( MAC_WXHWND(GetParent()->MacGetTopLevelWindowRef()) , &bounds, features , m_peer->GetControlRefAddr() );
        verify_noerr( err );

        MacPostControlCreate(pos, size) ;
    }

#ifndef __WXUNIVERSAL__
    // Don't give scrollbars to wxControls unless they ask for them
    if ( (! IsKindOf(CLASSINFO(wxControl)) && ! IsKindOf(CLASSINFO(wxStatusBar)))
         || (IsKindOf(CLASSINFO(wxControl)) && ((style & wxHSCROLL) || (style & wxVSCROLL))))
    {
        MacCreateScrollBars( style ) ;
    }
#endif

    wxWindowCreateEvent event(this);
    GetEventHandler()->AddPendingEvent(event);

    return true;
}

void wxWindowMac::MacChildAdded()
{
    if ( m_vScrollBar )
        m_vScrollBar->Raise() ;
    if ( m_hScrollBar )
        m_hScrollBar->Raise() ;
}

void wxWindowMac::MacPostControlCreate(const wxPoint& pos, const wxSize& size)
{
    wxASSERT_MSG( m_peer != NULL && m_peer->Ok() , wxT("No valid mac control") ) ;

    m_peer->SetReference( (URefCon) this ) ;
    GetParent()->AddChild( this );

    MacInstallEventHandler( (WXWidget) m_peer->GetControlRef() );

    ControlRef container = (ControlRef) GetParent()->GetHandle() ;
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    ::EmbedControl( m_peer->GetControlRef() , container ) ;
    GetParent()->MacChildAdded() ;

    // adjust font, controlsize etc
    DoSetWindowVariant( m_windowVariant ) ;

    m_peer->SetLabel( wxStripMenuCodes(m_label, wxStrip_Mnemonics) ) ;

    if (!m_macIsUserPane)
        SetInitialBestSize(size);

    SetCursor( *wxSTANDARD_CURSOR ) ;
}

void wxWindowMac::DoSetWindowVariant( wxWindowVariant variant )
{
    // Don't assert, in case we set the window variant before
    // the window is created
    // wxASSERT( m_peer->Ok() ) ;

    m_windowVariant = variant ;

    if (m_peer == NULL || !m_peer->Ok())
        return;

    ControlSize size ;
    ThemeFontID themeFont = kThemeSystemFont ;

    // we will get that from the settings later
    // and make this NORMAL later, but first
    // we have a few calculations that we must fix

    switch ( variant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            size = kControlSizeNormal;
            themeFont = kThemeSystemFont ;
            break ;

        case wxWINDOW_VARIANT_SMALL :
            size = kControlSizeSmall;
            themeFont = kThemeSmallSystemFont ;
            break ;

        case wxWINDOW_VARIANT_MINI :
           if (UMAGetSystemVersion() >= 0x1030 )
            {
                // not always defined in the headers
                size = 3 ;
                themeFont = 109 ;
            }
            else
            {
                size = kControlSizeSmall;
                themeFont = kThemeSmallSystemFont ;
            }
            break ;

        case wxWINDOW_VARIANT_LARGE :
            size = kControlSizeLarge;
            themeFont = kThemeSystemFont ;
            break ;

        default:
            wxFAIL_MSG(_T("unexpected window variant"));
            break ;
    }

    m_peer->SetData<ControlSize>(kControlEntireControl, kControlSizeTag, &size ) ;

    wxFont font ;
    font.MacCreateThemeFont( themeFont ) ;
    SetFont( font ) ;
}

void wxWindowMac::MacUpdateControlFont()
{
    m_peer->SetFont( GetFont() , GetForegroundColour() , GetWindowStyle() ) ;
    Refresh() ;
}

bool wxWindowMac::SetFont(const wxFont& font)
{
    bool retval = wxWindowBase::SetFont( font );

    MacUpdateControlFont() ;

    return retval;
}

bool wxWindowMac::SetForegroundColour(const wxColour& col )
{
    bool retval = wxWindowBase::SetForegroundColour( col );

    if (retval)
        MacUpdateControlFont();

    return retval;
}

bool wxWindowMac::SetBackgroundColour(const wxColour& col )
{
    if ( !wxWindowBase::SetBackgroundColour(col) && m_hasBgCol )
        return false ;

    wxBrush brush ;
    wxColour newCol(GetBackgroundColour());

    if ( newCol == wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) )
        brush.MacSetTheme( kThemeBrushDocumentWindowBackground ) ;
    else if ( newCol == wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) )
        brush.MacSetTheme( kThemeBrushDialogBackgroundActive ) ;
    else
        brush.SetColour( newCol ) ;

    MacSetBackgroundBrush( brush ) ;
    MacUpdateControlFont() ;

    return true ;
}

void wxWindowMac::MacSetBackgroundBrush( const wxBrush &brush )
{
    m_macBackgroundBrush = brush ;
    m_peer->SetBackground( brush ) ;
}

bool wxWindowMac::MacCanFocus() const
{
    // TODO : evaluate performance hits by looking up this value, eventually cache the results for a 1 sec or so
    // CAUTION : the value returned currently is 0 or 2, I've also found values of 1 having the same meaning,
    // but the value range is nowhere documented
    Boolean keyExistsAndHasValidFormat ;
    CFIndex fullKeyboardAccess = CFPreferencesGetAppIntegerValue( CFSTR("AppleKeyboardUIMode" ) ,
        kCFPreferencesCurrentApplication, &keyExistsAndHasValidFormat );

    if ( keyExistsAndHasValidFormat && fullKeyboardAccess > 0 )
    {
        return true ;
    }
    else
    {
        UInt32 features = 0 ;
        m_peer->GetFeatures( &features ) ;

        return features & ( kControlSupportsFocus | kControlGetsFocusOnClick ) ;
    }
}

void wxWindowMac::SetFocus()
{
    if ( !AcceptsFocus() )
            return ;

    wxWindow* former = FindFocus() ;
    if ( former == this )
        return ;

    // as we cannot rely on the control features to find out whether we are in full keyboard mode,
    // we can only leave in case of an error
    OSStatus err = m_peer->SetFocus( kControlFocusNextPart ) ;
    if ( err == errCouldntSetFocus )
        return ;

    SetUserFocusWindow( (WindowRef)MacGetTopLevelWindowRef() );

#if !TARGET_API_MAC_OSX
    // emulate carbon events when running under CarbonLib where they are not natively available
    if ( former )
    {
        EventRef evRef = NULL ;

        err = MacCreateEvent(
            NULL , kEventClassControl , kEventControlSetFocusPart , TicksToEventTime( TickCount() ) ,
            kEventAttributeUserEvent , &evRef );
        verify_noerr( err );

        wxMacCarbonEvent cEvent( evRef ) ;
        cEvent.SetParameter<ControlRef>( kEventParamDirectObject , (ControlRef) former->GetHandle() ) ;
        cEvent.SetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode , kControlFocusNoPart ) ;

        wxMacWindowEventHandler( NULL , evRef , former ) ;
        ReleaseEvent( evRef ) ;
    }

    // send new focus event
    {
        EventRef evRef = NULL ;

        err = MacCreateEvent(
            NULL , kEventClassControl , kEventControlSetFocusPart , TicksToEventTime( TickCount() ) ,
            kEventAttributeUserEvent , &evRef );
        verify_noerr( err );

        wxMacCarbonEvent cEvent( evRef ) ;
        cEvent.SetParameter<ControlRef>( kEventParamDirectObject , (ControlRef) GetHandle() ) ;
        cEvent.SetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode , kControlFocusNextPart ) ;

        wxMacWindowEventHandler( NULL , evRef , this ) ;
        ReleaseEvent( evRef ) ;
    }
#endif
}

void wxWindowMac::DoCaptureMouse()
{
    wxApp::s_captureWindow = this ;
}

wxWindow * wxWindowBase::GetCapture()
{
    return wxApp::s_captureWindow ;
}

void wxWindowMac::DoReleaseMouse()
{
    wxApp::s_captureWindow = NULL ;
}

#if wxUSE_DRAG_AND_DROP

void wxWindowMac::SetDropTarget(wxDropTarget *pDropTarget)
{
    if ( m_dropTarget != NULL )
        delete m_dropTarget;

    m_dropTarget = pDropTarget;
    if ( m_dropTarget != NULL )
    {
        // TODO:
    }
}

#endif

// Old-style File Manager Drag & Drop
void wxWindowMac::DragAcceptFiles(bool accept)
{
    // TODO:
}

// Returns the size of the native control. In the case of the toplevel window
// this is the content area root control

void wxWindowMac::MacGetPositionAndSizeFromControl(int& x, int& y,
                                           int& w, int& h) const
{
    wxFAIL_MSG( wxT("Not currently supported") ) ;
}

// From a wx position / size calculate the appropriate size of the native control

bool wxWindowMac::MacGetBoundsForControl(
    const wxPoint& pos,
    const wxSize& size,
    int& x, int& y,
    int& w, int& h , bool adjustOrigin ) const
{
    // the desired size, minus the border pixels gives the correct size of the control
    x = (int)pos.x;
    y = (int)pos.y;

    // TODO: the default calls may be used as soon as PostCreateControl Is moved here
    w = wxMax(size.x, 0) ; // WidthDefault( size.x );
    h = wxMax(size.y, 0) ; // HeightDefault( size.y ) ;

    x += MacGetLeftBorderSize() ;
    y += MacGetTopBorderSize() ;
    w -= MacGetLeftBorderSize() + MacGetRightBorderSize() ;
    h -= MacGetTopBorderSize() + MacGetBottomBorderSize() ;

    if ( adjustOrigin )
        AdjustForParentClientOrigin( x , y ) ;

    // this is in window relative coordinate, as this parent may have a border, its physical position is offset by this border
    if ( !GetParent()->IsTopLevel() )
    {
        x -= GetParent()->MacGetLeftBorderSize() ;
        y -= GetParent()->MacGetTopBorderSize() ;
    }

    return true ;
}

// Get window size (not client size)
void wxWindowMac::DoGetSize(int *x, int *y) const
{
    Rect bounds ;
    m_peer->GetRect( &bounds ) ;

    if (x)
       *x = bounds.right - bounds.left + MacGetLeftBorderSize() + MacGetRightBorderSize() ;
    if (y)
       *y = bounds.bottom - bounds.top + MacGetTopBorderSize() + MacGetBottomBorderSize() ;
}

// get the position of the bounds of this window in client coordinates of its parent
void wxWindowMac::DoGetPosition(int *x, int *y) const
{
    Rect bounds ;
    m_peer->GetRect( &bounds ) ;

    int x1 = bounds.left ;
    int y1 = bounds.top ;

    // get the wx window position from the native one
    x1 -= MacGetLeftBorderSize() ;
    y1 -= MacGetTopBorderSize() ;

    if ( !IsTopLevel() )
    {
        wxWindow *parent = GetParent();
        if ( parent )
        {
            // we must first adjust it to be in window coordinates of the parent,
            // as otherwise it gets lost by the ClientAreaOrigin fix
            x1 += parent->MacGetLeftBorderSize() ;
            y1 += parent->MacGetTopBorderSize() ;

            // and now to client coordinates
            wxPoint pt(parent->GetClientAreaOrigin());
            x1 -= pt.x ;
            y1 -= pt.y ;
        }
    }

    if (x)
       *x = x1 ;
    if (y)
       *y = y1 ;
}

void wxWindowMac::DoScreenToClient(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    wxCHECK_RET( window , wxT("TopLevel Window missing") ) ;

    Point localwhere = { 0, 0 } ;

    if (x)
        localwhere.h = *x ;
    if (y)
        localwhere.v = *y ;

    wxMacGlobalToLocal( window , &localwhere ) ;

    if (x)
       *x = localwhere.h ;
    if (y)
       *y = localwhere.v ;

    MacRootWindowToWindow( x , y ) ;

    wxPoint origin = GetClientAreaOrigin() ;
    if (x)
       *x -= origin.x ;
    if (y)
       *y -= origin.y ;
}

void wxWindowMac::DoClientToScreen(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    wxCHECK_RET( window , wxT("TopLevel window missing") ) ;

    wxPoint origin = GetClientAreaOrigin() ;
    if (x)
       *x += origin.x ;
    if (y)
       *y += origin.y ;

    MacWindowToRootWindow( x , y ) ;

    Point localwhere = { 0, 0 };
    if (x)
       localwhere.h = *x ;
    if (y)
       localwhere.v = *y ;

    wxMacLocalToGlobal( window, &localwhere ) ;

    if (x)
       *x = localwhere.h ;
    if (y)
       *y = localwhere.v ;
}

void wxWindowMac::MacClientToRootWindow( int *x , int *y ) const
{
    wxPoint origin = GetClientAreaOrigin() ;
    if (x)
       *x += origin.x ;
    if (y)
       *y += origin.y ;

    MacWindowToRootWindow( x , y ) ;
}

void wxWindowMac::MacRootWindowToClient( int *x , int *y ) const
{
    MacRootWindowToWindow( x , y ) ;

    wxPoint origin = GetClientAreaOrigin() ;
    if (x)
       *x -= origin.x ;
    if (y)
       *y -= origin.y ;
}

void wxWindowMac::MacWindowToRootWindow( int *x , int *y ) const
{
    wxPoint pt ;

    if (x)
        pt.x = *x ;
    if (y)
        pt.y = *y ;

    if ( !IsTopLevel() )
    {
        wxTopLevelWindowMac* top = MacGetTopLevelWindow();
        if (top)
        {
            pt.x -= MacGetLeftBorderSize() ;
            pt.y -= MacGetTopBorderSize() ;
            wxMacControl::Convert( &pt , m_peer , top->m_peer ) ;
        }
    }

    if (x)
        *x = (int) pt.x ;
    if (y)
        *y = (int) pt.y ;
}

void wxWindowMac::MacWindowToRootWindow( short *x , short *y ) const
{
    int x1 , y1 ;

    if (x)
        x1 = *x ;
    if (y)
        y1 = *y ;

    MacWindowToRootWindow( &x1 , &y1 ) ;

    if (x)
        *x = x1 ;
    if (y)
        *y = y1 ;
}

void wxWindowMac::MacRootWindowToWindow( int *x , int *y ) const
{
    wxPoint pt ;

    if (x)
        pt.x = *x ;
    if (y)
        pt.y = *y ;

    if ( !IsTopLevel() )
    {
        wxTopLevelWindowMac* top = MacGetTopLevelWindow();
        if (top)
        {
            wxMacControl::Convert( &pt , top->m_peer , m_peer ) ;
            pt.x += MacGetLeftBorderSize() ;
            pt.y += MacGetTopBorderSize() ;
        }
    }

    if (x)
        *x = (int) pt.x ;
    if (y)
        *y = (int) pt.y ;
}

void wxWindowMac::MacRootWindowToWindow( short *x , short *y ) const
{
    int x1 , y1 ;

    if (x)
        x1 = *x ;
    if (y)
        y1 = *y ;

    MacRootWindowToWindow( &x1 , &y1 ) ;

    if (x)
        *x = x1 ;
    if (y)
        *y = y1 ;
}

void wxWindowMac::MacGetContentAreaInset( int &left , int &top , int &right , int &bottom )
{
    RgnHandle rgn = NewRgn() ;

    if ( m_peer->GetRegion( kControlContentMetaPart , rgn ) == noErr )
    {
        Rect structure, content ;

        GetRegionBounds( rgn , &content ) ;
        m_peer->GetRect( &structure ) ;
        OffsetRect( &structure, -structure.left , -structure.top ) ;

        left = content.left - structure.left ;
        top = content.top - structure.top ;
        right = structure.right - content.right ;
        bottom = structure.bottom - content.bottom ;
    }
    else
    {
        left = top = right = bottom = 0 ;
    }

    DisposeRgn( rgn ) ;
}

wxSize wxWindowMac::DoGetSizeFromClientSize( const wxSize & size )  const
{
    wxSize sizeTotal = size;

    RgnHandle rgn = NewRgn() ;
    if ( m_peer->GetRegion( kControlContentMetaPart , rgn ) == noErr )
    {
        Rect content, structure ;
        GetRegionBounds( rgn , &content ) ;
        m_peer->GetRect( &structure ) ;

        // structure is in parent coordinates, but we only need width and height, so it's ok

        sizeTotal.x += (structure.right - structure.left) - (content.right - content.left) ;
        sizeTotal.y += (structure.bottom - structure.top) - (content.bottom - content.top) ;
    }

    DisposeRgn( rgn ) ;

    sizeTotal.x += MacGetLeftBorderSize() + MacGetRightBorderSize() ;
    sizeTotal.y += MacGetTopBorderSize() + MacGetBottomBorderSize() ;

    return sizeTotal;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowMac::DoGetClientSize( int *x, int *y ) const
{
    int ww, hh;

    RgnHandle rgn = NewRgn() ;
    Rect content ;
    if ( m_peer->GetRegion( kControlContentMetaPart , rgn ) == noErr )
        GetRegionBounds( rgn , &content ) ;
    else
        m_peer->GetRect( &content ) ;
    DisposeRgn( rgn ) ;

    ww = content.right - content.left ;
    hh = content.bottom - content.top ;

    if (m_hScrollBar  && m_hScrollBar->IsShown() )
        hh -= m_hScrollBar->GetSize().y ;

    if (m_vScrollBar  && m_vScrollBar->IsShown() )
        ww -= m_vScrollBar->GetSize().x ;

    if (x)
       *x = ww;
    if (y)
       *y = hh;
}

bool wxWindowMac::SetCursor(const wxCursor& cursor)
{
    if (m_cursor == cursor)
        return false;

    if (wxNullCursor == cursor)
    {
        if ( ! wxWindowBase::SetCursor( *wxSTANDARD_CURSOR ) )
            return false ;
    }
    else
    {
        if ( ! wxWindowBase::SetCursor( cursor ) )
            return false ;
    }

    wxASSERT_MSG( m_cursor.Ok(),
        wxT("cursor must be valid after call to the base version"));

    wxWindowMac *mouseWin = 0 ;
    {
        wxTopLevelWindowMac *tlw = MacGetTopLevelWindow() ;
        WindowRef window = (WindowRef) ( tlw ? tlw->MacGetWindowRef() : 0 ) ;

        ControlPartCode part ;
        ControlRef control ;
        Point pt ;
 #if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
        HIPoint hiPoint ;
        HIGetMousePosition(kHICoordSpaceWindow, window, &hiPoint);
        pt.h = hiPoint.x;
        pt.v = hiPoint.y;
 #else 
        CGrafPtr savePort ;
        Boolean swapped = QDSwapPort( GetWindowPort( window ) , &savePort ) ;

        // TODO: If we ever get a GetCurrentEvent... replacement
        // for the mouse position, use it...


        GetMouse( &pt ) ;
#endif
        control = wxMacFindControlUnderMouse( tlw , pt , window , &part ) ;
        if ( control )
            mouseWin = wxFindControlFromMacControl( control ) ;

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
         if ( swapped )
            QDSwapPort( savePort , NULL ) ;
#endif
    }

    if ( mouseWin == this && !wxIsBusy() )
        m_cursor.MacInstall() ;

    return true ;
}

#if wxUSE_MENUS
bool wxWindowMac::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->SetInvokingWindow(this);
    menu->UpdateUI();

    if ( x == wxDefaultCoord && y == wxDefaultCoord )
    {
        wxPoint mouse = wxGetMousePosition();
        x = mouse.x;
        y = mouse.y;
    }
    else
    {
        ClientToScreen( &x , &y ) ;
    }

    menu->MacBeforeDisplay( true ) ;
    long menuResult = ::PopUpMenuSelect((MenuHandle) menu->GetHMenu() , y, x, 0) ;
    if ( HiWord(menuResult) != 0 )
    {
        MenuCommand macid;
        GetMenuItemCommandID( GetMenuHandle(HiWord(menuResult)) , LoWord(menuResult) , &macid );
        int id = wxMacCommandToId( macid );
        wxMenuItem* item = NULL ;
        wxMenu* realmenu ;
        item = menu->FindItem( id, &realmenu ) ;
        if ( item )
        {
            if (item->IsCheckable())
                item->Check( !item->IsChecked() ) ;

            menu->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) ;
        }
    }

    menu->MacAfterDisplay( true ) ;
    menu->SetInvokingWindow( NULL );

  return true;
}
#endif

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowMac::DoSetToolTip(wxToolTip *tooltip)
{
    wxWindowBase::DoSetToolTip(tooltip);

    if ( m_tooltip )
        m_tooltip->SetWindow(this);
}

#endif

void wxWindowMac::MacInvalidateBorders()
{
    if ( m_peer == NULL )
        return ;

    bool vis = MacIsReallyShown() ;
    if ( !vis )
        return ;

    int outerBorder = MacGetLeftBorderSize() ;
    if ( m_peer->NeedsFocusRect() && m_peer->HasFocus() )
        outerBorder += 4 ;

    if ( outerBorder == 0 )
        return ;

    // now we know that we have something to do at all

    // as the borders are drawn on the parent we have to properly invalidate all these areas
    RgnHandle updateInner , updateOuter;
    Rect rect ;

    // this rectangle is in HIViewCoordinates under OSX and in Window Coordinates under Carbon
    updateInner = NewRgn() ;
    updateOuter = NewRgn() ;

    m_peer->GetRect( &rect ) ;
    RectRgn( updateInner, &rect ) ;
    InsetRect( &rect , -outerBorder , -outerBorder ) ;
    RectRgn( updateOuter, &rect ) ;
    DiffRgn( updateOuter, updateInner , updateOuter ) ;

#ifdef __WXMAC_OSX__
    GetParent()->m_peer->SetNeedsDisplay( updateOuter ) ;
#else
    WindowRef tlw = (WindowRef) MacGetTopLevelWindowRef() ;
    if ( tlw )
        InvalWindowRgn( tlw , updateOuter ) ;
#endif

    DisposeRgn( updateOuter ) ;
    DisposeRgn( updateInner ) ;
}

void wxWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    // this is never called for a toplevel window, so we know we have a parent
    int former_x , former_y , former_w, former_h ;

    // Get true coordinates of former position
    DoGetPosition( &former_x , &former_y ) ;
    DoGetSize( &former_w , &former_h ) ;

    wxWindow *parent = GetParent();
    if ( parent )
    {
        wxPoint pt(parent->GetClientAreaOrigin());
        former_x += pt.x ;
        former_y += pt.y ;
    }

    int actualWidth = width ;
    int actualHeight = height ;
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

    bool doMove = false, doResize = false ;

    if ( actualX != former_x || actualY != former_y )
        doMove = true ;

    if ( actualWidth != former_w || actualHeight != former_h )
        doResize = true ;

    if ( doMove || doResize )
    {
        // as the borders are drawn outside the native control, we adjust now

        wxRect bounds( wxPoint( actualX + MacGetLeftBorderSize() ,actualY + MacGetTopBorderSize() ),
            wxSize( actualWidth - (MacGetLeftBorderSize() + MacGetRightBorderSize()) ,
                actualHeight - (MacGetTopBorderSize() + MacGetBottomBorderSize()) ) ) ;

        Rect r ;
        wxMacRectToNative( &bounds , &r ) ;

        if ( !GetParent()->IsTopLevel() )
            wxMacWindowToNative( GetParent() , &r ) ;

        MacInvalidateBorders() ;

        m_cachedClippedRectValid = false ;
        m_peer->SetRect( &r ) ;

        wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified

        MacInvalidateBorders() ;

        MacRepositionScrollBars() ;
        if ( doMove )
        {
            wxPoint point(actualX, actualY);
            wxMoveEvent event(point, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event) ;
        }

        if ( doResize )
        {
            MacRepositionScrollBars() ;
            wxSize size(actualWidth, actualHeight);
            wxSizeEvent event(size, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event);
        }
    }
}

wxSize wxWindowMac::DoGetBestSize() const
{
    if ( m_macIsUserPane || IsTopLevel() )
        return wxWindowBase::DoGetBestSize() ;

    Rect    bestsize = { 0 , 0 , 0 , 0 } ;
    int bestWidth, bestHeight ;

    m_peer->GetBestRect( &bestsize ) ;
    if ( EmptyRect( &bestsize ) )
    {
        bestsize.left =
        bestsize.top = 0 ;
        bestsize.right =
        bestsize.bottom = 16 ;

        if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
        {
            bestsize.bottom = 16 ;
        }
#if wxUSE_SPINBTN
        else if ( IsKindOf( CLASSINFO( wxSpinButton ) ) )
        {
            bestsize.bottom = 24 ;
        }
#endif
        else
        {
            // return wxWindowBase::DoGetBestSize() ;
        }
    }

    bestWidth = bestsize.right - bestsize.left ;
    bestHeight = bestsize.bottom - bestsize.top ;
    if ( bestHeight < 10 )
        bestHeight = 13 ;

    return wxSize(bestWidth, bestHeight);
}

// set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
void wxWindowMac::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // get the current size and position...
    int currentX, currentY;
    int currentW, currentH;

    GetPosition(&currentX, &currentY);
    GetSize(&currentW, &currentH);

    // ... and don't do anything (avoiding flicker) if it's already ok
    if ( x == currentX && y == currentY &&
        width == currentW && height == currentH && ( height != -1 && width != -1 ) )
    {
        // TODO: REMOVE
        MacRepositionScrollBars() ; // we might have a real position shift

        return;
    }

    if ( !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
    {
        if ( x == wxDefaultCoord )
            x = currentX;
        if ( y == wxDefaultCoord )
            y = currentY;
    }

    AdjustForParentClientOrigin( x, y, sizeFlags );

    wxSize size = wxDefaultSize;
    if ( width == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
        {
            size = DoGetBestSize();
            width = size.x;
        }
        else
        {
            // just take the current one
            width = currentW;
        }
    }

    if ( height == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == wxDefaultCoord )
                size = DoGetBestSize();
            // else: already called DoGetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    DoMoveWindow( x, y, width, height );
}

wxPoint wxWindowMac::GetClientAreaOrigin() const
{
    RgnHandle rgn = NewRgn() ;
    Rect content ;
    if ( m_peer->GetRegion( kControlContentMetaPart , rgn ) == noErr )
    {
        GetRegionBounds( rgn , &content ) ;
    }
    else
    {
        content.left =
        content.top = 0 ;
    }

    DisposeRgn( rgn ) ;

    return wxPoint( content.left + MacGetLeftBorderSize() , content.top + MacGetTopBorderSize() );
}

void wxWindowMac::DoSetClientSize(int clientwidth, int clientheight)
{
    if ( clientwidth != wxDefaultCoord || clientheight != wxDefaultCoord )
    {
        int currentclientwidth , currentclientheight ;
        int currentwidth , currentheight ;

        GetClientSize( &currentclientwidth , &currentclientheight ) ;
        GetSize( &currentwidth , &currentheight ) ;

        DoSetSize( wxDefaultCoord , wxDefaultCoord , currentwidth + clientwidth - currentclientwidth ,
            currentheight + clientheight - currentclientheight , wxSIZE_USE_EXISTING ) ;
    }
}

void wxWindowMac::SetLabel(const wxString& title)
{
    m_label = wxStripMenuCodes(title, wxStrip_Mnemonics) ;

    if ( m_peer && m_peer->Ok() )
        m_peer->SetLabel( m_label ) ;

    Refresh() ;
}

wxString wxWindowMac::GetLabel() const
{
    return m_label ;
}

bool wxWindowMac::Show(bool show)
{
    bool former = MacIsReallyShown() ;
    if ( !wxWindowBase::Show(show) )
        return false;

    // TODO: use visibilityChanged Carbon Event for OSX
    if ( m_peer )
        m_peer->SetVisibility( show , true ) ;

    if ( former != MacIsReallyShown() )
        MacPropagateVisibilityChanged() ;

    return true;
}

bool wxWindowMac::Enable(bool enable)
{
    wxASSERT( m_peer->Ok() ) ;
    bool former = MacIsReallyEnabled() ;
    if ( !wxWindowBase::Enable(enable) )
        return false;

    m_peer->Enable( enable ) ;

    if ( former != MacIsReallyEnabled() )
        MacPropagateEnabledStateChanged() ;

    return true;
}

//
// status change propagations (will be not necessary for OSX later )
//

void wxWindowMac::MacPropagateVisibilityChanged()
{
#if !TARGET_API_MAC_OSX
    MacVisibilityChanged() ;

    wxWindowMac *child;
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        child = node->GetData();
        if ( child->IsShown() )
            child->MacPropagateVisibilityChanged() ;

        node = node->GetNext();
    }
#endif
}

void wxWindowMac::MacPropagateEnabledStateChanged()
{
#if !TARGET_API_MAC_OSX
    MacEnabledStateChanged() ;

    wxWindowMac *child;
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        child = node->GetData();
        if ( child->IsEnabled() )
            child->MacPropagateEnabledStateChanged() ;

        node = node->GetNext();
    }
#endif
}

void wxWindowMac::MacPropagateHiliteChanged()
{
#if !TARGET_API_MAC_OSX
    MacHiliteChanged() ;

    wxWindowMac *child;
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        child = node->GetData();
        if (child /* && child->IsEnabled() */)
            child->MacPropagateHiliteChanged() ;

        node = node->GetNext();
    }
#endif
}

//
// status change notifications
//

void wxWindowMac::MacVisibilityChanged()
{
}

void wxWindowMac::MacHiliteChanged()
{
}

void wxWindowMac::MacEnabledStateChanged()
{
}

//
// status queries on the inherited window's state
//

bool wxWindowMac::MacIsReallyShown()
{
    // only under OSX the visibility of the TLW is taken into account
    if ( m_isBeingDeleted )
        return false ;

#if TARGET_API_MAC_OSX
    if ( m_peer && m_peer->Ok() )
        return m_peer->IsVisible();
#endif

    wxWindow* win = this ;
    while ( win->IsShown() )
    {
        if ( win->IsTopLevel() )
            return true ;

        win = win->GetParent() ;
        if ( win == NULL )
            return true ;
    }

    return false ;
}

bool wxWindowMac::MacIsReallyEnabled()
{
    return m_peer->IsEnabled() ;
}

bool wxWindowMac::MacIsReallyHilited()
{
    return m_peer->IsActive();
}

void wxWindowMac::MacFlashInvalidAreas()
{
#if TARGET_API_MAC_OSX
    HIViewFlashDirtyArea( (WindowRef) MacGetTopLevelWindowRef() ) ;
#endif
}

int wxWindowMac::GetCharHeight() const
{
    wxClientDC dc( (wxWindowMac*)this ) ;

    return dc.GetCharHeight() ;
}

int wxWindowMac::GetCharWidth() const
{
    wxClientDC dc( (wxWindowMac*)this ) ;

    return dc.GetCharWidth() ;
}

void wxWindowMac::GetTextExtent(const wxString& string, int *x, int *y,
                           int *descent, int *externalLeading, const wxFont *theFont ) const
{
    const wxFont *fontToUse = theFont;
    if ( !fontToUse )
        fontToUse = &m_font;

    wxClientDC dc( (wxWindowMac*) this ) ;
    long lx,ly,ld,le ;
    dc.GetTextExtent( string , &lx , &ly , &ld, &le, (wxFont *)fontToUse ) ;
    if ( externalLeading )
        *externalLeading = le ;
    if ( descent )
        *descent = ld ;
    if ( x )
        *x = lx ;
    if ( y )
        *y = ly ;
}

/*
 * Rect is given in client coordinates, for further reading, read wxTopLevelWindowMac::InvalidateRect
 * we always intersect with the entire window, not only with the client area
 */

void wxWindowMac::Refresh(bool eraseBack, const wxRect *rect)
{
    if ( m_peer == NULL )
        return ;

    if ( !MacIsReallyShown() )
        return ;

    if ( rect )
    {
        Rect r ;

        wxMacRectToNative( rect , &r ) ;
        m_peer->SetNeedsDisplay( &r ) ;
    }
    else
    {
        m_peer->SetNeedsDisplay() ;
    }
}

void wxWindowMac::Freeze()
{
#if TARGET_API_MAC_OSX
    if ( !m_frozenness++ )
    {
        if ( m_peer && m_peer->Ok() )
            m_peer->SetDrawingEnabled( false ) ;
    }
#endif
}

void wxWindowMac::Thaw()
{
#if TARGET_API_MAC_OSX
    wxASSERT_MSG( m_frozenness > 0, wxT("Thaw() without matching Freeze()") );

    if ( !--m_frozenness )
    {
        if ( m_peer && m_peer->Ok() )
        {
            m_peer->SetDrawingEnabled( true ) ;
            m_peer->InvalidateWithChildren() ;
        }
    }
#endif
}

bool wxWindowMac::IsFrozen() const
{
    return m_frozenness != 0;
}

wxWindowMac *wxGetActiveWindow()
{
    // actually this is a windows-only concept
    return NULL;
}

// Coordinates relative to the window
void wxWindowMac::WarpPointer(int x_pos, int y_pos)
{
    // We really don't move the mouse programmatically under Mac.
}

void wxWindowMac::OnEraseBackground(wxEraseEvent& event)
{
    if ( MacGetTopLevelWindow() == NULL )
        return ;

#if TARGET_API_MAC_OSX
    if ( !m_macBackgroundBrush.Ok() || m_macBackgroundBrush.GetStyle() == wxTRANSPARENT )
    {
        event.Skip() ;
    }
    else
#endif
    {
        event.GetDC()->Clear() ;
    }
}

void wxWindowMac::OnNcPaint( wxNcPaintEvent& event )
{
    event.Skip() ;
}

int wxWindowMac::GetScrollPos(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetThumbPosition() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetThumbPosition() ;
    }

    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindowMac::GetScrollRange(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetRange() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetRange() ;
    }

    return 0;
}

int wxWindowMac::GetScrollThumb(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetThumbSize() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetThumbSize() ;
    }

    return 0;
}

void wxWindowMac::SetScrollPos(int orient, int pos, bool refresh)
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           m_hScrollBar->SetThumbPosition( pos ) ;
    }
    else
    {
       if ( m_vScrollBar )
           m_vScrollBar->SetThumbPosition( pos ) ;
    }
}

//
// we draw borders and grow boxes, are already set up and clipped in the current port / cgContextRef
// our own window origin is at leftOrigin/rightOrigin
//

void wxWindowMac::MacPaintBorders( int leftOrigin , int rightOrigin )
{
    if ( IsTopLevel() )
        return ;

    Rect rect ;
    bool hasFocus = m_peer->NeedsFocusRect() && m_peer->HasFocus() ;
    bool hasBothScrollbars = (m_hScrollBar && m_hScrollBar->IsShown()) && (m_vScrollBar && m_vScrollBar->IsShown()) ;

    // back to the surrounding frame rectangle
    m_peer->GetRect( &rect ) ;
    InsetRect( &rect, -1 , -1 ) ;

#if wxMAC_USE_CORE_GRAPHICS 
    {
        CGRect cgrect = CGRectMake( rect.left , rect.top , rect.right - rect.left ,
            rect.bottom - rect.top ) ;

        HIThemeFrameDrawInfo info ;
        memset( &info, 0 , sizeof(info) ) ;

        info.version = 0 ;
        info.kind = 0 ;
        info.state = IsEnabled() ? kThemeStateActive : kThemeStateInactive ;
        info.isFocused = hasFocus ;

        CGContextRef cgContext = (CGContextRef) GetParent()->MacGetCGContextRef() ;
        wxASSERT( cgContext ) ;

        if ( HasFlag(wxRAISED_BORDER) || HasFlag(wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER) )
        {
            info.kind = kHIThemeFrameTextFieldSquare ;
            HIThemeDrawFrame( &cgrect , &info , cgContext , kHIThemeOrientationNormal ) ;
        }
        else if ( HasFlag(wxSIMPLE_BORDER) )
        {
            info.kind = kHIThemeFrameListBox ;
            HIThemeDrawFrame( &cgrect , &info , cgContext , kHIThemeOrientationNormal ) ;
        }
        else if ( hasFocus )
        {
            HIThemeDrawFocusRect( &cgrect , true , cgContext , kHIThemeOrientationNormal ) ;
        }

        m_peer->GetRect( &rect ) ;
        if ( hasBothScrollbars )
        {
            int size = m_hScrollBar->GetWindowVariant() == wxWINDOW_VARIANT_NORMAL ? 16 : 12 ;
            CGRect cgrect = CGRectMake( rect.right - size , rect.bottom - size , size , size ) ;
            CGPoint cgpoint = CGPointMake( rect.right - size , rect.bottom - size ) ;
            HIThemeGrowBoxDrawInfo info ;
            memset( &info, 0, sizeof(info) ) ;
            info.version = 0 ;
            info.state = IsEnabled() ? kThemeStateActive : kThemeStateInactive ;
            info.kind = kHIThemeGrowBoxKindNone ;
            info.size = kHIThemeGrowBoxSizeNormal ;
            info.direction = kThemeGrowRight | kThemeGrowDown ;
            HIThemeDrawGrowBox( &cgpoint , &info , cgContext , kHIThemeOrientationNormal ) ;
        }
    }
 #else
    {
        wxTopLevelWindowMac* top = MacGetTopLevelWindow();
        if ( top )
        {
            wxPoint pt(0, 0) ;
            wxMacControl::Convert( &pt , GetParent()->m_peer , top->m_peer ) ;
            OffsetRect( &rect , pt.x , pt.y ) ;
        }

        if ( HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER) )
            DrawThemeEditTextFrame( &rect, IsEnabled() ? kThemeStateActive : kThemeStateInactive ) ;
        else if ( HasFlag(wxSIMPLE_BORDER) )
            DrawThemeListBoxFrame( &rect, IsEnabled() ? kThemeStateActive : kThemeStateInactive ) ;

        if ( hasFocus )
            DrawThemeFocusRect( &rect , true ) ;

        if ( hasBothScrollbars )
        {
            // GetThemeStandaloneGrowBoxBounds
            // DrawThemeStandaloneNoGrowBox
        }
    }
#endif
}

void wxWindowMac::RemoveChild( wxWindowBase *child )
{
    if ( child == m_hScrollBar )
        m_hScrollBar = NULL ;
    if ( child == m_vScrollBar )
        m_vScrollBar = NULL ;

    wxWindowBase::RemoveChild( child ) ;
}

// New function that will replace some of the above.
void wxWindowMac::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
    bool showScroller;
    bool triggerSizeEvent = false;

    if ( orient == wxHORIZONTAL )
    {
        if ( m_hScrollBar )
        {
            showScroller = ((range != 0) && (range > thumbVisible));
            if ( m_hScrollBar->IsShown() != showScroller )
            {
                m_hScrollBar->Show( showScroller );
                triggerSizeEvent = true;
            }

            m_hScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
        }
    }
    else
    {
        if ( m_vScrollBar )
        {
            showScroller = ((range != 0) && (range > thumbVisible));
            if ( m_vScrollBar->IsShown() != showScroller )
            {
                m_vScrollBar->Show( showScroller ) ;
                triggerSizeEvent = true;
            }

            m_vScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
        }
    }

    MacRepositionScrollBars() ;
    if ( triggerSizeEvent )
    {
        wxSizeEvent event(GetSize(), m_windowId);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
}

// Does a physical scroll
void wxWindowMac::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    if ( dx == 0 && dy == 0 )
        return ;

    int width , height ;
    GetClientSize( &width , &height ) ;

    {
        // note there currently is a bug in OSX which makes inefficient refreshes in case an entire control
        // area is scrolled, this does not occur if width and height are 2 pixels less,
        // TODO: write optimal workaround
        wxRect scrollrect( MacGetLeftBorderSize() , MacGetTopBorderSize() , width , height ) ;
        if ( rect )
            scrollrect.Intersect( *rect ) ;

        if ( m_peer->GetNeedsDisplay() )
        {
            // because HIViewScrollRect does not scroll the already invalidated area we have two options:
            // either immediate redraw or full invalidate
#if 1
            // is the better overall solution, as it does not slow down scrolling
            m_peer->SetNeedsDisplay() ;
#else
            // this would be the preferred version for fast drawing controls

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
            if ( UMAGetSystemVersion() >= 0x1030 )
                HIViewRender(m_peer->GetControlRef()) ;
            else
#endif
                Update() ;
#endif
        }

        // as the native control might be not a 0/0 wx window coordinates, we have to offset
        scrollrect.Offset( -MacGetLeftBorderSize() , -MacGetTopBorderSize() ) ;
        m_peer->ScrollRect( &scrollrect , dx , dy ) ;

        // becuase HIViewScrollRect does not scroll the already invalidated area we have two options
        // either immediate redraw or full invalidate
#if 0
        // is the better overall solution, as it does not slow down scrolling
        m_peer->SetNeedsDisplay() ;
#else
        // this would be the preferred version for fast drawing controls

        HIViewRender(m_peer->GetControlRef()) ;
#endif
    }

    wxWindowMac *child;
    int x, y, w, h;
    for (wxWindowList::compatibility_iterator node = GetChildren().GetFirst(); node; node = node->GetNext())
    {
        child = node->GetData();
        if (child == NULL)
            continue;
        if (child == m_vScrollBar)
            continue;
        if (child == m_hScrollBar)
            continue;
        if (child->IsTopLevel())
            continue;

        child->GetPosition( &x, &y );
        child->GetSize( &w, &h );
        if (rect)
        {
            wxRect rc( x, y, w, h );
            if (rect->Intersects( rc ))
                child->SetSize( x + dx, y + dy, w, h );
        }
        else
        {
            child->SetSize( x + dx, y + dy, w, h );
        }
    }
}

void wxWindowMac::MacOnScroll( wxScrollEvent &event )
{
    if ( event.GetEventObject() == m_vScrollBar || event.GetEventObject() == m_hScrollBar )
    {
        wxScrollWinEvent wevent;
        wevent.SetPosition(event.GetPosition());
        wevent.SetOrientation(event.GetOrientation());
        wevent.SetEventObject(this);

        if (event.GetEventType() == wxEVT_SCROLL_TOP)
            wevent.SetEventType( wxEVT_SCROLLWIN_TOP );
        else if (event.GetEventType() == wxEVT_SCROLL_BOTTOM)
            wevent.SetEventType( wxEVT_SCROLLWIN_BOTTOM );
        else if (event.GetEventType() == wxEVT_SCROLL_LINEUP)
            wevent.SetEventType( wxEVT_SCROLLWIN_LINEUP );
        else if (event.GetEventType() == wxEVT_SCROLL_LINEDOWN)
            wevent.SetEventType( wxEVT_SCROLLWIN_LINEDOWN );
        else if (event.GetEventType() == wxEVT_SCROLL_PAGEUP)
            wevent.SetEventType( wxEVT_SCROLLWIN_PAGEUP );
        else if (event.GetEventType() == wxEVT_SCROLL_PAGEDOWN)
            wevent.SetEventType( wxEVT_SCROLLWIN_PAGEDOWN );
        else if (event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)
            wevent.SetEventType( wxEVT_SCROLLWIN_THUMBTRACK );
        else if (event.GetEventType() == wxEVT_SCROLL_THUMBRELEASE)
            wevent.SetEventType( wxEVT_SCROLLWIN_THUMBRELEASE );

        GetEventHandler()->ProcessEvent(wevent);
    }
}

// Get the window with the focus
wxWindowMac *wxWindowBase::DoFindFocus()
{
    ControlRef control ;
    GetKeyboardFocus( GetUserFocusWindow() , &control ) ;
    return wxFindControlFromMacControl( control ) ;
}

void wxWindowMac::OnSetFocus( wxFocusEvent& event )
{
    // panel wants to track the window which was the last to have focus in it,
    // so we want to set ourselves as the window which last had focus
    //
    // notice that it's also important to do it upwards the tree because
    // otherwise when the top level panel gets focus, it won't set it back to
    // us, but to some other sibling

    // CS: don't know if this is still needed:
    //wxChildFocusEvent eventFocus(this);
    //(void)GetEventHandler()->ProcessEvent(eventFocus);

    if ( MacGetTopLevelWindow() && m_peer->NeedsFocusRect() )
    {
#if wxMAC_USE_CORE_GRAPHICS
        GetParent()->Refresh() ;
#else
        wxMacWindowStateSaver sv( this ) ;
        Rect rect ;

        m_peer->GetRect( &rect ) ;
        // auf den umgebenden Rahmen zurŸck
        InsetRect( &rect, -1 , -1 ) ;

        wxTopLevelWindowMac* top = MacGetTopLevelWindow();
        if ( top )
        {
            wxPoint pt(0, 0) ;
            wxMacControl::Convert( &pt , GetParent()->m_peer , top->m_peer ) ;
            rect.left += pt.x ;
            rect.right += pt.x ;
            rect.top += pt.y ;
            rect.bottom += pt.y ;
        }

        bool bIsFocusEvent = (event.GetEventType() == wxEVT_SET_FOCUS);
        DrawThemeFocusRect( &rect , bIsFocusEvent ) ;
        if ( !bIsFocusEvent )
        {
            // as this erases part of the frame we have to redraw borders
            // and because our z-ordering is not always correct (staticboxes)
            // we have to invalidate things, we cannot simple redraw
            MacInvalidateBorders() ;
        }
#endif
    }

    event.Skip();
}

void wxWindowMac::OnInternalIdle()
{
    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    if (wxUpdateUIEvent::CanUpdate(this))
        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
}

// Raise the window to the top of the Z order
void wxWindowMac::Raise()
{
    m_peer->SetZOrder( true , NULL ) ;
}

// Lower the window to the bottom of the Z order
void wxWindowMac::Lower()
{
    m_peer->SetZOrder( false , NULL ) ;
}

// static wxWindow *gs_lastWhich = NULL;

bool wxWindowMac::MacSetupCursor( const wxPoint& pt )
{
    // first trigger a set cursor event

    wxPoint clientorigin = GetClientAreaOrigin() ;
    wxSize clientsize = GetClientSize() ;
    wxCursor cursor ;
    if ( wxRect2DInt( clientorigin.x , clientorigin.y , clientsize.x , clientsize.y ).Contains( wxPoint2DInt( pt ) ) )
    {
        wxSetCursorEvent event( pt.x , pt.y );

        bool processedEvtSetCursor = GetEventHandler()->ProcessEvent(event);
        if ( processedEvtSetCursor && event.HasCursor() )
        {
            cursor = event.GetCursor() ;
        }
        else
        {
            // the test for processedEvtSetCursor is here to prevent using m_cursor
            // if the user code caught EVT_SET_CURSOR() and returned nothing from
            // it - this is a way to say that our cursor shouldn't be used for this
            // point
            if ( !processedEvtSetCursor && m_cursor.Ok() )
                cursor = m_cursor ;

            if ( !wxIsBusy() && !GetParent() )
                cursor = *wxSTANDARD_CURSOR ;
        }

        if ( cursor.Ok() )
            cursor.MacInstall() ;
    }

    return cursor.Ok() ;
}

wxString wxWindowMac::MacGetToolTipString( wxPoint &pt )
{
#if wxUSE_TOOLTIPS
    if ( m_tooltip )
        return m_tooltip->GetTip() ;
#endif

    return wxEmptyString ;
}

void wxWindowMac::ClearBackground()
{
    Refresh() ;
    Update() ;
}

void wxWindowMac::Update()
{
#if TARGET_API_MAC_OSX
    MacGetTopLevelWindow()->MacPerformUpdates() ;
#else
    ::Draw1Control( m_peer->GetControlRef() ) ;
#endif
}

wxTopLevelWindowMac* wxWindowMac::MacGetTopLevelWindow() const
{
    wxTopLevelWindowMac* win = NULL ;
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    if ( window )
        win = wxFindWinFromMacWindow( window ) ;

    return win ;
}

const wxRect& wxWindowMac::MacGetClippedClientRect() const
{
    MacUpdateClippedRects() ;

    return m_cachedClippedClientRect ;
}

const wxRect& wxWindowMac::MacGetClippedRect() const
{
    MacUpdateClippedRects() ;

    return m_cachedClippedRect ;
}

const wxRect&wxWindowMac:: MacGetClippedRectWithOuterStructure() const
{
    MacUpdateClippedRects() ;

    return m_cachedClippedRectWithOuterStructure ;
}

const wxRegion& wxWindowMac::MacGetVisibleRegion( bool includeOuterStructures )
{
    static wxRegion emptyrgn ;

    if ( !m_isBeingDeleted && MacIsReallyShown() /*m_peer->IsVisible() */ )
    {
        MacUpdateClippedRects() ;
        if ( includeOuterStructures )
            return m_cachedClippedRegionWithOuterStructure ;
        else
            return m_cachedClippedRegion ;
    }
    else
    {
        return emptyrgn ;
    }
}

void wxWindowMac::MacUpdateClippedRects() const
{
    if ( m_cachedClippedRectValid )
        return ;

    // includeOuterStructures is true if we try to draw somthing like a focus ring etc.
    // also a window dc uses this, in this case we only clip in the hierarchy for hard
    // borders like a scrollwindow, splitter etc otherwise we end up in a paranoia having
    // to add focus borders everywhere

    Rect r, rIncludingOuterStructures ;

    m_peer->GetRect( &r ) ;
    r.left -= MacGetLeftBorderSize() ;
    r.top -= MacGetTopBorderSize() ;
    r.bottom += MacGetBottomBorderSize() ;
    r.right += MacGetRightBorderSize() ;

    r.right -= r.left ;
    r.bottom -= r.top ;
    r.left = 0 ;
    r.top = 0 ;

    rIncludingOuterStructures = r ;
    InsetRect( &rIncludingOuterStructures , -4 , -4 ) ;

    wxRect cl = GetClientRect() ;
    Rect rClient = { cl.y , cl.x , cl.y + cl.height , cl.x + cl.width } ;

    int x , y ;
    wxSize size ;
    const wxWindow* child = this ;
    const wxWindow* parent = NULL ;

    while ( !child->IsTopLevel() && ( parent = child->GetParent() ) != NULL )
    {
        if ( parent->MacIsChildOfClientArea(child) )
        {
            size = parent->GetClientSize() ;
            wxPoint origin = parent->GetClientAreaOrigin() ;
            x = origin.x ;
            y = origin.y ;
        }
        else
        {
            // this will be true for scrollbars, toolbars etc.
            size = parent->GetSize() ;
            y = parent->MacGetTopBorderSize() ;
            x = parent->MacGetLeftBorderSize() ;
            size.x -= parent->MacGetLeftBorderSize() + parent->MacGetRightBorderSize() ;
            size.y -= parent->MacGetTopBorderSize() + parent->MacGetBottomBorderSize() ;
        }

        parent->MacWindowToRootWindow( &x, &y ) ;
        MacRootWindowToWindow( &x , &y ) ;

        Rect rparent = { y , x , y + size.y , x + size.x } ;

        // the wxwindow and client rects will always be clipped
        SectRect( &r , &rparent , &r ) ;
        SectRect( &rClient , &rparent , &rClient ) ;

        // the structure only at 'hard' borders
        if ( parent->MacClipChildren() ||
            ( parent->GetParent() && parent->GetParent()->MacClipGrandChildren() ) )
        {
            SectRect( &rIncludingOuterStructures , &rparent , &rIncludingOuterStructures ) ;
        }

        child = parent ;
    }

    m_cachedClippedRect = wxRect( r.left , r.top , r.right - r.left , r.bottom - r.top ) ;
    m_cachedClippedClientRect = wxRect( rClient.left , rClient.top ,
        rClient.right - rClient.left , rClient.bottom - rClient.top ) ;
    m_cachedClippedRectWithOuterStructure = wxRect(
        rIncludingOuterStructures.left , rIncludingOuterStructures.top ,
        rIncludingOuterStructures.right - rIncludingOuterStructures.left ,
        rIncludingOuterStructures.bottom - rIncludingOuterStructures.top ) ;

    m_cachedClippedRegionWithOuterStructure = wxRegion( m_cachedClippedRectWithOuterStructure ) ;
    m_cachedClippedRegion = wxRegion( m_cachedClippedRect ) ;
    m_cachedClippedClientRegion = wxRegion( m_cachedClippedClientRect ) ;

    m_cachedClippedRectValid = true ;
}

/*
    This function must not change the updatergn !
 */
bool wxWindowMac::MacDoRedraw( WXHRGN updatergnr , long time )
{
    bool handled = false ;
    Rect updatebounds ;
    RgnHandle updatergn = (RgnHandle) updatergnr ;
    GetRegionBounds( updatergn , &updatebounds ) ;

    // wxLogDebug(wxT("update for %s bounds %d, %d, %d, %d"), wxString(GetClassInfo()->GetClassName()).c_str(), updatebounds.left, updatebounds.top , updatebounds.right , updatebounds.bottom ) ;

    if ( !EmptyRgn(updatergn) )
    {
        RgnHandle newupdate = NewRgn() ;
        wxSize point = GetClientSize() ;
        wxPoint origin = GetClientAreaOrigin() ;
        SetRectRgn( newupdate , origin.x , origin.y , origin.x + point.x , origin.y + point.y ) ;
        SectRgn( newupdate , updatergn , newupdate ) ;

        // first send an erase event to the entire update area
        {
            // for the toplevel window this really is the entire area
            // for all the others only their client area, otherwise they
            // might be drawing with full alpha and eg put blue into
            // the grow-box area of a scrolled window (scroll sample)
            wxDC* dc = new wxWindowDC(this);
            if ( IsTopLevel() )
                dc->SetClippingRegion(wxRegion(updatergn));
            else
                dc->SetClippingRegion(wxRegion(newupdate));

            wxEraseEvent eevent( GetId(), dc );
            eevent.SetEventObject( this );
            GetEventHandler()->ProcessEvent( eevent );
            delete dc ;
        }

        // calculate a client-origin version of the update rgn and set m_updateRegion to that
        OffsetRgn( newupdate , -origin.x , -origin.y ) ;
        m_updateRegion = newupdate ;
        DisposeRgn( newupdate ) ;

        if ( !m_updateRegion.Empty() )
        {
            // paint the window itself

            wxPaintEvent event;
            event.SetTimestamp(time);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event);
            handled = true ;
        }

        // now we cannot rely on having its borders drawn by a window itself, as it does not
        // get the updateRgn wide enough to always do so, so we do it from the parent
        // this would also be the place to draw any custom backgrounds for native controls
        // in Composited windowing
        wxPoint clientOrigin = GetClientAreaOrigin() ;

        wxWindowMac *child;
        int x, y, w, h;
        for (wxWindowList::compatibility_iterator node = GetChildren().GetFirst(); node; node = node->GetNext())
        {
            child = node->GetData();
            if (child == NULL)
                continue;
            if (child == m_vScrollBar)
                continue;
            if (child == m_hScrollBar)
                continue;
            if (child->IsTopLevel())
                continue;
            if (!child->IsShown())
                continue;

            // only draw those in the update region (add a safety margin of 10 pixels for shadow effects

            child->GetPosition( &x, &y );
            child->GetSize( &w, &h );
            Rect childRect = { y , x , y + h , x + w } ;
            OffsetRect( &childRect , clientOrigin.x , clientOrigin.y ) ;
            InsetRect( &childRect , -10 , -10) ;

            if ( RectInRgn( &childRect , updatergn ) )
            {
                // paint custom borders
                wxNcPaintEvent eventNc( child->GetId() );
                eventNc.SetEventObject( child );
                if ( !child->GetEventHandler()->ProcessEvent( eventNc ) )
                {
#if wxMAC_USE_CORE_GRAPHICS 
                    child->MacPaintBorders(0, 0) ;
#else
                    {
                        wxWindowDC dc(this) ;
                        dc.SetClippingRegion(wxRegion(updatergn));
                        wxMacPortSetter helper(&dc) ;
                        child->MacPaintBorders(0, 0) ;
                    }
#endif
                }
            }
        }
    }

    return handled ;
}


WXWindow wxWindowMac::MacGetTopLevelWindowRef() const
{
    wxWindowMac *iter = (wxWindowMac*)this ;

    while ( iter )
    {
        if ( iter->IsTopLevel() )
            return ((wxTopLevelWindow*)iter)->MacGetWindowRef() ;

        iter = iter->GetParent() ;
    }

    return NULL ;
}

void wxWindowMac::MacCreateScrollBars( long style )
{
    wxASSERT_MSG( m_vScrollBar == NULL && m_hScrollBar == NULL , wxT("attempt to create window twice") ) ;

    if ( style & ( wxVSCROLL | wxHSCROLL ) )
    {
        bool hasBoth = ( style & wxVSCROLL ) && ( style & wxHSCROLL ) ;
        int scrlsize = MAC_SCROLLBAR_SIZE ;
        if ( GetWindowVariant() == wxWINDOW_VARIANT_SMALL || GetWindowVariant() == wxWINDOW_VARIANT_MINI )
        {
            scrlsize = MAC_SMALL_SCROLLBAR_SIZE ;
        }

        int adjust = hasBoth ? scrlsize - 1: 0 ;
        int width, height ;
        GetClientSize( &width , &height ) ;

        wxPoint vPoint(width - scrlsize, 0) ;
        wxSize vSize(scrlsize, height - adjust) ;
        wxPoint hPoint(0, height - scrlsize) ;
        wxSize hSize(width - adjust, scrlsize) ;

        if ( style & wxVSCROLL )
            m_vScrollBar = new wxScrollBar(this, wxID_ANY, vPoint, vSize , wxVERTICAL);

        if ( style  & wxHSCROLL )
            m_hScrollBar = new wxScrollBar(this, wxID_ANY, hPoint, hSize , wxHORIZONTAL);
    }

    // because the create does not take into account the client area origin
    // we might have a real position shift
    MacRepositionScrollBars() ;
}

bool wxWindowMac::MacIsChildOfClientArea( const wxWindow* child ) const
{
    bool result = ((child == NULL) || ((child != m_hScrollBar) && (child != m_vScrollBar)));

    return result ;
}

void wxWindowMac::MacRepositionScrollBars()
{
    if ( !m_hScrollBar && !m_vScrollBar )
        return ;

    bool hasBoth = (m_hScrollBar && m_hScrollBar->IsShown()) && ( m_vScrollBar && m_vScrollBar->IsShown()) ;
    int scrlsize = m_hScrollBar ? m_hScrollBar->GetSize().y : ( m_vScrollBar ? m_vScrollBar->GetSize().x : MAC_SCROLLBAR_SIZE ) ;
    int adjust = hasBoth ? scrlsize - 1 : 0 ;

    // get real client area
    int width, height ;
    GetSize( &width , &height );

    width -= MacGetLeftBorderSize() + MacGetRightBorderSize();
    height -= MacGetTopBorderSize() + MacGetBottomBorderSize();

    wxPoint vPoint( width - scrlsize, 0 ) ;
    wxSize vSize( scrlsize, height - adjust ) ;
    wxPoint hPoint( 0 , height - scrlsize ) ;
    wxSize hSize( width - adjust, scrlsize ) ;

#if 0
    int x = 0, y = 0, w, h ;
    GetSize( &w , &h ) ;

    MacClientToRootWindow( &x , &y ) ;
    MacClientToRootWindow( &w , &h ) ;

    wxWindowMac *iter = (wxWindowMac*)this ;

    int totW = 10000 , totH = 10000;
    while ( iter )
    {
        if ( iter->IsTopLevel() )
        {
            iter->GetSize( &totW , &totH ) ;
            break ;
        }

        iter = iter->GetParent() ;
    }

    if ( x == 0 )
    {
        hPoint.x = -1 ;
        hSize.x += 1 ;
    }
    if ( y == 0 )
    {
        vPoint.y = -1 ;
        vSize.y += 1 ;
    }

    if ( w - x >= totW )
    {
        hSize.x += 1 ;
        vPoint.x += 1 ;
    }
    if ( h - y >= totH )
    {
        vSize.y += 1 ;
        hPoint.y += 1 ;
    }
#endif

    if ( m_vScrollBar )
        m_vScrollBar->SetSize( vPoint.x , vPoint.y, vSize.x, vSize.y , wxSIZE_ALLOW_MINUS_ONE );
    if ( m_hScrollBar )
        m_hScrollBar->SetSize( hPoint.x , hPoint.y, hSize.x, hSize.y, wxSIZE_ALLOW_MINUS_ONE );
}

bool wxWindowMac::AcceptsFocus() const
{
    return MacCanFocus() && wxWindowBase::AcceptsFocus();
}

void wxWindowMac::MacSuperChangedPosition()
{
    // only window-absolute structures have to be moved i.e. controls

    m_cachedClippedRectValid = false ;

    wxWindowMac *child;
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        child = node->GetData();
        child->MacSuperChangedPosition() ;

        node = node->GetNext();
    }
}

void wxWindowMac::MacTopLevelWindowChangedPosition()
{
    // only screen-absolute structures have to be moved i.e. glcanvas

    wxWindowMac *child;
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        child = node->GetData();
        child->MacTopLevelWindowChangedPosition() ;

        node = node->GetNext();
    }
}

long wxWindowMac::MacGetLeftBorderSize() const
{
    if ( IsTopLevel() )
        return 0 ;

    SInt32 border = 0 ;

    if (HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER))
    {
        // this metric is only the 'outset' outside the simple frame rect
        GetThemeMetric( kThemeMetricEditTextFrameOutset , &border ) ;
        border += 1 ;
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
        // this metric is only the 'outset' outside the simple frame rect
        GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
        border += 1 ;
    }

    return border ;
}

long wxWindowMac::MacGetRightBorderSize() const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetTopBorderSize() const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetBottomBorderSize() const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacRemoveBordersFromStyle( long style )
{
    return style & ~wxBORDER_MASK ;
}

// Find the wxWindowMac at the current mouse position, returning the mouse
// position.
wxWindowMac * wxFindWindowAtPointer( wxPoint& pt )
{
    pt = wxGetMousePosition();
    wxWindowMac* found = wxFindWindowAtPoint(pt);

    return found;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    int x, y;

    wxGetMousePosition( &x, &y );

    return wxPoint(x, y);
}

void wxWindowMac::OnMouseEvent( wxMouseEvent &event )
{
    if ( event.GetEventType() == wxEVT_RIGHT_DOWN )
    {
        // copied from wxGTK : CS
        // VZ: shouldn't we move this to base class then?

        // generate a "context menu" event: this is similar to wxEVT_RIGHT_DOWN
        // except that:
        //
        // (a) it's a command event and so is propagated to the parent
        // (b) under MSW it can be generated from kbd too
        // (c) it uses screen coords (because of (a))
        wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU,
                                  this->GetId(),
                                  this->ClientToScreen(event.GetPosition()));
        if ( ! GetEventHandler()->ProcessEvent(evtCtx) )
            event.Skip() ;
    }
    else
    {
        event.Skip() ;
    }
}

void wxWindowMac::OnPaint( wxPaintEvent & event )
{
    if ( wxTheApp->MacGetCurrentEvent() != NULL && wxTheApp->MacGetCurrentEventHandlerCallRef() != NULL )
        CallNextEventHandler(
            (EventHandlerCallRef)wxTheApp->MacGetCurrentEventHandlerCallRef() ,
            (EventRef) wxTheApp->MacGetCurrentEvent() ) ;
}

void wxWindowMac::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED( mouseStillDown ) )
{
}

Rect wxMacGetBoundsForControl( wxWindow* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin )
{
    int x, y, w, h ;

    window->MacGetBoundsForControl( pos , size , x , y, w, h , adjustForOrigin ) ;
    Rect bounds = { y, x, y + h, x + w };

    return bounds ;
}

wxInt32 wxWindowMac::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    return eventNotHandledErr ;
}

bool wxWindowMac::Reparent(wxWindowBase *newParentBase)
{
    wxWindowMac *newParent = (wxWindowMac *)newParentBase;
    if ( !wxWindowBase::Reparent(newParent) )
        return false;

    // copied from MacPostControlCreate
    ControlRef container = (ControlRef) GetParent()->GetHandle() ;

    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;

    ::EmbedControl( m_peer->GetControlRef() , container ) ;

    return true;
}

bool wxWindowMac::SetTransparent(wxByte alpha)
{
#if wxMAC_USE_CORE_GRAPHICS
    if ( alpha != m_macAlpha )
    {
        m_macAlpha = alpha ;
        Refresh() ;
    }
    return true ;
#else
    return false ;
#endif
}


bool wxWindowMac::CanSetTransparent()
{
#if wxMAC_USE_CORE_GRAPHICS
    return true ;
#else
    return false ;
#endif
}

wxByte wxWindowMac::GetTransparent() const 
{
    return m_macAlpha ;
}
