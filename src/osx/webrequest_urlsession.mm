///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/webrequest_urlsession.mm
// Purpose:     wxWebRequest implementation using URLSession
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST_URLSESSION

#import <Foundation/Foundation.h>

#include "wx/osx/private/webrequest_urlsession.h"
#include "wx/osx/private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

@interface wxWebSessionDelegate : NSObject <NSURLSessionDataDelegate>
{
    wxWebSessionURLSession* m_session;
    NSMapTable* m_requests;
}

@end

@implementation wxWebSessionDelegate

- (id)initWithSession:(wxWebSessionURLSession*)session
{
    m_session = session;
    m_requests = [[NSMapTable weakToStrongObjectsMapTable] retain];
    return self;
}

- (void)dealloc
{
    [m_requests release];
    [super dealloc];
}

- (void)registerRequest:(wxWebRequestURLSession*)request task:(NSURLSessionTask*)task
{
    [m_requests setObject:[NSValue valueWithPointer:request] forKey:task];
}

- (wxWebRequestURLSession*)requestForTask:(NSURLSessionTask*)task
{
    wxWebRequestURLSession* request = NULL;
    NSValue* val = [m_requests objectForKey:task];
    if (val)
        request = static_cast<wxWebRequestURLSession*>(val.pointerValue);

    return request;
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
{
    wxUnusedVar(session);

    wxWebRequestURLSession* request = [self requestForTask:dataTask];
    if (request)
        request->GetResponseImplPtr()->HandleData(data);
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
    wxUnusedVar(session);

    wxWebRequestURLSession* request = [self requestForTask:task];
    if (error)
        request->SetState(wxWebRequest::State_Failed, wxCFStringRefFromGet(error.localizedDescription).AsString());
    else
        request->HandleCompletion();

    // After the task is completed it no longer needs to be mapped
    [m_requests removeObjectForKey:task];
}

@end

//
// wxWebRequestURLSession
//
wxWebRequestURLSession::wxWebRequestURLSession(wxWebSession& session,
                                               wxWebSessionURLSession& sessionImpl,
                                               wxEvtHandler* handler,
                                               const wxString& url,
                                               int winid):
    wxWebRequestImpl(session, handler, winid),
    m_sessionImpl(sessionImpl),
    m_url(url)
{
}

wxWebRequestURLSession::~wxWebRequestURLSession()
{
    [m_task release];
}

void wxWebRequestURLSession::Start()
{
    NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:
                                [NSURL URLWithString:wxCFStringRef(m_url).AsNSString()]];
    if (m_method.empty())
        req.HTTPMethod = m_dataSize ? @"POST" : @"GET";
    else
        req.HTTPMethod = wxCFStringRef(m_method).AsNSString();

    // Set request headers
    for (wxWebRequestHeaderMap::const_iterator it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        [req setValue:wxCFStringRef(it->second).AsNSString() forHTTPHeaderField:
         wxCFStringRef(it->first).AsNSString()];
    }

    if (m_dataSize)
    {
        // Read all upload data to memory buffer
        void* const buf = malloc(m_dataSize);
        m_dataStream->Read(buf, m_dataSize);

        // Create NSData from memory buffer, passing it ownership of the data.
        NSData* data = [NSData dataWithBytesNoCopy:buf length:m_dataSize];
        m_task = [[m_sessionImpl.GetSession() uploadTaskWithRequest:req fromData:data] retain];
    }
    else
    {
        // Create data task
        m_task = [[m_sessionImpl.GetSession() dataTaskWithRequest:req] retain];
    }

    // The session delegate needs to know which task is wrapped in which request
    [m_sessionImpl.GetDelegate() registerRequest:this task:m_task];

    m_response.reset(new wxWebResponseURLSession(*this, m_task));

    SetState(wxWebRequest::State_Active);
    [m_task resume];
}

void wxWebRequestURLSession::Cancel()
{
    [m_task cancel];
}

void wxWebRequestURLSession::HandleCompletion()
{
    if ( CheckServerStatus() )
        SetState(wxWebRequest::State_Completed);
}

wxWebAuthChallengeImplPtr wxWebRequestURLSession::GetAuthChallenge() const
{
    wxFAIL_MSG("not implemented");
    return wxWebAuthChallengeImplPtr();
}

wxFileOffset wxWebRequestURLSession::GetBytesSent() const
{
    return m_task.countOfBytesSent;
}

wxFileOffset wxWebRequestURLSession::GetBytesExpectedToSend() const
{
    return m_task.countOfBytesExpectedToSend;
}

wxFileOffset wxWebRequestURLSession::GetBytesReceived() const
{
    return m_task.countOfBytesReceived;
}

wxFileOffset wxWebRequestURLSession::GetBytesExpectedToReceive() const
{
    return m_task.countOfBytesExpectedToReceive;
}

//
// wxWebResponseURLSession
//

wxWebResponseURLSession::wxWebResponseURLSession(wxWebRequestURLSession& request,
                                                 WX_NSURLSessionTask task):
    wxWebResponseImpl(request)
{
    m_task = [task retain];

    Init();
}

wxWebResponseURLSession::~wxWebResponseURLSession()
{
    [m_task release];
}

void wxWebResponseURLSession::HandleData(WX_NSData data)
{
    void* buf = GetDataBuffer(data.length);
    [data getBytes:buf length:data.length];
    ReportDataReceived(data.length);
}

wxFileOffset wxWebResponseURLSession::GetContentLength() const
{
    return m_task.response.expectedContentLength;
}

wxString wxWebResponseURLSession::GetURL() const
{
    return wxCFStringRefFromGet(m_task.response.URL.absoluteString).AsString();
}

wxString wxWebResponseURLSession::GetHeader(const wxString& name) const
{
    NSHTTPURLResponse* httpResp = (NSHTTPURLResponse*) m_task.response;
    NSString* value = [httpResp.allHeaderFields objectForKey:wxCFStringRef(name).AsNSString()];
    if (value)
        return wxCFStringRefFromGet(value).AsString();
    else
        return wxString();
}

int wxWebResponseURLSession::GetStatus() const
{
    NSHTTPURLResponse* httpResp = (NSHTTPURLResponse*) m_task.response;
    return httpResp.statusCode;
}

wxString wxWebResponseURLSession::GetStatusText() const
{
    return wxCFStringRefFromGet([NSHTTPURLResponse localizedStringForStatusCode:GetStatus()]).AsString();
}

wxString wxWebResponseURLSession::GetSuggestedFileName() const
{
    return wxCFStringRefFromGet(m_task.response.suggestedFilename).AsString();
}

//
// wxWebSessionURLSession
//

wxWebSessionURLSession::wxWebSessionURLSession()
{
    m_delegate = [[wxWebSessionDelegate alloc] initWithSession:this];

    m_session = [[NSURLSession sessionWithConfiguration:
                  [NSURLSessionConfiguration defaultSessionConfiguration]
                                               delegate:m_delegate delegateQueue:nil] retain];
}

wxWebSessionURLSession::~wxWebSessionURLSession()
{
    [m_session release];
    [m_delegate release];
}

wxWebRequestImplPtr
wxWebSessionURLSession::CreateRequest(wxWebSession& session,
                                      wxEvtHandler* handler,
                                      const wxString& url,
                                      int winid)
{
    return wxWebRequestImplPtr(new wxWebRequestURLSession(session, *this, handler, url, winid));
}

wxVersionInfo wxWebSessionURLSession::GetLibraryVersionInfo()
{
    int verMaj, verMin, verMicro;
    wxGetOsVersion(&verMaj, &verMin, &verMicro);
    return wxVersionInfo("URLSession", verMaj, verMin, verMicro);
}

#endif // wxUSE_WEBREQUEST_URLSESSION
