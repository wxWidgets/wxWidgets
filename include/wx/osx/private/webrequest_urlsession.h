///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/webrequest_urlsession.h
// Purpose:     wxWebRequest implementation using URLSession
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_WEBREQUEST_URLSESSION_H
#define _WX_OSX_WEBREQUEST_URLSESSION_H

#if wxUSE_WEBREQUEST_URLSESSION

#include "wx/private/webrequest.h"

DECLARE_WXCOCOA_OBJC_CLASS(NSURLCredential);
DECLARE_WXCOCOA_OBJC_CLASS(NSURLSession);
DECLARE_WXCOCOA_OBJC_CLASS(NSURLSessionTask);
DECLARE_WXCOCOA_OBJC_CLASS(wxWebSessionDelegate);

class wxWebSessionURLSession;
class wxWebRequestURLSession;
class wxWebResponseURLSession;

class wxWebAuthChallengeURLSession : public wxWebAuthChallengeImpl
{
public:
    wxWebAuthChallengeURLSession(wxWebAuthChallenge::Source source,
                                 wxWebRequestURLSession& request)
        : wxWebAuthChallengeImpl(source),
          m_request(request)
    {
    }

    ~wxWebAuthChallengeURLSession();

    void SetCredentials(const wxWebCredentials& cred) wxOVERRIDE;

    WX_NSURLCredential GetURLCredential() const { return m_cred; }

private:
    wxWebRequestURLSession& m_request;
    WX_NSURLCredential m_cred = NULL;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeURLSession);
};

class wxWebResponseURLSession : public wxWebResponseImpl
{
public:
    wxWebResponseURLSession(wxWebRequestURLSession& request, WX_NSURLSessionTask task);

    ~wxWebResponseURLSession();

    wxFileOffset GetContentLength() const wxOVERRIDE;

    wxString GetURL() const wxOVERRIDE;

    wxString GetHeader(const wxString& name) const wxOVERRIDE;

    int GetStatus() const wxOVERRIDE;

    wxString GetStatusText() const wxOVERRIDE;

    wxString GetSuggestedFileName() const wxOVERRIDE;

    void HandleData(WX_NSData data);

private:
    WX_NSURLSessionTask m_task;

    wxDECLARE_NO_COPY_CLASS(wxWebResponseURLSession);
};

class wxWebRequestURLSession : public wxWebRequestImpl
{
public:
    wxWebRequestURLSession(wxWebSession& session,
                           wxWebSessionURLSession& sessionImpl,
                           wxEvtHandler* handler,
                           const wxString& url,
                           int winid);

    ~wxWebRequestURLSession();

    void Start() wxOVERRIDE;

    wxWebResponseImplPtr GetResponse() const wxOVERRIDE
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const wxOVERRIDE
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const wxOVERRIDE;

    wxFileOffset GetBytesExpectedToSend() const wxOVERRIDE;

    wxFileOffset GetBytesReceived() const wxOVERRIDE;

    wxFileOffset GetBytesExpectedToReceive() const wxOVERRIDE;

    wxWebRequestHandle GetNativeHandle() const wxOVERRIDE
    {
        return (wxWebRequestHandle)m_task;
    }

    void HandleCompletion();

    void HandleChallenge(wxWebAuthChallengeURLSession* challenge);

    void OnSetCredentials(const wxWebCredentials& cred);

    wxWebResponseURLSession* GetResponseImplPtr() const
        { return m_response.get(); }

    wxWebAuthChallengeURLSession* GetAuthChallengeImplPtr() const
        { return m_authChallenge.get(); }

private:
    void DoCancel() wxOVERRIDE;

    wxWebSessionURLSession& m_sessionImpl;
    wxString m_url;
    WX_NSURLSessionTask m_task;
    wxObjectDataPtr<wxWebResponseURLSession> m_response;
    wxObjectDataPtr<wxWebAuthChallengeURLSession> m_authChallenge;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestURLSession);
};

class wxWebSessionURLSession : public wxWebSessionImpl
{
public:
    wxWebSessionURLSession();

    ~wxWebSessionURLSession();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int winid = wxID_ANY) wxOVERRIDE;

    wxVersionInfo GetLibraryVersionInfo() wxOVERRIDE;

    wxWebSessionHandle GetNativeHandle() const wxOVERRIDE
    {
        return (wxWebSessionHandle)m_session;
    }

    WX_NSURLSession GetSession() { return m_session; }

    WX_wxWebSessionDelegate GetDelegate() { return m_delegate; }

private:
    WX_NSURLSession m_session;
    WX_wxWebSessionDelegate m_delegate;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionURLSession);
};

class wxWebSessionFactoryURLSession : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() wxOVERRIDE
    { return new wxWebSessionURLSession(); }
};

#endif // wxUSE_WEBREQUEST_URLSESSION

#endif // _WX_OSX_WEBREQUEST_URLSESSION_H
