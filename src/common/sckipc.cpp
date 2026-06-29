/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sckipc.cpp
// Purpose:     Interprocess communication implementation (wxSocket version)
// Author:      Julian Smart
// Modified by: Guilhem Lavaux (big rewrite) May 1997, 1998
//              Guillermo Rodriguez (updated for wxSocket v2) Jan 2000
//                                  (callbacks deprecated)    Mar 2000
//              Vadim Zeitlin (added support for Unix sockets) Apr 2002
//                            (use buffering, many fixes/cleanup) Oct 2008
//              JP Mattia (use messages, fix reporting of errors
//                         and use in multithreading) Sep 2024
// Created:     1993
// Copyright:   (c) Julian Smart 1993
//              (c) Guilhem Lavaux 1997, 1998
//              (c) 2000 Guillermo Rodriguez <guille@iies.es>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SOCKETS && wxUSE_IPC

#include "wx/sckipc.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/module.h"
    #include "wx/app.h"
#endif

#include "wx/evtloop.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <set>
#include <functional>

#include "wx/socket.h"
#include "wx/thread.h"

class wxIPCMessageBase;

// --------------------------------------------------------------------------
// Global variables
// --------------------------------------------------------------------------

wxCRIT_SECT_DECLARE_MEMBER(gs_critical_io);

// --------------------------------------------------------------------------
// macros and constants
// --------------------------------------------------------------------------

namespace
{

// Message codes (don't change them to avoid breaking the existing code using
// wxIPC protocol!)
enum IPCCode
{
    IPC_NULL            = 0,
    IPC_EXECUTE         = 1,
    IPC_REQUEST         = 2,
    IPC_POKE            = 3,
    IPC_ADVISE_START    = 4,
    IPC_ADVISE_REQUEST  = 5, // not used
    IPC_ADVISE          = 6,
    IPC_ADVISE_STOP     = 7,
    IPC_REQUEST_REPLY   = 8,
    IPC_FAIL            = 9,
    IPC_CONNECT         = 10,
    IPC_DISCONNECT      = 11,
    IPC_MAX
};

// A random header, which is used to detect a loss-of-sync on the IPC
// data stream. The header is 24-bits, and the IPCCode above is sent in the
// last 8 bits.
const wxUint32 IPCCodeHeader=0x439d9600;

} // anonymous namespace

// headers needed for umask()
#ifdef __UNIX_LIKE__
    #include <sys/types.h>
    #include <sys/stat.h>
#endif // __UNIX_LIKE__

#define wxNO_RETURN_MESSAGE nullptr

const long wxIPCTimeout = 10; // socket timeout, in seconds


// For IPC returning a char* buffer. wxWidgets docs say that the user is not
// supposed to free the memory.  Each buffer pointer is assigned to a list
// sequentially, and the buffer memory is not freed until MAX_MSG_BUFFERS have
// been assigned.
#define MAX_MSG_BUFFERS 2048


// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// get the address object for the given server name, the caller must delete it
static wxSockAddress *
GetAddressFromName(const wxString& serverName,
                   const wxString& host = wxEmptyString)
{
    // we always use INET sockets under non-Unix systems
#if defined(__UNIX__) && !defined(__WINDOWS__) && !defined(__WINE__)
    // under Unix, if the server name looks like a path, create a AF_UNIX
    // socket instead of AF_INET one
    if ( serverName.Find(wxT('/')) != wxNOT_FOUND )
    {
        wxUNIXaddress *addr = new wxUNIXaddress;
        addr->Filename(serverName);

        return addr;
    }
#endif // Unix/!Unix
    {
        wxIPV4address *addr = new wxIPV4address;
        addr->Service(serverName);
        if ( !host.empty() )
        {
            addr->Hostname(host);
        }

        return addr;
    }
}

// --------------------------------------------------------------------------
// wxTCPEventHandler declaration (private class)
// --------------------------------------------------------------------------

class wxTCPEventHandler : public wxEvtHandler
{
public:
    wxTCPEventHandler() : wxEvtHandler()
    {
        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            m_bufferList[i] = nullptr;

        m_nextAvailable = 0;
    }

    ~wxTCPEventHandler()
    {
        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            if (m_bufferList[i])
                delete[] m_bufferList[i];
    }

    void OnSocketInput(wxSocketEvent& event);
    void OnSocketConnection(wxSocketEvent& event);

    bool ProcessMessage(wxIPCMessageBase* msg, wxSocketBase *socket);

    bool SendAndGetReply(wxIPCMessageBase& send_msg,
                         IPCCode expected_code,
                         wxSocketBase* socket,
                         wxIPCMessageBase** return_msgptr);
    bool SendAndGetReply_MainThread(wxIPCMessageBase& send_msg,
                                    IPCCode expected_code,
                                    wxSocketBase* socket,
                                    wxIPCMessageBase** return_msgptr);
    bool SendAndGetReply_WorkerThread(wxIPCMessageBase& send_msg,
                                      IPCCode expected_code,
                                      wxIPCMessageBase** return_msgptr);

    void SendFailMessage(const wxString& reason, wxSocketBase* socket);
    void HandleDisconnect(wxTCPConnection *connection);

    wxIPCMessageBase* ReadMessageFromSocket(wxSocketBase *socket);
    bool WriteMessageToSocket(wxIPCMessageBase& msg);
    bool PeekAtMessageInSocket(wxSocketBase *socket);

    char* GetBufPtr(size_t size);

    // This event handler is a single process-wide object shared by every IPC
    // connection (client and server) and it lives until the program exits.
    // Socket events are queued on it and may still be pending after the socket
    // they refer to has been destroyed: Dispatching a stale event would
    // dereference a dangling wxSocketBase pointer. To guard against this we keep
    // the set of sockets that currently have a live connection and ignore events
    // for any socket which is not in it. The check compares only pointer values,
    // so it is safe even if the socket has already been freed.
    void RegisterConnectionSocket(wxSocketBase* socket);
    void UnregisterConnectionSocket(wxSocketBase* socket);
    bool IsConnectionSocket(wxSocketBase* socket);

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_socket_processing);

    // Runs fn on the main thread, blocking the caller until completion (on the
    // main thread fn runs directly). All IPC socket I/O must happen on the main
    // thread: the wxFDIODispatcher used by the main event loop in Unix+Mac is not
    // safe to mutate from worker threads, so worker-thread socket usage is
    // funneled through this method.
    void RunOnMainThread(const std::function<void()>& fn);

    // The handoff objects between a worker thread blocked in SendAndGetReply()
    // and the main thread's OnSocketInput(). Only one reply is ever pending at a
    // time, which is guaranteed by m_cs_awaiting_reply.
    wxMutex m_replyMutex;
    wxCondition m_replyCond{m_replyMutex};
    struct PendingReply
    {
        bool              active   = false;
        IPCCode           expected = IPC_NULL;
        wxIPCMessageBase* reply    = nullptr;
        bool              done     = false;
        bool              failed   = false;
    } m_pending;

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_awaiting_reply);

    bool DeliverPendingReply(wxIPCMessageBase* msg);
    void FailPendingReply();

    wxIPCMessageBase* GetIPCMessageFromCode(IPCCode code, wxSocketBase* socket);

    void PostSocketInputEvent(wxSocketBase* socket);
private:
    wxTCPConnection* GetConnection(wxSocketBase* socket);

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_assign_buffer);

    char* m_bufferList[MAX_MSG_BUFFERS];
    int m_nextAvailable;

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_connection_sockets);
    std::set<wxSocketBase*> m_connectionSockets;

    // Sockets the main thread is currently reading. A WAITALL Read/Peek pumps the
    // event loop while it waits, so on MSW the GUI message pump can re-dispatch a
    // wxSOCKET_INPUT for a socket we are already reading; OnSocketInput() consults
    // this set and bails to avoid a re-entrant read (which trips wxSocketReadGuard,
    // "read reentrancy?"). All reads run on the main thread, so this needs no lock.
    std::set<wxSocketBase*> m_socketsBeingRead;
    bool IsReadingSocket(wxSocketBase* socket) const
        { return m_socketsBeingRead.count(socket) != 0; }

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxTCPEventHandler);
};

enum
{
    _SOCKET_INPUT_ID = 1000,
    _SERVER_SOCKET_CONNECTION_ID
};

namespace
{

// RAII marker recording that the main thread is reading a given socket, so a
// re-entrant OnSocketInput() (dispatched while a WAITALL Read/Peek pumps the
// event loop) can detect it and bail instead of re-reading the same socket.
class MainThreadReadGuard
{
public:
    MainThreadReadGuard(std::set<wxSocketBase*>& set, wxSocketBase* socket)
        : m_set(set), m_socket(socket) { m_set.insert(m_socket); }
    ~MainThreadReadGuard() { m_set.erase(m_socket); }

private:
    std::set<wxSocketBase*>& m_set;
    wxSocketBase* const m_socket;
    wxDECLARE_NO_COPY_CLASS(MainThreadReadGuard);
};

} // anonymous namespace

// --------------------------------------------------------------------------
// wxTCPEventHandlerModule (private class)
// --------------------------------------------------------------------------

class wxTCPEventHandlerModule : public wxModule
{
public:
    wxTCPEventHandlerModule() : wxModule() { }

    // get the global wxTCPEventHandler creating it if necessary
    static wxTCPEventHandler& GetHandler()
    {
        if ( !ms_handler )
            ms_handler = new wxTCPEventHandler;

        return *ms_handler;
    }

    // as ms_handler is initialized on demand, don't do anything in OnInit()
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override { wxDELETE(ms_handler); }

private:
    static wxTCPEventHandler *ms_handler;

    wxDECLARE_DYNAMIC_CLASS(wxTCPEventHandlerModule);
    wxDECLARE_NO_COPY_CLASS(wxTCPEventHandlerModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxTCPEventHandlerModule, wxModule);

wxTCPEventHandler *wxTCPEventHandlerModule::ms_handler = nullptr;

// --------------------------------------------------------------------------
// wxIPCMessageBase
// --------------------------------------------------------------------------

// This class and its derivatives manage the reading and writing of data from
// the socket.
class wxIPCMessageBase : public wxObject
{
public:
    wxIPCMessageBase(wxSocketBase* socket = nullptr,
                     wxTCPEventHandler* handler = nullptr)
        : m_write_data(nullptr), m_handler(handler)
    {
        Init(socket);
    }

    wxIPCMessageBase(wxSocketBase* socket, const void* data)
        : m_write_data(data), m_handler(nullptr)
    {
        Init(socket);
    }
    virtual ~wxIPCMessageBase() {};

    bool IsOk() const { return m_ipc_code != IPC_NULL; }

    // Accessors for the base object
    IPCCode GetIPCCode() const { return m_ipc_code; }
    void SetIPCCode(IPCCode ipc_code) { m_ipc_code = ipc_code; }

    wxSocketBase* GetSocket() const { return m_socket; }
    void SetSocket(wxSocketBase* socket) { m_socket = socket; }

    wxSocketError GetError() const { return m_error; }
    void SetError(wxSocketError error) { m_error = error; }

    // These accessors are here to avoid repetition in the derived objects.
    wxIPCFormat GetIPCFormat() const { return m_ipc_format; }
    void SetIPCFormat(wxIPCFormat ipc_format) { m_ipc_format = ipc_format; }

    const void* GetReadData() const { return m_read_data; }
    void SetReadData(void *data) { m_read_data = data; }

    size_t GetSize() const { return m_size; }
    void SetSize(size_t size) { m_size = size; }

    wxString GetItem() const { return m_item; }
    void SetItem(const wxString& item) { m_item = item; }

protected:
    void Init(wxSocketBase* socket)
    {
        SetIPCCode(IPC_NULL);
        SetSocket(socket);
        SetError(wxSOCKET_NOERROR);

        SetIPCFormat(wxIPC_INVALID);
        SetReadData(nullptr);
        SetSize(0);
    }

    virtual bool DataFromSocket() = 0;
    virtual bool DataToSocket() = 0;

    // primitives for read/write to socket

    bool VerifyValidSocket()
    {
        if (m_socket && m_socket->IsOk() && m_socket->IsConnected() ) return true;

        SetError(wxSOCKET_INVSOCK);
        return false;
    }

    bool Read32(wxUint32& word)
    {
        word = 0;

        if ( !VerifyValidSocket() )
            return false;

        m_socket->Read(reinterpret_cast<char *>(&word), 4);

        return VerifyLastReadCount(4);
    }

    // Reads nbytes of data from the socket into a pre-allocated buffer
    bool ReadData(void* buffer, wxUint32& nbytes)
    {
        if ( !VerifyValidSocket() )
            return false;

        m_socket->Read(buffer, nbytes);

        return VerifyLastReadCount(nbytes);
    }

    bool ReadSizeAndData();
    bool ReadIPCCode()
    {
        wxUint32 code_with_header;
        if (!Read32(code_with_header))
            return false;

        if ((code_with_header & 0xFFFFFF00) != IPCCodeHeader)
        {
            // The expected data is misaligned, which is bad.
            SetError(wxSOCKET_IOERR);
            return false;
        }

        SetIPCCode(static_cast<IPCCode>(code_with_header & 0xFF));
        return true;
    }

    bool ReadIPCFormat()
    {
        if ( !VerifyValidSocket() )
            return false;

        m_socket->Read(reinterpret_cast<char *>(&m_ipc_format), 1);

        return VerifyLastReadCount(1);
    }

    bool ReadString(wxString& str);
    bool VerifyLastReadCount(wxUint32 nbytes)
    {
        if ( !VerifyValidSocket() )
            return false;

        if (m_socket->Error())
        {
            SetError(m_socket->LastError());
            return false;
        }

        if (m_socket->LastReadCount() != nbytes)
        {
            // The expected data is misaligned, which is bad.
            SetError(wxSOCKET_IOERR);
            return false;
        }
        return true;
    }

    bool Write32(wxUint32 word)
    {
        if ( !VerifyValidSocket() )
            return false;

        m_socket->Write(reinterpret_cast<char *>(&word), 4);

        return VerifyLastWriteCount(4);
    }

    bool WriteData(const void* data, wxUint32 nbytes)
    {
        if ( !VerifyValidSocket() )
            return false;

        m_socket->Write(data, nbytes);

        return VerifyLastWriteCount(nbytes);
    }

    bool WriteSizeAndData()
    {
        if (!Write32(m_size))
            return false;

        return WriteData(m_write_data, m_size);
    }

    bool WriteIPCCode()
    {
        wxUint32 code_with_header = IPCCodeHeader | GetIPCCode();
        return Write32(code_with_header);
    }

    bool WriteIPCFormat()
    {
        if ( !VerifyValidSocket() )
            return false;

        m_socket->Write(reinterpret_cast<char *>(&m_ipc_format), 1);

        return VerifyLastWriteCount(1);
    }

    bool WriteString(const wxString& str);

    bool VerifyLastWriteCount(wxUint32 nbytes)
    {
        if ( !VerifyValidSocket() )
            return false;

        if (m_socket->Error())
        {
            SetError(m_socket->LastError());
            return false;
        }

        if (m_socket->LastWriteCount() != nbytes)
        {
            SetError(wxSOCKET_IOERR);
            return false;
        }
        return true;
    }

    IPCCode m_ipc_code;

    wxSocketBase* m_socket;
    wxSocketError m_error;

    // Members used in most of the derived messages
    wxUint32 m_size;
    wxIPCFormat m_ipc_format;
    wxString m_item;

    // immutable pointer to data from connection object
    const void* m_write_data;

    // pointer to data read from socket
    void* m_read_data;

    wxTCPEventHandler *m_handler;

    friend wxTCPEventHandler;

    wxDECLARE_NO_COPY_CLASS(wxIPCMessageBase);
    wxDECLARE_CLASS(wxIPCMessageBase);
};

// Reads a 32-bit word to get the desired buffer m_size from the socket, allocates
// a buffer of that size, then read m_size bytes worth of data from the socket
// into m_read_data.
bool wxIPCMessageBase::ReadSizeAndData()
{
    if (!Read32(m_size))
        return false;

    wxASSERT_MSG( m_handler, "No handler for read allocation");
    if ( !m_handler )
        return false;

    m_read_data = m_handler->GetBufPtr(m_size);

    m_socket->Read(m_read_data, m_size);
    return VerifyLastReadCount(m_size);
}

bool wxIPCMessageBase::ReadString(wxString& str)
{
    wxUint32 len = 0;
    if ( !Read32(len) )
        return false;

    if ( len > 0 )
    {

#if wxUSE_UNICODE
        wxCharBuffer buf(len);
        if ( !buf || !ReadData(buf.data(), len) )
            return false;
#else
        wxStringBuffer buf(str, len);
        if ( !buf || !ReadData(buf,len) )
            return false;
#endif

        if ( !VerifyLastReadCount(len))
            return false;

#if wxUSE_UNICODE
        str = wxConvUTF8.cMB2WC(buf.data(), len, nullptr);
#endif
    }

    return true;
}

bool wxIPCMessageBase::WriteString(const wxString& str)
{
#if wxUSE_UNICODE
    const wxWX2MBbuf buf = str.mb_str(wxConvUTF8);
    size_t len = buf.length();
#else
    const wxWX2MBbuf buf = str.mb_str();
    size_t len = str.size();
#endif

    if (len > 0)
        return Write32(len) && WriteData(buf, len);
    else
        return Write32(len);
}


// ==========================================================================
// wxIPCMessages
// ==========================================================================

class wxIPCMessageExecute : public wxIPCMessageBase
{
public:
    wxIPCMessageExecute(wxSocketBase* socket = nullptr,
                        wxTCPEventHandler* handler = nullptr)
        : wxIPCMessageBase(socket, handler)
    {
        SetIPCCode(IPC_EXECUTE);
    }

    wxIPCMessageExecute(wxSocketBase* socket,
                        const void* data,
                        size_t size,
                        wxIPCFormat format)
        : wxIPCMessageBase(socket, data)
    {
        SetIPCCode(IPC_EXECUTE);
        SetSize((wxUint32) size);
        SetIPCFormat(format);
    }

protected:
    bool DataToSocket() override
    {
        return WriteIPCFormat() && WriteSizeAndData();
    }

    bool DataFromSocket() override
    {
        return ReadIPCFormat() && ReadSizeAndData();
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageExecute);
};

class wxIPCMessageRequest : public wxIPCMessageBase
{
public:
    wxIPCMessageRequest(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_REQUEST);
        SetIPCFormat(wxIPC_INVALID);
    }

    wxIPCMessageRequest(wxSocketBase* socket,
                        const wxString& item,
                        wxIPCFormat format)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_REQUEST);
        SetItem(item);
        SetIPCFormat(format);
    }

protected:
    bool DataToSocket() override
    {
        return WriteIPCFormat() && WriteString(m_item);
    }

    bool DataFromSocket() override
    {
        return ReadIPCFormat() && ReadString(m_item);
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageRequest);
};

class wxIPCMessageRequestReply : public wxIPCMessageBase
{
public:
    wxIPCMessageRequestReply(wxSocketBase* socket = nullptr,
                             wxTCPEventHandler* handler = nullptr)
        : wxIPCMessageBase(socket, handler)
    {
        SetIPCCode(IPC_REQUEST_REPLY);
    }

    wxIPCMessageRequestReply(wxSocketBase* socket,
                             const void* user_data,
                             size_t user_size,
                             const wxString& item,
                             wxIPCFormat format)
        : wxIPCMessageBase(socket, user_data)
    {
        SetIPCCode(IPC_REQUEST_REPLY);
        SetItem(item);
        SetIPCFormat(format);

        wxUint32 len = user_size;
        if ( user_size == wxNO_LEN )
        {
            switch ( format )
            {
            case wxIPC_TEXT:
            case wxIPC_UTF8TEXT:
                len = strlen((const char *)user_data) + 1;  // includes final NUL
                break;
            case wxIPC_UNICODETEXT:
                len = (wcslen((const wchar_t *)user_data) + 1) * sizeof(wchar_t);  // includes final NUL
                break;
            default:
                len = 0;
            }
        }
        SetSize(len);
    }

protected:
    bool DataToSocket() override
    {
        return WriteIPCFormat() && WriteString(m_item) && WriteSizeAndData();
    }

    bool DataFromSocket() override
    {
        return ReadIPCFormat() && ReadString(m_item) && ReadSizeAndData();
    }
    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageRequestReply);
};

class wxIPCMessagePoke : public wxIPCMessageBase
{
public:
    wxIPCMessagePoke(wxSocketBase* socket = nullptr,
                     wxTCPEventHandler* handler = nullptr)
        : wxIPCMessageBase(socket, handler)
    {
        SetIPCCode(IPC_POKE);
    }

    wxIPCMessagePoke(wxSocketBase* socket,
                     const wxString& item,
                     const void* data,
                     size_t size,
                     wxIPCFormat format)
        : wxIPCMessageBase(socket, data)
    {
        SetIPCCode(IPC_POKE);
        SetItem(item);
        SetIPCFormat(format);
        SetSize(size);
    }

protected:
    bool DataToSocket() override
    {
        return WriteIPCFormat() && WriteString(m_item) && WriteSizeAndData();
    }

    bool DataFromSocket() override
    {
        return ReadIPCFormat() && ReadString(m_item) && ReadSizeAndData();
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessagePoke);
};

class wxIPCMessageAdviseStart : public wxIPCMessageBase
{
public:
    wxIPCMessageAdviseStart(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_ADVISE_START);
    }

    wxIPCMessageAdviseStart(wxSocketBase* socket, const wxString& item)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_ADVISE_START);
        SetItem(item);
    }

protected:
    bool DataToSocket() override
    {
        return WriteString(m_item);
    }

    bool DataFromSocket() override
    {
        return ReadString(m_item);
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageAdviseStart);
};

class wxIPCMessageAdviseStop : public wxIPCMessageBase
{
public:
    wxIPCMessageAdviseStop(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_ADVISE_STOP);
    }

    wxIPCMessageAdviseStop(wxSocketBase* socket, const wxString& item)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_ADVISE_STOP);
        SetItem(item);
    }

protected:
    bool DataToSocket() override
    {
        return WriteString(m_item);
    }

    bool DataFromSocket() override
    {
        return ReadString(m_item);
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageAdviseStop);
};

class wxIPCMessageAdvise : public wxIPCMessageBase
{
public:
    wxIPCMessageAdvise(wxSocketBase* socket = nullptr,
                       wxTCPEventHandler* handler = nullptr)
        : wxIPCMessageBase(socket, handler)
    {
        SetIPCCode(IPC_ADVISE);
    }

    wxIPCMessageAdvise(wxSocketBase* socket,
                       const wxString& item,
                       const void* data,
                       size_t size,
                       wxIPCFormat format)
        : wxIPCMessageBase(socket, data)
    {
        SetIPCCode(IPC_ADVISE);
        SetItem(item);
        SetIPCFormat(format);
        SetSize(size);
    }

protected:
    bool DataToSocket() override
    {
        return WriteIPCFormat() && WriteString(m_item) && WriteSizeAndData();
    }

    bool DataFromSocket() override
    {
        return ReadIPCFormat() && ReadString(m_item) && ReadSizeAndData();
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageAdvise);
};

// m_item to be used for failure reason (for debug)
class wxIPCMessageFail : public wxIPCMessageBase
{
public:
    wxIPCMessageFail(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_FAIL);
    }

    wxIPCMessageFail(wxSocketBase* socket, const wxString& item)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_FAIL);
        SetItem(item);
    }

protected:
    bool DataToSocket() override
    {
        return WriteString(m_item);
    }

    bool DataFromSocket() override
    {
        return ReadString(m_item);
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageFail);
};

// Message returned when socket fails to read an wxIPCMessage
class wxIPCMessageNull : public wxIPCMessageBase
{
public:
    wxIPCMessageNull(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_NULL);
    }

protected:
    bool DataToSocket() override
    {
        return false;
    }

    bool DataFromSocket() override
    {
        return false;
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageNull);
};

class wxIPCMessageConnect : public wxIPCMessageBase
{
public:
    wxIPCMessageConnect(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_CONNECT);
    }

    wxIPCMessageConnect(wxSocketBase* socket, const wxString& topic)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_CONNECT);
        SetTopic(topic);
    }

    wxString GetTopic() const { return m_topic; }
    void SetTopic(const wxString& topic) { m_topic = topic; }

protected:
    bool DataToSocket() override
    {
        return WriteString(m_topic);
    }

    bool DataFromSocket() override
    {
        bool b = ReadString(m_topic);
        return b;
    }

    wxString m_topic;

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageConnect);
};


class wxIPCMessageDisconnect : public wxIPCMessageBase
{
public:
    wxIPCMessageDisconnect(wxSocketBase* socket = nullptr)
        : wxIPCMessageBase(socket)
    {
        SetIPCCode(IPC_DISCONNECT);
    }

protected:
    bool DataToSocket() override
    {
        return true;
    }

    bool DataFromSocket() override
    {
        return true;
    }

    wxDECLARE_DYNAMIC_CLASS(wxIPCMessageDisconnect);
};

// Utility to ensure deletion of wxIPCMessageBase after use
class wxIPCMessageBaseLocker
{
public:
    wxIPCMessageBaseLocker(wxIPCMessageBase* msg)
    {
        m_msg = msg;
    }

    ~wxIPCMessageBaseLocker()
    {
        if (m_msg) delete m_msg;
    }

    wxIPCMessageBase* m_msg;
};

// ==========================================================================
// implementation
// ==========================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxTCPServer, wxServerBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxTCPClient, wxClientBase);
wxIMPLEMENT_CLASS(wxTCPConnection, wxConnectionBase);

wxIMPLEMENT_CLASS(wxIPCMessageBase,wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageExecute,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageRequest,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageRequestReply,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessagePoke,wxIPCMessageBase);

wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageAdviseStart,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageAdviseStop,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageAdvise,wxIPCMessageBase);

wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageConnect,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageDisconnect,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageFail,wxIPCMessageBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxIPCMessageNull,wxIPCMessageBase);

// --------------------------------------------------------------------------
// wxTCPClient
// --------------------------------------------------------------------------

wxTCPClient::wxTCPClient()
    : wxClientBase()
{
}

bool wxTCPClient::ValidHost(const wxString& host)
{
    wxIPV4address addr;

    return addr.Hostname(host);
}

wxConnectionBase *wxTCPClient::MakeConnection(const wxString& host,
                                              const wxString& serverName,
                                              const wxString& topic)
{
    wxSockAddress *addr = GetAddressFromName(serverName, host);
    if ( !addr )
        return nullptr;

    wxSocketClient * const client = new wxSocketClient(wxSOCKET_WAITALL);

    bool ok = client->Connect(*addr);
    delete addr;


    if ( ok )
    {
        wxTCPEventHandler* handler = &wxTCPEventHandlerModule::GetHandler();

        // Send topic name, and enquire whether this has succeeded
        wxIPCMessageConnect msg(client, topic);
        if ( !handler->WriteMessageToSocket(msg) )
        {
            client->Destroy();
            return nullptr;
        }

        wxIPCMessageBase* msg_reply = handler->ReadMessageFromSocket(client);
        wxIPCMessageBaseLocker lock(msg_reply);

        // OK! Confirmation.
        if ( msg_reply->GetIPCCode() == IPC_CONNECT )
        {
            wxTCPConnection *
                connection = (wxTCPConnection *)OnMakeConnection ();

            if ( connection )
            {
                if (wxDynamicCast(connection, wxTCPConnection))
                {
                    connection->m_sock = client;
                    connection->m_sock->SetTimeout(wxIPCTimeout);
                    connection->m_handler = handler;
                    connection->m_topic = topic;

                    client->SetEventHandler(*handler, _SOCKET_INPUT_ID);
                    client->SetClientData(connection);
                    handler->RegisterConnectionSocket(client);
                    client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
                    client->Notify(true);
                    return connection;
                }
                else
                {
                    delete connection;
                    // and fall through to delete everything else
                }
            }
        }
        else if ( msg_reply->GetIPCCode() == IPC_FAIL )
        {
            wxIPCMessageFail* msg_fail =
                wxDynamicCast(msg_reply, wxIPCMessageFail);

            if (msg_fail)
                wxLogDebug("FailMessage received: " + msg_fail->GetItem());
        }
    }

    // Something went wrong
    client->Destroy();

    return nullptr;
}

wxConnectionBase *wxTCPClient::OnMakeConnection()
{
    return new wxTCPConnection();
}

// --------------------------------------------------------------------------
// wxTCPServer
// --------------------------------------------------------------------------

wxTCPServer::wxTCPServer()
    : wxServerBase()
{
    m_server = nullptr;
}

bool wxTCPServer::Create(const wxString& serverName)
{
    // Destroy previous server, if any
    if (m_server)
    {
        m_server->SetClientData(nullptr);
        m_server->Destroy();
        m_server = nullptr;
    }

    wxSockAddress *addr = GetAddressFromName(serverName);
    if ( !addr )
        return false;

#ifdef __UNIX_LIKE__
    mode_t umaskOld;
    if ( addr->Type() == wxSockAddress::UNIX )
    {
        // ensure that the file doesn't exist as otherwise calling socket()
        // would fail
        int rc = remove(serverName.fn_str());
        if ( rc < 0 && errno != ENOENT )
        {
            delete addr;

            return false;
        }

        // also set the umask to prevent the others from reading our file
        umaskOld = umask(077);
    }
    else
    {
        // unused anyhow but shut down the compiler warnings
        umaskOld = 0;
    }
#endif // __UNIX_LIKE__

    // Create a socket listening on the specified port (reusing it to allow
    // restarting the server listening on the same port as was used by the
    // previous instance of this server)
    m_server = new wxSocketServer(*addr, wxSOCKET_WAITALL | wxSOCKET_REUSEADDR);

#ifdef __UNIX_LIKE__
    if ( addr->Type() == wxSockAddress::UNIX )
    {
        // restore the umask
        umask(umaskOld);

        // save the file name to remove it later
        m_filename = serverName;
    }
#endif // __UNIX_LIKE__

    delete addr;

    if (!m_server->IsOk())
    {
        m_server->Destroy();
        m_server = nullptr;

        return false;
    }

    m_server->SetEventHandler(wxTCPEventHandlerModule::GetHandler(),
                              _SERVER_SOCKET_CONNECTION_ID);
    m_server->SetClientData(this);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    return true;
}

wxTCPServer::~wxTCPServer()
{
    if ( m_server )
    {
        m_server->SetClientData(nullptr);
        m_server->Destroy();
    }

#ifdef __UNIX_LIKE__
    if ( !m_filename.empty() )
    {
        if ( remove(m_filename.fn_str()) != 0 )
        {
            wxLogDebug(wxT("Stale AF_UNIX file '%s' left."), m_filename);
        }
    }
#endif // __UNIX_LIKE__
}

wxConnectionBase *
wxTCPServer::OnAcceptConnection(const wxString& WXUNUSED(topic))
{
    return new wxTCPConnection();
}

// --------------------------------------------------------------------------
// wxTCPConnection
// --------------------------------------------------------------------------

void wxTCPConnection::Init()
{
    m_sock = nullptr;
    m_handler = nullptr;
}

wxTCPConnection::~wxTCPConnection()
{
    Disconnect();

    if ( m_sock )
    {
        if ( m_handler )
            m_handler->UnregisterConnectionSocket(m_sock);
        m_sock->SetClientData(nullptr);
        m_sock->Destroy();
    }
}

void wxTCPConnection::Compress(bool WXUNUSED(on))
{
    // TODO
}

// Calls that CLIENT can make.
bool wxTCPConnection::Disconnect()
{
    if ( !GetConnected() )
        return true;

    if ( m_sock && m_handler)
    {
        // Send the disconnect message to the peer.
        wxIPCMessageDisconnect msg(m_sock);
        if ( !m_handler->WriteMessageToSocket(msg) )
            wxLogDebug("Failed to send IPC_DISCONNECT message");

        m_sock->Notify(false);
        m_sock->Close();
    }

    SetConnected(false);

    return true;
}

bool wxTCPConnection::DoExecute(const void *data,
                                size_t size,
                                wxIPCFormat format)
{
    if ( !m_handler )
        return false;

    wxIPCMessageExecute msg(m_sock, data, size, format);
    return m_handler->WriteMessageToSocket(msg);
}

const void *wxTCPConnection::Request(const wxString& item,
                                     size_t *size,
                                     wxIPCFormat format)
{
    if ( !m_handler )
        return nullptr;

    wxIPCMessageRequest msg(m_sock, item, format);
    wxIPCMessageBase* msg_reply = nullptr;

    if ( !m_handler->SendAndGetReply(msg, IPC_REQUEST_REPLY, m_sock, &msg_reply) )
        return nullptr;

    wxIPCMessageBaseLocker lockmsg(msg_reply);
    if ( !msg_reply || !msg_reply->IsOk() )
        return nullptr;

    if (size)
      *size = msg_reply->GetSize();

    return msg_reply->GetReadData();
}

bool wxTCPConnection::DoPoke(const wxString& item,
                             const void *data,
                             size_t size,
                             wxIPCFormat format)
{
    if ( !m_handler )
        return false;

    wxIPCMessagePoke msg(m_sock, item, data, size, format);
    return m_handler->WriteMessageToSocket(msg);
}

bool wxTCPConnection::StartAdvise(const wxString& item)
{
    if ( !m_handler )
        return false;

    wxIPCMessageAdviseStart msg(m_sock, item);
    return m_handler->SendAndGetReply(msg, IPC_ADVISE_START, m_sock, wxNO_RETURN_MESSAGE);
}

bool wxTCPConnection::StopAdvise (const wxString& item)
{
    if ( !m_handler )
        return false;

    wxIPCMessageAdviseStop msg(m_sock, item);
    return m_handler->SendAndGetReply(msg, IPC_ADVISE_STOP, m_sock, wxNO_RETURN_MESSAGE);
}


// Calls that Server can make
bool wxTCPConnection::DoAdvise(const wxString& item,
                               const void *data,
                               size_t size,
                               wxIPCFormat format)
{
    if ( !m_handler )
        return false;

    wxIPCMessageAdvise msg(m_sock, item, data, size, format);
    return m_handler->WriteMessageToSocket(msg);
}

// --------------------------------------------------------------------------
// wxTCPEventHandler implementation (private class)
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxTCPEventHandler, wxEvtHandler)
    EVT_SOCKET(_SOCKET_INPUT_ID, wxTCPEventHandler::OnSocketInput)
    EVT_SOCKET(_SERVER_SOCKET_CONNECTION_ID, wxTCPEventHandler::OnSocketConnection)
wxEND_EVENT_TABLE()

// Function that gets called when wxSocket receives info. It always
// runs on wxThread::Main
void wxTCPEventHandler::OnSocketInput(wxSocketEvent &event)
{
    wxCRIT_SECT_LOCKER(socket_processing_lock, m_cs_socket_processing);

    // The handler may receive a socket event that was queued before its socket
    // was destroyed (e.g. a wxSOCKET_LOST generated as a connection is being torn
    // down). Such an event must be ignored without dereferencing the socket,
    // since it now points to freed memory. Even obtaining the typed pointer via
    // GetSocket() performs a checked downcast (wxObject* -> wxSocketBase*), which
    // is undefined behavior on a freed object and trips UBSAN's vptr check, so
    // use reinterpret_cast (not vptr-instrumented) purely for the registry
    // pointer-value lookup below. IsConnectionSocket() only compares the pointer
    // value and never dereferences it, so it is safe to call here.
    wxSocketBase *sock =
        reinterpret_cast<wxSocketBase *>(event.GetEventObject());
    if ( !IsConnectionSocket(sock) )
        return;

    // Re-entrancy guard (see m_socketsBeingRead): the WAITALL Read/Peek below pumps
    // the event loop while waiting, and on MSW the GUI message pump can re-dispatch
    // a wxSOCKET_INPUT for a socket this thread is already reading. Bail rather than
    // re-read it (which trips wxSocketReadGuard, "read reentrancy?"): the in-progress
    // read consumes the data, and the outer loop / a re-posted wxSOCKET_INPUT drains
    // anything left.
    if ( IsReadingSocket(sock) )
        return;

    wxTCPConnection * connection = GetConnection(sock);

    // This socket is being deleted
    if ( !connection )
    {
        FailPendingReply();
        return;
    }

    if ( event.GetSocketEvent() == wxSOCKET_LOST )
    {
        FailPendingReply();
        HandleDisconnect(connection);
        return;
    }

    // Process all of the pending messages in the socket buffer.
    MainThreadReadGuard readGuard(m_socketsBeingRead, sock);
    while ( PeekAtMessageInSocket(sock) )
    {
        wxIPCMessageBase* msg = ReadMessageFromSocket(sock);

        // If a worker thread is blocked waiting for this reply, hand it over.
        if ( DeliverPendingReply(msg) )
            continue;   // msg ownership handed to the waiting worker

        wxIPCMessageBaseLocker lock_msg(msg);
        if ( !ProcessMessage(msg, sock) )
        {
            FailPendingReply();
            break;
        }
    };
}

// Process a wxSOCKET_CONNECTION event for wxServer. It always runs on
// wxThread::Main
void wxTCPEventHandler::OnSocketConnection(wxSocketEvent &event)
{
    wxSocketServer *server = (wxSocketServer *) event.GetSocket();
    if (!server)
        return;
    wxTCPServer *ipcserv = (wxTCPServer *) server->GetClientData();

    // This socket is being deleted; skip this event
    if (!ipcserv)
        return;

    if ( event.GetSocketEvent() != wxSOCKET_CONNECTION )
        return;

    // Accept the connection, getting a new socket
    wxSocketBase *sock = server->Accept();
    if (!sock)
        return;

    if ( !sock->IsOk() )
    {
        sock->Destroy();
        return;
    }

    wxIPCMessageBase* msg = ReadMessageFromSocket(sock);
    wxIPCMessageBaseLocker lock(msg);

    if ( msg->GetIPCCode() == IPC_CONNECT )
    {
        wxIPCMessageConnect* msg_conn = (wxIPCMessageConnect*) msg;

        if ( msg_conn )
        {
            if (wxDynamicCast(msg, wxIPCMessageConnect))
            {
                wxString topic = msg_conn->GetTopic();

                wxTCPConnection *new_connection =
                    (wxTCPConnection *) ipcserv->OnAcceptConnection(topic);

                if ( new_connection )
                {
                    if ( wxDynamicCast(new_connection, wxTCPConnection) )
                    {
                        // Acknowledge success
                        wxIPCMessageConnect msg_reply(sock, topic);

                        if ( WriteMessageToSocket(msg_reply) )
                        {
                            new_connection->m_topic = topic;
                            new_connection->m_sock = sock;
                            new_connection->m_handler = &wxTCPEventHandlerModule::GetHandler();

                            sock->SetTimeout(wxIPCTimeout);
                            sock->SetEventHandler(wxTCPEventHandlerModule::GetHandler(),
                                                  _SOCKET_INPUT_ID);
                            sock->SetClientData(new_connection);
                            RegisterConnectionSocket(sock);
                            sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
                            sock->Notify(true);
                            return;
                        }
                    }

                    delete new_connection;
                }
            }
        }
    }

    SendFailMessage("IPC CONNECT failed to create valid connection", sock);
    sock->Destroy();
}

// Process a single wxIPCMessage from the socket. Returns true if processing
// can continue, or false if processing should stop, usually because a
// disconnect was received.
bool wxTCPEventHandler::ProcessMessage(wxIPCMessageBase* msg, wxSocketBase *socket)
{
    if ( !msg || !msg->IsOk() )
        return false;

    wxTCPConnection * const connection = GetConnection(socket);
    if ( !connection || !connection->GetConnected() )
        return false; // socket is being deleted

    wxString errmsg;

    switch ( msg->GetIPCCode() )
    {
    case IPC_EXECUTE:
    {
        wxIPCMessageExecute* msg_execute =
            wxDynamicCast(msg, wxIPCMessageExecute);

        if ( msg_execute )
            connection->OnExecute(connection->m_topic,
                                  msg_execute->GetReadData(),
                                  msg_execute->GetSize(),
                                  msg_execute->GetIPCFormat());
        else
            errmsg = "No data read for IPC Execute";
    }
    break;

    case IPC_ADVISE:
    {
        wxIPCMessageAdvise* msg_advise =
            wxDynamicCast(msg, wxIPCMessageAdvise);

        if ( msg_advise )
            connection->OnAdvise(connection->m_topic,
                                 msg_advise->GetItem(),
                                 msg_advise->GetReadData(),
                                 msg_advise->GetSize(),
                                 msg_advise->GetIPCFormat());
        else
            errmsg = "No data read for IPC Advise";
    }
    break;

    case IPC_ADVISE_START:
    {
        wxIPCMessageAdviseStart* msg_advise_start =
            wxDynamicCast(msg, wxIPCMessageAdviseStart);

        if ( !msg_advise_start )
        {
            errmsg = "No data read for IPC Advise Start";
            break;
        }

        if ( connection->OnStartAdvise(connection->m_topic,
                                       msg_advise_start->GetItem()) )
        {
            // reply to confirm
            wxIPCMessageAdviseStart reply(socket,
                                          msg_advise_start->GetItem());
            if ( !WriteMessageToSocket(reply) )
                errmsg = "Failed to confirm IPC StartAdvise";
        }
        else
        {
            errmsg = "IPC StartAdvise refused by peer";
        }

    }
    break;

    case IPC_ADVISE_STOP:
    {
        wxIPCMessageAdviseStop* msg_advise_stop =
            wxDynamicCast(msg, wxIPCMessageAdviseStop);

        if ( !msg_advise_stop )
        {
            errmsg = "No data read for IPC Advise Stop";
            break;
        }

        if ( connection->OnStopAdvise(connection->m_topic, msg_advise_stop->GetItem()) )
        {
            // reply to confirm
            wxIPCMessageAdviseStop reply(socket, msg_advise_stop->GetItem());
            if ( !WriteMessageToSocket(reply) )
                errmsg = "Failed to confirm IPC StopAdvise";
        }
        else
        {
            errmsg = "IPC StopAdvise refused by peer";
        }

    }
    break;

    case IPC_POKE:
    {
        wxIPCMessagePoke* msg_poke =
            wxDynamicCast(msg, wxIPCMessagePoke);

        if ( msg_poke )
            connection->OnPoke(connection->m_topic,
                               msg_poke->GetItem(),
                               msg_poke->GetReadData(),
                               msg_poke->GetSize(),
                               msg_poke->GetIPCFormat());
        else
            errmsg = "No data read for IPC Poke";
    }
    break;

    case IPC_REQUEST:
    {
        wxIPCMessageRequest* msg_request =
            wxDynamicCast(msg, wxIPCMessageRequest);

        if ( !msg_request )
        {
            errmsg = "No data read for IPC Request";
            break;
        }

        size_t user_size = wxNO_LEN;
        const void *user_data = connection->OnRequest(connection->m_topic,
                                                      msg->GetItem(),
                                                      &user_size,
                                                      msg->GetIPCFormat());
        if ( !user_data )
        {
            SendFailMessage("No reply data for request.", socket);
            break;
        }

        wxIPCMessageRequestReply msg_reply(socket,
                                           user_data,
                                           user_size,
                                           msg->GetItem(),
                                           msg->GetIPCFormat());

        if ( !WriteMessageToSocket(msg_reply) )
            errmsg = "Reply failed for IPC Request";
    }
    break;

    case IPC_DISCONNECT:
        HandleDisconnect(connection);
        return false;

    case IPC_FAIL:
    {
        wxIPCMessageFail* msg_fail =
            wxDynamicCast(msg, wxIPCMessageFail);

        if ( msg_fail )
            wxLogDebug("Unexpected IPC_FAIL received: " + msg_fail->GetItem());
        else
            wxLogDebug("Unexpected IPC_FAIL, and data read failed.");

        return false;
    }

    // Silence unused-enum warnings from the compiler. These should never be
    // received in this method.
    case IPC_ADVISE_REQUEST:
    case IPC_REQUEST_REPLY:
    case IPC_CONNECT:
    case IPC_MAX:
        wxLogDebug("Invalid IPC code %d received", msg->GetIPCCode());
        return false;

    default:
        wxLogDebug("Unknown message code %d received. IPC data may be misaligned",
                   msg);
        return false;
    }

    if ( !errmsg.IsEmpty() )
    {
        wxLogDebug(errmsg);
        SendFailMessage(errmsg, socket);
    }

    return true;
}

//Runs fn on the main thread, blocking the caller until completion. On the
// main thread, fn runs directly.
void wxTCPEventHandler::RunOnMainThread(const std::function<void()>& fn)
{
    if ( wxThread::IsMain() )
    {
        fn();
        return;
    }

    // Marshal to the main thread and block until it has run the work. CallAfter()
    // posts an async method-call event that the main event loop dispatches; the
    // worker waits on the semaphore meanwhile, so the by-reference captures stay
    // valid. The wait is bounded by wxIPCTimeout.
    wxSemaphore done;
    CallAfter([&fn, &done] {
        fn();
        done.Post();
    });
    done.Wait();
}

// Messages that are sent to the server with an expected reply are handled here.
// The main complication is making sure that the request and reply are matched,
// which is handle by making sure that there is only one pending request at a
// time.
bool wxTCPEventHandler::SendAndGetReply(wxIPCMessageBase& send_msg,
                                        IPCCode expected_code,
                                        wxSocketBase* socket,
                                        wxIPCMessageBase** return_msgptr)
{
    // The structure of FindMessage changes greatly whether we are on
    // wxThread::Main or not.  Divide them into two submethods:
    if (wxThread::IsMain())
        return SendAndGetReply_MainThread(send_msg, expected_code, socket, return_msgptr);
    else
        return SendAndGetReply_WorkerThread(send_msg, expected_code, return_msgptr);
}

namespace
{

// RAII guard that leaves an already-entered critical section on destruction
// (the inverse of wxCRIT_SECT_LOCKER, which enters on construction). Used when
// the section was acquired with TryEnter() in a custom wait loop.
class CritSectLeaver
{
public:
    explicit CritSectLeaver(wxCriticalSection& cs) : m_cs(cs) {}
    ~CritSectLeaver() { m_cs.Leave(); }

private:
    wxCriticalSection& m_cs;
    wxDECLARE_NO_COPY_CLASS(CritSectLeaver);
};

} // anonymous namespace

// Take over socket processing from OnSocketInput until we find the
// return message.
bool wxTCPEventHandler::SendAndGetReply_MainThread(wxIPCMessageBase& send_msg,
                                                   IPCCode expected_code,
                                                   wxSocketBase* socket,
                                                   wxIPCMessageBase** return_msgptr)
{
    // A worker thread may have triggered m_cs_awaiting_reply while parked in
    // RunOnMainThread(), waiting for *this* (the main) thread to run its
    // marshalled socket I/O. Blocking on the lock here would stop us pumping the
    // event loop and deadlock. So acquire it without blocking the loop: spin on
    // TryEnter(), pumping pending events (the worker's RunOnMainThread() jobs)
    // and socket I/O (its reply) between attempts so the worker can finish and
    // release the lock.
    while ( !m_cs_awaiting_reply.TryEnter() )
    {
        wxEventLoopBase* const loop = wxEventLoopBase::GetActive();
        if ( !loop )
        {
            // No event loop to pump, so no worker can be waiting on us; the only
            // safe thing left is to block until the lock is free.
            m_cs_awaiting_reply.Enter();
            break;
        }

        if ( wxTheApp )
            wxTheApp->ProcessPendingEvents();
        loop->DispatchTimeout(1);
    }
    CritSectLeaver find_message_lock(m_cs_awaiting_reply);

    wxCRIT_SECT_LOCKER(socket_processing_lock, m_cs_socket_processing);

    // Mark the socket as being read on this thread so a wxSOCKET_INPUT re-dispatched
    // while our Read/Write below pumps the event loop bails out of OnSocketInput()
    // instead of re-reading the socket (see m_socketsBeingRead).
    MainThreadReadGuard readGuard(m_socketsBeingRead, socket);

    if ( !WriteMessageToSocket(send_msg) )
        return false;

    while ( GetConnection(socket) )
    {
        wxIPCMessageBase* msg = ReadMessageFromSocket(socket);
        PostSocketInputEvent(socket); // in case more messages are waiting

        if ( msg && msg->GetIPCCode() == expected_code )
        {
            // The correct message has been found, but more messages
            // may follow.
            PostSocketInputEvent(socket);

            if (return_msgptr)
                *return_msgptr = msg;
            else
                delete msg;

            return true;
        }

        // Not the correct msg to be returned.
        wxIPCMessageBaseLocker lock(msg);
        if (!ProcessMessage(msg, socket) )
            return false;
    };

    return false;
}

// worker thread
bool wxTCPEventHandler::SendAndGetReply_WorkerThread(wxIPCMessageBase& send_msg,
                                                     IPCCode expected_code,
                                                     wxIPCMessageBase** return_msgptr)
{
    // Serialize reply-expecting commands: only one reply is pending at a time.
    wxCRIT_SECT_LOCKER(txlock, m_cs_awaiting_reply);

    // Register the pending reply *before* writing, so a reply that comes back
    // before we start waiting is still recorded (DeliverPendingReply() stores it
    // and we observe it via the predicate in the wait loop below, rather than
    // blocking forever, so there is no lost-wakeup race even though we release
    // m_replyMutex between here and the wait).
    {
        wxMutexLocker setup(m_replyMutex);
        m_pending.active   = true;
        m_pending.expected = expected_code;
        m_pending.reply    = nullptr;
        m_pending.failed   = false;
    }

    // Write WITHOUT holding m_replyMutex. WriteMessageToSocket() marshals the
    // actual I/O to the main thread (RunOnMainThread) and blocks this worker
    // until the main thread runs it. The main thread's OnSocketInput() needs
    // m_replyMutex to hand replies over (DeliverPendingReply()), so holding it
    // across the write would deadlock: the worker waits on the main thread while
    // the main thread waits on the mutex the worker holds.
    bool ok = false;
    if ( WriteMessageToSocket(send_msg) )
    {
        wxMutexLocker waitlock(m_replyMutex);

        // wxCondition requires its mutex be held around WaitTimeout() (the wait
        // releases it while blocked and re-acquires it before returning). The
        // predicate guards against a reply delivered between the write and here.
        while ( !m_pending.reply && !m_pending.failed )
        {
            if ( m_replyCond.WaitTimeout(wxIPCTimeout * 1000) == wxCOND_TIMEOUT )
                break;
        }

        if ( m_pending.reply && m_pending.reply->GetIPCCode() == expected_code )
        {
            if ( return_msgptr )
                *return_msgptr = m_pending.reply;
            else
                delete m_pending.reply;
            m_pending.reply = nullptr;
            ok = true;
        }
    }

    // Drop any reply we are not handing back to the caller, and clear the
    // pending state under the lock.
    wxMutexLocker cleanup(m_replyMutex);
    if ( !ok && m_pending.reply )
    {
        delete m_pending.reply;
        m_pending.reply = nullptr;
    }
    m_pending.active = false;
    return ok;
}

bool wxTCPEventHandler::DeliverPendingReply(wxIPCMessageBase* msg)
{
    // The match against m_pending must be tested under m_replyMutex. Reading
    // m_pending.active / m_pending.expected here without the lock races the
    // worker thread updating them.
    wxMutexLocker lock(m_replyMutex);

    if ( !m_pending.active )
        return false;

    if ( !msg || msg->GetIPCCode() != m_pending.expected )
        return false;

    m_pending.reply = msg;     // worker now owns msg
    m_replyCond.Signal();
    return true;
}

// When a worker is waiting on a pending reply and some failure occurs,
// FailPendingReply makes the worker fail fast instead of waiting for timeout.
void wxTCPEventHandler::FailPendingReply()
{
    wxMutexLocker lock(m_replyMutex);
    if ( !m_pending.active )
        return;

    m_pending.failed = true;
    m_replyCond.Signal();
}

// Utility to post a wxSOCKET_INPUT event to the socket. This is sometimes
// necessary because sometimes we receive a single wxSOCKET_INPUT for several
// wxIPCMessages received, but we processed only one.  By reposting
// wxSOCKET_INPUT, the main loop re-scans and drains the rest of the pending
// IPCMessages.  A spurious event is harmless: the handler peeks, finds nothing,
// and returns.
void wxTCPEventHandler::PostSocketInputEvent(wxSocketBase* socket)
{
    if ( socket && socket->GetEventHandler() )
    {
        wxSocketEvent event(wxID_ANY);
        event.m_event = wxSOCKET_INPUT;
        event.m_clientData = socket->GetClientData();
        event.SetEventObject(socket);

        socket->GetEventHandler()->AddPendingEvent(event);
    }
}

// Create a wxIPCMessage object from the given code. Caller is responsible for
// deleting the message when done.
wxIPCMessageBase* wxTCPEventHandler::GetIPCMessageFromCode(IPCCode code, wxSocketBase* socket)
{
    switch ( code )
    {
    case IPC_EXECUTE:
        return new wxIPCMessageExecute(socket, this);

    case IPC_REQUEST:
        return new wxIPCMessageRequest(socket);

    case IPC_POKE:
        return new wxIPCMessagePoke(socket, this);

    case IPC_ADVISE_START:
        return new wxIPCMessageAdviseStart(socket);

    case IPC_ADVISE:
        return new wxIPCMessageAdvise(socket, this);

    case IPC_ADVISE_STOP:
        return new wxIPCMessageAdviseStop(socket);

    case IPC_REQUEST_REPLY:
        return new wxIPCMessageRequestReply(socket, this);

    case IPC_FAIL:
        return new wxIPCMessageFail(socket);

    case IPC_CONNECT:
        return new wxIPCMessageConnect(socket);

    case IPC_DISCONNECT:
        return new wxIPCMessageDisconnect(socket);

    default:
        return new wxIPCMessageNull(socket);
    }
}

void wxTCPEventHandler::SendFailMessage(const wxString& reason, wxSocketBase* socket)
{
    wxIPCMessageFail msg(socket, reason);

    if (!WriteMessageToSocket(msg) )
        wxLogDebug("Failed to send IPC_FAIL message: " + reason);
}

void wxTCPEventHandler::HandleDisconnect(wxTCPConnection *connection)
{
    // Connection was closed (either gracefully or not): destroy everything
    UnregisterConnectionSocket(connection->m_sock);
    connection->m_sock->Notify(false);
    connection->m_sock->Close();

    // Don't leave references to this soon-to-be-dangling connection in the
    // socket as it won't be destroyed immediately as its destruction will be
    // delayed in case there are more events pending for it
    connection->m_sock->SetClientData(nullptr);

    connection->SetConnected(false);
    connection->OnDisconnect();
}

// Reads a single message from the socket. Returns wxIPCMessageNull when no
// message was read.  The returned message must be freed by the caller.
wxIPCMessageBase* wxTCPEventHandler::ReadMessageFromSocket(wxSocketBase* socket)
{
    // ptr to returned message when successful
    wxIPCMessageBase *msg = nullptr;
    IPCCode code = IPC_NULL;
    bool ok_read = false;

    RunOnMainThread( [&] {
        // Serialize all socket I/O: wxSocketBase is not safe for concurrent use
        // from multiple threads on the same connection. The lock is taken here,
        // inside the marshalled work, so it is only ever held on the thread that
        // actually performs the I/O.
        wxCRIT_SECT_LOCKER(lock, gs_critical_io);
        wxIPCMessageNull null_msg(socket);
        ok_read = null_msg.ReadIPCCode();
        if (ok_read)
        {
            code = null_msg.GetIPCCode();
            msg = GetIPCMessageFromCode(null_msg.GetIPCCode(), socket);
            ok_read = msg->DataFromSocket();
        }
    });

    if ( ok_read )
        return msg;

    // failure
    delete msg;
    return new wxIPCMessageNull(socket);
};

// Writes this message object to the socket.
bool wxTCPEventHandler::WriteMessageToSocket(wxIPCMessageBase& msg)
{
    bool ok_write = false;
    RunOnMainThread( [&] {
        // See ReadMessageFromSocket(): gs_critical_io is taken inside the
        // marshalled work so it is never held across the worker->main handoff.
        wxCRIT_SECT_LOCKER(lock, gs_critical_io);
        ok_write = msg.WriteIPCCode() && msg.DataToSocket();
    });

    return ok_write;
};

// We determine if there is more data waiting in the socket buffer for read.
// Return true if there are enough bytes there for the IPCCodeHeader and
// IPCCode.
bool wxTCPEventHandler::PeekAtMessageInSocket(wxSocketBase* socket)
{
    if ( !socket || !socket->IsOk() )
        return false;

    wxUint32 code_with_header = 0;
    bool enough = false;

    RunOnMainThread( [&] {
        // See ReadMessageFromSocket(): gs_critical_io is taken inside the
        // marshalled work so it is never held across the worker->main handoff.
        wxCRIT_SECT_LOCKER(lock, gs_critical_io);
        socket->Peek(reinterpret_cast<char *>(&code_with_header), 4);
        enough = socket->LastCount() == 4;
    });

    return enough;
}

char* wxTCPEventHandler::GetBufPtr(size_t size)
{
    wxCRIT_SECT_LOCKER(lock, m_cs_assign_buffer);

    if (m_bufferList[m_nextAvailable] != nullptr)
        delete[] m_bufferList[m_nextAvailable];  // Free the memory from the last use

    char* ptr = new char[size];

    m_bufferList[m_nextAvailable] = ptr;
    m_nextAvailable = (m_nextAvailable + 1) % MAX_MSG_BUFFERS;

    return ptr;
}

wxTCPConnection* wxTCPEventHandler::GetConnection(wxSocketBase* socket)
{
    if ( !socket || !socket->IsOk() )
        return nullptr;

    return static_cast<wxTCPConnection *>(socket->GetClientData());
}

void wxTCPEventHandler::RegisterConnectionSocket(wxSocketBase* socket)
{
    wxCRIT_SECT_LOCKER(lock, m_cs_connection_sockets);
    m_connectionSockets.insert(socket);
}

void wxTCPEventHandler::UnregisterConnectionSocket(wxSocketBase* socket)
{
    wxCRIT_SECT_LOCKER(lock, m_cs_connection_sockets);
    m_connectionSockets.erase(socket);
}

bool wxTCPEventHandler::IsConnectionSocket(wxSocketBase* socket)
{
    wxCRIT_SECT_LOCKER(lock, m_cs_connection_sockets);
    return m_connectionSockets.find(socket) != m_connectionSockets.end();
}

#endif // wxUSE_SOCKETS && wxUSE_IPC
