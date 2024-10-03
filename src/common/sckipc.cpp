/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sckipc.cpp
// Purpose:     Interprocess communication implementation (wxSocket version)
// Author:      Julian Smart
// Modified by: Guilhem Lavaux (big rewrite) May 1997, 1998
//              Guillermo Rodriguez (updated for wxSocket v2) Jan 2000
//                                  (callbacks deprecated)    Mar 2000
//              Vadim Zeitlin (added support for Unix sockets) Apr 2002
//                            (use buffering, many fixes/cleanup) Oct 2008
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
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "wx/socket.h"

class wxIPCMessageBase;

// --------------------------------------------------------------------------
// Global variables
// --------------------------------------------------------------------------

wxCRIT_SECT_DECLARE_MEMBER(gs_critical_read);
wxCRIT_SECT_DECLARE_MEMBER(gs_critical_write);

// --------------------------------------------------------------------------
// macros and constants
// --------------------------------------------------------------------------

namespace
{

// Message codes (don't change them to avoid breaking the existing code using
// wxIPC protocol!)
enum IPCCode
{
    IPC_EXECUTE         = 1,
    IPC_REQUEST         = 2,
    IPC_POKE            = 3,
    IPC_ADVISE_START    = 4,
    IPC_ADVISE_REQUEST  = 5,
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
// wxTCPEventHandler stuff (private class)
// --------------------------------------------------------------------------

class wxTCPEventHandler : public wxEvtHandler
{
public:
    wxTCPEventHandler() : wxEvtHandler() { }

    void Client_OnRequest(wxSocketEvent& event);
    void Server_OnRequest(wxSocketEvent& event);

private:
    void HandleDisconnect(wxTCPConnection *connection);

    wxIPCMessageBase* ReadMessageFromSocket(wxSocketBase *socket);
    bool WriteMessageToSocket(wxIPCMessageBase& msg);
    bool PeekAtMessageInSocket(wxSocketBase *socket);

    char* GetBufPtr(size_t size);

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxTCPEventHandler);
};

enum
{
    _CLIENT_ONREQUEST_ID = 1000,
    _SERVER_ONREQUEST_ID
};

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

// Reads a 32-bit size from the socket, allocates a buffer of that size, then
// read nbytes worth of data from the socket into m_read_data.
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

// Member var item to be used for failure reason (for debug)
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
        wxLogMessage("write topic: " + m_topic);
        return WriteString(m_topic);
    }

    bool DataFromSocket() override
    {
        bool b = ReadString(m_topic);
        wxLogMessage("read topic: " + m_topic);
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
    wxIPCSocketStreams * const streams = new wxIPCSocketStreams(*client);

    bool ok = client->Connect(*addr);
    delete addr;

    if ( ok )
    {
        // Send topic name, and enquire whether this has succeeded
        IPCOutput(streams).Write(IPC_CONNECT, topic);

        unsigned char msg = streams->Read8();

        // OK! Confirmation.
        if (msg == IPC_CONNECT)
        {
            wxTCPConnection *
                connection = (wxTCPConnection *)OnMakeConnection ();

            if (connection)
            {
                if (wxDynamicCast(connection, wxTCPConnection))
                {
                    connection->m_topic = topic;
                    connection->m_sock  = client;
                    connection->m_streams = streams;
                    client->SetEventHandler(wxTCPEventHandlerModule::GetHandler(),
                                            _CLIENT_ONREQUEST_ID);
                    client->SetClientData(connection);
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
    }

    // Something went wrong, delete everything
    delete streams;
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
                              _SERVER_ONREQUEST_ID);
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
    m_streams = nullptr;
}

wxTCPConnection::~wxTCPConnection()
{
    Disconnect();

    if ( m_sock )
    {
        m_sock->SetClientData(nullptr);
        m_sock->Destroy();
    }

    delete m_streams;
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

    // Send the disconnect message to the peer.
    IPCOutput(m_streams).Write8(IPC_DISCONNECT);

    if ( m_sock )
    {
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
    if ( !m_sock->IsConnected() )
        return false;

    // Prepare EXECUTE message
    IPCOutput out(m_streams);
    out.Write8(IPC_EXECUTE);
    out.Write8(format);

    out.WriteData(data, size);

    return true;
}

const void *wxTCPConnection::Request(const wxString& item,
                                     size_t *size,
                                     wxIPCFormat format)
{
    if ( !m_sock->IsConnected() )
        return nullptr;

    IPCOutput(m_streams).Write(IPC_REQUEST, item, format);

    const int ret = m_streams->Read8();
    if ( ret != IPC_REQUEST_REPLY )
        return nullptr;

    // ReadData() needs a non-null size pointer but the client code can call us
    // with null pointer (this makes sense if it knows that it always works
    // with NUL-terminated strings)
    size_t sizeFallback;
    return m_streams->ReadData(this, size ? size : &sizeFallback);
}

bool wxTCPConnection::DoPoke(const wxString& item,
                             const void *data,
                             size_t size,
                             wxIPCFormat format)
{
    if ( !m_sock->IsConnected() )
        return false;

    IPCOutput out(m_streams);
    out.Write(IPC_POKE, item, format);
    out.WriteData(data, size);

    return true;
}

bool wxTCPConnection::StartAdvise(const wxString& item)
{
    if ( !m_sock->IsConnected() )
        return false;

    IPCOutput(m_streams).Write(IPC_ADVISE_START, item);

    const int ret = m_streams->Read8();

    return ret == IPC_ADVISE_START;
}

bool wxTCPConnection::StopAdvise (const wxString& item)
{
    if ( !m_sock->IsConnected() )
        return false;

    IPCOutput(m_streams).Write(IPC_ADVISE_STOP, item);

    const int ret = m_streams->Read8();

    return ret == IPC_ADVISE_STOP;
}

// Calls that SERVER can make
bool wxTCPConnection::DoAdvise(const wxString& item,
                               const void *data,
                               size_t size,
                               wxIPCFormat format)
{
    if ( !m_sock->IsConnected() )
        return false;

    IPCOutput out(m_streams);
    out.Write(IPC_ADVISE, item, format);
    out.WriteData(data, size);

    return true;
}

// --------------------------------------------------------------------------
// wxTCPEventHandler (private class)
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxTCPEventHandler, wxEvtHandler)
    EVT_SOCKET(_CLIENT_ONREQUEST_ID, wxTCPEventHandler::Client_OnRequest)
    EVT_SOCKET(_SERVER_ONREQUEST_ID, wxTCPEventHandler::Server_OnRequest)
wxEND_EVENT_TABLE()

void wxTCPEventHandler::HandleDisconnect(wxTCPConnection *connection)
{
    // connection was closed (either gracefully or not): destroy everything
    connection->m_sock->Notify(false);
    connection->m_sock->Close();

    // don't leave references to this soon-to-be-dangling connection in the
    // socket as it won't be destroyed immediately as its destruction will be
    // delayed in case there are more events pending for it
    connection->m_sock->SetClientData(nullptr);

    connection->SetConnected(false);
    connection->OnDisconnect();
}

{
        return;

    // This socket is being deleted; skip this event
        return;

        return;




    {

            {


            }














            break;


            break;

            break;
    }


void wxTCPEventHandler::Server_OnRequest(wxSocketEvent &event)
{
    wxSocketServer *server = (wxSocketServer *) event.GetSocket();
    if (!server)
        return;
    wxTCPServer *ipcserv = (wxTCPServer *) server->GetClientData();

    // This socket is being deleted; skip this event
    if (!ipcserv)
        return;

    if (event.GetSocketEvent() != wxSOCKET_CONNECTION)
        return;

    // Accept the connection, getting a new socket
    wxSocketBase *sock = server->Accept();
    if (!sock)
        return;
    if (!sock->IsOk())
    {
        sock->Destroy();
        return;
    }

    wxIPCSocketStreams *streams = new wxIPCSocketStreams(*sock);

    {
        IPCOutput out(streams);

        const int msg = streams->Read8();
        if ( msg == IPC_CONNECT )
        {
            const wxString topic = streams->ReadString();

            wxTCPConnection *new_connection =
                (wxTCPConnection *)ipcserv->OnAcceptConnection (topic);

            if (new_connection)
            {
                if (wxDynamicCast(new_connection, wxTCPConnection))
                {
                    // Acknowledge success
                    out.Write8(IPC_CONNECT);

                    new_connection->m_sock = sock;
                    new_connection->m_streams = streams;
                    new_connection->m_topic = topic;
                    sock->SetEventHandler(wxTCPEventHandlerModule::GetHandler(),
                                          _CLIENT_ONREQUEST_ID);
                    sock->SetClientData(new_connection);
                    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
                    sock->Notify(true);
                    return;
                }
                else
                {
                    delete new_connection;
                    // and fall through to delete everything else
                }
            }
        }

        // Something went wrong, send failure message and delete everything
        out.Write8(IPC_FAIL);
    } // IPCOutput object is destroyed here, before destroying stream

    delete streams;
    sock->Destroy();
// Reads a single message from the socket. Returns wxIPCMessageNull when no
// message was read.  The returned message must be freed by the caller.
wxIPCMessageBase* wxTCPEventHandler::ReadMessageFromSocket(wxSocketBase* socket)
{
    // ensure that we read from the socket without any read call from another
    // thread
    wxCRIT_SECT_LOCKER(lock, gs_critical_read);

    wxIPCMessageNull* null_msg = new wxIPCMessageNull(socket);
    if ( !null_msg->ReadIPCCode() )
        return null_msg;

    // ptr to returned message when successful
    wxIPCMessageBase *msg = nullptr;

    switch ( null_msg->GetIPCCode() )
    {
    case IPC_EXECUTE:
        msg = new wxIPCMessageExecute(socket, this);
        break;

    case IPC_REQUEST:
        msg = new wxIPCMessageRequest(socket);
        break;

    case IPC_POKE:
        msg = new wxIPCMessagePoke(socket, this);
        break;

    case IPC_ADVISE_START:
        msg = new wxIPCMessageAdviseStart(socket);
        break;

    case IPC_ADVISE:
        msg = new wxIPCMessageAdvise(socket, this);
        break;

    case IPC_ADVISE_STOP:
        msg = new wxIPCMessageAdviseStop(socket);
        break;

    case IPC_REQUEST_REPLY:
        msg = new wxIPCMessageRequestReply(socket, this);
        break;

    case IPC_FAIL:
        msg = new wxIPCMessageFail(socket);
        break;

    case IPC_CONNECT:
        msg = new wxIPCMessageConnect(socket);
        break;

    case IPC_DISCONNECT:
        msg = new wxIPCMessageDisconnect(socket);
        break;

    default:
        // faulty message indicates data misalignment
        null_msg->SetError(wxSOCKET_IOERR);
        return null_msg;
    }

    if (!msg->DataFromSocket())
    {
        null_msg->SetError(msg->GetError());
        delete msg;
        return null_msg;
    }

    delete null_msg;
    return msg;
};

// Writes this message object to the socket.
bool wxTCPEventHandler::WriteMessageToSocket(wxIPCMessageBase& msg)
{
    // ensure that we write to the socket without any write call from another
    // thread
    wxCRIT_SECT_LOCKER(lock, gs_critical_write);

    return msg.WriteIPCCode() && msg.DataToSocket();
};

// We determine if there is more data waiting in the socket buffer for read.
// Return true if there are enough bytes there for the IPCCodeHeader and
// IPCCode.
bool wxTCPEventHandler::PeekAtMessageInSocket(wxSocketBase* socket)
{
    wxCRIT_SECT_LOCKER(lock, gs_critical_read);

    if ( !socket || !socket->IsOk() )
        return false;

    wxUint32 code_with_header = 0;

    socket->Peek(reinterpret_cast<char *>(&code_with_header), 4);

    return socket->LastCount() == 4;
}

}

#endif // wxUSE_SOCKETS && wxUSE_IPC
