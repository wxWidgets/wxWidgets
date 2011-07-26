/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/webview_webkit.mm
// Purpose:     wxWebViewWebKit - embeddable web kit control,
//                             OS X implementation of web view component
// Author:      Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Modified by:
// Created:     2004-4-16
// RCS-ID:      $Id$
// Copyright:   (c) Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// http://developer.apple.com/mac/library/documentation/Cocoa/Reference/WebKit/Classes/WebView_Class/Reference/Reference.html

#include "wx/osx/webview_webkit.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#if wxHAVE_WEB_BACKEND_OSX_WEBKIT

#ifdef __WXCOCOA__
#include "wx/cocoa/autorelease.h"
#else
#include "wx/osx/private.h"

#include <WebKit/WebKit.h>
#include <WebKit/HIWebView.h>
#include <WebKit/CarbonUtils.h>
#endif

#include <Foundation/NSURLError.h>

// FIXME: find cleaner way to find the wxWidgets ID of a webview than this hack
#include <map>
std::map<WebView*, wxWebViewWebKit*> wx_webviewctrls;

#define DEBUG_WEBKIT_SIZING 0

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWebViewWebKit, wxControl)

BEGIN_EVENT_TABLE(wxWebViewWebKit, wxControl)
#if defined(__WXMAC__) && wxOSX_USE_CARBON
    EVT_SIZE(wxWebViewWebKit::OnSize)
#endif
END_EVENT_TABLE()

#if defined(__WXOSX__) && wxOSX_USE_CARBON

// ----------------------------------------------------------------------------
// Carbon Events handlers
// ----------------------------------------------------------------------------

// prototype for function in src/osx/carbon/nonownedwnd.cpp
void SetupMouseEvent( wxMouseEvent &wxevent , wxMacCarbonEvent &cEvent );

static const EventTypeSpec eventList[] =
{
    //{ kEventClassControl, kEventControlTrack } ,
    { kEventClassMouse, kEventMouseUp },
    { kEventClassMouse, kEventMouseDown },
    { kEventClassMouse, kEventMouseMoved },
    { kEventClassMouse, kEventMouseDragged },

    { kEventClassKeyboard, kEventRawKeyDown } ,
    { kEventClassKeyboard, kEventRawKeyRepeat } ,
    { kEventClassKeyboard, kEventRawKeyUp } ,
    { kEventClassKeyboard, kEventRawKeyModifiersChanged } ,

    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,
    { kEventClassTextInput, kEventTextInputUpdateActiveInputArea } ,

#if DEBUG_WEBKIT_SIZING == 1
    { kEventClassControl, kEventControlBoundsChanged } ,
#endif
};

// mix this in from window.cpp
pascal OSStatus wxMacUnicodeTextEventHandler(EventHandlerCallRef handler,
                                             EventRef event, void *data) ;

// NOTE: This is mostly taken from KeyboardEventHandler in toplevel.cpp, but
// that expects the data pointer is a top-level window, so I needed to change
// that in this case. However, once 2.8 is out, we should factor out the common
// logic among the two functions and merge them.
static pascal OSStatus wxWebKitKeyEventHandler(EventHandlerCallRef handler,
                                               EventRef event, void *data)
{
    OSStatus result = eventNotHandledErr ;
    wxMacCarbonEvent cEvent( event ) ;

    wxWebViewWebKit* thisWindow = (wxWebViewWebKit*) data ;
    wxWindow* focus = thisWindow ;

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
    if ( GetEventParameter(event, kEventParamKeyUnicodes, typeUnicodeText,
                           NULL, 0 , &dataSize, NULL ) == noErr)
    {
        UniChar buf[2] ;
        int numChars = dataSize / sizeof( UniChar) + 1;

        UniChar* charBuf = buf ;

        if ( numChars * 2 > 4 )
            charBuf = new UniChar[ numChars ] ;
        GetEventParameter(event, kEventParamKeyUnicodes, typeUnicodeText, NULL,
                          dataSize , NULL , charBuf) ;
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

    GetEventParameter(event, kEventParamKeyMacCharCodes, typeChar, NULL,
                      sizeof(char), NULL, &charCode );
    GetEventParameter(event, kEventParamKeyCode, typeUInt32, NULL,
                      sizeof(UInt32), NULL, &keyCode );
    GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL,
                      sizeof(UInt32), NULL, &modifiers );
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL,
                      sizeof(Point), NULL, &point );

    UInt32 message = (keyCode << 8) + charCode;
    switch ( GetEventKind( event ) )
    {
        case kEventRawKeyRepeat :
        case kEventRawKeyDown :
        {
            WXEVENTREF formerEvent = wxTheApp->MacGetCurrentEvent() ;
            WXEVENTHANDLERCALLREF formerHandler =
                wxTheApp->MacGetCurrentEventHandlerCallRef() ;

            wxTheApp->MacSetCurrentEvent( event , handler ) ;
            if ( /* focus && */ wxTheApp->MacSendKeyDownEvent(
                focus, message, modifiers, when, point.h, point.v, uniChar[0]))
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

static pascal OSStatus wxWebViewWebKitEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    wxWebViewWebKit* thisWindow = (wxWebViewWebKit*) data ;
    wxNonOwnedWindow* tlw = NULL;
    if (thisWindow)
        tlw = thisWindow->MacGetTopLevelWindow();

    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    wxWindow* currentMouseWindow = thisWindow ;

    if ( wxApp::s_captureWindow )
        currentMouseWindow = wxApp::s_captureWindow;

    switch ( GetEventClass( event ) )
    {
        case kEventClassKeyboard:
        {
            result = wxWebKitKeyEventHandler(handler, event, data);
            break;
        }

        case kEventClassTextInput:
        {
            result = wxMacUnicodeTextEventHandler(handler, event, data);
            break;
        }

        case kEventClassMouse:
        {
            switch ( GetEventKind( event ) )
            {
                case kEventMouseDragged :
                case kEventMouseMoved :
                case kEventMouseDown :
                case kEventMouseUp :
                {
                    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
                    SetupMouseEvent( wxevent , cEvent ) ;

                    currentMouseWindow->ScreenToClient( &wxevent.m_x , &wxevent.m_y ) ;
                    wxevent.SetEventObject( currentMouseWindow ) ;
                    wxevent.SetId( currentMouseWindow->GetId() ) ;

                    if ( currentMouseWindow->GetEventHandler()->ProcessEvent(wxevent) )
                    {
                        result = noErr;
                    }

                    break; // this should enable WebKit to fire mouse dragged and mouse up events...
                }
                default :
                    break ;
            }
        }
        default:
            break;
    }

    result = CallNextEventHandler(handler, event);
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxWebViewWebKitEventHandler )

#endif

//---------------------------------------------------------
// helper functions for NSString<->wxString conversion
//---------------------------------------------------------

inline wxString wxStringWithNSString(NSString *nsstring)
{
#if wxUSE_UNICODE
    return wxString([nsstring UTF8String], wxConvUTF8);
#else
    return wxString([nsstring lossyCString]);
#endif // wxUSE_UNICODE
}

inline NSString* wxNSStringWithWxString(const wxString &wxstring)
{
#if wxUSE_UNICODE
    return [NSString stringWithUTF8String: wxstring.mb_str(wxConvUTF8)];
#else
    return [NSString stringWithCString: wxstring.c_str() length:wxstring.Len()];
#endif // wxUSE_UNICODE
}

@interface MyFrameLoadMonitor : NSObject
{
    wxWebViewWebKit* webKitWindow;
}

- initWithWxWindow: (wxWebViewWebKit*)inWindow;

@end

@interface MyPolicyDelegate : NSObject
{
    wxWebViewWebKit* webKitWindow;
}

- initWithWxWindow: (wxWebViewWebKit*)inWindow;

@end

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxWebViewWebKit::Create(wxWindow *parent,
                                 wxWindowID winID,
                                 const wxString& strURL,
                                 const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxString& name)
{
    m_busy = false;
    //m_pageTitle = _("Untitled Page");

 //still needed for wxCocoa??
/*
    int width, height;
    wxSize sizeInstance;
    if (size.x == wxDefaultCoord || size.y == wxDefaultCoord)
    {
        m_parent->GetClientSize(&width, &height);
        sizeInstance.x = width;
        sizeInstance.y = height;
    }
    else
    {
        sizeInstance.x = size.x;
        sizeInstance.y = size.y;
    }
*/
    // now create and attach WebKit view...
#ifdef __WXCOCOA__
    wxControl::Create(parent, m_windowID, pos, sizeInstance, style, name);
    SetSize(pos.x, pos.y, sizeInstance.x, sizeInstance.y);

    wxTopLevelWindowCocoa *topWin = wxDynamicCast(this, wxTopLevelWindowCocoa);
    NSWindow* nsWin = topWin->GetNSWindow();
    NSRect rect;
    rect.origin.x = pos.x;
    rect.origin.y = pos.y;
    rect.size.width = sizeInstance.x;
    rect.size.height = sizeInstance.y;
    m_webView = (WebView*)[[WebView alloc] initWithFrame:rect
                                               frameName:@"webkitFrame"
                                               groupName:@"webkitGroup"];
    SetNSView(m_webView);
    [m_cocoaNSView release];

    if(m_parent) m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,sizeInstance);
#else
    wxControl::Create(parent, winID, pos, size, style, wxDefaultValidator, name);

#if wxOSX_USE_CARBON
    m_peer = new wxMacControl(this);
    WebInitForCarbon();
    HIWebViewCreate( m_peer->GetControlRefAddr() );

    m_webView = (WebView*) HIWebViewGetWebView( m_peer->GetControlRef() );

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    if ( UMAGetSystemVersion() >= 0x1030 )
        HIViewChangeFeatures( m_peer->GetControlRef() , kHIViewIsOpaque , 0 ) ;
#endif
    InstallControlEventHandler(m_peer->GetControlRef(),
                               GetwxWebViewWebKitEventHandlerUPP(),
                               GetEventTypeCount(eventList), eventList, this,
                              (EventHandlerRef *)&m_webKitCtrlEventHandler);
#else
    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    m_webView = [[WebView alloc] initWithFrame:r
                                     frameName:@"webkitFrame"
                                     groupName:@"webkitGroup"];
    m_peer = new wxWidgetCocoaImpl( this, m_webView );
#endif

    wx_webviewctrls[m_webView] = this;

    MacPostControlCreate(pos, size);

#if wxOSX_USE_CARBON
    HIViewSetVisible( m_peer->GetControlRef(), true );
#endif
    [m_webView setHidden:false];

#endif

    // Register event listener interfaces
    MyFrameLoadMonitor* myFrameLoadMonitor =
            [[MyFrameLoadMonitor alloc] initWithWxWindow: this];

    [m_webView setFrameLoadDelegate:myFrameLoadMonitor];

    // this is used to veto page loads, etc.
    MyPolicyDelegate* myPolicyDelegate =
            [[MyPolicyDelegate alloc] initWithWxWindow: this];

    [m_webView setPolicyDelegate:myPolicyDelegate];

    InternalLoadURL(strURL);
    return true;
}

wxWebViewWebKit::~wxWebViewWebKit()
{
    MyFrameLoadMonitor* myFrameLoadMonitor = [m_webView frameLoadDelegate];
    MyPolicyDelegate* myPolicyDelegate = [m_webView policyDelegate];
    [m_webView setFrameLoadDelegate: nil];
    [m_webView setPolicyDelegate: nil];

    if (myFrameLoadMonitor)
        [myFrameLoadMonitor release];

    if (myPolicyDelegate)
        [myPolicyDelegate release];
}

// ----------------------------------------------------------------------------
// public methods
// ----------------------------------------------------------------------------

void wxWebViewWebKit::InternalLoadURL(const wxString &url)
{
    if( !m_webView )
        return;

    [[m_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:
            [NSURL URLWithString:wxNSStringWithWxString(url)]]];
}

bool wxWebViewWebKit::CanGoBack()
{
    if ( !m_webView )
        return false;

    return [m_webView canGoBack];
}

bool wxWebViewWebKit::CanGoForward()
{
    if ( !m_webView )
        return false;

    return [m_webView canGoForward];
}

void wxWebViewWebKit::GoBack()
{
    if ( !m_webView )
        return;

    bool result = [(WebView*)m_webView goBack];

    // TODO: return result (if it also exists in other backends...)
    //return result;
}

void wxWebViewWebKit::GoForward()
{
    if ( !m_webView )
        return;

    bool result = [(WebView*)m_webView goForward];

    // TODO: return result (if it also exists in other backends...)
    //return result;
}

void wxWebViewWebKit::Reload(wxWebViewReloadFlags flags)
{
    if ( !m_webView )
        return;

    if (flags & wxWEB_VIEW_RELOAD_NO_CACHE)
    {
        // TODO: test this indeed bypasses the cache
        [[m_webView preferences] setUsesPageCache:NO];
        [[m_webView mainFrame] reload];
        [[m_webView preferences] setUsesPageCache:YES];
    }
    else
    {
        [[m_webView mainFrame] reload];
    }
}

void wxWebViewWebKit::Stop()
{
    if ( !m_webView )
        return;

    [[m_webView mainFrame] stopLoading];
}

bool wxWebViewWebKit::CanGetPageSource()
{
    if ( !m_webView )
        return false;

    WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
    return ( [[dataSource representation] canProvideDocumentSource] );
}

wxString wxWebViewWebKit::GetPageSource()
{

    if (CanGetPageSource())
	{
        WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
		wxASSERT (dataSource != nil);

		id<WebDocumentRepresentation> representation = [dataSource representation];
		wxASSERT (representation != nil);

		NSString* source = [representation documentSource];
		if (source == nil)
		{
			return wxEmptyString;
		}

        return wxStringWithNSString( source );
    }

    return wxEmptyString;
}

wxString wxWebViewWebKit::GetSelection()
{
    if ( !m_webView )
        return wxEmptyString;

    NSString* selectedText = [[m_webView selectedDOMRange] toString];
    return wxStringWithNSString( selectedText );
}

bool wxWebViewWebKit::CanIncreaseTextSize()
{
    if ( !m_webView )
        return false;

    if ([m_webView canMakeTextLarger])
        return true;
    else
        return false;
}

void wxWebViewWebKit::IncreaseTextSize()
{
    if ( !m_webView )
        return;

    if (CanIncreaseTextSize())
        [m_webView makeTextLarger:(WebView*)m_webView];
}

bool wxWebViewWebKit::CanDecreaseTextSize()
{
    if ( !m_webView )
        return false;

    if ([m_webView canMakeTextSmaller])
        return true;
    else
        return false;
}

void wxWebViewWebKit::DecreaseTextSize()
{
    if ( !m_webView )
        return;

    if (CanDecreaseTextSize())
        [m_webView makeTextSmaller:(WebView*)m_webView];
}

void wxWebViewWebKit::Print()
{

    // TODO: allow specifying the "show prompt" parameter in Print() ?
    bool showPrompt = true;

    if ( !m_webView )
        return;

    id view = [[[m_webView mainFrame] frameView] documentView];
    NSPrintOperation *op = [NSPrintOperation printOperationWithView:view
                                 printInfo: [NSPrintInfo sharedPrintInfo]];
    if (showPrompt)
    {
        [op setShowsPrintPanel: showPrompt];
        // in my tests, the progress bar always freezes and it stops the whole
        // print operation. do not turn this to true unless there is a 
        // workaround for the bug.
        [op setShowsProgressPanel: false];
    }
    // Print it.
    [op runOperation];
}

void wxWebViewWebKit::SetEditable(bool enable)
{
    if ( !m_webView )
        return;

    [m_webView setEditable:enable ];
}

bool wxWebViewWebKit::IsEditable()
{
    if ( !m_webView )
        return false;

    return [m_webView isEditable];
}

void wxWebViewWebKit::SetZoomType(wxWebViewZoomType zoomType)
{
    // there is only one supported zoom type at the moment so this setter
    // does nothing beyond checking sanity
    wxASSERT(zoomType == wxWEB_VIEW_ZOOM_TYPE_TEXT);
}

wxWebViewZoomType wxWebViewWebKit::GetZoomType() const
{
    // for now that's the only one that is supported
    // FIXME: does the default zoom type change depending on webkit versions? :S
    //        Then this will be wrong
    return wxWEB_VIEW_ZOOM_TYPE_TEXT;
}

bool wxWebViewWebKit::CanSetZoomType(wxWebViewZoomType type) const
{
    switch (type)
    {
        // for now that's the only one that is supported
        // TODO: I know recent versions of webkit support layout zoom too,
        //       check if we can support it
        case wxWEB_VIEW_ZOOM_TYPE_TEXT:
            return true;

        default:
            return false;
    }
}

int wxWebViewWebKit::GetScrollPos()
{
    id result = [[m_webView windowScriptObject]
                    evaluateWebScript:@"document.body.scrollTop"];
    return [result intValue];
}

void wxWebViewWebKit::SetScrollPos(int pos)
{
    if ( !m_webView )
        return;

    wxString javascript;
    javascript.Printf(wxT("document.body.scrollTop = %d;"), pos);
    [[m_webView windowScriptObject] evaluateWebScript:
            (NSString*)wxNSStringWithWxString( javascript )];
}

wxString wxWebViewWebKit::GetSelectedText()
{
    NSString* selection = [[m_webView selectedDOMRange] markupString];
    if (!selection) return wxEmptyString;

    return wxStringWithNSString(selection);
}

void wxWebViewWebKit::RunScript(const wxString& javascript)
{
    if ( !m_webView )
        return;

    [[m_webView windowScriptObject] evaluateWebScript:
                    (NSString*)wxNSStringWithWxString( javascript )];
}

void wxWebViewWebKit::OnSize(wxSizeEvent &event)
{
#if defined(__WXMAC_) && wxOSX_USE_CARBON
    // This is a nasty hack because WebKit seems to lose its position when it is
    // embedded in a control that is not itself the content view for a TLW.
    // I put it in OnSize because these calcs are not perfect, and in fact are
    // basically guesses based on reverse engineering, so it's best to give
    // people the option of overriding OnSize with their own calcs if need be.
    // I also left some test debugging print statements as a convenience if
    // a(nother) problem crops up.

    wxWindow* tlw = MacGetTopLevelWindow();

    NSRect frame = [(WebView*)m_webView frame];
    NSRect bounds = [(WebView*)m_webView bounds];

#if DEBUG_WEBKIT_SIZING
    fprintf(stderr,"Carbon window x=%d, y=%d, width=%d, height=%d\n",
            GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);
    fprintf(stderr, "Cocoa window frame x=%G, y=%G, width=%G, height=%G\n",
            frame.origin.x, frame.origin.y,
            frame.size.width, frame.size.height);
    fprintf(stderr, "Cocoa window bounds x=%G, y=%G, width=%G, height=%G\n",
            bounds.origin.x, bounds.origin.y,
            bounds.size.width, bounds.size.height);
#endif

    // This must be the case that Apple tested with, because well, in this one case
    // we don't need to do anything! It just works. ;)
    if (GetParent() == tlw) return;

    // since we no longer use parent coordinates, we always want 0,0.
    int x = 0;
    int y = 0;

    HIRect rect;
    rect.origin.x = x;
    rect.origin.y = y;

#if DEBUG_WEBKIT_SIZING
    printf("Before conversion, origin is: x = %d, y = %d\n", x, y);
#endif

    // NB: In most cases, when calling HIViewConvertRect, what people want is to
    // use GetRootControl(), and this tripped me up at first. But in fact, what
    // we want is the root view, because we need to make the y origin relative
    // to the very top of the window, not its contents, since we later flip
    // the y coordinate for Cocoa.
    HIViewConvertRect (&rect, m_peer->GetControlRef(),
                                HIViewGetRoot(
                                    (WindowRef) MacGetTopLevelWindowRef()
                                 ));

    x = (int)rect.origin.x;
    y = (int)rect.origin.y;

#if DEBUG_WEBKIT_SIZING
    printf("Moving Cocoa frame origin to: x = %d, y = %d\n", x, y);
#endif

    if (tlw){
        //flip the y coordinate to convert to Cocoa coordinates
        y = tlw->GetSize().y - ((GetSize().y) + y);
    }

#if DEBUG_WEBKIT_SIZING
    printf("y = %d after flipping value\n", y);
#endif

    frame.origin.x = x;
    frame.origin.y = y;
    [(WebView*)m_webView setFrame:frame];

    if (IsShown())
        [(WebView*)m_webView display];
    event.Skip();
#endif
}

void wxWebViewWebKit::MacVisibilityChanged(){
#if defined(__WXMAC__) && wxOSX_USE_CARBON
    bool isHidden = !IsControlVisible( m_peer->GetControlRef());
    if (!isHidden)
        [(WebView*)m_webView display];

    [m_webView setHidden:isHidden];
#endif
}

void wxWebViewWebKit::LoadUrl(const wxString& url)
{
    InternalLoadURL(url);
}

wxString wxWebViewWebKit::GetCurrentURL()
{
    return wxStringWithNSString([m_webView mainFrameURL]);
}

wxString wxWebViewWebKit::GetCurrentTitle()
{
    return GetPageTitle();
}

float wxWebViewWebKit::GetWebkitZoom()
{
    return [m_webView textSizeMultiplier];
}

void wxWebViewWebKit::SetWebkitZoom(float zoom)
{
    [m_webView setTextSizeMultiplier:zoom];
}

wxWebViewZoom wxWebViewWebKit::GetZoom()
{
    float zoom = GetWebkitZoom();

    // arbitrary way to map float zoom to our common zoom enum
    if (zoom <= 0.55)
    {
        return wxWEB_VIEW_ZOOM_TINY;
    }
    else if (zoom > 0.55 && zoom <= 0.85)
    {
        return wxWEB_VIEW_ZOOM_SMALL;
    }
    else if (zoom > 0.85 && zoom <= 1.15)
    {
        return wxWEB_VIEW_ZOOM_MEDIUM;
    }
    else if (zoom > 1.15 && zoom <= 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGE;
    }
    else if (zoom > 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGEST;
    }

    // to shut up compilers, this can never be reached logically
    wxASSERT(false);
    return wxWEB_VIEW_ZOOM_MEDIUM;
}

void wxWebViewWebKit::SetZoom(wxWebViewZoom zoom)
{
    // arbitrary way to map our common zoom enum to float zoom
    switch (zoom)
    {
        case wxWEB_VIEW_ZOOM_TINY:
            SetWebkitZoom(0.4f);
            break;

        case wxWEB_VIEW_ZOOM_SMALL:
            SetWebkitZoom(0.7f);
            break;

        case wxWEB_VIEW_ZOOM_MEDIUM:
            SetWebkitZoom(1.0f);
            break;

        case wxWEB_VIEW_ZOOM_LARGE:
            SetWebkitZoom(1.3);
            break;

        case wxWEB_VIEW_ZOOM_LARGEST:
            SetWebkitZoom(1.6);
            break;

        default:
            wxASSERT(false);
    }

}

void wxWebViewWebKit::SetPage(const wxString& src, const wxString& baseUrl)
{
   if ( !m_webView )
        return;

    [[m_webView mainFrame] loadHTMLString:(NSString*)wxNSStringWithWxString(src)
                                  baseURL:[NSURL URLWithString:
                                    wxNSStringWithWxString( baseUrl )]];
}

void wxWebViewWebKit::Cut()
{
    if ( !m_webView )
        return;

    [(WebView*)m_webView cut:m_webView];
}

void wxWebViewWebKit::Copy()
{
    if ( !m_webView )
        return;

    [(WebView*)m_webView copy:m_webView];
}

void wxWebViewWebKit::Paste()
{
    if ( !m_webView )
        return;

    [(WebView*)m_webView paste:m_webView];
}

void wxWebViewWebKit::DeleteSelection()
{
    if ( !m_webView )
        return;

    [(WebView*)m_webView deleteSelection];
}

//------------------------------------------------------------
// Listener interfaces
//------------------------------------------------------------

// NB: I'm still tracking this down, but it appears the Cocoa window
// still has these events fired on it while the Carbon control is being
// destroyed. Therefore, we must be careful to check both the existence
// of the Carbon control and the event handler before firing events.

@implementation MyFrameLoadMonitor

- initWithWxWindow: (wxWebViewWebKit*)inWindow
{
    [super init];
    webKitWindow = inWindow;    // non retained
    return self;
}

- (void)webView:(WebView *)sender
    didStartProvisionalLoadForFrame:(WebFrame *)frame
{
	wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = true;
}

- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame
{
	wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = true;

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxString target = wxStringWithNSString([frame name]);
        wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
                                       wx_webviewctrls[sender]->GetId(),
                                       wxStringWithNSString( url ),
                                       target, false);

        if (webKitWindow && webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
    }
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
	wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = false;

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];

        wxString target = wxStringWithNSString([frame name]);
        wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_LOADED,
                                       wx_webviewctrls[sender]->GetId(),
                                       wxStringWithNSString( url ),
                                       target, false);

        if (webKitWindow && webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
    }
}

wxString nsErrorToWxHtmlError(NSError* error, wxWebNavigationError* out)
{
    *out = wxWEB_NAV_ERR_OTHER;

    if ([[error domain] isEqualToString:NSURLErrorDomain])
    {
        switch ([error code])
        {
            case NSURLErrorCannotFindHost:
            case NSURLErrorFileDoesNotExist:
            case NSURLErrorRedirectToNonExistentLocation:
                *out = wxWEB_NAV_ERR_NOT_FOUND;
                break;

            case NSURLErrorResourceUnavailable:
            case NSURLErrorHTTPTooManyRedirects:
            case NSURLErrorDataLengthExceedsMaximum:
            case NSURLErrorBadURL:
            case NSURLErrorFileIsDirectory:
                *out = wxWEB_NAV_ERR_REQUEST;
                break;

            case NSURLErrorTimedOut:
            case NSURLErrorDNSLookupFailed:
            case NSURLErrorNetworkConnectionLost:
            case NSURLErrorCannotConnectToHost:
            case NSURLErrorNotConnectedToInternet:
            //case NSURLErrorInternationalRoamingOff:
            //case NSURLErrorCallIsActive:
            //case NSURLErrorDataNotAllowed:
                *out = wxWEB_NAV_ERR_CONNECTION;
                break;

            case NSURLErrorCancelled:
            case NSURLErrorUserCancelledAuthentication:
                *out = wxWEB_NAV_ERR_USER_CANCELLED;
                break;

            case NSURLErrorCannotDecodeRawData:
            case NSURLErrorCannotDecodeContentData:
            case NSURLErrorBadServerResponse:
            case NSURLErrorCannotParseResponse:
                *out = wxWEB_NAV_ERR_REQUEST;
                break;

            case NSURLErrorUserAuthenticationRequired:
            case NSURLErrorSecureConnectionFailed:
            case NSURLErrorClientCertificateRequired:
                *out = wxWEB_NAV_ERR_AUTH;
                break;

            case NSURLErrorNoPermissionsToReadFile:
				*out = wxWEB_NAV_ERR_SECURITY;
                break;

            case NSURLErrorServerCertificateHasBadDate:
            case NSURLErrorServerCertificateUntrusted:
            case NSURLErrorServerCertificateHasUnknownRoot:
            case NSURLErrorServerCertificateNotYetValid:
            case NSURLErrorClientCertificateRejected:
                *out = wxWEB_NAV_ERR_CERTIFICATE;
                break;
        }
    }

    wxString message = wxStringWithNSString([error localizedDescription]);
    NSString* detail = [error localizedFailureReason];
    if (detail != NULL)
    {
        message = message + " (" + wxStringWithNSString(detail) + ")";
    }
    return message;
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError*) error
        forFrame:(WebFrame *)frame
{
	wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = false;

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];

        wxWebNavigationError type;
        wxString description = nsErrorToWxHtmlError(error, &type);
		wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_ERROR,
		                               wx_webviewctrls[sender]->GetId(),
                                       wxStringWithNSString( url ),
                                       wxEmptyString, false);
		thisEvent.SetString(description);
		thisEvent.SetInt(type);

		if (webKitWindow && webKitWindow->GetEventHandler())
		{
			webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
		}
    }
}

- (void)webView:(WebView *)sender
        didFailProvisionalLoadWithError:(NSError*)error
                               forFrame:(WebFrame *)frame
{
	wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = false;

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL]
                            absoluteString];

		wxWebNavigationError type;
        wxString description = nsErrorToWxHtmlError(error, &type);
		wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_ERROR,
		                               wx_webviewctrls[sender]->GetId(),
                                       wxStringWithNSString( url ),
                                       wxEmptyString, false);
		thisEvent.SetString(description);
		thisEvent.SetInt(type);

		if (webKitWindow && webKitWindow->GetEventHandler())
			webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
    }
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title
                                         forFrame:(WebFrame *)frame
{
    if (webKitWindow && frame == [sender mainFrame])
    {
        webKitWindow->SetPageTitle(wxStringWithNSString( title ));
    }
}
@end

@implementation MyPolicyDelegate

- initWithWxWindow: (wxWebViewWebKit*)inWindow
{
    [super init];
    webKitWindow = inWindow;    // non retained
    return self;
}

- (void)webView:(WebView *)sender
        decidePolicyForNavigationAction:(NSDictionary *)actionInformation
                                request:(NSURLRequest *)request
                                  frame:(WebFrame *)frame
                       decisionListener:(id<WebPolicyDecisionListener>)listener
{
    wxUnusedVar(frame);

    wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    wx_webviewctrls[sender]->m_busy = true;
    NSString *url = [[request URL] absoluteString];
    wxString target = wxStringWithNSString([frame name]);
    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATING,
                                   wx_webviewctrls[sender]->GetId(),
                                   wxStringWithNSString( url ), target, true);

    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);

    if (thisEvent.IsVetoed())
    {
        wx_webviewctrls[sender]->m_busy = false;
        [listener ignore];
    }
    else
    {
        [listener use];
    }
}

- (void)webView:(WebView *)sender 
      decidePolicyForNewWindowAction:(NSDictionary *)actionInformation
                             request:(NSURLRequest *)request
                        newFrameName:(NSString *)frameName
                    decisionListener:(id < WebPolicyDecisionListener >)listener
{
    wxUnusedVar(actionInformation);
    
    wxASSERT(wx_webviewctrls.find(sender) != wx_webviewctrls.end());
    NSString *url = [[request URL] absoluteString];
    wxString target = wxStringWithNSString([frame name]);
    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NEWWINDOW,
                                   wx_webviewctrls[sender]->GetId(),
                                   wxStringWithNSString( url ), target, true);

    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);

    [listener ignore];
}
@end

//#endif //wxHAVE_WEB_BACKEND_OSX_WEBKIT
