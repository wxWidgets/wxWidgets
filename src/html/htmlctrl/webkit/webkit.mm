/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmlctrl/webkit/webkit.mm
// Purpose:     wxWebKitCtrl - embeddable web kit control
// Author:      Jethro Grassie / Kevin Ollivier
// Modified by:
// Created:     2004-4-16
// Copyright:   (c) Jethro Grassie / Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/splitter.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_WEBKIT

#include "wx/osx/private.h"

#include <WebKit/WebKit.h>
#if __MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_15
#include <WebKit/HIWebView.h>
#include <WebKit/CarbonUtils.h>
#endif

#include "wx/html/webkit.h"

#define DEBUG_WEBKIT_SIZING 0

extern WXDLLEXPORT_DATA(const char) wxWebKitCtrlNameStr[] = "webkitctrl";

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebKitCtrl, wxControl);

wxBEGIN_EVENT_TABLE(wxWebKitCtrl, wxControl)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxWebKit Events
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebKitStateChangedEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_WEBKIT_STATE_CHANGED, wxWebKitStateChangedEvent );

wxWebKitStateChangedEvent::wxWebKitStateChangedEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_STATE_CHANGED);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
}

wxIMPLEMENT_DYNAMIC_CLASS(wxWebKitBeforeLoadEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_WEBKIT_BEFORE_LOAD, wxWebKitBeforeLoadEvent );

wxWebKitBeforeLoadEvent::wxWebKitBeforeLoadEvent( wxWindow* win )
{
    m_cancelled = false;
    SetEventType( wxEVT_WEBKIT_BEFORE_LOAD);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
}


wxIMPLEMENT_DYNAMIC_CLASS(wxWebKitNewWindowEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_WEBKIT_NEW_WINDOW, wxWebKitNewWindowEvent );

wxWebKitNewWindowEvent::wxWebKitNewWindowEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_NEW_WINDOW);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
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

@interface MyFrameLoadMonitor : NSObject<WebFrameLoadDelegate>
{
    wxWebKitCtrl* webKitWindow;
}

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow;

@end

@interface MyPolicyDelegate : NSObject<WebPolicyDelegate>
{
    wxWebKitCtrl* webKitWindow;
}

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow;

@end

@interface MyUIDelegate : NSObject<WebUIDelegate>
{
    wxWebKitCtrl* webKitWindow;
}

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow;

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
    DontCreatePeer();
    wxControl::Create(parent, winID, pos, size, style , validator , name);
    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    m_webView = [[WebView alloc] initWithFrame:r frameName:@"webkitFrame" groupName:@"webkitGroup"];

    SetPeer(new wxWidgetCocoaImpl( this, m_webView ));
    MacPostControlCreate(pos, size);
    [m_webView setHidden:false];


    // Register event listener interfaces
    
    MyFrameLoadMonitor* myFrameLoadMonitor = [[MyFrameLoadMonitor alloc] initWithWxWindow: this];
    [m_webView setFrameLoadDelegate:myFrameLoadMonitor];
    m_frameLoadMonitor = myFrameLoadMonitor;
    
    // this is used to veto page loads, etc.
    MyPolicyDelegate* myPolicyDelegate = [[MyPolicyDelegate alloc] initWithWxWindow: this];
    [m_webView setPolicyDelegate:myPolicyDelegate];
    m_policyDelegate = myPolicyDelegate;

    // this is used to provide printing support for JavaScript
    MyUIDelegate* myUIDelegate = [[MyUIDelegate alloc] initWithWxWindow: this];
    [m_webView setUIDelegate:myUIDelegate];
    m_UIDelegate = myUIDelegate;
    
    LoadURL(m_currentURL);
    return true;
}

wxWebKitCtrl::~wxWebKitCtrl()
{
    [m_webView setFrameLoadDelegate: nil];
    [m_webView setPolicyDelegate: nil];
    [m_webView setUIDelegate: nil];
    
    if (m_frameLoadMonitor)
        [m_frameLoadMonitor release];
        
    if (m_policyDelegate)
        [m_policyDelegate release];

    if (m_UIDelegate)
        [m_UIDelegate release];
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
    if ([result isKindOfClass:[NSNumber class]]){
        // __NSCFBoolean is a subclass of NSNumber
        if (strcmp([result objCType], @encode(BOOL)) == 0){
            if ([result boolValue])
                resultAsString = @"true";
            else
                resultAsString = @"false";
        }
        else
            resultAsString = [NSString stringWithFormat:@"%@", result];
    }
    else if ([result isKindOfClass:[NSString class]])
        resultAsString = result;
    else if ([result isKindOfClass:[WebScriptObject class]])
        resultAsString = [result stringRepresentation];
    else
        return wxString(); // This can happen, see e.g. #12361.

    return wxStringWithNSString( resultAsString );
}

void wxWebKitCtrl::OnSize(wxSizeEvent &event){
    event.Skip();
}

void wxWebKitCtrl::MacVisibilityChanged(){
}

//------------------------------------------------------------
// Listener interfaces
//------------------------------------------------------------

// NB: I'm still tracking this down, but it appears the Cocoa window
// still has these events fired on it while the Carbon control is being
// destroyed. Therefore, we must be careful to check both the existence
// of the Carbon control and the event handler before firing events.

@implementation MyFrameLoadMonitor

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow
{
    if ( self = [super init] )
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_NEGOTIATING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame
{
    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_TRANSFERRING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_STOP);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    wxUnusedVar(error);

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    wxUnusedVar(error);

    if (webKitWindow && frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
    if (webKitWindow && frame == [sender mainFrame]){
        webKitWindow->SetPageTitle(wxStringWithNSString( title ));
    }
}
@end

@implementation MyPolicyDelegate

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow
{
    if ( self = [super init] )
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (void)webView:(WebView *)sender decidePolicyForNavigationAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id<WebPolicyDecisionListener>)listener
{
    wxUnusedVar(sender);
    wxUnusedVar(frame);

    wxWebKitBeforeLoadEvent thisEvent(webKitWindow);

    // Get the navigation type.
    NSNumber *n = [actionInformation objectForKey:WebActionNavigationTypeKey];
    int actionType = [n intValue];
    thisEvent.SetNavigationType( wxNavTypeFromWebNavType(actionType) );

    NSString *url = [[request URL] absoluteString];
    thisEvent.SetURL( wxStringWithNSString( url ) );

    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);

    if (thisEvent.IsCancelled())
        [listener ignore];
    else
        [listener use];
}

- (void)webView:(WebView *)sender decidePolicyForNewWindowAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request newFrameName:(NSString *)frameName decisionListener:(id < WebPolicyDecisionListener >)listener
{
    wxUnusedVar(sender);
    wxUnusedVar(actionInformation);
    wxWebKitNewWindowEvent thisEvent(webKitWindow);

    NSString *url = [[request URL] absoluteString];
    thisEvent.SetURL( wxStringWithNSString( url ) );
    thisEvent.SetTargetName( wxStringWithNSString( frameName ) );
    
    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);

    [listener use];
}
@end


@implementation MyUIDelegate

- (id)initWithWxWindow: (wxWebKitCtrl*)inWindow
{
    if ( self = [super init] )
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (void)webView:(WebView *)sender printFrameView:(WebFrameView *)frameView
{
    wxUnusedVar(sender);
    wxUnusedVar(frameView);

    webKitWindow->Print(true);
}
@end

#endif //wxUSE_WEBKIT
