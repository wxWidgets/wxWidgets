/////////////////////////////////////////////////////////////////////////////
// Name:        webkit.mm
// Purpose:     wxWebKitCtrl - embeddable web kit control
// Author:      Jethro Grassie / Kevin Ollivier
// Modified by:
// Created:     2004-4-16
// RCS-ID:      $Id$
// Copyright:   (c) Jethro Grassie / Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "webkit.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/splitter.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_WEBKIT

#ifdef __WXCOCOA__
#include "wx/cocoa/autorelease.h"
#else
#include "wx/mac/uma.h"
#include <Carbon/Carbon.h>
#include <WebKit/WebKit.h>
#include <WebKit/HIWebView.h>
#include <WebKit/CarbonUtils.h>
#endif

#include "wx/html/webkit.h"

#define DEBUG_WEBKIT_SIZING 0

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWebKitCtrl, wxControl)

BEGIN_EVENT_TABLE(wxWebKitCtrl, wxControl)
    EVT_SIZE(wxWebKitCtrl::OnSize)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Carbon Events handlers
// ----------------------------------------------------------------------------

// prototype for function in src/mac/carbon/toplevel.cpp
void SetupMouseEvent( wxMouseEvent &wxevent , wxMacCarbonEvent &cEvent );

static const EventTypeSpec eventList[] =
{
    //{ kEventClassControl, kEventControlTrack } ,
    { kEventClassMouse, kEventMouseUp },
    { kEventClassMouse, kEventMouseDown },
    { kEventClassMouse, kEventMouseMoved },
    { kEventClassMouse, kEventMouseDragged },
#if DEBUG_WEBKIT_SIZING == 1
    { kEventClassControl, kEventControlBoundsChanged } ,
#endif
};

static pascal OSStatus wxWebKitCtrlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    wxWebKitCtrl* thisWindow = (wxWebKitCtrl*) data ;
    wxTopLevelWindowMac* tlw = NULL;
    if (thisWindow)
        tlw = thisWindow->MacGetTopLevelWindow();

    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;
    
    wxWindow* currentMouseWindow = thisWindow ;
    
    if ( wxApp::s_captureWindow )
        currentMouseWindow = wxApp::s_captureWindow;

    switch ( GetEventKind( event ) )
    {
        case kEventMouseDragged :
        case kEventMouseMoved :
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

        case kEventControlBoundsChanged:
            {
                // this is just here for debugging, so we can note any differences between
                // native event sizes and the sizes the wxWindow receives.
                Rect origBounds = cEvent.GetParameter<Rect>(kEventParamOriginalBounds, typeQDRectangle) ;
                Rect prevBounds = cEvent.GetParameter<Rect>(kEventParamPreviousBounds, typeQDRectangle) ;
                Rect curBounds = cEvent.GetParameter<Rect>(kEventParamCurrentBounds, typeQDRectangle) ;
                    
                fprintf(stderr, "Orig bounds x=%d, y=%d, height=%d, width=%d\n", origBounds.left, origBounds.top, origBounds.bottom -origBounds.top, origBounds.right - origBounds.left);
                fprintf(stderr, "Prev bounds x=%d, y=%d, height=%d, width=%d\n", prevBounds.left, prevBounds.top, prevBounds.bottom -prevBounds.top, prevBounds.right - prevBounds.left);
                fprintf(stderr, "Cur bounds x=%d, y=%d, height=%d, width=%d\n", curBounds.left, curBounds.top, curBounds.bottom -curBounds.top, curBounds.right - curBounds.left);
            }
        default :
            break ;
    }

    result = CallNextEventHandler(handler, event);
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxWebKitCtrlEventHandler )


// ----------------------------------------------------------------------------
// wxWebKit Events
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxWebKitStateChangedEvent, wxCommandEvent )

DEFINE_EVENT_TYPE( wxEVT_WEBKIT_STATE_CHANGED )

wxWebKitStateChangedEvent::wxWebKitStateChangedEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_STATE_CHANGED);
    SetEventObject( win );
    SetId(win->GetId());
}

IMPLEMENT_DYNAMIC_CLASS( wxWebKitBeforeLoadEvent, wxCommandEvent )

DEFINE_EVENT_TYPE( wxEVT_WEBKIT_BEFORE_LOAD )

wxWebKitBeforeLoadEvent::wxWebKitBeforeLoadEvent( wxWindow* win )
{
    m_cancelled = false;
    SetEventType( wxEVT_WEBKIT_BEFORE_LOAD);
    SetEventObject( win );
    SetId(win->GetId());
}

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

inline int wxNavTypeFromWebNavType(int type){
    if (type == WebNavigationTypeLinkClicked)
        return wxWEBKIT_NAV_LINK_CLICKED;
    
    if (type == WebNavigationTypeFormSubmitted)
        return wxWEBKIT_NAV_FORM_SUBMITTED;
        
    if (type == WebNavigationTypeBackForward)
        return wxWEBKIT_NAV_BACK_NEXT;
        
    if (type == WebNavigationTypeReload)
        return wxWEBKIT_NAV_RELOAD;
        
    if (type == WebNavigationTypeFormResubmitted)
        return wxWEBKIT_NAV_FORM_RESUBMITTED;
        
    return wxWEBKIT_NAV_OTHER;
}

@interface MyFrameLoadMonitor : NSObject
{
    wxWebKitCtrl* webKitWindow;
}

- initWithWxWindow: (wxWebKitCtrl*)inWindow;

@end

@interface MyPolicyDelegate : NSObject
{
    wxWebKitCtrl* webKitWindow;
}

- initWithWxWindow: (wxWebKitCtrl*)inWindow;

@end

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxWebKitCtrl::Create(wxWindow *parent,
                                 wxWindowID winID,
                                 const wxString& strURL,
                                 const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxValidator& validator,
                                 const wxString& name)
{

    m_currentURL = strURL;
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
    wxControl::Create(parent, m_windowID, pos, sizeInstance, style , validator , name);
    SetSize(pos.x, pos.y, sizeInstance.x, sizeInstance.y);

    wxTopLevelWindowCocoa *topWin = wxDynamicCast(this, wxTopLevelWindowCocoa);
    NSWindow* nsWin = topWin->GetNSWindow();
    NSRect rect;
    rect.origin.x = pos.x;
    rect.origin.y = pos.y;
    rect.size.width = sizeInstance.x;
    rect.size.height = sizeInstance.y;
    m_webView = (WebView*)[[WebView alloc] initWithFrame:rect frameName:@"webkitFrame" groupName:@"webkitGroup"];
    SetNSView(m_webView);
    [m_cocoaNSView release];

    if(m_parent) m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,sizeInstance);
#else
    m_macIsUserPane = false;
    wxControl::Create(parent, winID, pos, size, style , validator , name);
    m_peer = new wxMacControl(this);
    WebInitForCarbon();
    HIWebViewCreate( m_peer->GetControlRefAddr() );

    m_webView = (WebView*) HIWebViewGetWebView( m_peer->GetControlRef() );
    
    MacPostControlCreate(pos, size);
    HIViewSetVisible( m_peer->GetControlRef(), true );
    [m_webView setHidden:false];
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    if ( UMAGetSystemVersion() >= 0x1030 )
        HIViewChangeFeatures( m_peer->GetControlRef() , kHIViewIsOpaque , 0 ) ;
#endif
    InstallControlEventHandler( m_peer->GetControlRef() , GetwxWebKitCtrlEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this,
        (EventHandlerRef *)&m_webKitCtrlEventHandler);

#endif

    // Register event listener interfaces
    MyFrameLoadMonitor* myFrameLoadMonitor = [[MyFrameLoadMonitor alloc] initWithWxWindow: this];
    [m_webView setFrameLoadDelegate:myFrameLoadMonitor];
    
    // this is used to veto page loads, etc.
    MyPolicyDelegate* myPolicyDelegate = [[MyPolicyDelegate alloc] initWithWxWindow: this];
    [m_webView setPolicyDelegate:myPolicyDelegate];
    
    LoadURL(m_currentURL);
    return true;
}

wxWebKitCtrl::~wxWebKitCtrl()
{

}

// ----------------------------------------------------------------------------
// public methods
// ----------------------------------------------------------------------------

void wxWebKitCtrl::LoadURL(const wxString &url)
{
    if( !m_webView )
        return;

    [[m_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:wxNSStringWithWxString(url)]]];

    m_currentURL = url;
}

bool wxWebKitCtrl::CanGoBack(){
    if ( !m_webView )
        return false;

    return [m_webView canGoBack];
}

bool wxWebKitCtrl::CanGoForward(){
    if ( !m_webView )
        return false;

    return [m_webView canGoForward];
}

bool wxWebKitCtrl::GoBack(){
    if ( !m_webView )
        return false;

    bool result = [(WebView*)m_webView goBack];
    return result;
}

bool wxWebKitCtrl::GoForward(){
    if ( !m_webView )
        return false;

    bool result = [(WebView*)m_webView goForward];
    return result;
}

void wxWebKitCtrl::Reload(){
    if ( !m_webView )
        return;

    [[m_webView mainFrame] reload];
}

void wxWebKitCtrl::Stop(){
    if ( !m_webView )
        return;

    [[m_webView mainFrame] stopLoading];
}

bool wxWebKitCtrl::CanGetPageSource(){
    if ( !m_webView )
        return false;

    WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
    return ( [[dataSource representation] canProvideDocumentSource] );
}

wxString wxWebKitCtrl::GetPageSource(){

    if (CanGetPageSource()){
        WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
        return wxStringWithNSString( [[dataSource representation] documentSource] );
    }

    return wxEmptyString;
}

wxString wxWebKitCtrl::GetSelection(){
    if ( !m_webView )
        return wxEmptyString;
        
    NSString* selectedText = [[m_webView selectedDOMRange] toString];
    return wxStringWithNSString( selectedText );
}

bool wxWebKitCtrl::CanIncreaseTextSize(){
    if ( !m_webView )
        return false;
        
    if ([m_webView canMakeTextLarger])
        return true;
    else
        return false;
}

void wxWebKitCtrl::IncreaseTextSize(){
    if ( !m_webView )
        return;
        
    if (CanIncreaseTextSize())
        [m_webView makeTextLarger:(WebView*)m_webView];
}

bool wxWebKitCtrl::CanDecreaseTextSize(){
    if ( !m_webView )
        return false;
        
    if ([m_webView canMakeTextSmaller])
        return true;
    else
        return false;
}

void wxWebKitCtrl::DecreaseTextSize(){
    if ( !m_webView )
        return;
        
    if (CanDecreaseTextSize())
        [m_webView makeTextSmaller:(WebView*)m_webView];
}

void wxWebKitCtrl::SetPageSource(const wxString& source, const wxString& baseUrl){
    if ( !m_webView )
        return;

    [[m_webView mainFrame] loadHTMLString:(NSString*)wxNSStringWithWxString( source ) baseURL:[NSURL URLWithString:wxNSStringWithWxString( baseUrl )]];

}

void wxWebKitCtrl::Print(bool showPrompt){
    if ( !m_webView )
        return;
    
    id view = [[[m_webView mainFrame] frameView] documentView]; 
    NSPrintOperation *op = [NSPrintOperation printOperationWithView:view printInfo: [NSPrintInfo sharedPrintInfo]]; 
    if (showPrompt){
        [op setShowsPrintPanel: showPrompt];
        // in my tests, the progress bar always freezes and it stops the whole print operation.
        // do not turn this to true unless there is a workaround for the bug.
        [op setShowsProgressPanel: false];
    }
    // Print it. 
    [op runOperation]; 
}

void wxWebKitCtrl::MakeEditable(bool enable){
    if ( !m_webView )
        return;

    [m_webView setEditable:enable ];
}

bool wxWebKitCtrl::IsEditable(){
    if ( !m_webView )
        return false;

    return [m_webView isEditable];
}

int wxWebKitCtrl::GetScrollPos(){
    id result = [[m_webView windowScriptObject] evaluateWebScript:@"document.body.scrollTop"];
    return [result intValue];
}   

void wxWebKitCtrl::SetScrollPos(int pos){
    if ( !m_webView )
        return;
        
    wxString javascript; 
    javascript.Printf(wxT("document.body.scrollTop = %d;"), pos);
    [[m_webView windowScriptObject] evaluateWebScript:(NSString*)wxNSStringWithWxString( javascript )];
}

wxString wxWebKitCtrl::RunScript(const wxString& javascript){
    if ( !m_webView )
        return wxEmptyString;    
        
    id result = [[m_webView windowScriptObject] evaluateWebScript:(NSString*)wxNSStringWithWxString( javascript )];
    
    NSString* resultAsString;
    wxString resultAsWxString = wxEmptyString;
    NSString* className = NSStringFromClass([result class]);
    if ([className isEqualToString:@"NSCFNumber"])
        resultAsString = [NSString stringWithFormat:@"%@", result];
    else if ([className isEqualToString:@"NSCFString"])
        resultAsString = result;
    else if ([className isEqualToString:@"NSCFBoolean"]){
        if ([result boolValue])
            resultAsString = @"true";
        else
            resultAsString = @"false";
    }
    else if ([className isEqualToString:@"WebScriptObject"])
        resultAsString = [result stringRepresentation];
    else
        fprintf(stderr, "wxWebKitCtrl::RunScript - Unexpected return type: %s!\n", [className UTF8String]);

    resultAsWxString = wxStringWithNSString( resultAsString );
    return resultAsWxString;
}

void wxWebKitCtrl::OnSize(wxSizeEvent &event){
    // This is a nasty hack because WebKit seems to lose its position when it is embedded
    // in a control that is not itself the content view for a TLW.
    // I put it in OnSize because these calcs are not perfect, and in fact are basically
    // guesses based on reverse engineering, so it's best to give people the option of
    // overriding OnSize with their own calcs if need be.
    // I also left some test debugging print statements as a convenience if a(nother)
    // problem crops up.

    wxWindow* tlw = MacGetTopLevelWindow();

    NSRect frame = [m_webView frame];
    NSRect bounds = [m_webView bounds];
    
#if DEBUG_WEBKIT_SIZING
    fprintf(stderr,"Carbon window x=%d, y=%d, width=%d, height=%d\n", GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);
    fprintf(stderr, "Cocoa window frame x=%G, y=%G, width=%G, height=%G\n", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    fprintf(stderr, "Cocoa window bounds x=%G, y=%G, width=%G, height=%G\n", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
#endif

    // This must be the case that Apple tested with, because well, in this one case
    // we don't need to do anything! It just works. ;)
    if (GetParent() == tlw){
        return;
    }

    int x = GetPosition().x; 
    int y = GetPosition().y;
    
    HIRect rect;
    rect.origin.x = x;
    rect.origin.y = y;

#if DEBUG_WEBKIT_SIZING
    printf("Before conversion, origin is: x = %d, y = %d\n", x, y);
#endif

    // NB: In most cases, when calling HIViewConvertRect, what people want is to use GetRootControl(),
    // and this tripped me up at first. But in fact, what we want is the root view, because we need to
    // make the y origin relative to the very top of the window, not its contents, since we later flip 
    // the y coordinate for Cocoa. 
    HIViewConvertRect (&rect, HIViewGetSuperview( m_peer->GetControlRef() ), 
                                HIViewGetRoot( (WindowRef) MacGetTopLevelWindowRef() ) );

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
    [m_webView setFrame:frame];
   
    if (IsShown())
        [(WebView*)m_webView display];
    event.Skip();
}

void wxWebKitCtrl::MacVisibilityChanged(){
    bool isHidden = !IsControlVisible( m_peer->GetControlRef());
    if (!isHidden)
        [(WebView*)m_webView display];

    [m_webView setHidden:isHidden];
}

//------------------------------------------------------------
// Listener interfaces
//------------------------------------------------------------

@implementation MyFrameLoadMonitor

- initWithWxWindow: (wxWebKitCtrl*)inWindow
{
    [super init];
    webKitWindow = inWindow;    // non retained
    return self;
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_NEGOTIATING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_TRANSFERRING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_STOP);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        webKitWindow->SetPageTitle(wxStringWithNSString( title ));
    }
}
@end

@implementation MyPolicyDelegate

- initWithWxWindow: (wxWebKitCtrl*)inWindow
{
    [super init];
    webKitWindow = inWindow;    // non retained
    return self;
}

- (void)webView:(WebView *)sender decidePolicyForNavigationAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id<WebPolicyDecisionListener>)listener
{
    wxWebKitBeforeLoadEvent thisEvent(webKitWindow);

    // Get the navigation type. 
    NSNumber *n = [actionInformation objectForKey:WebActionNavigationTypeKey]; 
    int actionType = [n intValue];
    thisEvent.SetNavigationType( wxNavTypeFromWebNavType(actionType) );
    
    NSString *url = [[request URL] absoluteString];
    thisEvent.SetURL( wxStringWithNSString( url ) );
    
    webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);

	if (thisEvent.IsCancelled())
        [listener ignore];
    else
        [listener use];
}

@end

#endif //wxUSE_WEBKIT
