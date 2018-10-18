///////////////////////////////////////////////////////////////////////////////
// Name:        wx/webrequest.h
// Purpose:     wxWebRequest base classes
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBREQUEST_H
#define _WX_WEBREQUEST_H

#include "wx/defs.h"

#if wxUSE_WEBREQUEST

#include "wx/event.h"
#include "wx/object.h"
#include "wx/scopedptr.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"

class wxWebResponse;
class wxWebSession;

class WXDLLIMPEXP_NET wxWebRequest : public wxEvtHandler, public wxRefCounter
{
public:
    virtual void SetHeader(const wxString& name, const wxString& value) = 0;

    virtual void SetMethod(const wxString& method) = 0;

    virtual void SetData(const wxString& text, const wxString& contentType) = 0;

    virtual void SetData(const wxInputStream& dataStream, const wxString& contentType) = 0;

    virtual void SetIgnoreServerErrorStatus(bool ignore) = 0;

    virtual void Start() = 0;

    virtual void Cancel() = 0;

    virtual const wxWebResponse* GetResponse() const = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxWebRequest);
};

class WXDLLIMPEXP_NET wxWebResponse
{
public:
    virtual wxString GetURL() const = 0;

    virtual wxString GetHeader(const wxString& name) const = 0;

    virtual int GetStatus() const = 0;

    virtual wxString GetStatusText() const = 0;

    virtual wxInputStream& GetStream() const = 0;

    virtual wxString AsString(wxMBConv* conv = NULL) const = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxWebResponse);
};

class WXDLLIMPEXP_NET wxWebSessionFactory
{
public:
    virtual wxWebSession* Create() = 0;
};

WX_DECLARE_STRING_HASH_MAP(wxSharedPtr<wxWebSessionFactory>, wxStringWebSessionFactoryMap);

extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendDefault[];
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendWinHTTP[];
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendURLSession[];
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendCURL[];

class WXDLLIMPEXP_NET wxWebSession
{
public:
    virtual ~wxWebSession() { }

    virtual wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) = 0;

    virtual void SetHeader(const wxString& name, const wxString& value) = 0;

    static wxWebSession& GetDefault();

    static wxWebSession* New(const wxString& backend = wxWebSessionBackendDefault);

    static void RegisterFactory(const wxString& backend, wxSharedPtr<wxWebSessionFactory> factory);

    static bool IsBackendAvailable(const wxString& backend);

protected:
    wxWebSession() { }

private:

    static wxScopedPtr<wxWebSession> ms_defaultSession;
    static wxStringWebSessionFactoryMap ms_factoryMap;

    static void InitFactoryMap();
};

class WXDLLIMPEXP_NET wxWebRequestEvent : public wxEvent
{
public:
    wxWebRequestEvent() {}
    wxWebRequestEvent(wxEventType type, int id, wxWebResponse* response = NULL,
        const wxString& errorDesc = "")
        : wxEvent(id, type),
        m_response(response), m_errorDescription(errorDesc)
    { }

    wxWebResponse* GetResponse() const { return m_response; }

    const wxString& GetErrorDescription() const { return m_errorDescription; }

private:
    wxWebResponse* m_response;
    wxString m_errorDescription;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_NET, wxEVT_WEBREQUEST_READY, wxWebRequestEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_NET, wxEVT_WEBREQUEST_FAILED, wxWebRequestEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_NET, wxEVT_WEBREQUEST_AUTH_REQUIRED, wxWebRequestEvent);


#endif // wxUSE_WEBREQUEST

#endif // _WX_WEBREQUEST_H
