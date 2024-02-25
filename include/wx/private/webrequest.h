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

// Default buffer size when a fixed-size buffer must be used.
const int wxWEBREQUEST_BUFFER_SIZE = 64 * 1024;

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
    virtual ~wxWebRequestImpl() = default;

    void SetHeader(const wxString& name, const wxString& value)
    { m_headers[name] = value; }

    void SetMethod(const wxString& method) { m_method = method; }

    void SetData(const wxString& text, const wxString& contentType, const wxMBConv& conv = wxConvUTF8);

    bool SetData(std::unique_ptr<wxInputStream>& dataStream, const wxString& contentType, wxFileOffset dataSize = wxInvalidOffset);

    void SetStorage(wxWebRequest::Storage storage) { m_storage = storage; }

    wxWebRequest::Storage GetStorage() const { return m_storage; }

    // Precondition for this method checked by caller: current state is idle.
    virtual void Start() = 0;

    // Precondition for this method checked by caller: not idle and not already
    // cancelled.
    void Cancel();

    virtual wxWebResponseImplPtr GetResponse() const = 0;

    virtual wxWebAuthChallengeImplPtr GetAuthChallenge() const = 0;

    int GetId() const { return m_id; }

    wxWebSession& GetSession() const { return m_session; }

    wxWebRequest::State GetState() const { return m_state; }

    virtual wxFileOffset GetBytesSent() const = 0;

    virtual wxFileOffset GetBytesExpectedToSend() const = 0;

    virtual wxFileOffset GetBytesReceived() const;

    virtual wxFileOffset GetBytesExpectedToReceive() const;

    virtual wxWebRequestHandle GetNativeHandle() const = 0;

    void DisablePeerVerify(bool disable) { m_peerVerifyDisabled = disable; }

    bool IsPeerVerifyDisabled() const { return m_peerVerifyDisabled; }

    void SetState(wxWebRequest::State state, const wxString& failMsg = wxString());

    void ReportDataReceived(size_t sizeReceived);

    wxEvtHandler* GetHandler() const { return m_handler; }

protected:
    wxString m_method;
    wxWebRequest::Storage m_storage;
    wxWebRequestHeaderMap m_headers;
    wxFileOffset m_dataSize;
    std::unique_ptr<wxInputStream> m_dataStream;
    bool m_peerVerifyDisabled;

    wxWebRequestImpl(wxWebSession& session,
                     wxWebSessionImpl& sessionImpl,
                     wxEvtHandler* handler,
                     int id);

    bool WasCancelled() const { return m_cancelled; }

    // Call SetState() with either State_Failed or State_Completed appropriate
    // for the response status.
    void SetFinalStateFromStatus();

private:
    // Called from public Cancel() at most once per object.
    virtual void DoCancel() = 0;

    // Called to notify about the state change in the main thread by SetState()
    // (which can itself be called from a different one).
    //
    // It also releases a reference added when switching to the active state by
    // SetState() when leaving it.
    void ProcessStateEvent(wxWebRequest::State state, const wxString& failMsg);


    wxWebSession& m_session;
    wxEvtHandler* const m_handler;
    const int m_id;
    wxWebRequest::State m_state;
    wxFileOffset m_bytesReceived;
    wxCharBuffer m_dataText;

    // Initially false, set to true after the first call to Cancel().
    bool m_cancelled;

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

protected:
    wxWebRequestImpl& m_request;
    size_t m_readSize;

    explicit wxWebResponseImpl(wxWebRequestImpl& request);

    // Called from derived class ctor to finish initialization which can't be
    // performed in ctor itself as it needs to use pure virtual method.
    void Init();

    void* GetDataBuffer(size_t sizeNeeded);

    void ReportDataReceived(size_t sizeReceived);

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

    virtual bool Initialize() { return true; }

    virtual ~wxWebSessionFactory() = default;
};

// ----------------------------------------------------------------------------
// wxWebSessionImpl
// ----------------------------------------------------------------------------

class wxWebSessionImpl : public wxRefCounterMT
{
public:
    virtual ~wxWebSessionImpl() = default;

    virtual wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id) = 0;

    virtual wxVersionInfo GetLibraryVersionInfo() = 0;

    void AddCommonHeader(const wxString& name, const wxString& value)
        { m_headers[name] = value; }

    void SetTempDir(const wxString& dir) { m_tempDir = dir; }

    wxString GetTempDir() const;

    const wxWebRequestHeaderMap& GetHeaders() const { return m_headers; }

    virtual wxWebSessionHandle GetNativeHandle() const = 0;

protected:
    wxWebSessionImpl();

private:
    // Make it a friend to allow accessing our m_headers.
    friend class wxWebRequest;

    wxWebRequestHeaderMap m_headers;
    wxString m_tempDir;

    wxDECLARE_NO_COPY_CLASS(wxWebSessionImpl);
};

#endif // _WX_PRIVATE_WEBREQUEST_H_
