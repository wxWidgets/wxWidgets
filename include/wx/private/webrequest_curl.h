///////////////////////////////////////////////////////////////////////////////
// Name:        wx/webrequest_curl.h
// Purpose:     wxWebRequest implementation using libcurl
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBREQUEST_CURL_H
#define _WX_WEBREQUEST_CURL_H

#if wxUSE_WEBREQUEST_CURL

#include "wx/private/webrequest.h"

#include "wx/thread.h"
#include "wx/vector.h"
#include "wx/timer.h"

#include "curl/curl.h"

#include <unordered_map>

class wxWebRequestCURL;
class wxWebResponseCURL;
class wxWebSessionCURL;
class wxWebSessionSyncCURL;
class SocketPoller;

class wxWebAuthChallengeCURL : public wxWebAuthChallengeImpl
{
public:
    wxWebAuthChallengeCURL(wxWebAuthChallenge::Source source,
                           wxWebRequestCURL& request);

    void SetCredentials(const wxWebCredentials& cred) override;

private:
    wxWebRequestCURL& m_request;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeCURL);
};

class wxWebRequestCURL : public wxWebRequestImpl
{
public:
    // Ctor for async requests: creates a new libcurl handle and owns it.
    wxWebRequestCURL(wxWebSession& session,
                     wxWebSessionCURL& sessionImpl,
                     wxEvtHandler* handler,
                     const wxString& url,
                     int id);

    // Ctor for sync requests: uses the libcurl handle from the session and
    // doesn't own it.
    wxWebRequestCURL(wxWebSessionSyncCURL& sessionImpl,
                     const wxString& url);

    ~wxWebRequestCURL();

    wxWebRequest::Result Execute() override;

    void Start() override;

    wxWebResponseImplPtr GetResponse() const override
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const override
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const override;

    wxFileOffset GetBytesExpectedToSend() const override;

    CURL* GetHandle() const { return m_handle; }

    wxWebRequestHandle GetNativeHandle() const override
    {
        return (wxWebRequestHandle)GetHandle();
    }

    bool StartRequest();

    void HandleCompletion();

    wxString GetError() const;

    // Method called from libcurl callback
    size_t CURLOnRead(char* buffer, size_t size);

private:
    // Common initialization for sync and async requests performed when the
    // request is created.
    void DoStartPrepare(const wxString& url);

    // This function is again common for sync and async requests, but is called
    // right before starting, or executing, the request.
    //
    // If it returns result with State_Failed, the request should be aborted.
    wxWebRequest::Result DoFinishPrepare();

    // Convert the status of the completed request to our result structure and,
    // if necessary, initialize m_authChallenge.
    wxWebRequest::Result DoHandleCompletion();

    void DoCancel() override;

    // This is only used for async requests.
    wxWebSessionCURL* const m_sessionCURL;

    // This pointer is only owned by this object when using async requests.
    CURL* const m_handle;
    char m_errorBuffer[CURL_ERROR_SIZE];
    struct curl_slist *m_headerList = nullptr;
    wxObjectDataPtr<wxWebResponseCURL> m_response;
    wxObjectDataPtr<wxWebAuthChallengeCURL> m_authChallenge;
    wxFileOffset m_bytesSent;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestCURL);
};

class wxWebResponseCURL : public wxWebResponseImpl
{
public:
    explicit wxWebResponseCURL(wxWebRequestCURL& request);

    wxFileOffset GetContentLength() const override;

    wxString GetURL() const override;

    wxString GetHeader(const wxString& name) const override;

    int GetStatus() const override;

    wxString GetStatusText() const override { return m_statusText; }


    // Methods called from libcurl callbacks
    size_t CURLOnWrite(void *buffer, size_t size);
    size_t CURLOnHeader(const char* buffer, size_t size);
    int CURLOnProgress(curl_off_t);

private:
    wxWebRequestHeaderMap m_headers;
    wxString m_statusText;
    wxFileOffset m_knownDownloadSize;

    CURL* GetHandle() const
    { return static_cast<wxWebRequestCURL&>(m_request).GetHandle(); }

    wxDECLARE_NO_COPY_CLASS(wxWebResponseCURL);
};

// Common base class for synchronous and asynchronous sessions.
class wxWebSessionBaseCURL : public wxWebSessionImpl
{
public:
    explicit wxWebSessionBaseCURL(Mode mode);
    ~wxWebSessionBaseCURL();

    wxVersionInfo GetLibraryVersionInfo() const override;

    static bool CurlRuntimeAtLeastVersion(unsigned int, unsigned int,
                                          unsigned int);

protected:
    static int ms_activeSessions;
    static unsigned int ms_runtimeVersion;
};

// Sync session implementation uses libcurl "easy" API.
class wxWebSessionSyncCURL : public wxWebSessionBaseCURL
{
public:
    wxWebSessionSyncCURL();
    ~wxWebSessionSyncCURL();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& WXUNUSED(session),
                  wxEvtHandler* WXUNUSED(handler),
                  const wxString& WXUNUSED(url),
                  int WXUNUSED(id)) override
    {
        wxFAIL_MSG("This method should not be called for synchronous sessions");

        return wxWebRequestImplPtr{};
    }

    wxWebRequestImplPtr
    CreateRequestSync(wxWebSessionSync& session, const wxString& url) override;

    wxWebSessionHandle GetNativeHandle() const override
    {
        return (wxWebSessionHandle)m_handle;
    }

    CURL* GetHandle() const { return m_handle; }

private:
    CURL* m_handle = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionSyncCURL);
};

// Async session implementation uses libcurl "multi" API.
class wxWebSessionCURL : public wxWebSessionBaseCURL, public wxEvtHandler
{
public:
    wxWebSessionCURL();

    ~wxWebSessionCURL();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id = wxID_ANY) override;

    wxWebRequestImplPtr
    CreateRequestSync(wxWebSessionSync& WXUNUSED(session),
                      const wxString& WXUNUSED(url)) override
    {
        wxFAIL_MSG("This method should not be called for asynchronous sessions");

        return wxWebRequestImplPtr{};
    }

    wxWebSessionHandle GetNativeHandle() const override
    {
        return (wxWebSessionHandle)m_handle;
    }

    bool StartRequest(wxWebRequestCURL& request);

    void CancelRequest(wxWebRequestCURL* request);

    void RequestHasTerminated(wxWebRequestCURL* request);

private:
    static int TimerCallback(CURLM*, long, void*);
    static int SocketCallback(CURL*, curl_socket_t, int, void*, void*);

    void ProcessTimerCallback(long);
    void TimeoutNotification(wxTimerEvent&);
    void ProcessTimeoutNotification();
    void ProcessSocketCallback(CURL*, curl_socket_t, int);
    void ProcessSocketPollerResult(wxThreadEvent&);
    void CheckForCompletedTransfers();
    void FailRequest(CURL*, const wxString&);
    void StopActiveTransfer(CURL*);
    void RemoveActiveSocket(CURL*);

    using TransferSet = std::unordered_map<CURL*, wxWebRequestCURL*>;
    using CurlSocketMap = std::unordered_map<CURL*, curl_socket_t>;

    TransferSet m_activeTransfers;
    CurlSocketMap m_activeSockets;

    SocketPoller* m_socketPoller = nullptr;
    wxTimer m_timeoutTimer;
    CURLM* m_handle = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionCURL);
};

class wxWebSessionFactoryCURL : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() override
    {
        return new wxWebSessionCURL();
    }

    wxWebSessionImpl* CreateSync() override
    {
        return new wxWebSessionSyncCURL();
    }
};

#endif // wxUSE_WEBREQUEST_CURL

#endif
