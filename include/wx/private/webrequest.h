///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/webrequest.h
// Purpose:     wxWebRequest implementation classes
// Author:      Vadim Zeitlin
// Created:     2020-12-26
// Copyright:   (c) 2020 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WEBREQUEST_H_
#define _WX_PRIVATE_WEBREQUEST_H_

#include "wx/ffile.h"

#include "wx/private/refcountermt.h"

#include <memory>
#include <unordered_map>

using wxWebRequestHeaderMap = std::unordered_map<wxString, wxString>;

// Trace mask used for the messages in wxWebRequest code.
#define wxTRACE_WEBREQUEST "webrequest"

// ----------------------------------------------------------------------------
// wxWebAuthChallengeImpl
// ----------------------------------------------------------------------------

class wxWebAuthChallengeImpl : public wxRefCounterMT
{
public:
    virtual ~wxWebAuthChallengeImpl() = default;

    wxWebAuthChallenge::Source GetSource() const { return m_source; }

    virtual void SetCredentials(const wxWebCredentials& cred) = 0;

protected:
    explicit wxWebAuthChallengeImpl(wxWebAuthChallenge::Source source)
        : m_source(source) { }

private:
    const wxWebAuthChallenge::Source m_source;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeImpl);
};

// ----------------------------------------------------------------------------
// wxWebRequestImpl
// ----------------------------------------------------------------------------

class wxWebRequestImpl : public wxRefCounterMT
{
public:
    using Result = wxWebRequest::Result;

    // Return true if this is an async request, false if it's synchronous.
    bool IsAsync() const { return m_session != nullptr; }

    virtual ~wxWebRequestImpl() = default;

    void SetHeader(const wxString& name, const wxString& value)
    { m_headers[name] = value; }

    void SetMethod(const wxString& method) { m_method = method; }

    void SetData(const wxString& text, const wxString& contentType, const wxMBConv& conv = wxConvUTF8);

    bool SetData(std::unique_ptr<wxInputStream>& dataStream, const wxString& contentType, wxFileOffset dataSize = wxInvalidOffset);

    void SetStorage(wxWebRequest::Storage storage) { m_storage = storage; }

    wxWebRequest::Storage GetStorage() const { return m_storage; }

    // This method is called to execute the request in a synchronous way.
    virtual Result Execute() = 0;

    // This method is called to start execution of an asynchronous request.
    //
    // Precondition for this method checked by caller: current state is idle.
    virtual void Start() = 0;

    // This method can be called to cancel execution of an asynchronous request.
    //
    // Precondition for this method checked by caller: not idle and not already
    // cancelled.
    void Cancel();

    virtual wxWebResponseImplPtr GetResponse() const = 0;

    virtual wxWebAuthChallengeImplPtr GetAuthChallenge() const = 0;

    int GetId() const { return m_id; }

    // This one is only valid for async requests.
    wxWebSession& GetSession() const { return *m_session; }

    // This one can be always called.
    wxWebSessionImpl& GetSessionImpl() const { return m_sessionImpl; }

    wxWebRequest::State GetState() const { return m_state; }

    virtual wxFileOffset GetBytesSent() const = 0;

    virtual wxFileOffset GetBytesExpectedToSend() const = 0;

    virtual wxFileOffset GetBytesReceived() const;

    virtual wxFileOffset GetBytesExpectedToReceive() const;

    virtual wxWebRequestHandle GetNativeHandle() const = 0;

    void MakeInsecure(int flags) { m_securityFlags = flags; }

    int GetSecurityFlags() const { return m_securityFlags; }

    void SetState(wxWebRequest::State state, const wxString& failMsg = wxString());

    void ReportDataReceived(size_t sizeReceived);

    wxEvtHandler* GetHandler() const { return m_handler; }

protected:
    wxString m_method;
    wxWebRequest::Storage m_storage = wxWebRequest::Storage_Memory;
    wxWebRequestHeaderMap m_headers;
    wxFileOffset m_dataSize = 0;
    std::unique_ptr<wxInputStream> m_dataStream;
    int m_securityFlags = 0;

    // Ctor for async requests.
    wxWebRequestImpl(wxWebSession& session,
                     wxWebSessionImpl& sessionImpl,
                     wxEvtHandler* handler,
                     int id);

    // Ctor for sync requests.
    explicit wxWebRequestImpl(wxWebSessionImpl& sessionImpl);

    bool WasCancelled() const { return m_cancelled; }

    // Get the HTTP method to use: this will be m_method if it's non-empty,
    // POST is we have any data to send, and GET otherwise.
    //
    // Returned string is always in upper case.
    wxString GetHTTPMethod() const;

    // Get wxWebRequest::State and, optionally, error message corresponding to
    // the given response (response must be valid here).
    static Result GetResultFromHTTPStatus(const wxWebResponseImplPtr& response);

    // Call SetState() with either State_Failed or State_Completed appropriate
    // for the response status.
    void SetFinalStateFromStatus()
    {
        HandleResult(GetResultFromHTTPStatus(GetResponse()));
    }

    // Unconditionally call SetState() with the parameters corresponding to the
    // given result.
    void HandleResult(const Result& result)
    {
        SetState(result.state, result.error);
    }

    // Call SetState() if the result is an error (State_Failed) and return
    // false in this case, otherwise just return true.
    bool CheckResult(const Result& result)
    {
        if ( !result )
        {
            HandleResult(result);
            return false;
        }

        return true;
    }

private:
    // Called from public Cancel() at most once per object.
    virtual void DoCancel() = 0;

    // Called to notify about the state change in the main thread by SetState()
    // (which can itself be called from a different one).
    //
    // It also releases a reference added when switching to the active state by
    // SetState() when leaving it.
    void ProcessStateEvent(wxWebRequest::State state, const wxString& failMsg);

    wxWebSessionImpl& m_sessionImpl;

    // These parameters are only valid for async requests.
    wxWebSession* const m_session;
    wxEvtHandler* const m_handler;
    const int m_id;
    wxWebRequest::State m_state = wxWebRequest::State_Idle;
    wxFileOffset m_bytesReceived = 0;
    wxCharBuffer m_dataText;

    // Initially false, set to true after the first call to Cancel().
    bool m_cancelled = false;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestImpl);
};

// ----------------------------------------------------------------------------
// wxWebResponseImpl
// ----------------------------------------------------------------------------

class wxWebResponseImpl : public wxRefCounterMT
{
public:
    virtual ~wxWebResponseImpl();

    virtual wxFileOffset GetContentLength() const = 0;

    virtual wxString GetURL() const = 0;

    virtual wxString GetHeader(const wxString& name) const = 0;

    virtual wxString GetMimeType() const;

    virtual wxString GetContentType() const;

    virtual int GetStatus() const = 0;

    virtual wxString GetStatusText() const = 0;

    virtual wxInputStream* GetStream() const;

    virtual wxString GetSuggestedFileName() const;

    wxString AsString() const;

    virtual wxString GetDataFile() const;

    // Open data file if necessary, i.e. if using wxWebRequest::Storage_File.
    //
    // Returns result with State_Failed if the file is needed but couldn't be
    // opened.
    wxNODISCARD wxWebRequest::Result InitFileStorage();

    void ReportDataReceived(size_t sizeReceived);

protected:
    wxWebRequestImpl& m_request;

    explicit wxWebResponseImpl(wxWebRequestImpl& request);

    void* GetDataBuffer(size_t sizeNeeded);

    // This function can optionally be called to preallocate the read buffer,
    // if the total amount of data to be downloaded is known in advance.
    void PreAllocBuffer(size_t sizeNeeded);

private:
    // Called by wxWebRequestImpl only.
    friend class wxWebRequestImpl;
    void Finalize();

    wxMemoryBuffer m_readBuffer;
    mutable wxFFile m_file;
    mutable std::unique_ptr<wxInputStream> m_stream;

    wxDECLARE_NO_COPY_CLASS(wxWebResponseImpl);
};

// ----------------------------------------------------------------------------
// wxWebSessionFactory
// ----------------------------------------------------------------------------

class wxWebSessionFactory
{
public:
    virtual wxWebSessionImpl* Create() = 0;
    virtual wxWebSessionImpl* CreateSync() = 0;

    virtual bool Initialize() { return true; }

    virtual ~wxWebSessionFactory() = default;
};

// ----------------------------------------------------------------------------
// wxWebSessionImpl
// ----------------------------------------------------------------------------

class wxWebSessionImpl : public wxRefCounterMT
{
public:
    // This session class can be used either synchronously or asynchronously,
    // but the mode must be chosen at the time of the object creation and
    // cannot be changed later.
    enum class Mode
    {
        Async,
        Sync
    };

    virtual ~wxWebSessionImpl() = default;

    // Only one of these functions is actually implemented in async/sync
    // session implementation classes respectively. This is ugly, but allows to
    // add support for sync requests/sessions without completely rewriting
    // wxWebRequest code.

    virtual wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id) = 0;

    virtual wxWebRequestImplPtr
    CreateRequestSync(wxWebSessionSync& session, const wxString& url) = 0;

    virtual wxVersionInfo GetLibraryVersionInfo() = 0;

    void AddCommonHeader(const wxString& name, const wxString& value)
        { m_headers[name] = value; }

    void SetTempDir(const wxString& dir) { m_tempDir = dir; }

    wxString GetTempDir() const;

    virtual bool SetProxy(const wxWebProxy& proxy)
        { m_proxy = proxy; return true; }
    const wxWebProxy& GetProxy() const { return m_proxy; }

    const wxWebRequestHeaderMap& GetHeaders() const { return m_headers; }

    virtual wxWebSessionHandle GetNativeHandle() const = 0;

    virtual bool EnablePersistentStorage(bool WXUNUSED(enable)) { return false; }

protected:
    explicit wxWebSessionImpl(Mode mode);

    bool IsAsync() const { return m_mode == Mode::Async; }

private:
    // Make it a friend to allow accessing our m_headers.
    friend class wxWebRequest;

    const Mode m_mode;

    wxWebRequestHeaderMap m_headers;
    wxString m_tempDir;
    wxWebProxy m_proxy{wxWebProxy::Default()};


    wxDECLARE_NO_COPY_CLASS(wxWebSessionImpl);
};

#endif // _WX_PRIVATE_WEBREQUEST_H_
