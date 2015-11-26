/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/ipcclient.cpp
// Purpose:     wxIPC client side benchmarks
// Author:      Vadim Zeitlin
// Created:     2008-10-13
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "bench.h"

#include "wx/evtloop.h"

// do this before including wx/ipc.h under Windows to use TCP even there
#define wxUSE_DDE_FOR_IPC 0
#include "wx/ipc.h"
#include "../../samples/ipc/ipcsetup.h"

namespace
{

class PokeAdviseConn : public wxConnection
{
public:
    PokeAdviseConn() { m_gotAdvised = false; }

    bool GotAdvised()
    {
        if ( !m_gotAdvised )
            return false;

        m_gotAdvised = false;

        return true;
    }

    const wxString& GetItem() const { return m_item; }

    virtual bool OnAdvise(const wxString& topic,
                          const wxString& item,
                          const void *data,
                          size_t size,
                          wxIPCFormat format)
    {
        m_gotAdvised = true;

        if ( topic != IPC_BENCHMARK_TOPIC ||
                item != IPC_BENCHMARK_ITEM ||
                    !IsTextFormat(format) )
        {
            m_item = "ERROR";
            return false;
        }

        m_item = GetTextFromData(data, size, format);

        return true;
    }

private:
    wxString m_item;
    bool m_gotAdvised;

    wxDECLARE_NO_COPY_CLASS(PokeAdviseConn);
};

class PokeAdviseClient : public wxClient
{
public:
    // provide a convenient helper taking care of connecting to the right
    // server/service/topic and returning the connection of the derived type
    // (or NULL if we failed to connect)
    PokeAdviseConn *Connect()
    {
        wxString host = Bench::GetStringParameter();
        if ( host.empty() )
            host = IPC_HOST;

        wxString service;
        int port = Bench::GetNumericParameter();
        if ( !port )
            service = IPC_SERVICE;
        else
            service.Printf("%d", port);

        return static_cast<PokeAdviseConn *>(
                MakeConnection(host, service, IPC_BENCHMARK_TOPIC));
    }


    // override base class virtual to use a custom connection class
    virtual wxConnectionBase *OnMakeConnection()
    {
        return new PokeAdviseConn;
    }
};

class PokeAdvisePersistentConnection
{
public:
    PokeAdvisePersistentConnection()
    {
        m_client = new PokeAdviseClient;
        m_conn = m_client->Connect();
        if ( m_conn )
            m_conn->StartAdvise(IPC_BENCHMARK_ITEM);
    }

    ~PokeAdvisePersistentConnection()
    {
        if ( m_conn )
        {
            m_conn->StopAdvise(IPC_BENCHMARK_ITEM);
            m_conn->Disconnect();
        }

        delete m_client;
    }

    PokeAdviseConn *Get() const { return m_conn; }

private:
    PokeAdviseClient *m_client;
    PokeAdviseConn *m_conn;
    bool m_initDone;

    wxDECLARE_NO_COPY_CLASS(PokeAdvisePersistentConnection);
};

PokeAdvisePersistentConnection *theConnection = NULL;

bool ConnInit()
{
    theConnection = new PokeAdvisePersistentConnection;
    if ( !theConnection->Get() )
    {
        delete theConnection;
        theConnection = NULL;
        return false;
    }

    return true;
}

void ConnDone()
{
    delete theConnection;
}

} // anonymous namespace

BENCHMARK_FUNC_WITH_INIT(IPCPokeAdvise, ConnInit, ConnDone)
{
    wxEventLoop loop;

    PokeAdviseConn * const conn = theConnection->Get();

    const wxString s(1024, '@');

    if ( !conn->Poke(IPC_BENCHMARK_ITEM, s) )
        return false;

    while ( !conn->GotAdvised() )
        loop.Dispatch();

    if ( conn->GetItem() != s )
        return false;

    return true;
}
