/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindowMac
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/scrolbar.h"
#include "wx/statbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/tooltip.h"
#include "wx/statusbr.h"
#include "wx/menuitem.h"
#include "wx/spinctrl.h"
#include "wx/log.h"
#include "wx/geometry.h"

#include "wx/toolbar.h"
#include "wx/dc.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#define wxWINDOW_HSCROLL 5998
#define wxWINDOW_VSCROLL 5997
#define MAC_SCROLLBAR_SIZE 16

#include "wx/mac/uma.h"
#ifndef __DARWIN__
#include <Windows.h>
#include <ToolUtils.h>
#endif

#if TARGET_API_MAC_OSX
#ifndef __HIVIEW__
	#include <HIToolbox/HIView.h>
#endif
#endif

#if  wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include <string.h>

extern wxList wxPendingDelete;

#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowMac, wxWindowBase)
#else // __WXMAC__
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif // __WXUNIVERSAL__/__WXMAC__

#if !USE_SHARED_LIBRARY

BEGIN_EVENT_TABLE(wxWindowMac, wxWindowBase)
    EVT_NC_PAINT(wxWindowMac::OnNcPaint)
    EVT_ERASE_BACKGROUND(wxWindowMac::OnEraseBackground)
// TODO    EVT_PAINT(wxWindowMac::OnPaint)
    EVT_SYS_COLOUR_CHANGED(wxWindowMac::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindowMac::OnInitDialog)
//    EVT_SET_FOCUS(wxWindowMac::OnSetFocus)
    EVT_MOUSE_EVENTS(wxWindowMac::OnMouseEvent)
END_EVENT_TABLE()

#endif

#define wxMAC_DEBUG_REDRAW 0
#ifndef wxMAC_DEBUG_REDRAW
#define wxMAC_DEBUG_REDRAW 0
#endif

#define wxMAC_USE_THEME_BORDER 0

// ---------------------------------------------------------------------------
// Carbon Events
// ---------------------------------------------------------------------------
 
extern long wxMacTranslateKey(unsigned char key, unsigned char code) ;
pascal OSStatus wxMacSetupControlBackground( ControlRef iControl , SInt16 iMessage , SInt16 iDepth , Boolean iIsColor ) ;

#if TARGET_API_MAC_OSX

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_3 
enum {
  kEventControlVisibilityChanged = 157
};
#endif

#endif

static const EventTypeSpec eventList[] =
{
    { kEventClassControl , kEventControlHit } ,
#if TARGET_API_MAC_OSX
    { kEventClassControl , kEventControlDraw } ,
    { kEventClassControl , kEventControlVisibilityChanged } ,
    { kEventClassControl , kEventControlEnabledStateChanged } ,
    { kEventClassControl , kEventControlHiliteChanged } ,
    { kEventClassControl , kEventControlSetFocusPart } ,
//	{ kEventClassControl , kEventControlInvalidateForSizeChange } , // 10.3 only
//  { kEventClassControl , kEventControlBoundsChanged } ,
#endif
} ;

static pascal OSStatus wxMacWindowControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;
    
    ControlRef controlRef ;
    wxWindowMac* thisWindow = (wxWindowMac*) data ;

    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    switch( GetEventKind( event ) )
    {
#if TARGET_API_MAC_OSX
        case kEventControlDraw :
            {
                RgnHandle updateRgn = NULL ;

                wxRegion visRegion = thisWindow->MacGetVisibleRegion() ;
                if ( cEvent.GetParameter<RgnHandle>(kEventParamRgnHandle, &updateRgn) != noErr ) 
                {
                    updateRgn = (RgnHandle) visRegion.GetWXHRGN() ;
                }
                // GrafPtr myport = cEvent.GetParameter<GrafPtr>(kEventParamGrafPort,typeGrafPtr) ;

#if 0 
              // in case we would need a coregraphics compliant background erase first
                // now usable to track redraws
                CGContextRef cgContext = cEvent.GetParameter<CGContextRef>(kEventParamCGContextRef) ;
                if ( thisWindow->MacIsUserPane() )
                {
                    static float color = 0.5 ;
                    static channel = 0 ;
 	                HIRect bounds;
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
                if ( thisWindow->MacDoRedraw( updateRgn , cEvent.GetTicks() ) )
                    result = noErr ;
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
        case kEventControlSetFocusPart :
            {
                Boolean focusEverything = false ;
                ControlPartCode controlPart = cEvent.GetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode );
                if ( cEvent.GetParameter<Boolean>(kEventParamControlFocusEverything , &focusEverything ) == noErr )
                {
                }
                if ( controlPart == kControlFocusNoPart )
                {
        #if wxUSE_CARET
                    if ( thisWindow->GetCaret() )
                    {
                        thisWindow->GetCaret()->OnKillFocus();
                    }
        #endif // wxUSE_CARET
                    wxFocusEvent event(wxEVT_KILL_FOCUS, thisWindow->GetId());
                    event.SetEventObject(thisWindow);
                    thisWindow->GetEventHandler()->ProcessEvent(event) ;
                }
                else
                {
                    // panel wants to track the window which was the last to have focus in it
                    wxChildFocusEvent eventFocus(thisWindow);
                    thisWindow->GetEventHandler()->ProcessEvent(eventFocus);
                    
        #if wxUSE_CARET
                    if ( thisWindow->GetCaret() )
                    {
                        thisWindow->GetCaret()->OnSetFocus();
                    }
        #endif // wxUSE_CARET

                    wxFocusEvent event(wxEVT_SET_FOCUS, thisWindow->GetId());
                    event.SetEventObject(thisWindow);
                    thisWindow->GetEventHandler()->ProcessEvent(event) ;
                }
            }
            break ;
#endif
        case kEventControlHit :
            {
                result = thisWindow->MacControlHit( handler , event ) ;
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
        case kEventClassControl :
            result = wxMacWindowControlEventHandler( handler, event, data ) ;
            break ;
        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacWindowEventHandler )

// ---------------------------------------------------------------------------
// UserPane events for non OSX builds
// ---------------------------------------------------------------------------
 
static pascal void wxMacControlUserPaneDrawProc(ControlRef control, SInt16 part)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_RET( win , wxT("Callback from unkown control") ) ;
    win->MacControlUserPaneDrawProc(part) ;
}

static pascal ControlPartCode wxMacControlUserPaneHitTestProc(ControlRef control, Point where)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_MSG( win , kControlNoPart , wxT("Callback from unkown control") ) ;
    return win->MacControlUserPaneHitTestProc(where.h , where.v) ;
}

static pascal ControlPartCode wxMacControlUserPaneTrackingProc(ControlRef control, Point startPt, ControlActionUPP actionProc)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_MSG( win , kControlNoPart , wxT("Callback from unkown control") ) ;
    return win->MacControlUserPaneTrackingProc( startPt.h , startPt.v , (void*) actionProc) ;
}

static pascal void wxMacControlUserPaneIdleProc(ControlRef control)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_RET( win , wxT("Callback from unkown control") ) ;
    win->MacControlUserPaneIdleProc() ;
}

static pascal ControlPartCode wxMacControlUserPaneKeyDownProc(ControlRef control, SInt16 keyCode, SInt16 charCode, SInt16 modifiers)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_MSG( win , kControlNoPart , wxT("Callback from unkown control") ) ;
    return win->MacControlUserPaneKeyDownProc(keyCode,charCode,modifiers) ;
}

static pascal void wxMacControlUserPaneActivateProc(ControlRef control, Boolean activating)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_RET( win , wxT("Callback from unkown control") ) ;
    win->MacControlUserPaneActivateProc(activating) ;
}

static pascal ControlPartCode wxMacControlUserPaneFocusProc(ControlRef control, ControlFocusPart action)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_MSG( win , kControlNoPart , wxT("Callback from unkown control") ) ;
    return win->MacControlUserPaneFocusProc(action) ;
}

static pascal void wxMacControlUserPaneBackgroundProc(ControlRef control, ControlBackgroundPtr info)
{
    wxWindow * win = wxFindControlFromMacControl(control) ;
    wxCHECK_RET( win , wxT("Callback from unkown control") ) ;
    win->MacControlUserPaneBackgroundProc(info) ;
}

void wxWindowMac::MacControlUserPaneDrawProc(wxInt16 part) 
{
    RgnHandle rgn = NewRgn() ;
    GetClip( rgn ) ;
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
    return kControlNoPart ;
}

void wxWindowMac::MacControlUserPaneBackgroundProc(void* info) 
{
}

ControlUserPaneDrawUPP gControlUserPaneDrawUPP = NULL ;
ControlUserPaneHitTestUPP gControlUserPaneHitTestUPP = NULL ;
ControlUserPaneTrackingUPP gControlUserPaneTrackingUPP = NULL ;
ControlUserPaneIdleUPP gControlUserPaneIdleUPP = NULL ;
ControlUserPaneKeyDownUPP gControlUserPaneKeyDownUPP = NULL ;
ControlUserPaneActivateUPP gControlUserPaneActivateUPP = NULL ;
ControlUserPaneFocusUPP gControlUserPaneFocusUPP = NULL ;
ControlUserPaneBackgroundUPP gControlUserPaneBackgroundUPP = NULL ;

// ===========================================================================
// implementation
// ===========================================================================

wxList wxWinMacControlList(wxKEY_INTEGER);

wxWindow *wxFindControlFromMacControl(ControlRef inControl )
{
    wxNode *node = wxWinMacControlList.Find((long)inControl);
    if (!node)
        return NULL;
    return (wxControl *)node->GetData();
}

void wxAssociateControlWithMacControl(ControlRef inControl, wxWindow *control)
{
    // adding NULL ControlRef is (first) surely a result of an error and
    // (secondly) breaks native event processing
    wxCHECK_RET( inControl != (ControlRef) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    if ( !wxWinMacControlList.Find((long)inControl) )
        wxWinMacControlList.Append((long)inControl, control);
}

void wxRemoveMacControlAssociation(wxWindow *control)
{
    wxWinMacControlList.DeleteObject(control);
}

// UPP functions
ControlActionUPP wxMacLiveScrollbarActionUPP = NULL ;

ControlColorUPP wxMacSetupControlBackgroundUPP = NULL ;

// we have to setup the brush in the current port and return noErr
// or return an error code so that the control manager walks further up the 
// hierarchy to find a correct background

pascal OSStatus wxMacSetupControlBackground( ControlRef iControl , SInt16 iMessage , SInt16 iDepth , Boolean iIsColor )
{
    OSStatus status = paramErr ;
    switch( iMessage )
    {
        case kControlMsgApplyTextColor :
            break ;
        case kControlMsgSetUpBackground :
            {
                wxWindow*  wx = (wxWindow*) wxFindControlFromMacControl( iControl ) ;
                if ( wx != NULL )
                {
                    /*
                    const wxBrush &brush = wx->MacGetBackgroundBrush() ;
                    if  ( brush.Ok() )
                    {
                        
                        wxDC::MacSetupBackgroundForCurrentPort( brush ) ;
                        */
                        // this clipping is only needed for non HIView
                        
                        RgnHandle clip = NewRgn() ;
                        int x = 0 , y = 0;

                        wx->MacWindowToRootWindow( &x,&y ) ;
                        CopyRgn( (RgnHandle) wx->MacGetVisibleRegion().GetWXHRGN() , clip ) ;
                        OffsetRgn( clip , x , y ) ;
                        SetClip( clip ) ;
                        DisposeRgn( clip ) ;

                        status = noErr ;
                        /*
                    }
                    else if ( wx->MacIsUserPane() )
                    {
                        // if we don't have a valid brush for such a control, we have to call the
                        // setup of our parent ourselves
                        status = SetUpControlBackground( (ControlRef) wx->GetParent()->GetHandle() , iDepth , iIsColor ) ;
                    }
                    */
                }
            }
            break ;
        default :
            break ;
    }
    return status ;
}


pascal void wxMacLiveScrollbarActionProc( ControlRef control , ControlPartCode partCode ) ;
pascal void wxMacLiveScrollbarActionProc( ControlRef control , ControlPartCode partCode )
{
    if ( partCode != 0)
    {
        wxWindow*  wx = wxFindControlFromMacControl( control ) ;
        if ( wx )
        {
            wx->MacHandleControlClick( (WXWidget) control , partCode , true /* stillDown */ ) ;
        }
    }
}

// ----------------------------------------------------------------------------
 // constructors and such
// ----------------------------------------------------------------------------

void wxWindowMac::Init()
{
    m_frozenness = 0 ;
#if WXWIN_COMPATIBILITY_2_4
    m_backgroundTransparent = FALSE;
#endif

    // as all windows are created with WS_VISIBLE style...
    m_isShown = TRUE;

    m_hScrollBar = NULL ;
    m_vScrollBar = NULL ;
    m_macBackgroundBrush = wxNullBrush ;

	m_macControl = NULL ;

    m_macIsUserPane = TRUE;
    
    // make sure all proc ptrs are available
    
    if ( gControlUserPaneDrawUPP == NULL )
    {
        gControlUserPaneDrawUPP = NewControlUserPaneDrawUPP( wxMacControlUserPaneDrawProc ) ;
        gControlUserPaneHitTestUPP = NewControlUserPaneHitTestUPP( wxMacControlUserPaneHitTestProc ) ;
        gControlUserPaneTrackingUPP = NewControlUserPaneTrackingUPP( wxMacControlUserPaneTrackingProc ) ;
        gControlUserPaneIdleUPP = NewControlUserPaneIdleUPP( wxMacControlUserPaneIdleProc ) ;
        gControlUserPaneKeyDownUPP = NewControlUserPaneKeyDownUPP( wxMacControlUserPaneKeyDownProc ) ;
        gControlUserPaneActivateUPP = NewControlUserPaneActivateUPP( wxMacControlUserPaneActivateProc ) ;
        gControlUserPaneFocusUPP = NewControlUserPaneFocusUPP( wxMacControlUserPaneFocusProc ) ;
        gControlUserPaneBackgroundUPP = NewControlUserPaneBackgroundUPP( wxMacControlUserPaneBackgroundProc ) ;
    }
    if ( wxMacLiveScrollbarActionUPP == NULL )
    {
        wxMacLiveScrollbarActionUPP = NewControlActionUPP( wxMacLiveScrollbarActionProc );
    }

    if ( wxMacSetupControlBackgroundUPP == NULL )
    {
        wxMacSetupControlBackgroundUPP = NewControlColorUPP( wxMacSetupControlBackground ) ;
    }

    // we need a valid font for the encodings
    wxWindowBase::SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
}

// Destructor
wxWindowMac::~wxWindowMac()
{
    SendDestroyEvent();

    m_isBeingDeleted = TRUE;

#ifndef __WXUNIVERSAL__
    // VS: make sure there's no wxFrame with last focus set to us:
    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        wxFrame *frame = wxDynamicCast(win, wxFrame);
        if ( frame )
        {
            if ( frame->GetLastFocus() == this )
            {
                frame->SetLastFocus((wxWindow*)NULL);
            }
            break;
        }
    }
#endif // __WXUNIVERSAL__
    
    // wxRemoveMacControlAssociation( this ) ;
    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxWindow *parent = GetParent() ;
    if ( parent )
    {
        if (parent->GetDefaultItem() == (wxButton*) this)
            parent->SetDefaultItem(NULL);
    }
    if ( (ControlRef) m_macControl )
    {
        // in case the callback might be called during destruction
        wxRemoveMacControlAssociation( this) ;
        ::SetControlColorProc( (ControlRef) m_macControl , NULL ) ;
        ::DisposeControl( (ControlRef) m_macControl ) ;
        m_macControl = NULL ;
    }

    if ( g_MacLastWindow == this )
    {
        g_MacLastWindow = NULL ;
    }

    wxFrame* frame = wxDynamicCast( wxGetTopLevelParent( this ) , wxFrame ) ;
    if ( frame )
    {
        if ( frame->GetLastFocus() == this )
            frame->SetLastFocus( NULL ) ;
    }

    DestroyChildren();

    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
        delete m_dropTarget;
        m_dropTarget = NULL;
    }
#endif // wxUSE_DRAG_AND_DROP
}

//

void wxWindowMac::MacInstallEventHandler()
{
    wxAssociateControlWithMacControl( (ControlRef) m_macControl , this ) ;
    InstallControlEventHandler( (ControlRef) m_macControl, GetwxMacWindowEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this, 
        (EventHandlerRef *)&m_macControlEventHandler);

}

// Constructor
bool wxWindowMac::Create(wxWindowMac *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindowMac without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    parent->AddChild(this);

    m_windowVariant = parent->GetWindowVariant() ;
        
    if ( m_macIsUserPane )
    {
        Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
        
        UInt32 features = 0
			| kControlSupportsEmbedding 
//			| kControlSupportsLiveFeedback 
//			| kControlHasSpecialBackground  
//			| kControlSupportsCalcBestRect 
//			| kControlHandlesTracking 
			| kControlSupportsFocus 
//			| kControlWantsActivate 
//			| kControlWantsIdle
			; 

        ::CreateUserPaneControl( MAC_WXHWND(GetParent()->MacGetTopLevelWindowRef()) , &bounds, features , (ControlRef*) &m_macControl); 

        MacPostControlCreate(pos,size) ;
#if !TARGET_API_MAC_OSX
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneDrawProcTag, 
               sizeof(gControlUserPaneDrawUPP),(Ptr) &gControlUserPaneDrawUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneHitTestProcTag, 
               sizeof(gControlUserPaneHitTestUPP),(Ptr) &gControlUserPaneHitTestUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneTrackingProcTag, 
               sizeof(gControlUserPaneTrackingUPP),(Ptr) &gControlUserPaneTrackingUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneIdleProcTag, 
               sizeof(gControlUserPaneIdleUPP),(Ptr) &gControlUserPaneIdleUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneKeyDownProcTag, 
               sizeof(gControlUserPaneKeyDownUPP),(Ptr) &gControlUserPaneKeyDownUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneActivateProcTag, 
               sizeof(gControlUserPaneActivateUPP),(Ptr) &gControlUserPaneActivateUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneFocusProcTag, 
               sizeof(gControlUserPaneFocusUPP),(Ptr) &gControlUserPaneFocusUPP);
        SetControlData((ControlRef) m_macControl,kControlEntireControl,kControlUserPaneBackgroundProcTag, 
               sizeof(gControlUserPaneBackgroundUPP),(Ptr) &gControlUserPaneBackgroundUPP);
#endif        
    }
#ifndef __WXUNIVERSAL__
    // Don't give scrollbars to wxControls unless they ask for them
    if ( (! IsKindOf(CLASSINFO(wxControl)) && ! IsKindOf(CLASSINFO(wxStatusBar))) ||
         (IsKindOf(CLASSINFO(wxControl)) && ( style & wxHSCROLL || style & wxVSCROLL)))
    {
        MacCreateScrollBars( style ) ;
    }
#endif

    wxWindowCreateEvent event(this);
    GetEventHandler()->AddPendingEvent(event);

    return TRUE;
}

void wxWindowMac::MacPostControlCreate(const wxPoint& pos, const wxSize& size)
{
    wxASSERT_MSG( (ControlRef) m_macControl != NULL , wxT("No valid mac control") ) ;

    ::SetControlReference( (ControlRef) m_macControl , (long) this ) ;

    MacInstallEventHandler();

    ControlRef container = (ControlRef) GetParent()->GetHandle() ;
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    ::EmbedControl( (ControlRef) m_macControl , container ) ;

    // adjust font, controlsize etc
    DoSetWindowVariant( m_windowVariant ) ;

#if !TARGET_API_MAC_OSX
    // eventually we can fix some clipping issues be reactivating this hook 
    //if ( m_macIsUserPane )
    // SetControlColorProc( (ControlRef) m_macControl , wxMacSetupControlBackgroundUPP ) ;
#endif
    
    UMASetControlTitle( (ControlRef) m_macControl , wxStripMenuCodes(m_label) , m_font.GetEncoding() ) ;

    wxSize new_size = size ;
    if (!m_macIsUserPane)
    {    
        wxSize best_size( DoGetBestSize() );
        
        if (size.x == -1) {
            new_size.x = best_size.x;
        }
        if (size.y == -1) {
            new_size.y = best_size.y;
        }
        SetSize( pos.x, pos.y , new_size.x, new_size.y,wxSIZE_USE_EXISTING );
    }

    SetCursor( *wxSTANDARD_CURSOR ) ;

}

void wxWindowMac::DoSetWindowVariant( wxWindowVariant variant )
{
    // Don't assert, in case we set the window variant before
    // the window is created
    // wxASSERT( m_macControl != NULL ) ;

    m_windowVariant = variant ;

    if (!m_macControl)
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
	::SetControlData( (ControlRef) m_macControl , kControlEntireControl, kControlSizeTag, sizeof( ControlSize ), &size );

    wxFont font ;
    font.MacCreateThemeFont( themeFont ) ;
    SetFont( font ) ;
}

void wxWindowMac::MacUpdateControlFont() 
{
	ControlFontStyleRec	fontStyle;
	if ( m_font.MacGetThemeFontID() != kThemeCurrentPortFont )
	{
	    switch( m_font.MacGetThemeFontID() )
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
	    fontStyle.font = m_font.MacGetFontNum() ;
	    fontStyle.style = m_font.MacGetFontStyle() ;
	    fontStyle.size = m_font.MacGetFontSize() ;
	    fontStyle.flags = kControlUseFontMask | kControlUseFaceMask | kControlUseSizeMask ;
	}

    fontStyle.just = teJustLeft ;
    fontStyle.flags |= kControlUseJustMask ;
    if ( ( GetWindowStyle() & wxALIGN_MASK ) & wxALIGN_CENTER_HORIZONTAL )
        fontStyle.just = teJustCenter ;
    else if ( ( GetWindowStyle() & wxALIGN_MASK ) & wxALIGN_RIGHT )
        fontStyle.just = teJustRight ;

    
    fontStyle.foreColor = MAC_WXCOLORREF(GetForegroundColour().GetPixel() ) ;
    fontStyle.flags |= kControlUseForeColorMask ;
	
	::SetControlFontStyle( (ControlRef) m_macControl , &fontStyle );
	Refresh() ;
}

bool wxWindowMac::SetFont(const wxFont& font)
{
    bool retval = !wxWindowBase::SetFont( font ) ;
    
    MacUpdateControlFont() ;
    
    return retval;
}

bool wxWindowMac::SetForegroundColour(const wxColour& col )
{
    if ( !wxWindowBase::SetForegroundColour(col) )
        return false ;
        
    MacUpdateControlFont() ;
    
    return true ;
}

bool wxWindowMac::SetBackgroundColour(const wxColour& col )
{
    if ( !wxWindowBase::SetBackgroundColour(col) && m_hasBgCol )
        return false ;

    wxBrush brush ;
    if ( col == wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) )
    {
        brush.MacSetTheme( kThemeBrushDocumentWindowBackground ) ;
    }
    else if (  col == wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE ) )
    {
        brush.MacSetTheme( kThemeBrushDialogBackgroundActive ) ; 
    } 
    else
    {
        brush.SetColour( col ) ;
    }
    MacSetBackgroundBrush( brush ) ;
        
    MacUpdateControlFont() ;
    
    return true ;
}


bool wxWindowMac::MacCanFocus() const
{
#if 0
    // there is currently no way to determinate whether the window is running in full keyboard
    // access mode, therefore we cannot rely on these features yet
    UInt32 features = 0 ;
    GetControlFeatures( (ControlRef) m_macControl , &features ) ;
    return features & ( kControlSupportsFocus | kControlGetsFocusOnClick ) ; 
#endif
    return true ;
}


void wxWindowMac::SetFocus()
{
    if ( AcceptsFocus() )
    {
#if !TARGET_API_MAC_OSX
        wxWindow* former = FindFocus() ;
#endif
        OSStatus err = SetKeyboardFocus(  (WindowRef) MacGetTopLevelWindowRef() , (ControlRef) GetHandle()  , kControlFocusNextPart ) ;
        // as we cannot rely on the control features to find out whether we are in full keyboard mode, we can only
        // leave in case of an error
        if ( err == errCouldntSetFocus )
            return ;

#if !TARGET_API_MAC_OSX
        // emulate carbon events when running under carbonlib where they are not natively available
        if ( former )
        {
            EventRef evRef = NULL ;
            verify_noerr( MacCreateEvent( NULL , kEventClassControl , kEventControlSetFocusPart , TicksToEventTime( TickCount() ) , kEventAttributeUserEvent ,
                &evRef ) );

            wxMacCarbonEvent cEvent( evRef ) ;
            cEvent.SetParameter<ControlRef>( kEventParamDirectObject , (ControlRef) former->GetHandle() ) ;
            cEvent.SetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode , kControlFocusNoPart ) ;
            
            wxMacWindowEventHandler( NULL , evRef , former ) ;
            ReleaseEvent(evRef) ;
        }
        // send new focus event
        {
            EventRef evRef = NULL ;
            verify_noerr( MacCreateEvent( NULL , kEventClassControl , kEventControlSetFocusPart , TicksToEventTime( TickCount() ) , kEventAttributeUserEvent ,
                &evRef ) );

            wxMacCarbonEvent cEvent( evRef ) ;
            cEvent.SetParameter<ControlRef>( kEventParamDirectObject , (ControlRef) GetHandle() ) ;
            cEvent.SetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode , kControlFocusNextPart ) ;
            
            wxMacWindowEventHandler( NULL , evRef , this ) ;
            ReleaseEvent(evRef) ;
        }
#endif
    }
}


void wxWindowMac::DoCaptureMouse()
{
    wxTheApp->s_captureWindow = this ;
}

wxWindow* wxWindowBase::GetCapture()
{
    return wxTheApp->s_captureWindow ;
}

void wxWindowMac::DoReleaseMouse()
{
    wxTheApp->s_captureWindow = NULL ;
}

#if    wxUSE_DRAG_AND_DROP

void wxWindowMac::SetDropTarget(wxDropTarget *pDropTarget)
{
    if ( m_dropTarget != 0 ) {
        delete m_dropTarget;
    }

    m_dropTarget = pDropTarget;
    if ( m_dropTarget != 0 )
    {
        // TODO
    }
}

#endif

// Old style file-manager drag&drop
void wxWindowMac::DragAcceptFiles(bool accept)
{
    // TODO
}

void wxWindowMac::MacGetPositionAndSizeFromControl(int& x, int& y,
                                           int& w, int& h) const 
{
    Rect bounds ;
    GetControlBounds( (ControlRef) m_macControl , &bounds ) ;   


    x = bounds.left ;
    y = bounds.top ;
    w = bounds.right - bounds.left ;
    h = bounds.bottom - bounds.top ;

    wxTopLevelWindow* tlw = wxDynamicCast( this , wxTopLevelWindow ) ;
    if ( tlw )
    {
        Point tlworigin =  { 0 , 0  } ;
        GrafPtr port ;
        bool swapped = QDSwapPort( UMAGetWindowPort( (WindowRef) tlw->MacGetWindowRef() ) , &port ) ;
        ::LocalToGlobal( &tlworigin ) ;
        if ( swapped )
            ::SetPort( port ) ;
        x = tlworigin.h ;
        y = tlworigin.v ;    
    }
}

bool wxWindowMac::MacGetBoundsForControl(const wxPoint& pos,
                                       const wxSize& size,
                                       int& x, int& y,
                                       int& w, int& h , bool adjustOrigin ) const 
{
    x = (int)pos.x;
    y = (int)pos.y;
    // todo the default calls may be used as soon as PostCreateControl Is moved here
    w = size.x ; // WidthDefault( size.x );
    h = size.y ; // HeightDefault( size.y ) ;
#if !TARGET_API_MAC_OSX
    GetParent()->MacWindowToRootWindow( &x , &y ) ;
#endif
    if ( adjustOrigin )
        AdjustForParentClientOrigin( x , y ) ;
    return true ;
}

// Get total size
void wxWindowMac::DoGetSize(int *x, int *y) const
{
#if TARGET_API_MAC_OSX
    int x1 , y1 , w1 ,h1 ;
    MacGetPositionAndSizeFromControl( x1 , y1, w1 ,h1 ) ;
    if(x)   *x = w1 ;
    if(y)   *y = h1 ;
    
#else
    Rect bounds ;
    GetControlBounds( (ControlRef) m_macControl , &bounds ) ;   
    if(x)   *x = bounds.right - bounds.left ;
    if(y)   *y = bounds.bottom - bounds.top ;
#endif
}

void wxWindowMac::DoGetPosition(int *x, int *y) const
{
 #if TARGET_API_MAC_OSX
    int x1 , y1 , w1 ,h1 ;
    MacGetPositionAndSizeFromControl( x1 , y1, w1 ,h1 ) ;
    if ( !IsTopLevel() )
    {
        wxWindow *parent = GetParent();
        if ( parent )
        {
            wxPoint pt(parent->GetClientAreaOrigin());
            x1 -= pt.x ;
            y1 -= pt.y ;
        }
    }
    if(x)   *x = x1 ;
    if(y)   *y = y1 ;
 #else
    Rect bounds ;
    GetControlBounds( (ControlRef) m_macControl , &bounds ) ;   
    wxCHECK_RET( GetParent() , wxT("Missing Parent") ) ;
    
    int xx = bounds.left ;
    int yy = bounds.top ;
    
    if ( !GetParent()->IsTopLevel() )
    {
        GetControlBounds( (ControlRef) GetParent()->GetHandle() , &bounds ) ;
    
        xx -= bounds.left ;
        yy -= bounds.top ;
    }
    
    wxPoint pt(GetParent()->GetClientAreaOrigin());
    xx -= pt.x;
    yy -= pt.y;

    if(x)   *x = xx;
    if(y)   *y = yy;
#endif
}

void wxWindowMac::DoScreenToClient(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    
    wxCHECK_RET( window , wxT("TopLevel Window Missing") ) ;
    
    {
        Point localwhere = {0,0} ;

        if(x) localwhere.h = * x ;
        if(y) localwhere.v = * y ;
        
        wxMacPortSaver s((GrafPtr)GetWindowPort( window )) ;
        ::GlobalToLocal( &localwhere ) ;
        if(x)   *x = localwhere.h ;
        if(y)   *y = localwhere.v ;

    }
    MacRootWindowToWindow( x , y ) ;
    
    wxPoint origin = GetClientAreaOrigin() ;
    if(x)   *x -= origin.x ;
    if(y)   *y -= origin.y ;
}

void wxWindowMac::DoClientToScreen(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    wxCHECK_RET( window , wxT("TopLevel Window Missing") ) ;

    wxPoint origin = GetClientAreaOrigin() ;
    if(x)   *x += origin.x ;
    if(y)   *y += origin.y ;

    MacWindowToRootWindow( x , y ) ;

    {
        Point localwhere = { 0,0 };
        if(x)   localwhere.h = * x ;
        if(y)   localwhere.v = * y ;

        wxMacPortSaver s((GrafPtr)GetWindowPort( window )) ;
        ::LocalToGlobal( &localwhere ) ;
        if(x)   *x = localwhere.h ;
        if(y)   *y = localwhere.v ;
    }
}

void wxWindowMac::MacClientToRootWindow( int *x , int *y ) const
{
    wxPoint origin = GetClientAreaOrigin() ;
    if(x)   *x += origin.x ;
    if(y)   *y += origin.y ;

    MacWindowToRootWindow( x , y ) ;
}

void wxWindowMac::MacRootWindowToClient( int *x , int *y ) const
{
    MacRootWindowToWindow( x , y ) ;

    wxPoint origin = GetClientAreaOrigin() ;
    if(x)   *x -= origin.x ;
    if(y)   *y -= origin.y ;
}

void wxWindowMac::MacWindowToRootWindow( int *x , int *y ) const
{
 #if TARGET_API_MAC_OSX
    HIPoint pt ;
    if ( x ) pt.x = *x ;
    if ( y ) pt.y = *y ;

    if ( !IsTopLevel() )
        HIViewConvertPoint( &pt , (ControlRef) m_macControl , (ControlRef) MacGetTopLevelWindow()->GetHandle()  ) ;
    
    if ( x ) *x = (int) pt.x ;
    if ( y ) *y = (int) pt.y ;
 #else
    if ( !IsTopLevel() )
    {
        Rect bounds ;
        GetControlBounds( (ControlRef) m_macControl , &bounds ) ;   
        if(x)   *x += bounds.left ;
        if(y)   *y += bounds.top ;
    }
#endif
}

void wxWindowMac::MacWindowToRootWindow( short *x , short *y ) const
{
    int x1 , y1 ;
    if ( x ) x1 = *x ;
    if ( y ) y1 = *y ;
    MacWindowToRootWindow( &x1 , &y1 ) ;
    if ( x ) *x = x1 ;
    if ( y ) *y = y1 ;
}

void wxWindowMac::MacRootWindowToWindow( int *x , int *y ) const
{
 #if TARGET_API_MAC_OSX
    HIPoint pt ;
    if ( x ) pt.x = *x ;
    if ( y ) pt.y = *y ;

    if ( !IsTopLevel() )
        HIViewConvertPoint( &pt , (ControlRef) MacGetTopLevelWindow()->GetHandle()  , (ControlRef) m_macControl ) ;
    
    if ( x ) *x = (int) pt.x ;
    if ( y ) *y = (int) pt.y ;
 #else
    if ( !IsTopLevel() )
    {
        Rect bounds ;
        GetControlBounds( (ControlRef) m_macControl , &bounds ) ;   
        if(x)   *x -= bounds.left ;
        if(y)   *y -= bounds.top ;
    }
#endif
}

void wxWindowMac::MacRootWindowToWindow( short *x , short *y ) const
{
    int x1 , y1 ;
    if ( x ) x1 = *x ;
    if ( y ) y1 = *y ;
    MacRootWindowToWindow( &x1 , &y1 ) ;
    if ( x ) *x = x1 ;
    if ( y ) *y = y1 ;
}

void wxWindowMac::MacGetContentAreaInset( int &left , int &top , int &right , int &bottom )
{
    RgnHandle rgn = NewRgn() ;
    Rect content ;
    if ( GetControlRegion( (ControlRef) m_macControl , kControlContentMetaPart , rgn ) == noErr )
    {
        GetRegionBounds( rgn , &content ) ;
        DisposeRgn( rgn ) ;
    }
    else
    {
        GetControlBounds( (ControlRef) m_macControl , &content ) ;
    }
    Rect structure ;
    GetControlBounds( (ControlRef) m_macControl , &structure ) ;
#if !TARGET_API_MAC_OSX    
    OffsetRect( &content , -structure.left , -structure.top ) ;
#endif
    left = content.left - structure.left ;
    top = content.top  - structure.top ;
    right = structure.right - content.right ;
    bottom = structure.bottom - content.bottom ;
}

wxSize wxWindowMac::DoGetSizeFromClientSize( const wxSize & size )  const
{
    wxSize sizeTotal = size;

    RgnHandle rgn = NewRgn() ;

    Rect content ;
    
    if ( GetControlRegion( (ControlRef) m_macControl , kControlContentMetaPart , rgn ) == noErr )
    {
        GetRegionBounds( rgn , &content ) ;
        DisposeRgn( rgn ) ;
    }
    else
    {
        GetControlBounds( (ControlRef) m_macControl , &content ) ;
    }
    Rect structure ;
    GetControlBounds( (ControlRef) m_macControl , &structure ) ;
#if !TARGET_API_MAC_OSX    
    OffsetRect( &content , -structure.left , -structure.top ) ;
#endif

    sizeTotal.x += (structure.right - structure.left) - (content.right - content.left) ;
    sizeTotal.y += (structure.bottom - structure.top) - (content.bottom - content.top ) ; 

    sizeTotal.x += MacGetLeftBorderSize(  ) + MacGetRightBorderSize( ) ;
    sizeTotal.y += MacGetTopBorderSize(  ) + MacGetBottomBorderSize( ) ;

    return sizeTotal;
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowMac::DoGetClientSize(int *x, int *y) const
{
    int ww, hh;

    RgnHandle rgn = NewRgn() ;
    Rect content ;
    if ( GetControlRegion( (ControlRef) m_macControl , kControlContentMetaPart , rgn ) == noErr )
    {
        GetRegionBounds( rgn , &content ) ;
        DisposeRgn( rgn ) ;
    }
    else
    {
        GetControlBounds( (ControlRef) m_macControl , &content ) ;
    }
#if !TARGET_API_MAC_OSX
    Rect structure ;
    GetControlBounds( (ControlRef) m_macControl , &structure ) ;
    OffsetRect( &content , -structure.left , -structure.top ) ;
#endif 
    ww = content.right - content.left ;
    hh = content.bottom - content.top ;
    
    ww -= MacGetLeftBorderSize(  )  + MacGetRightBorderSize(  ) ;
    hh -= MacGetTopBorderSize(  ) + MacGetBottomBorderSize( );

    if ( (m_vScrollBar && m_vScrollBar->IsShown()) || (m_hScrollBar  && m_hScrollBar->IsShown()) )
    {
        int x1 = 0 ;
        int y1 = 0 ;
        int w  ;
        int h  ;
        GetSize( &w , &h ) ;
        
        MacClientToRootWindow( &x1 , &y1 ) ;
        MacClientToRootWindow( &w , &h ) ;

        wxWindowMac *iter = (wxWindowMac*)this ;

        int totW = 10000 , totH = 10000;
        while( iter )
        {
            if ( iter->IsTopLevel() )
            {
                iter->GetSize( &totW , &totH ) ;
                break ;
            }

            iter = iter->GetParent() ;
        }

        if (m_hScrollBar  && m_hScrollBar->IsShown() )
        {
            hh -= MAC_SCROLLBAR_SIZE;
            if ( h-y1 >= totH )
            {
                hh += 1 ;
            }
        }
        if (m_vScrollBar  && m_vScrollBar->IsShown() )
        {
            ww -= MAC_SCROLLBAR_SIZE;
            if ( w-x1 >= totW )
            {
                ww += 1 ;
            }
        }
    }
    if(x)   *x = ww;
    if(y)   *y = hh;

}

bool wxWindowMac::SetCursor(const wxCursor& cursor)
{
    if (m_cursor == cursor)
        return FALSE;

    if (wxNullCursor == cursor)
    {
        if ( ! wxWindowBase::SetCursor( *wxSTANDARD_CURSOR ) )
            return FALSE ;
    }
    else
    {
        if ( ! wxWindowBase::SetCursor( cursor ) )
            return FALSE ;
    }

    wxASSERT_MSG( m_cursor.Ok(),
        wxT("cursor must be valid after call to the base version"));

    /*
    
    TODO why do we have to use current coordinates ?
    
    Point pt ;
    wxWindowMac *mouseWin ;
    GetMouse( &pt ) ;

    // Change the cursor NOW if we're within the correct window

    
    if ( MacGetWindowFromPoint( wxPoint( pt.h , pt.v ) , &mouseWin ) )
    {
        if ( mouseWin == this && !wxIsBusy() )
        {
            m_cursor.MacInstall() ;
        }
    }
    */
    if ( !wxIsBusy() )
    {
        m_cursor.MacInstall() ;
    }

    return TRUE ;
}

#if wxUSE_MENUS
bool wxWindowMac::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->SetInvokingWindow(this);
    menu->UpdateUI();
    ClientToScreen( &x , &y ) ;

    menu->MacBeforeDisplay( true ) ;
    long menuResult = ::PopUpMenuSelect((MenuHandle) menu->GetHMenu() ,y,x, 0) ;
    if ( HiWord(menuResult) != 0 )
    {
        MenuCommand id ;
        GetMenuItemCommandID( GetMenuHandle(HiWord(menuResult)) , LoWord(menuResult) , &id ) ;
        wxMenuItem* item = NULL ;
        wxMenu* realmenu ;
        item = menu->FindItem(id, &realmenu) ;
        if (item->IsCheckable())
        {
            item->Check( !item->IsChecked() ) ;
        }
        menu->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) ;
    }
    menu->MacAfterDisplay( true ) ;

    menu->SetInvokingWindow(NULL);

  return TRUE;
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

#endif // wxUSE_TOOLTIPS

void wxWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    int former_x , former_y , former_w, former_h ;
#if !TARGET_API_MAC_OSX
    DoGetPosition( &former_x , &former_y ) ;
    DoGetSize( &former_w , &former_h ) ;
#else
    MacGetPositionAndSizeFromControl( former_x , former_y , former_w , former_h ) ;
#endif

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
        // we don't adjust twice for the origin
        Rect r = wxMacGetBoundsForControl(this , wxPoint( actualX,actualY), wxSize( actualWidth, actualHeight ) , false ) ;
        bool vis = IsControlVisible( (ControlRef) m_macControl ) ;
#if TARGET_API_MAC_OSX
        // the HIViewSetFrame call itself should invalidate the areas, but when testing with the UnicodeTextCtrl it does not !
        if ( vis )
            SetControlVisibility(  (ControlRef)m_macControl , false , true ) ;
        HIRect hir = { r.left , r.top , r.right - r.left , r.bottom - r.top } ;
        HIViewSetFrame ( (ControlRef) m_macControl , &hir ) ;
        if ( vis )
            SetControlVisibility(  (ControlRef)m_macControl , true , true ) ;
#else
        if ( vis )
            SetControlVisibility(  (ControlRef)m_macControl , false , true ) ;
        SetControlBounds( (ControlRef) m_macControl , &r ) ;
        if ( vis )
            SetControlVisibility(  (ControlRef)m_macControl , true , true ) ;
#endif
        MacRepositionScrollBars() ;
        if ( doMove )
        {
            wxPoint point(actualX,actualY);
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
    short   baselineoffset ;
    int bestWidth, bestHeight ;
    ::GetBestControlRect( (ControlRef) m_macControl , &bestsize , &baselineoffset ) ;

    if ( EmptyRect( &bestsize ) )
    {
        baselineoffset = 0;
        bestsize.left = bestsize.top = 0 ;
        bestsize.right = 16 ;
        bestsize.bottom = 16 ;
        if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
        {
            bestsize.bottom = 16 ;
        }
        else if ( IsKindOf( CLASSINFO( wxSpinButton ) ) )
        {
            bestsize.bottom = 24 ; 
        }
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
//    return wxWindowBase::DoGetBestSize() ;
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
    GetPosition(&currentX, &currentY);

    int currentW,currentH;
    GetSize(&currentW, &currentH);

    // ... and don't do anything (avoiding flicker) if it's already ok
    if ( x == currentX && y == currentY &&
        width == currentW && height == currentH && ( height != -1 && width != -1 ) )
    {
        // TODO REMOVE
        MacRepositionScrollBars() ; // we might have a real position shift
        return;
    }

    if ( x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        x = currentX;
    if ( y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        y = currentY;

    AdjustForParentClientOrigin(x, y, sizeFlags);

    wxSize size(-1, -1);
    if ( width == -1 )
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

    if ( height == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == -1 )
            {
                size = DoGetBestSize();
            }
            //else: already called DoGetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    DoMoveWindow(x, y, width, height);

}

wxPoint wxWindowMac::GetClientAreaOrigin() const
{
    RgnHandle rgn = NewRgn() ;
    Rect content ;
    GetControlRegion( (ControlRef) m_macControl , kControlContentMetaPart , rgn ) ;
    GetRegionBounds( rgn , &content ) ;
    DisposeRgn( rgn ) ;
#if !TARGET_API_MAC_OSX
    // if the content rgn is empty / not supported
    // don't attempt to correct the coordinates to wxWindow relative ones
    if (!::EmptyRect( &content ) )
    {
        Rect structure ;
        GetControlBounds( (ControlRef) m_macControl , &structure ) ;
        OffsetRect( &content , -structure.left , -structure.top ) ;
    }
#endif 

    return wxPoint( content.left + MacGetLeftBorderSize(  ) , content.top + MacGetTopBorderSize(  ) );
}

void wxWindowMac::DoSetClientSize(int clientwidth, int clientheight)
{
    if ( clientheight != -1 || clientheight != -1 )
    {
        int currentclientwidth , currentclientheight ;
        int currentwidth , currentheight ;

        GetClientSize( &currentclientwidth , &currentclientheight ) ;
        GetSize( &currentwidth , &currentheight ) ;

        DoSetSize( -1 , -1 , currentwidth + clientwidth - currentclientwidth ,
            currentheight + clientheight - currentclientheight , wxSIZE_USE_EXISTING ) ;
    }
}

void wxWindowMac::SetTitle(const wxString& title)
{
    m_label = wxStripMenuCodes(title) ;

    if ( m_macControl )
    {
		UMASetControlTitle( (ControlRef) m_macControl , m_label , m_font.GetEncoding() ) ;
    }
    Refresh() ;
}

wxString wxWindowMac::GetTitle() const
{
    return m_label ;
}

bool wxWindowMac::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;
    
    // TODO use visibilityChanged Carbon Event for OSX
    bool former = MacIsReallyShown() ;
    
    SetControlVisibility( (ControlRef) m_macControl , show , true ) ;
    if ( former != MacIsReallyShown() )
        MacPropagateVisibilityChanged() ;
    return TRUE;
}

bool wxWindowMac::Enable(bool enable)
{
    wxASSERT( m_macControl != NULL ) ;
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    bool former = MacIsReallyEnabled() ;
    if ( enable )
        EnableControl( (ControlRef) m_macControl ) ;
    else
        DisableControl( (ControlRef) m_macControl ) ;

    if ( former != MacIsReallyEnabled() )
        MacPropagateEnabledStateChanged() ;
    return TRUE;
}

//  
// status change propagations (will be not necessary for OSX later )
//

void wxWindowMac::MacPropagateVisibilityChanged()
{
#if !TARGET_API_MAC_OSX
    MacVisibilityChanged() ;
    
    wxWindowListNode *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindowMac *child = node->GetData();
        if ( child->IsShown() )
            child->MacPropagateVisibilityChanged(  ) ;
        node = node->GetNext();
    }
#endif
}

void wxWindowMac::MacPropagateEnabledStateChanged( )
{
#if !TARGET_API_MAC_OSX
    MacEnabledStateChanged() ;
    
    wxWindowListNode *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindowMac *child = node->GetData();
        if ( child->IsEnabled() )
            child->MacPropagateEnabledStateChanged() ;
        node = node->GetNext();
    }
#endif
}

void wxWindowMac::MacPropagateHiliteChanged( )
{
#if !TARGET_API_MAC_OSX
    MacHiliteChanged() ;
    
    wxWindowListNode *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindowMac *child = node->GetData();
        // if ( child->IsEnabled() )
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
#if TARGET_API_MAC_OSX
    return IsControlVisible( (ControlRef) m_macControl ) ;
#else
    wxWindow* win = this ;
    while( win->IsShown()  )
    {
        if ( win->IsTopLevel() )
            return true ;
        
        win = win->GetParent() ;
        if ( win == NULL )
            return true ;
            
    } ;
    return false ;
#endif
}

bool wxWindowMac::MacIsReallyEnabled() 
{
    return IsControlEnabled( (ControlRef) m_macControl ) ;
}

bool wxWindowMac::MacIsReallyHilited() 
{
    return IsControlActive( (ControlRef) m_macControl ) ;
}

//
//
//

int wxWindowMac::GetCharHeight() const
{
    wxClientDC dc ( (wxWindowMac*)this ) ;
    return dc.GetCharHeight() ;
}

int wxWindowMac::GetCharWidth() const
{
    wxClientDC dc ( (wxWindowMac*)this ) ;
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
#if TARGET_API_MAC_OSX
    if ( rect == NULL )
        HIViewSetNeedsDisplay( (ControlRef) m_macControl , true ) ; 
    else
    {
        RgnHandle update = NewRgn() ;
        SetRectRgn( update , rect->x , rect->y , rect->x + rect->width , rect->y + rect->height ) ;
        SectRgn( (RgnHandle) MacGetVisibleRegion().GetWXHRGN() , update , update ) ;
        wxPoint origin = GetClientAreaOrigin() ;
        OffsetRgn( update, origin.x , origin.y ) ;        
        HIViewSetNeedsDisplayInRegion( (ControlRef) m_macControl , update , true ) ;
    }
#else
/*
        RgnHandle updateRgn = NewRgn() ;
        if ( rect == NULL )
        {
            CopyRgn( (RgnHandle) MacGetVisibleRegion().GetWXHRGN() , updateRgn ) ;
        }
        else
        {
            SetRectRgn( updateRgn , rect->x , rect->y , rect->x + rect->width , rect->y + rect->height ) ;
            SectRgn( (RgnHandle) MacGetVisibleRegion().GetWXHRGN() , updateRgn , updateRgn ) ;        
        }
        InvalWindowRgn( (WindowRef) MacGetTopLevelWindowRef() , updateRgn ) ;
        DisposeRgn(updateRgn) ;
*/
    if ( IsControlVisible( (ControlRef) m_macControl ) )
    {
        SetControlVisibility( (ControlRef) m_macControl , false , false ) ;
        SetControlVisibility( (ControlRef) m_macControl , true , true ) ;
    }
    /*
    if ( MacGetTopLevelWindow() == NULL )
        return ;

    if ( !IsControlVisible( (ControlRef) m_macControl ) )
    	return ;
 
     wxPoint client = GetClientAreaOrigin();
    int x1 = -client.x;
    int y1 = -client.y;
    int x2 = m_width - client.x;
    int y2 = m_height - client.y;

    if (IsKindOf( CLASSINFO(wxButton)))
    {
        // buttons have an "aura"
        y1 -= 5;
        x1 -= 5;
        y2 += 5;
        x2 += 5;
    }

    Rect clientrect = { y1, x1, y2, x2 };

    if ( rect )
    {
        Rect r = { rect->y , rect->x , rect->y + rect->height , rect->x + rect->width } ;
        SectRect( &clientrect , &r , &clientrect ) ;
    }

    if ( !EmptyRect( &clientrect ) )
    {
      int top = 0 , left = 0 ;

      MacClientToRootWindow( &left , &top ) ;
      OffsetRect( &clientrect , left , top ) ;

      MacGetTopLevelWindow()->MacInvalidate( &clientrect , eraseBack ) ;
    }
    */
#endif
}

void wxWindowMac::Freeze()
{
#if TARGET_API_MAC_OSX
    if ( !m_frozenness++ )
    {
        HIViewSetDrawingEnabled( (HIViewRef) m_macControl , false ) ;
    }
#endif
}

#if TARGET_API_MAC_OSX
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

void wxWindowMac::Thaw()
{
#if TARGET_API_MAC_OSX
    wxASSERT_MSG( m_frozenness > 0, _T("Thaw() without matching Freeze()") );

    if ( !--m_frozenness )
    {
        HIViewSetDrawingEnabled( (HIViewRef) m_macControl , true ) ;
        InvalidateControlAndChildren( (HIViewRef) m_macControl )  ;
        // HIViewSetNeedsDisplay( (HIViewRef) m_macControl , true ) ;
    }
#endif
}

void wxWindowMac::MacRedrawControl()
{
/*
    if ( (ControlRef) m_macControl && MacGetTopLevelWindowRef() && IsControlVisible( (ControlRef) m_macControl ) )
    {
#if TARGET_API_MAC_CARBON
        Update() ;
#else
        wxClientDC dc(this) ;
        wxMacPortSetter helper(&dc) ;
        wxMacWindowClipper clipper(this) ;
        wxDC::MacSetupBackgroundForCurrentPort( MacGetBackgroundBrush() ) ;
        UMADrawControl( (ControlRef) m_macControl ) ;
#endif
    }
*/
}
 
/* TODO
void wxWindowMac::OnPaint(wxPaintEvent& event)
{
    // why don't we skip that here ?
}
*/

wxWindowMac *wxGetActiveWindow()
{
    // actually this is a windows-only concept
    return NULL;
}

// Coordinates relative to the window
void wxWindowMac::WarpPointer (int x_pos, int y_pos)
{
    // We really don't move the mouse programmatically under Mac.
}

void wxWindowMac::OnEraseBackground(wxEraseEvent& event)
{
    if ( m_macBackgroundBrush.Ok() == false || m_macBackgroundBrush.GetStyle() == wxTRANSPARENT )
    {
        event.Skip() ;
    }
    else
        event.GetDC()->Clear() ;
}

void wxWindowMac::OnNcPaint( wxNcPaintEvent& event )
{
    wxWindowDC dc(this) ;
    wxMacPortSetter helper(&dc) ;

    MacPaintBorders( dc.m_macLocalOrigin.x , dc.m_macLocalOrigin.y) ;
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

void wxWindowMac::MacPaintBorders( int left , int top )
{
    if( IsTopLevel() )
        return ;
        
    int major,minor;
    wxGetOsVersion( &major, &minor );

    RGBColor white = { 0xFFFF, 0xFFFF , 0xFFFF } ;
    RGBColor face = { 0xDDDD, 0xDDDD , 0xDDDD } ;
    
    RGBColor darkShadow = { 0x0000, 0x0000 , 0x0000 } ;
    RGBColor lightShadow = { 0x4444, 0x4444 , 0x4444 } ;
    // OS X has lighter border edges than classic:
    if (major >= 10) 
    {
        darkShadow.red 		= 0x8E8E;
        darkShadow.green 	= 0x8E8E;
        darkShadow.blue 	= 0x8E8E;
        lightShadow.red 	= 0xBDBD;
        lightShadow.green 	= 0xBDBD;
        lightShadow.blue 	= 0xBDBD;
	}
    
    PenNormal() ;

    int w , h ;
    GetSize( &w , &h ) ;
    if (HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER) )
    {
#if wxMAC_USE_THEME_BORDER
        Rect rect = { top , left , m_height + top , m_width + left } ;
        SInt32 border = 0 ;
        /*
        GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
        InsetRect( &rect , border , border );
        DrawThemeListBoxFrame(&rect,IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
        */

        DrawThemePrimaryGroup(&rect  ,IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
#else
        bool sunken = HasFlag( wxSUNKEN_BORDER ) ;
        RGBForeColor( &face );
        MoveTo( left + 0 , top + h - 2 );
        LineTo( left + 0 , top + 0 );
        LineTo( left + w - 2 , top + 0 );

        MoveTo( left + 2 , top + h - 3 );
        LineTo( left + w - 3 , top + h - 3 );
        LineTo( left + w - 3 , top + 2 );

        RGBForeColor( sunken ? &face : &darkShadow );
        MoveTo( left + 0 , top + h - 1 );
        LineTo( left + w - 1 , top + h - 1 );
        LineTo( left + w - 1 , top + 0 );

        RGBForeColor( sunken ? &lightShadow : &white );
        MoveTo( left + 1 , top + h - 3 );
        LineTo( left + 1, top + 1 );
        LineTo( left + w - 3 , top + 1 );

        RGBForeColor( sunken ? &white : &lightShadow );
        MoveTo( left + 1 , top + h - 2 );
        LineTo( left + w - 2 , top + h - 2 );
        LineTo( left + w - 2 , top + 1 );

        RGBForeColor( sunken ? &darkShadow : &face );
        MoveTo( left + 2 , top + h - 4 );
        LineTo( left + 2 , top + 2 );
        LineTo( left + w - 4 , top + 2 );
#endif
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
        Rect rect = { top , left , h + top , w + left } ;
        RGBForeColor( &darkShadow ) ;
        FrameRect( &rect ) ;
    }
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
    if ( orient == wxHORIZONTAL )
    {
        if ( m_hScrollBar )
        {
            if ( range == 0 || thumbVisible >= range )
            {
                if ( m_hScrollBar->IsShown() )
                    m_hScrollBar->Show(false) ;
            }
            else
            {
                if ( !m_hScrollBar->IsShown() )
                    m_hScrollBar->Show(true) ;
                m_hScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
            }
        }
    }
    else
    {
        if ( m_vScrollBar )
        {
            if ( range == 0 || thumbVisible >= range )
            {
                if ( m_vScrollBar->IsShown() )
                    m_vScrollBar->Show(false) ;
            }
            else
            {
                if ( !m_vScrollBar->IsShown() )
                    m_vScrollBar->Show(true) ;
                m_vScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
            }
        }
    }
    MacRepositionScrollBars() ;
}

// Does a physical scroll
void wxWindowMac::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    if( dx == 0 && dy ==0 )
        return ;
        

    {
        wxClientDC dc(this) ;
        wxMacPortSetter helper(&dc) ;

        int width , height ;
        GetClientSize( &width , &height ) ;


        wxPoint pos;
        pos.x = pos.y = 0; 
 
        Rect scrollrect;
        // TODO take out the boundaries
        GetControlBounds( (ControlRef) m_macControl, &scrollrect);
        
        RgnHandle updateRgn = NewRgn() ;
        if ( rect )
        {
            Rect r = { dc.YLOG2DEVMAC(rect->y) , dc.XLOG2DEVMAC(rect->x) , dc.YLOG2DEVMAC(rect->y + rect->height) ,
                dc.XLOG2DEVMAC(rect->x + rect->width) } ;
            SectRect( &scrollrect , &r , &scrollrect ) ;
        }
        ScrollRect( &scrollrect , dx , dy , updateRgn ) ;
#if TARGET_CARBON
        //KO: The docs say ScrollRect creates an update region, which thus calls an update event
        // but it seems the update only refreshes the background of the control, rather than calling 
        // kEventControlDraw, so we need to force a proper update here. There has to be a better 
        // way of doing this... (Note that code below under !TARGET_CARBON does not work either...)
        Update();
#endif        
        // we also have to scroll the update rgn in this rectangle 
        // in order not to loose updates
#if !TARGET_CARBON
        WindowRef rootWindow = (WindowRef) MacGetTopLevelWindowRef() ;
        RgnHandle formerUpdateRgn = NewRgn() ;
        RgnHandle scrollRgn = NewRgn() ;
        RectRgn( scrollRgn , &scrollrect ) ;
        GetWindowUpdateRgn( rootWindow , formerUpdateRgn ) ;
        Point pt = {0,0} ;
        LocalToGlobal( &pt ) ;
        OffsetRgn( formerUpdateRgn , -pt.h , -pt.v ) ;
        SectRgn( formerUpdateRgn , scrollRgn , formerUpdateRgn ) ;
        if ( !EmptyRgn( formerUpdateRgn ) )
        {
            MacOffsetRgn( formerUpdateRgn , dx , dy ) ;
            SectRgn( formerUpdateRgn , scrollRgn , formerUpdateRgn ) ;
            InvalWindowRgn(rootWindow  ,  formerUpdateRgn ) ;
        }
        InvalWindowRgn(rootWindow  ,  updateRgn ) ;
        DisposeRgn( updateRgn ) ;
        DisposeRgn( formerUpdateRgn ) ;
        DisposeRgn( scrollRgn ) ;
#endif
    }

    for (wxWindowListNode *node = GetChildren().GetFirst(); node; node = node->GetNext())
    {
        wxWindowMac *child = node->GetData();
        if (child == m_vScrollBar) continue;
        if (child == m_hScrollBar) continue;
        if (child->IsTopLevel()) continue;

        int x,y;
        child->GetPosition( &x, &y );
        int w,h;
        child->GetSize( &w, &h );
        if (rect)
        {
            wxRect rc(x,y,w,h);
            if (rect->Intersects(rc))
                child->SetSize( x+dx, y+dy, w, h );
        }
        else
        {
            child->SetSize( x+dx, y+dy, w, h );	    	
        }        
    }
    
// TODO remove, was moved higher up    Update() ;

}

void wxWindowMac::MacOnScroll(wxScrollEvent &event )
{
    if ( event.m_eventObject == m_vScrollBar || event.m_eventObject == m_hScrollBar )
    {
        wxScrollWinEvent wevent;
        wevent.SetPosition(event.GetPosition());
        wevent.SetOrientation(event.GetOrientation());
        wevent.m_eventObject = this;

        if (event.m_eventType == wxEVT_SCROLL_TOP)
            wevent.m_eventType = wxEVT_SCROLLWIN_TOP;
        else if (event.m_eventType == wxEVT_SCROLL_BOTTOM)
            wevent.m_eventType = wxEVT_SCROLLWIN_BOTTOM;
        else if (event.m_eventType == wxEVT_SCROLL_LINEUP)
            wevent.m_eventType = wxEVT_SCROLLWIN_LINEUP;
        else if (event.m_eventType == wxEVT_SCROLL_LINEDOWN)
            wevent.m_eventType = wxEVT_SCROLLWIN_LINEDOWN;
        else if (event.m_eventType == wxEVT_SCROLL_PAGEUP)
            wevent.m_eventType = wxEVT_SCROLLWIN_PAGEUP;
        else if (event.m_eventType == wxEVT_SCROLL_PAGEDOWN)
            wevent.m_eventType = wxEVT_SCROLLWIN_PAGEDOWN;
        else if (event.m_eventType == wxEVT_SCROLL_THUMBTRACK)
            wevent.m_eventType = wxEVT_SCROLLWIN_THUMBTRACK;
        else if (event.m_eventType == wxEVT_SCROLL_THUMBRELEASE)
            wevent.m_eventType = wxEVT_SCROLLWIN_THUMBRELEASE;

        GetEventHandler()->ProcessEvent(wevent);
    }
}

// Get the window with the focus
wxWindowMac *wxWindowBase::FindFocus()
{
    ControlRef control ;
    GetKeyboardFocus( GetUserFocusWindow() , &control ) ;
    return wxFindControlFromMacControl( control ) ;
}

void wxWindowMac::OnSetFocus(wxFocusEvent& event)
{
    // panel wants to track the window which was the last to have focus in it,
    // so we want to set ourselves as the window which last had focus
    //
    // notice that it's also important to do it upwards the tree becaus
    // otherwise when the top level panel gets focus, it won't set it back to
    // us, but to some other sibling

    // CS:don't know if this is still needed:
    //wxChildFocusEvent eventFocus(this);
    //(void)GetEventHandler()->ProcessEvent(eventFocus);

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
#if TARGET_API_MAC_OSX
    HIViewSetZOrder((ControlRef)m_macControl,kHIViewZOrderAbove, NULL) ;
#endif
}

// Lower the window to the bottom of the Z order
void wxWindowMac::Lower()
{
#if TARGET_API_MAC_OSX
    HIViewSetZOrder((ControlRef)m_macControl,kHIViewZOrderBelow, NULL) ;
#endif
}


// static wxWindow *gs_lastWhich = NULL;

bool wxWindowMac::MacSetupCursor( const wxPoint& pt)
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
            {
                cursor = m_cursor ;
            }
            if ( wxIsBusy() )
            {
            }
            else
            {
                if ( !GetParent() )
                    cursor = *wxSTANDARD_CURSOR  ;
            }
        }
        if ( cursor.Ok() )
            cursor.MacInstall() ;
    }
    return cursor.Ok() ;
}

wxString wxWindowMac::MacGetToolTipString( wxPoint &pt )
{
    if ( m_tooltip )
    {
        return m_tooltip->GetTip() ;
    }
    return wxEmptyString ;
}

void wxWindowMac::Update()
{
#if TARGET_API_MAC_OSX
    HIViewSetNeedsDisplay( (ControlRef) m_macControl , true ) ;
#else
    ::Draw1Control( (ControlRef) m_macControl ) ;
#endif
}

wxTopLevelWindowMac* wxWindowMac::MacGetTopLevelWindow() const
{
    wxTopLevelWindowMac* win = NULL ;
    WindowRef window = (WindowRef) MacGetTopLevelWindowRef() ;
    if ( window )
    {
        win = wxFindWinFromMacWindow( window ) ;
    }
    return win ;
}
wxRegion wxWindowMac::MacGetVisibleRegion( bool includeOuterStructures )
{

    Rect r ;
    RgnHandle visRgn = NewRgn() ;
    RgnHandle tempRgn = NewRgn() ;
    if ( IsControlVisible( (ControlRef) m_macControl ) )
    {
        GetControlBounds( (ControlRef) m_macControl , &r ) ;
        if (! MacGetTopLevelWindow()->MacUsesCompositing() )
        {
            MacRootWindowToWindow( &r.left , & r.top ) ;
            MacRootWindowToWindow( &r.right , & r.bottom ) ;
        }
        else
        {
            r.right -= r.left ;
            r.bottom -= r.top ;
            r.left = 0 ;
            r.top = 0 ;
        }
        if ( includeOuterStructures )
            InsetRect( &r , -3 , -3 ) ;
        RectRgn( visRgn , &r ) ;
        if ( !IsTopLevel() )
        {
            wxWindow* child = this ;
            wxWindow* parent = child->GetParent() ;
            while( parent )
            {
                int x , y ;
                wxSize size ;
                // we have to find a better clipping algorithm here, in order not to clip things
                // positioned like status and toolbar
                if ( 1 /* parent->IsTopLevel() && child->IsKindOf( CLASSINFO( wxToolBar ) ) */ )
                {
                    size = parent->GetSize() ;
                    x = y = 0 ;
                }
                else
                {
                    size = parent->GetClientSize() ;
                    wxPoint origin = parent->GetClientAreaOrigin() ;
                    x = origin.x ;
                    y = origin.y ;
                }
                parent->MacWindowToRootWindow( &x, &y ) ;
                MacRootWindowToWindow( &x , &y ) ;

                SetRectRgn( tempRgn ,
                    x + parent->MacGetLeftBorderSize() , y + parent->MacGetTopBorderSize() ,
                    x + size.x - parent->MacGetRightBorderSize(),
                    y + size.y - parent->MacGetBottomBorderSize()) ;

                SectRgn( visRgn , tempRgn , visRgn ) ;
                if ( parent->IsTopLevel() )
                    break ;
                child = parent ;
                parent = child->GetParent() ;
            }
        }
    }

    wxRegion vis = visRgn ;
    DisposeRgn( visRgn ) ;
    DisposeRgn( tempRgn ) ;
    return vis ;
}

/*
    This function must not change the updatergn !
 */
bool wxWindowMac::MacDoRedraw( WXHRGN updatergnr , long time ) 
{
    RgnHandle updatergn = (RgnHandle) updatergnr ;
    bool handled = false ;
    
    // calculate a client-origin version of the update rgn and set m_updateRegion to that
    {
        RgnHandle newupdate = NewRgn() ;
        wxSize point = GetClientSize() ;
        wxPoint origin = GetClientAreaOrigin() ;
        SetRectRgn( newupdate , origin.x , origin.y , origin.x + point.x , origin.y+point.y ) ;
        SectRgn( newupdate , updatergn , newupdate ) ;
        OffsetRgn( newupdate , -origin.x , -origin.y ) ;
        m_updateRegion = newupdate ;
        DisposeRgn( newupdate ) ; 
    }

    if ( !EmptyRgn(updatergn) )
    {
        wxWindowDC dc(this);
        if (!EmptyRgn(updatergn))
            dc.SetClippingRegion(wxRegion(updatergn));
        
        wxEraseEvent eevent( GetId(), &dc );
        eevent.SetEventObject( this );
        GetEventHandler()->ProcessEvent( eevent );
 
        if ( !m_updateRegion.Empty() )
        {
            // paint the window itself
            wxPaintEvent event;
            event.m_timeStamp = time ;
            event.SetEventObject(this);
            handled = GetEventHandler()->ProcessEvent(event); 

            // paint custom borders
            wxNcPaintEvent eventNc( GetId() );
            eventNc.SetEventObject( this );
            GetEventHandler()->ProcessEvent( eventNc );
        }
    }
    return handled ;
}

void wxWindowMac::MacRedraw( WXHRGN updatergnr , long time, bool erase)
{
    RgnHandle updatergn = (RgnHandle) updatergnr ;
    // updatergn is always already clipped to our boundaries
    // if we are in compositing mode then it is in relative to the upper left of the control
    // if we are in non-compositing, then it is relatvie to the uppder left of the content area
    // of the toplevel window
    // it is in window coordinates, not in client coordinates

    // ownUpdateRgn is the area that this window has to repaint, it is in window coordinates
    RgnHandle ownUpdateRgn = NewRgn() ;
    CopyRgn( updatergn , ownUpdateRgn ) ;

    if ( MacGetTopLevelWindow()->MacUsesCompositing() == false )
    { 
        Rect bounds;
        UMAGetControlBoundsInWindowCoords( (ControlRef)m_macControl, &bounds );
        RgnHandle controlRgn = NewRgn();
        RectRgn( controlRgn, &bounds );
        //KO: This sets the ownUpdateRgn to the area of this control that is inside
        // the window update region
        SectRgn( ownUpdateRgn, controlRgn, ownUpdateRgn ); 
        DisposeRgn( controlRgn );
    
        //KO: convert ownUpdateRgn to local coordinates
        OffsetRgn( ownUpdateRgn, -bounds.left, -bounds.top ); 
    }
        
    MacDoRedraw( ownUpdateRgn , time ) ;
    DisposeRgn( ownUpdateRgn ) ;

}

WXWindow wxWindowMac::MacGetTopLevelWindowRef() const
{
    wxWindowMac *iter = (wxWindowMac*)this ;

    while( iter )
    {
        if ( iter->IsTopLevel() )
            return ((wxTopLevelWindow*)iter)->MacGetWindowRef() ;

        iter = iter->GetParent() ;
    }
    wxASSERT_MSG( 1 , wxT("No valid mac root window") ) ;
    return NULL ;
}

void wxWindowMac::MacCreateScrollBars( long style )
{
    wxASSERT_MSG( m_vScrollBar == NULL && m_hScrollBar == NULL , wxT("attempt to create window twice") ) ;

    bool hasBoth = ( style & wxVSCROLL ) && ( style & wxHSCROLL ) ;
    int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1: 0 ;
    int width, height ;
    GetClientSize( &width , &height ) ;

    wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
    wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
    wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
    wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;

    m_vScrollBar = new wxScrollBar(this, wxWINDOW_VSCROLL, vPoint,
        vSize , wxVERTICAL);

    if ( style & wxVSCROLL )
    {

    }
    else
    {
        m_vScrollBar->Show(false) ;
    }
    m_hScrollBar = new wxScrollBar(this, wxWINDOW_HSCROLL, hPoint,
        hSize , wxHORIZONTAL);
    if ( style  & wxHSCROLL )
    {
    }
    else
    {
        m_hScrollBar->Show(false) ;
    }

    // because the create does not take into account the client area origin
    MacRepositionScrollBars() ; // we might have a real position shift
}

void wxWindowMac::MacRepositionScrollBars()
{
    bool hasBoth = ( m_hScrollBar && m_hScrollBar->IsShown()) && ( m_vScrollBar && m_vScrollBar->IsShown()) ;
    int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1 : 0 ;

    // get real client area

    int width  ;
    int height  ;
    GetSize( &width , &height ) ;

    width -= MacGetLeftBorderSize() + MacGetRightBorderSize();
    height -= MacGetTopBorderSize() + MacGetBottomBorderSize();

    wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
    wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
    wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
    wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;

    int x = 0 ;
    int y = 0 ;
    int w ;
    int h ;
    GetSize( &w , &h ) ;

    MacClientToRootWindow( &x , &y ) ;
    MacClientToRootWindow( &w , &h ) ;

    wxWindowMac *iter = (wxWindowMac*)this ;

    int totW = 10000 , totH = 10000;
    while( iter )
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

    if ( w-x >= totW )
    {
        hSize.x += 1 ;
        vPoint.x += 1 ;
    }

    if ( h-y >= totH )
    {
        vSize.y += 1 ;
        hPoint.y += 1 ;
    }

    if ( m_vScrollBar )
    {
        m_vScrollBar->SetSize( vPoint.x , vPoint.y, vSize.x, vSize.y , wxSIZE_ALLOW_MINUS_ONE);
    }
    if ( m_hScrollBar )
    {
        m_hScrollBar->SetSize( hPoint.x , hPoint.y, hSize.x, hSize.y, wxSIZE_ALLOW_MINUS_ONE);
    }
}

bool wxWindowMac::AcceptsFocus() const
{
    return MacCanFocus() && wxWindowBase::AcceptsFocus();
}

void wxWindowMac::MacSuperChangedPosition()
{
    // only window-absolute structures have to be moved i.e. controls

    wxWindowListNode *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindowMac *child = node->GetData();
        child->MacSuperChangedPosition() ;
        node = node->GetNext();
    }
}

void wxWindowMac::MacTopLevelWindowChangedPosition()
{
    // only screen-absolute structures have to be moved i.e. glcanvas

    wxWindowListNode *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindowMac *child = node->GetData();
        child->MacTopLevelWindowChangedPosition() ;
        node = node->GetNext();
    }
}

long wxWindowMac::MacGetLeftBorderSize( ) const
{
    if( IsTopLevel() )
        return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
        SInt32 border = 3 ;
#if wxMAC_USE_THEME_BORDER
          GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
#endif
          return border ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
          SInt32 border = 3 ;
#if wxMAC_USE_THEME_BORDER
          GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
#endif
          return border ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
        return 1 ;
    }
    return 0 ;
}

long wxWindowMac::MacGetRightBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetTopBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetBottomBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacRemoveBordersFromStyle( long style )
{
    return style & ~( wxDOUBLE_BORDER | wxSUNKEN_BORDER | wxRAISED_BORDER | wxBORDER | wxSTATIC_BORDER ) ;
}

// Find the wxWindowMac at the current mouse position, returning the mouse
// position.
wxWindowMac* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    wxWindowMac* found = wxFindWindowAtPoint(pt);
    return found;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    int x, y;
    wxGetMousePosition(& x, & y);
    return wxPoint(x, y);
}

void wxWindowMac::OnMouseEvent( wxMouseEvent &event )
{
	if ( event.GetEventType() == wxEVT_RIGHT_DOWN )
	{
		// copied from wxGTK : CS
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

void wxWindowMac::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED( mouseStillDown ) ) 
{
}

Rect wxMacGetBoundsForControl( wxWindow* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin ) 
{
    int x ,y , w ,h ;
    
    window->MacGetBoundsForControl( pos , size , x , y, w, h , adjustForOrigin) ;
    Rect bounds =  { y , x , y+h , x+w  };
    return bounds ;
}

wxInt32 wxWindowMac::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    return eventNotHandledErr ;
}


