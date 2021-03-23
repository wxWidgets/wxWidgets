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

    wxFileOffset GetContentLength() const wxOVERRIDE { return m_contentLength; }

    wxString GetURL() const wxOVERRIDE;

    wxString GetHeader(const wxString& name) const wxOVERRIDE;

    int GetStatus() const wxOVERRIDE;

    wxString GetStatusText() const wxOVERRIDE;

    bool ReadData();

    bool ReportAvailableData(DWORD dataLen);

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

    void SetCredentials(const wxWebCredentials& cred) wxOVERRIDE;

private:
    wxWebRequestWinHTTP& m_request;
    DWORD m_target;
    DWORD m_selectedScheme;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeWinHTTP);
};

class wxWebRequestWinHTTP : public wxWebRequestImpl
{
public:
    wxWebRequestWinHTTP(wxWebSession& session,
                        wxWebSessionWinHTTP& sessionImpl,
                        wxEvtHandler* handler,
                        const wxString& url,
                        int id);

    ~wxWebRequestWinHTTP();

    void Start() wxOVERRIDE;

    wxWebResponseImplPtr GetResponse() const wxOVERRIDE
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const wxOVERRIDE
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const wxOVERRIDE { return m_dataWritten; }

    wxFileOffset GetBytesExpectedToSend() const wxOVERRIDE { return m_dataSize; }

    void HandleCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength);

    HINTERNET GetHandle() const { return m_request; }

    wxWebRequestHandle GetNativeHandle() const wxOVERRIDE
    {
        return (wxWebRequestHandle)GetHandle();
    }

private:
    void DoCancel() wxOVERRIDE;

    wxWebSessionWinHTTP& m_sessionImpl;
    wxString m_url;
    HINTERNET m_connect;
    HINTERNET m_request;
    wxObjectDataPtr<wxWebResponseWinHTTP> m_response;
    wxObjectDataPtr<wxWebAuthChallengeWinHTTP> m_authChallenge;
    wxMemoryBuffer m_dataWriteBuffer;
    wxFileOffset m_dataWritten;

    void SendRequest();

    void WriteData();

    void CreateResponse();

    // Set the state to State_Failed with the error string including the
    // provided description of the operation and the error message for this
    // error code.
    void SetFailed(const wxString& operation, DWORD errorCode);

    void SetFailedWithLastError(const wxString& operation)
    {
        SetFailed(operation, ::GetLastError());
    }

    friend class wxWebAuthChallengeWinHTTP;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestWinHTTP);
};

class wxWebSessionWinHTTP : public wxWebSessionImpl
{
public:
    wxWebSessionWinHTTP();

    ~wxWebSessionWinHTTP();

    static bool Initialize();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id) wxOVERRIDE;

    wxVersionInfo GetLibraryVersionInfo() wxOVERRIDE;

    HINTERNET GetHandle() const { return m_handle; }

    wxWebSessionHandle GetNativeHandle() const wxOVERRIDE
    {
        return (wxWebSessionHandle)GetHandle();
    }

private:
    HINTERNET m_handle;

    bool Open();

    wxDECLARE_NO_COPY_CLASS(wxWebSessionWinHTTP);
};

class wxWebSessionFactoryWinHTTP : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() wxOVERRIDE
    {
        return new wxWebSessionWinHTTP();
    }

    bool Initialize() wxOVERRIDE
    {
        return wxWebSessionWinHTTP::Initialize();
    }
};

#endif // _WX_MSW_WEBREQUEST_WINHTTP_H
