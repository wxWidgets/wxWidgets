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

DECLARE_WXCOCOA_OBJC_CLASS(NSURLSession);
DECLARE_WXCOCOA_OBJC_CLASS(NSURLSessionTask);
DECLARE_WXCOCOA_OBJC_CLASS(wxWebSessionDelegte);

class wxWebSessionURLSession;
class wxWebResponseURLSession;

class WXDLLIMPEXP_NET wxWebResponseURLSession: public wxWebResponse
{
public:
    wxWebResponseURLSession(wxWebRequest& request, WX_NSURLSessionTask task);

    ~wxWebResponseURLSession();

    wxInt64 GetContentLength() const wxOVERRIDE;

    wxString GetURL() const wxOVERRIDE;

    wxString GetHeader(const wxString& name) const wxOVERRIDE;

    int GetStatus() const wxOVERRIDE;

    wxString GetStatusText() const wxOVERRIDE;

    wxString GetSuggestedFileName() const wxOVERRIDE;

    void HandleData(WX_NSData data);

private:
    WX_NSURLSessionTask m_task;
};

class WXDLLIMPEXP_NET wxWebRequestURLSession: public wxWebRequest
{
public:
    wxWebRequestURLSession(wxWebSessionURLSession& session, const wxString& url, int id);

    ~wxWebRequestURLSession();

    void Start() wxOVERRIDE;

    void Cancel() wxOVERRIDE;

    wxWebResponse* GetResponse() const wxOVERRIDE
    { return m_response.get(); }

    wxWebAuthChallenge* GetAuthChallenge() const wxOVERRIDE;

    wxFileOffset GetBytesSent() const wxOVERRIDE;

    wxFileOffset GetBytesExpectedToSend() const wxOVERRIDE;

    wxFileOffset GetBytesReceived() const wxOVERRIDE;

    wxFileOffset GetBytesExpectedToReceive() const wxOVERRIDE;

    void HandleCompletion();

private:
    wxString m_url;
    WX_NSURLSessionTask m_task;
    wxScopedPtr<wxWebResponseURLSession> m_response;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestURLSession);
};

class WXDLLIMPEXP_NET wxWebSessionURLSession: public wxWebSession
{
public:
    wxWebSessionURLSession();

    ~wxWebSessionURLSession();

    wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) wxOVERRIDE;

    wxVersionInfo GetLibraryVersionInfo() wxOVERRIDE;

    WX_NSURLSession GetSession() { return m_session; }

    WX_wxWebSessionDelegte GetDelegate() { return m_delegate; }

private:
    WX_NSURLSession m_session;
    WX_wxWebSessionDelegte m_delegate;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionURLSession);
};

class WXDLLIMPEXP_NET wxWebSessionFactoryURLSession: public wxWebSessionFactory
{
public:
    wxWebSession* Create() wxOVERRIDE
    { return new wxWebSessionURLSession(); }
};

#endif // wxUSE_WEBREQUEST_URLSESSION

#endif // _WX_OSX_WEBREQUEST_URLSESSION_H
