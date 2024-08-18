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

#include "wx/secretstore.h"

// Note that this class is intentionally defined outside of wxUSE_WEBREQUEST
// test as it's also used in wxCredentialEntryDialog and can be made available
// even if wxWebRequest itself is disabled.
class wxWebCredentials
{
public:
    wxWebCredentials(const wxString& user = wxString(),
                     const wxSecretValue& password = wxSecretValue())
        : m_user(user), m_password(password)
    {
    }

    const wxString& GetUser() const { return m_user; }
    const wxSecretValue& GetPassword() const { return m_password; }

private:
    wxString m_user;
    wxSecretValue m_password;
};

#if wxUSE_WEBREQUEST

#include "wx/event.h"
#include "wx/object.h"
#include "wx/stream.h"
#include "wx/versioninfo.h"

class wxWebResponse;
class wxWebSession;
class wxWebSessionFactory;

typedef struct wxWebRequestHandleOpaque* wxWebRequestHandle;
typedef struct wxWebSessionHandleOpaque* wxWebSessionHandle;

class wxWebAuthChallengeImpl;
class wxWebRequestImpl;
class wxWebResponseImpl;
class wxWebSessionImpl;

typedef wxObjectDataPtr<wxWebAuthChallengeImpl> wxWebAuthChallengeImplPtr;
typedef wxObjectDataPtr<wxWebRequestImpl> wxWebRequestImplPtr;
typedef wxObjectDataPtr<wxWebResponseImpl> wxWebResponseImplPtr;
typedef wxObjectDataPtr<wxWebSessionImpl> wxWebSessionImplPtr;

class WXDLLIMPEXP_NET wxWebAuthChallenge
{
public:
    enum Source
    {
        Source_Server,
        Source_Proxy
    };

    wxWebAuthChallenge();
    wxWebAuthChallenge(const wxWebAuthChallenge& other);
    wxWebAuthChallenge& operator=(const wxWebAuthChallenge& other);
    ~wxWebAuthChallenge();

    bool IsOk() const { return m_impl.get() != nullptr; }

    Source GetSource() const;

    void SetCredentials(const wxWebCredentials& cred);

private:
    // Ctor is used by wxWebRequest only.
    friend class wxWebRequest;
    explicit wxWebAuthChallenge(const wxWebAuthChallengeImplPtr& impl);

    wxWebAuthChallengeImplPtr m_impl;
};

class WXDLLIMPEXP_NET wxWebResponse
{
public:
    wxWebResponse();
    wxWebResponse(const wxWebResponse& other);
    wxWebResponse& operator=(const wxWebResponse& other);
    ~wxWebResponse();

    bool IsOk() const { return m_impl.get() != nullptr; }

    wxFileOffset GetContentLength() const;

    wxString GetURL() const;

    wxString GetHeader(const wxString& name) const;

    wxString GetMimeType() const;

    wxString GetContentType() const;

    int GetStatus() const;

    wxString GetStatusText() const;

    wxInputStream* GetStream() const;

    wxString GetSuggestedFileName() const;

    wxString AsString() const;

    wxString GetDataFile() const;

protected:
    // Ctor is used by wxWebRequest and implementation classes to create public
    // objects from the existing implementation pointers.
    friend class wxWebRequestBase;
    friend class wxWebRequestImpl;
    friend class wxWebResponseImpl;
    explicit wxWebResponse(const wxWebResponseImplPtr& impl);

    wxWebResponseImplPtr m_impl;
};

class WXDLLIMPEXP_NET wxWebRequestBase
{
public:
    enum State
    {
        State_Idle,
        State_Unauthorized,
        State_Active,
        State_Completed,
        State_Failed,
        State_Cancelled
    };

    enum Storage
    {
        Storage_Memory,
        Storage_File,
        Storage_None
    };

    struct Result
    {
        static Result Ok(State state = State_Active)
        {
            Result result;
            result.state = state;
            return result;
        }

        static Result Cancelled()
        {
            Result result;
            result.state = State_Cancelled;
            return result;
        }

        static Result Error(const wxString& error)
        {
            Result result;
            result.state = State_Failed;
            result.error = error;
            return result;
        }

        static Result Unauthorized(const wxString& error)
        {
            Result result;
            result.state = State_Unauthorized;
            result.error = error;
            return result;
        }

        bool operator!() const
        {
            return state == State_Failed;
        }

        State state = State_Idle;
        wxString error;
    };

    bool IsOk() const { return m_impl.get() != nullptr; }

    void SetHeader(const wxString& name, const wxString& value);

    void SetMethod(const wxString& method);

    void SetData(const wxString& text, const wxString& contentType, const wxMBConv& conv = wxConvUTF8);

    bool SetData(wxInputStream* dataStream, const wxString& contentType, wxFileOffset dataSize = wxInvalidOffset);

    void SetStorage(Storage storage);

    Storage GetStorage() const;

    wxWebResponse GetResponse() const;

    wxFileOffset GetBytesSent() const;

    wxFileOffset GetBytesExpectedToSend() const;

    wxFileOffset GetBytesReceived() const;

    wxFileOffset GetBytesExpectedToReceive() const;

    wxWebRequestHandle GetNativeHandle() const;

    enum
    {
        Ignore_Certificate = 1,
        Ignore_Host = 2,
        Ignore_All = Ignore_Certificate | Ignore_Host
    };

    void MakeInsecure(int flags = Ignore_All);
    int GetSecurityFlags() const;

    void DisablePeerVerify(bool disable = true)
    {
        MakeInsecure(disable ? Ignore_Certificate : 0);
    }

    bool IsPeerVerifyDisabled() const
    {
        return (GetSecurityFlags() & Ignore_Certificate) != 0;
    }

protected:
    wxWebRequestBase();
    explicit wxWebRequestBase(const wxWebRequestImplPtr& impl);
    wxWebRequestBase(const wxWebRequestBase& other);
    wxWebRequestBase& operator=(const wxWebRequestBase& other);
    ~wxWebRequestBase();

    wxWebRequestImplPtr m_impl;
};

class WXDLLIMPEXP_NET wxWebRequest : public wxWebRequestBase
{
public:
    wxWebRequest() = default;
    wxWebRequest(const wxWebRequest& other) = default;
    wxWebRequest& operator=(const wxWebRequest& other) = default;

    void Start();

    void Cancel();

    wxWebAuthChallenge GetAuthChallenge() const;

    int GetId() const;

    wxWebSession& GetSession() const;

    State GetState() const;

private:
    // Ctor is used by wxWebSession and implementation classes to create
    // wxWebRequest objects from the existing implementation pointers.
    friend class wxWebSession;
    friend class wxWebRequestImpl;
    friend class wxWebResponseImpl;
    explicit wxWebRequest(const wxWebRequestImplPtr& impl)
        : wxWebRequestBase(impl)
    {
    }
};

class WXDLLIMPEXP_NET wxWebRequestSync : public wxWebRequestBase
{
public:
    wxWebRequestSync() = default;
    wxWebRequestSync(const wxWebRequestSync& other) = default;
    wxWebRequestSync& operator=(const wxWebRequestSync& other) = default;

    // Possible return values for the state here are State_Completed,
    // State_Failed and State_Unauthorized.
    Result Execute() const;

private:
    friend class wxWebSessionSync;

    explicit wxWebRequestSync(const wxWebRequestImplPtr& impl)
        : wxWebRequestBase(impl)
    {
    }
};


// Describe the proxy to be used by the web session.
class wxWebProxy
{
public:
    static wxWebProxy FromURL(const wxString& url)
    {
        return wxWebProxy(Type::URL, url);
    }

    static wxWebProxy Disable() { return wxWebProxy(Type::Disabled); }
    static wxWebProxy Default() { return wxWebProxy(Type::Default); }

    enum class Type
    {
        URL,
        Disabled,
        Default
    };

    Type GetType() const { return m_type; }

    const wxString& GetURL() const
    {
        wxASSERT( m_type == Type::URL );
        return m_url;
    }

private:
    wxWebProxy(Type type, const wxString& url = wxString{})
        : m_type(type), m_url(url)
    {
    }

    // These fields never change but can't be const because we want these
    // objects to be copyable/assignable.
    Type m_type;
    wxString m_url;
};

extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendWinHTTP[];
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendURLSession[];
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendCURL[];

// Common base class for synchronous and asynchronous web sessions.
class WXDLLIMPEXP_NET wxWebSessionBase
{
public:
    // Default ctor creates an invalid session object, only IsOpened() can be
    // called on it.
    wxWebSessionBase();

    wxWebSessionBase(const wxWebSessionBase& other);
    wxWebSessionBase& operator=(const wxWebSessionBase& other);
    ~wxWebSessionBase();

    // Can be used to check if the given backend is available without actually
    // creating a session using it.
    static bool IsBackendAvailable(const wxString& backend);

    wxVersionInfo GetLibraryVersionInfo();

    void AddCommonHeader(const wxString& name, const wxString& value);

    void SetTempDir(const wxString& dir);
    wxString GetTempDir() const;

    bool SetProxy(const wxWebProxy& proxy);

    bool IsOpened() const;

    void Close();

    bool EnablePersistentStorage(bool enable = true);

    wxWebSessionHandle GetNativeHandle() const;

private:
    static void RegisterFactory(const wxString& backend,
                                wxWebSessionFactory* factory);

    static void InitFactoryMap();

protected:
    // This function handles empty backend string correctly, i.e. returns the
    // default backend in this case.
    //
    // The returned pointer should not be deleted by the caller.
    //
    // If the specified backend is not found, returns a null pointer.
    static wxWebSessionFactory* FindFactory(const wxString& backend);

    explicit wxWebSessionBase(const wxWebSessionImplPtr& impl);

    wxWebSessionImplPtr m_impl;
};

// Web session class for using asynchronous web requests, suitable for use in
// the main thread of GUI applications.
class WXDLLIMPEXP_NET wxWebSession : public wxWebSessionBase
{
public:
    wxWebSession() = default;

    wxWebSession(const wxWebSession& other) = default;
    wxWebSession& operator=(const wxWebSession& other) = default;

    // Objects of this class can't be created directly, use the following
    // factory functions to get access to them.
    static wxWebSession& GetDefault();

    static wxWebSession New(const wxString& backend = wxString());

    wxWebRequest
    CreateRequest(wxEvtHandler* handler, const wxString& url, int id = wxID_ANY);

private:
    explicit wxWebSession(const wxWebSessionImplPtr& impl)
        : wxWebSessionBase(impl)
    {
    }
};

// Web session class for using synchronous web requests, suitable for use in
// background worker threads.
class WXDLLIMPEXP_NET wxWebSessionSync : public wxWebSessionBase
{
public:
    wxWebSessionSync() = default;

    wxWebSessionSync(const wxWebSessionSync& other) = default;
    wxWebSessionSync& operator=(const wxWebSessionSync& other) = default;

    // Objects of this class can't be created directly, use the following
    // factory functions to get access to them.
    static wxWebSessionSync& GetDefault();

    static wxWebSessionSync New(const wxString& backend = wxString());

    wxWebRequestSync CreateRequest(const wxString& url);

private:
    explicit wxWebSessionSync(const wxWebSessionImplPtr& impl)
        : wxWebSessionBase(impl)
    {
    }
};

class WXDLLIMPEXP_NET wxWebRequestEvent : public wxEvent
{
public:
    wxWebRequestEvent(wxEventType type = wxEVT_NULL,
                      int id = wxID_ANY,
                      wxWebRequest::State state = wxWebRequest::State_Idle,
                      const wxWebRequest& request = wxWebRequest(),
                      const wxWebResponse& response = wxWebResponse(),
                      const wxString& errorDesc = wxString())
        : wxEvent(id, type),
        m_state(state), m_request(request), m_response(response),
        m_errorDescription(errorDesc)
    { }

    wxWebRequest::State GetState() const { return m_state; }

    const wxWebRequest& GetRequest() const { return m_request; }

    const wxWebResponse& GetResponse() const { return m_response; }

    const wxString& GetErrorDescription() const { return m_errorDescription; }

    const wxString& GetDataFile() const { return m_dataFile; }

    void SetDataFile(const wxString& dataFile) { m_dataFile = dataFile; }

    const void* GetDataBuffer() const { return m_dataBuf.GetData(); }

    size_t GetDataSize() const { return m_dataBuf.GetDataLen(); }

    void SetDataBuffer(const wxMemoryBuffer& dataBuf) { m_dataBuf = dataBuf; }

    wxEvent* Clone() const override { return new wxWebRequestEvent(*this); }

private:
    wxWebRequest::State m_state;
    const wxWebRequest m_request;
    const wxWebResponse m_response; // may be invalid
    wxString m_dataFile;
    wxMemoryBuffer m_dataBuf;
    wxString m_errorDescription;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_NET, wxEVT_WEBREQUEST_STATE, wxWebRequestEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_NET, wxEVT_WEBREQUEST_DATA, wxWebRequestEvent);

#endif // wxUSE_WEBREQUEST

#endif // _WX_WEBREQUEST_H
