///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/webrequest_winhttp.h
// Purpose:     wxWebRequest WinHTTP implementation
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WEBREQUEST_WINHTTP_H
#define _WX_MSW_WEBREQUEST_WINHTTP_H

#include "wx/private/webrequest.h"

#include "wx/msw/wrapwin.h"
#include <winhttp.h>
#include "wx/buffer.h"

class wxWebSessionWinHTTP;
class wxWebRequestWinHTTP;

class wxWebResponseWinHTTP : public wxWebResponseImpl
{
public:
    wxWebResponseWinHTTP(wxWebRequestWinHTTP& request);

    wxFileOffset GetContentLength() const override { return m_contentLength; }

    wxString GetURL() const override;

    wxString GetHeader(const wxString& name) const override;

    int GetStatus() const override;

    wxString GetStatusText() const override;

    bool ReadData(DWORD* bytesRead = nullptr);

private:
    HINTERNET m_requestHandle;
    wxFileOffset m_contentLength;

    wxDECLARE_NO_COPY_CLASS(wxWebResponseWinHTTP);
};

class wxWebAuthChallengeWinHTTP : public wxWebAuthChallengeImpl
{
public:
    wxWebAuthChallengeWinHTTP(wxWebAuthChallenge::Source source,
                              wxWebRequestWinHTTP& request);

    bool Init();

    wxWebRequest::Result DoSetCredentials(const wxWebCredentials& cred);

    void SetCredentials(const wxWebCredentials& cred) override;

private:
    wxWebRequestWinHTTP& m_request;
    DWORD m_target;
    DWORD m_selectedScheme;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeWinHTTP);
};

class wxWebRequestWinHTTP : public wxWebRequestImpl
{
public:
    // Ctor for asynchronous requests.
    wxWebRequestWinHTTP(wxWebSession& session,
                        wxWebSessionWinHTTP& sessionImpl,
                        wxEvtHandler* handler,
                        const wxString& url,
                        int id);

    // Ctor for synchronous requests.
    wxWebRequestWinHTTP(wxWebSessionWinHTTP& sessionImpl,
                        const wxString& url);

    ~wxWebRequestWinHTTP();

    wxWebRequest::Result Execute() override;

    void Start() override;

    wxWebResponseImplPtr GetResponse() const override
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const override
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const override { return m_dataWritten; }

    wxFileOffset GetBytesExpectedToSend() const override { return m_dataSize; }

    void HandleCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength);

    HINTERNET GetHandle() const { return m_request; }

    wxWebRequestHandle GetNativeHandle() const override
    {
        return (wxWebRequestHandle)GetHandle();
    }

private:
    void DoCancel() override;

    // Initialize m_connect and m_request. This is always synchronous.
    wxNODISCARD Result DoPrepareRequest();

    // Write next chunk of data to the request.
    //
    // Precondition: m_dataWritten < m_dataSize.
    //
    // Fills the output parameter with the number of bytes written in
    // synchronous mode. In asynchronous mode, the number of bytes written is
    // returned later and this argument must be null.
    wxNODISCARD Result DoWriteData(DWORD* bytesWritten = nullptr);


    wxWebSessionWinHTTP& m_sessionImpl;
    wxString m_url;
    HINTERNET m_connect = nullptr;
    HINTERNET m_request = nullptr;
    wxObjectDataPtr<wxWebResponseWinHTTP> m_response;
    wxObjectDataPtr<wxWebAuthChallengeWinHTTP> m_authChallenge;
    wxMemoryBuffer m_dataWriteBuffer;
    wxFileOffset m_dataWritten = 0;

    // Store authentication information from the URL, if any, as well as a flag
    // which is reset after the first attempt to use it, so that we don't try
    // to do it an infinite loop.
    wxWebCredentials m_credentialsFromURL;
    bool m_tryCredentialsFromURL = false;

    // Proxy credentials (if any) are stored in the session, but we need store
    // the same flag for them as for the server credentials here.
    bool m_tryProxyCredentials = false;


    wxNODISCARD Result SendRequest();

    // Write data, if any, and call CreateResponse() if there is nothing left
    // to write.
    void WriteData();

    wxNODISCARD Result CreateResponse();

    wxNODISCARD Result InitAuthIfNeeded();

    // Return error result with the error message built from the name of the
    // operation and WinHTTP error code.
    wxNODISCARD Result Fail(const wxString& operation, DWORD errorCode);

    // Call Fail() with the error message built from the given operation
    // description and the last error code.
    wxNODISCARD Result FailWithLastError(const wxString& operation)
    {
        return Fail(operation, ::GetLastError());
    }

    // These functions can only be used for asynchronous requests.
    void SetFailed(const wxString& operation, DWORD errorCode)
    {
        return HandleResult(Fail(operation, errorCode));
    }

    void SetFailedWithLastError(const wxString& operation)
    {
        return HandleResult(FailWithLastError(operation));
    }

    friend class wxWebAuthChallengeWinHTTP;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestWinHTTP);
};

class wxWebSessionWinHTTP : public wxWebSessionImpl
{
public:
    explicit wxWebSessionWinHTTP(Mode mode);

    ~wxWebSessionWinHTTP();

    static bool Initialize();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id) override;

    wxWebRequestImplPtr
    CreateRequestSync(wxWebSessionSync& WXUNUSED(session),
                      const wxString& WXUNUSED(url)) override;

    wxVersionInfo GetLibraryVersionInfo() const override;

    bool SetProxy(const wxWebProxy& proxy) override;

    HINTERNET GetHandle() const { return m_handle; }

    wxWebSessionHandle GetNativeHandle() const override
    {
        return (wxWebSessionHandle)GetHandle();
    }

    // Used by wxWebRequestWinHTTP to get the proxy credentials.
    bool HasProxyCredentials() const
    {
        return !m_proxyCredentials.GetUser().empty();
    }

    const wxWebCredentials& GetProxyCredentials() const
    {
        return m_proxyCredentials;
    }

private:
    HINTERNET m_handle = nullptr;

    bool Open();

    wxWebCredentials m_proxyCredentials;
    wxString m_proxyURLWithoutCredentials;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionWinHTTP);
};

class wxWebSessionFactoryWinHTTP : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() override
    {
        return new wxWebSessionWinHTTP(wxWebSessionImpl::Mode::Async);
    }

    wxWebSessionImpl* CreateSync() override
    {
        return new wxWebSessionWinHTTP(wxWebSessionImpl::Mode::Sync);
    }

    bool Initialize() override
    {
        return wxWebSessionWinHTTP::Initialize();
    }
};

#endif // _WX_MSW_WEBREQUEST_WINHTTP_H
