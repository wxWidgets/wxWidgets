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

DECLARE_WXCOCOA_OBJC_CLASS(NSError);
DECLARE_WXCOCOA_OBJC_CLASS(NSURLComponents);
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

    void SetCredentials(const wxWebCredentials& cred) override;

    WX_NSURLCredential GetURLCredential() const { return m_cred; }

private:
    wxWebRequestURLSession& m_request;
    WX_NSURLCredential m_cred = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeURLSession);
};

class wxWebResponseURLSession : public wxWebResponseImpl
{
public:
    wxWebResponseURLSession(wxWebRequestURLSession& request, WX_NSURLSessionTask task);

    ~wxWebResponseURLSession();

    wxFileOffset GetContentLength() const override;

    wxString GetURL() const override;

    wxString GetHeader(const wxString& name) const override;

    int GetStatus() const override;

    wxString GetStatusText() const override;

    wxString GetSuggestedFileName() const override;

    void HandleData(WX_NSData data);

private:
    WX_NSURLSessionTask m_task;

    wxDECLARE_NO_COPY_CLASS(wxWebResponseURLSession);
};

class wxWebRequestURLSession : public wxWebRequestImpl
{
public:
    // Ctor for asynchronous requests.
    wxWebRequestURLSession(wxWebSession& session,
                           wxWebSessionURLSession& sessionImpl,
                           wxEvtHandler* handler,
                           const wxString& url,
                           int winid);

    // Ctor for synchronous requests.
    wxWebRequestURLSession(wxWebSessionURLSession& sessionImpl,
                           const wxString& url);

    ~wxWebRequestURLSession();

    Result Execute() override;

    void Start() override;

    wxWebResponseImplPtr GetResponse() const override
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const override
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const override;

    wxFileOffset GetBytesExpectedToSend() const override;

    wxFileOffset GetBytesReceived() const override;

    wxFileOffset GetBytesExpectedToReceive() const override;

    wxWebRequestHandle GetNativeHandle() const override
    {
        return (wxWebRequestHandle)m_task;
    }

    Result GetResultAfterCompletion(WX_NSError error);

    void HandleCompletion(WX_NSError error);

    void HandleChallenge(wxWebAuthChallengeURLSession* challenge);

    void OnSetCredentials(const wxWebCredentials& cred);

    wxWebResponseURLSession* GetResponseImplPtr() const
        { return m_response.get(); }

    wxWebAuthChallengeURLSession* GetAuthChallengeImplPtr() const
        { return m_authChallenge.get(); }

private:
    void DoCancel() override;

    // This is a blatant ODR-violation, but there doesn't seem to be any way to
    // declare a function taking a block in (non-Objective) C++, so just skip
    // its declaration when compiling pure C++ code.
#if defined(__OBJC__)
    // Common part of Execute() and Start(), used for both synchronous and
    // asynchronous requests, but for the completion handler can only be
    // non-nil in the synchronous case.
    Result
    DoPrepare(void (^completionHandler)(NSData*, NSURLResponse*, NSError*));
#endif // __OBJC__

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
    explicit wxWebSessionURLSession(Mode mode);

    ~wxWebSessionURLSession();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int winid = wxID_ANY) override;

    wxWebRequestImplPtr
    CreateRequestSync(wxWebSessionSync& session,
                      const wxString& url) override;

    wxVersionInfo GetLibraryVersionInfo() const override;

    wxWebSessionHandle GetNativeHandle() const override
    {
        return (wxWebSessionHandle)m_session;
    }

    bool SetProxy(const wxWebProxy& proxy) override;

    bool EnablePersistentStorage(bool enable) override;

    WX_NSURLSession GetSession();

    WX_wxWebSessionDelegate GetDelegate() { return m_delegate; }

private:
    WX_NSURLSession m_session = nullptr;
    WX_wxWebSessionDelegate m_delegate;
#if !wxOSX_USE_IPHONE
    WX_NSURLComponents m_proxyURL = nullptr;
#endif // !wxOSX_USE_IPHONE
    bool m_persistentStorageEnabled = false;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionURLSession);
};

class wxWebSessionFactoryURLSession : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() override
    {
        return new wxWebSessionURLSession(wxWebSessionImpl::Mode::Async);
    }

    wxWebSessionImpl* CreateSync() override
    {
        return new wxWebSessionURLSession(wxWebSessionImpl::Mode::Sync);
    }
};

#endif // wxUSE_WEBREQUEST_URLSESSION

#endif // _WX_OSX_WEBREQUEST_URLSESSION_H
