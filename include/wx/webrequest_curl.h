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

#include "wx/thread.h"
#include "wx/vector.h"

#include "curl/curl.h"

class wxWebResponseCURL;
class wxWebRequestCURL;

class WXDLLIMPEXP_NET wxWebAuthChallengeCURL : public wxWebAuthChallenge
{
public:
    explicit wxWebAuthChallengeCURL(Source source, wxWebRequestCURL& request);

    bool Init();

    void SetCredentials(const wxString& user, const wxString& password) wxOVERRIDE;

private:
    wxWebRequestCURL& m_request;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeCURL);
};

class WXDLLIMPEXP_NET wxWebRequestCURL: public wxWebRequest
{
public:
    wxWebRequestCURL(wxWebSession& session, int id, const wxString& url);

    ~wxWebRequestCURL();

    void Start() wxOVERRIDE;

    void Cancel() wxOVERRIDE;

    wxWebResponse* GetResponse() const wxOVERRIDE;

    wxWebAuthChallenge* GetAuthChallenge() const wxOVERRIDE
    { return m_authChallenge.get(); }

    wxFileOffset GetBytesSent() const wxOVERRIDE;

    wxFileOffset GetBytesExpectedToSend() const wxOVERRIDE;

    CURL* GetHandle() const { return m_handle; }

    bool StartRequest();

    void HandleCompletion();

    wxString GetError() const;

    size_t ReadData(char* buffer, size_t size);

private:
    CURL* m_handle;
    char m_errorBuffer[CURL_ERROR_SIZE];
    struct curl_slist *m_headerList;
    wxScopedPtr<wxWebResponseCURL> m_response;
    wxScopedPtr<wxWebAuthChallengeCURL> m_authChallenge;
    wxFileOffset m_bytesSent;

    void DestroyHeaderList();

    wxDECLARE_NO_COPY_CLASS(wxWebRequestCURL);
};

class WXDLLIMPEXP_NET wxWebResponseCURL : public wxWebResponse
{
public:
    wxWebResponseCURL(wxWebRequest& request);

    wxInt64 GetContentLength() const wxOVERRIDE;

    wxString GetURL() const wxOVERRIDE;

    wxString GetHeader(const wxString& name) const wxOVERRIDE;

    int GetStatus() const wxOVERRIDE;

    wxString GetStatusText() const wxOVERRIDE { return m_statusText; }

    size_t WriteData(void *buffer, size_t size);

    size_t AddHeaderData(const char* buffer, size_t size);

private:
    wxWebRequestHeaderMap m_headers;
    wxString m_statusText;

    CURL* GetHandle() const
    { return static_cast<wxWebRequestCURL&>(m_request).GetHandle(); }

    wxDECLARE_NO_COPY_CLASS(wxWebResponseCURL);
};

class WXDLLIMPEXP_NET wxWebSessionCURL: public wxWebSession, private wxThreadHelper
{
public:
    wxWebSessionCURL();

    ~wxWebSessionCURL();

    wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) wxOVERRIDE;

    wxVersionInfo GetLibraryVersionInfo() wxOVERRIDE;

    bool StartRequest(wxWebRequestCURL& request);

    void CancelRequest(wxWebRequestCURL* request);

protected:
    wxThread::ExitCode Entry() wxOVERRIDE;

private:
    CURLM* m_handle;
    wxMutex m_mutex;
    wxCondition m_condition;
    bool m_shuttingDown;
    wxMutex m_cancelledMutex;
    wxVector< wxObjectDataPtr<wxWebRequestCURL> > m_cancelledRequests;

    void Initialize();

    static int ms_activeSessions;

    static void InitializeCURL();

    static void CleanupCURL();

    wxDECLARE_NO_COPY_CLASS(wxWebSessionCURL);
};

class WXDLLIMPEXP_NET wxWebSessionFactoryCURL: public wxWebSessionFactory
{
public:
    wxWebSession* Create() wxOVERRIDE
    { return new wxWebSessionCURL(); }
};

#endif // wxUSE_WEBREQUEST_CURL

#endif
