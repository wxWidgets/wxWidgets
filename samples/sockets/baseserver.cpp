/////////////////////////////////////////////////////////////////////////////
// Name:        samples/sockbase/client.cpp
// Purpose:     Sockets sample for wxBase
// Author:      Lukasz Michalski
// Modified by:
// Created:     27.06.2005
// Copyright:   (c) 2005 Lukasz Michalski <lmichalski@user.sourceforge.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wx.h"
#include "wx/socket.h"
#include "wx/event.h"
#include "wx/list.h"
#include "wx/cmdline.h"
#include "wx/datetime.h"
#include "wx/timer.h"
#include "wx/thread.h"

const char *GetSocketErrorMsg(int pSockError)
{
    switch(pSockError)
    {
        case wxSOCKET_NOERROR:
            return "wxSOCKET_NOERROR";

        case wxSOCKET_INVOP:
            return "wxSOCKET_INVOP";

        case wxSOCKET_IOERR:
            return "wxSOCKET_IOERR";

        case wxSOCKET_INVADDR:
            return "wxSOCKET_INVADDR";

        case wxSOCKET_NOHOST:
            return "wxSOCKET_NOHOST";

        case wxSOCKET_INVPORT:
            return "wxSOCKET_INVPORT";

        case wxSOCKET_WOULDBLOCK:
            return "wxSOCKET_WOULDBLOCK";

        case wxSOCKET_TIMEDOUT:
            return "wxSOCKET_TIMEDOUT";

        case wxSOCKET_MEMERR:
            return "wxSOCKET_MEMERR";

        default:
            return "Unknown";
    }
}

//event sent by workers to server class
//after client is served
const wxEventType wxEVT_WORKER = wxNewEventType();
#define EVT_WORKER(func) wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_WORKER, -1, -1, (wxObjectEventFunction) (wxEventFunction) (WorkerEventFunction) & func, (wxObject *) nullptr ),

class WorkerEvent : public wxEvent
{
public:
    WorkerEvent(void* pSender)
    {
        SetId(-1);
        SetEventType(wxEVT_WORKER);
        m_sender = pSender;
        m_exit = false;
        m_workerFailed = false;
    }

    virtual wxEvent* Clone() const override
    {
        return new WorkerEvent(*this);
    }

    void* m_sender;
    bool m_exit;
    bool m_workerFailed;
};

typedef void (wxEvtHandler::*WorkerEventFunction)(WorkerEvent&);

class ThreadWorker;
class EventWorker;

WX_DECLARE_LIST(ThreadWorker, TList);
WX_DECLARE_LIST(EventWorker, EList);

//main server class contains listening socket
//and list of two type worker classes that serve clients
class Server : public wxApp
{
    wxDECLARE_EVENT_TABLE();
public:
    Server() : m_maxConnections(-1) {}
    ~Server() {}
private:
    enum WorkMode
    {
      MIXED,
      THREADS,
      EVENTS
    };

    virtual bool OnInit() override;
    virtual int OnExit() override;

    void OnInitCmdLine(wxCmdLineParser& pParser) override;
    bool OnCmdLineParsed(wxCmdLineParser& pParser) override;

    void OnSocketEvent(wxSocketEvent& pEvent);
    void OnWorkerEvent(WorkerEvent& pEvent);
    void OnTimerEvent(wxTimerEvent& pEvent);
    void DumpStatistics();

    TList m_threadWorkers;
    EList m_eventWorkers;
    WorkMode m_workMode;
    wxSocketServer* m_listeningSocket;

    // statistics
    unsigned m_threadWorkersCreated;
    unsigned m_threadWorkersDone;
    unsigned m_threadWorkersFailed;
    unsigned m_maxThreadWorkers;

    unsigned m_eventWorkersCreated;
    unsigned m_eventWorkersDone;
    unsigned m_eventWorkersFailed;
    unsigned m_maxEventWorkers;

    long int m_maxConnections;

    unsigned short m_port;

    wxTimer mTimer;
};

wxDECLARE_APP(Server);

// just some common things shared between ThreadWorker and EventWorker
class WorkerBase
{
protected:
    // outputs log message with IP and TCP port number prepended
    void LogWorker(const wxString& msg, wxLogLevel level = wxLOG_Info)
    {
        wxLogGeneric(level,
                     "%s:%d %s", m_peer.IPAddress(), m_peer.Service(), msg);
    }

    wxIPV4address m_peer;
};

//thread based worker reads signature and all data first from connected client
//and resends data to client after reading
class ThreadWorker : public wxThread, private WorkerBase
{
public:
    ThreadWorker(wxSocketBase* pSocket);
    virtual ExitCode Entry() override;

private:
    wxSocketBase* m_socket;
};

//event based worker reads signature and creates buffer for incoming data.
//When part of data arrives this worker resends it as soon as possible.
class EventWorker : public wxEvtHandler, private WorkerBase
{
public:
    EventWorker(wxSocketBase* pSock);
    virtual ~EventWorker();

private:
    wxSocketBase* m_socket;

    unsigned char m_signature[2];
    char* m_inbuf;
    int m_infill;
    int m_size;
    char* m_outbuf;
    int m_outfill;
    int m_written;

    void OnSocketEvent(wxSocketEvent& pEvent);
    void DoWrite();
    void DoRead();

    wxDECLARE_EVENT_TABLE();
};

/******************* Implementation ******************/
wxIMPLEMENT_APP_CONSOLE(Server);

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TList);
WX_DEFINE_LIST(EList);


void
Server::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxApp::OnInitCmdLine(pParser);
    pParser.AddSwitch("t","threads","Use thread based workers only");
    pParser.AddSwitch("e","events","Use event based workers only");
    pParser.AddOption("m","max","Exit after <n> connections",
                      wxCMD_LINE_VAL_NUMBER);
    pParser.AddOption("p","port","listen on given port (default 3000)",
                      wxCMD_LINE_VAL_NUMBER);
}

void
Server::DumpStatistics()
{
    wxString mode;
    switch(m_workMode)
    {
        case EVENTS:
            mode = "Event based workers";
            break;
        case THREADS:
            mode = "Thread based workers";
            break;
        case MIXED:
            mode = "Event and thread based workers";
            break;
    }
    wxLogMessage("Server mode: %s",mode);
    wxLogMessage("\t\t\t\tThreads\tEvents\tTotal");
    wxLogMessage("Workers created:\t\t%d\t%d\t%d",
                 m_threadWorkersCreated,
                 m_eventWorkersCreated,
                 m_threadWorkersCreated + m_eventWorkersCreated);
    wxLogMessage("Max concurrent workers:\t%d\t%d\t%d",
                 m_maxThreadWorkers,
                 m_maxEventWorkers,
                 m_maxThreadWorkers + m_maxEventWorkers);
    wxLogMessage("Workers failed:\t\t%d\t%d\t%d",
                 m_threadWorkersFailed,
                 m_eventWorkersFailed,
                 m_threadWorkersFailed + m_eventWorkersFailed);
    wxLogMessage("Workers done:\t\t%d\t%d\t%d",
                 m_threadWorkersDone,
                 m_eventWorkersDone,
                 m_threadWorkersDone + m_eventWorkersDone);

    if ((int)(m_threadWorkersDone+m_eventWorkersDone) == m_maxConnections)
    {
        wxLogMessage("%ld connection(s) served, exiting",m_maxConnections);
        ExitMainLoop();
    }
}


bool
Server::OnCmdLineParsed(wxCmdLineParser& pParser)
{
    if (pParser.Found("verbose"))
    {
        wxLog::AddTraceMask("wxSocket");
        wxLog::AddTraceMask("epolldispatcher");
        wxLog::AddTraceMask("selectdispatcher");
        wxLog::AddTraceMask("thread");
        wxLog::AddTraceMask("events");
        wxLog::AddTraceMask("timer");
    }

    if (pParser.Found("m",&m_maxConnections))
    {
        wxLogMessage("%ld connection(s) to exit",m_maxConnections);
    }

    long port;
    if (pParser.Found("p", &port))
    {
        if ( port <= 0 || port > USHRT_MAX )
        {
            wxLogError("Invalid port number %ld, must be in 0..%u range.",
                       port, USHRT_MAX);
            return false;
        }

        m_port = static_cast<unsigned short>(port);
        wxLogMessage("Will listen on port %u", m_port);
    }

    if (pParser.Found("t"))
        m_workMode = THREADS;
    else if (pParser.Found("e"))
        m_workMode = EVENTS;
    else
        m_workMode = MIXED;

    return wxApp::OnCmdLineParsed(pParser);
}

bool Server::OnInit()
{
    wxLog* logger = new wxLogStderr();
    wxLog::SetActiveTarget(logger);

    m_port = 3000;

    //send interesting things to console
    if (!wxApp::OnInit())
        return false;

    //setup listening socket
    wxIPV4address la;
    la.Service(m_port);
    m_listeningSocket = new wxSocketServer(la,wxSOCKET_NOWAIT|wxSOCKET_REUSEADDR);
    m_listeningSocket->SetEventHandler(*this);
    m_listeningSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_listeningSocket->Notify(true);
    if (!m_listeningSocket->IsOk())
    {
        wxLogError("Cannot bind listening socket");
        return false;
    }

    m_threadWorkersCreated = 0;
    m_threadWorkersDone = 0;
    m_threadWorkersFailed = 0;
    m_maxThreadWorkers = 0;

    m_eventWorkersCreated = 0;
    m_eventWorkersDone = 0;
    m_eventWorkersFailed = 0;
    m_maxEventWorkers = 0;

    wxLogMessage("Server listening at port %u, waiting for connections", m_port);
    return true;
}


int Server::OnExit()
{
    for ( TList::compatibility_iterator it = m_threadWorkers.GetFirst();
          it;
          it = it->GetNext() )
    {
        it->GetData()->Wait();
        delete it->GetData();
    }

    for ( EList::compatibility_iterator it2 = m_eventWorkers.GetFirst();
          it2;
          it2->GetNext() )
    {
        delete it2->GetData();
    }

    m_threadWorkers.Clear();
    m_eventWorkers.Clear();
    m_listeningSocket->Destroy();
    return 0;
}

void Server::OnSocketEvent(wxSocketEvent& pEvent)
{
    switch(pEvent.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            wxLogError("Unexpected wxSOCKET_INPUT in wxSocketServer");
            break;
        case wxSOCKET_OUTPUT:
            wxLogError("Unexpected wxSOCKET_OUTPUT in wxSocketServer");
        break;
        case wxSOCKET_CONNECTION:
        {
            wxSocketBase* sock = m_listeningSocket->Accept();
            wxIPV4address addr;
            if (!sock->GetPeer(addr))
            {
                wxLogError("Server: cannot get peer info");
            } else {
                wxLogMessage("Got connection from %s:%d",addr.IPAddress(), addr.Service());
            }
            bool createThread;

            if (m_workMode != MIXED)
                createThread = m_workMode == THREADS;
            else
                createThread = (wxDateTime::Now().GetSecond())%2 == 0;

            if (createThread)
            {
                ThreadWorker* c = new ThreadWorker(sock);
                if (c->Create() == wxTHREAD_NO_ERROR)
                {
                    m_threadWorkers.Append(c);
                    if (m_threadWorkers.GetCount() > m_maxThreadWorkers)
                    m_maxThreadWorkers++;
                    m_threadWorkersCreated++;
                    c->Run();
                }
                else
                {
                    wxLogError("Server: cannot create next thread (current threads: %d", m_threadWorkers.size());
                }
            }
            else
            {
                EventWorker* w = new EventWorker(sock);
                m_eventWorkers.Append(w);
                if (m_eventWorkers.GetCount() > m_maxEventWorkers)
                m_maxEventWorkers++;
                m_eventWorkersCreated++;
            }
        }
        break;
        case wxSOCKET_LOST:
            wxLogError("Unexpected wxSOCKET_LOST in wxSocketServer");
        break;
    }
}

void  Server::OnWorkerEvent(WorkerEvent& pEvent)
{
    //wxLogMessage("Got worker event");
    for(TList::compatibility_iterator it = m_threadWorkers.GetFirst(); it ; it = it->GetNext())
    {
        if (it->GetData() == pEvent.m_sender)
        {
            wxLogVerbose("Deleting thread worker (%lu left)",
                         static_cast<unsigned long>( m_threadWorkers.GetCount() ));
            it->GetData()->Wait();
            delete it->GetData();
            m_threadWorkers.DeleteNode(it);
            if (!pEvent.m_workerFailed)
                m_threadWorkersDone++;
            else
                m_threadWorkersFailed++;
            break;
        }
    }
    for(EList::compatibility_iterator it2 = m_eventWorkers.GetFirst(); it2 ; it2 = it2->GetNext())
    {
        if (it2->GetData() == pEvent.m_sender)
        {
            wxLogVerbose("Deleting event worker (%lu left)",
                         static_cast<unsigned long>( m_eventWorkers.GetCount() ));
            delete it2->GetData();
            m_eventWorkers.DeleteNode(it2);
            if (!pEvent.m_workerFailed)
                m_eventWorkersDone++;
            else
                m_eventWorkersFailed++;
            break;
        }
    }

    if (m_eventWorkers.GetCount() == 0 && m_threadWorkers.GetCount() == 0)
    {
        mTimer.Start(1000,true);
    }
}

void Server::OnTimerEvent(wxTimerEvent&)
{
  DumpStatistics();
}


wxBEGIN_EVENT_TABLE(Server,wxEvtHandler)
  EVT_SOCKET(wxID_ANY,Server::OnSocketEvent)
  EVT_WORKER(Server::OnWorkerEvent)
  EVT_TIMER(wxID_ANY,Server::OnTimerEvent)
wxEND_EVENT_TABLE()


ThreadWorker::ThreadWorker(wxSocketBase* pSocket) : wxThread(wxTHREAD_JOINABLE)
{
    m_socket = pSocket;
    //Notify() cannot be called in thread context. We have to detach from main loop
    //before switching thread contexts.
    m_socket->Notify(false);
    m_socket->SetFlags(wxSOCKET_WAITALL|wxSOCKET_BLOCK);
    pSocket->GetPeer(m_peer);
}

wxThread::ExitCode ThreadWorker::Entry()
{
    WorkerEvent e(this);
    if (!m_socket->IsConnected())
    {
        LogWorker("ThreadWorker: not connected",wxLOG_Error);
        return 0;
    }
    int to_process = -1;
    if (m_socket->IsConnected())
    {
        unsigned char signature[2];
        LogWorker("ThreadWorker: reading for data");
        to_process = 2;
        do
        {
            m_socket->Read(&signature,to_process);
            if (m_socket->Error())
            {
                LogWorker("ThreadWorker: Read error",wxLOG_Error);
                wxGetApp().AddPendingEvent(e);
                return 0;
            }
            to_process -= m_socket->LastCount();
            LogWorker(wxString::Format("to_process: %d",to_process));

        }
        while (!m_socket->Error() && to_process != 0);

        if (signature[0] == 0)
        {
            e.m_exit = true;
            return 0;
        }

        if (signature[0] == 0xCE)
        {
            LogWorker("This server does not support test2 from GUI client",wxLOG_Error);
            e.m_workerFailed = true;
            e.m_exit = true;
            return 0;
        }
        int size = signature[1] * (signature[0] == 0xBE ? 1 : 1024);
        char* buf = new char[size];
        LogWorker(wxString::Format("Message signature: chunks: %d, kilobytes: %d, size: %d (bytes)",signature[0],signature[1],size));

        to_process = size;
        LogWorker(wxString::Format("ThreadWorker: reading %d bytes of data",to_process));

        do
        {
            m_socket->Read(buf,to_process);
            if (m_socket->Error())
            {
                LogWorker("ThreadWorker: Read error",wxLOG_Error);
                wxGetApp().AddPendingEvent(e);
                return 0;
            }
            to_process -= m_socket->LastCount();
            LogWorker(wxString::Format("ThreadWorker: %d bytes readed, %d todo",m_socket->LastCount(),to_process));

        }
        while(!m_socket->Error() && to_process != 0);

        to_process = size;

        do
        {
            m_socket->Write(buf,to_process);
            if (m_socket->Error())
            {
                LogWorker("ThreadWorker: Write error",wxLOG_Error);
                break;
            }
           to_process -= m_socket->LastCount();
           LogWorker(wxString::Format("ThreadWorker: %d bytes written, %d todo",m_socket->LastCount(),to_process));
        }
        while(!m_socket->Error() && to_process != 0);
    }

    LogWorker("ThreadWorker: done");
    e.m_workerFailed = to_process != 0;
    m_socket->Destroy();
    wxGetApp().AddPendingEvent(e);
    return 0;
}

EventWorker::EventWorker(wxSocketBase* pSock)
  : m_socket(pSock),
    m_inbuf(nullptr),
    m_infill(0),
    m_outbuf(nullptr),
    m_outfill(0)
{
    m_socket->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG);
    m_socket->Notify(true);
    m_socket->SetEventHandler(*this);
    m_socket->SetFlags(wxSOCKET_NOWAIT);
    m_socket->GetPeer(m_peer);
}

EventWorker::~EventWorker()
{
    m_socket->Destroy();
    delete [] m_inbuf;
    delete [] m_outbuf;
}

void
EventWorker::DoRead()
{
    if (m_inbuf == nullptr)
    {
        //read message header
        do
        {
            m_socket->Read(m_signature + m_infill, 2 - m_infill);
            if (m_socket->Error())
            {
                if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
                {
                    LogWorker(wxString::Format("Read error (%d): %s",m_socket->LastError(),GetSocketErrorMsg(m_socket->LastError())),wxLOG_Error);
                    m_socket->Close();
                }
            }
            else
            {
                m_infill += m_socket->LastCount();
                if (m_infill == 2)
                {
                    unsigned char chunks = m_signature[1];
                    unsigned char type = m_signature[0];
                    if (type == 0xCE)
                    {
                        LogWorker("This server does not support test2 from GUI client",wxLOG_Error);
                        m_written = -1; //wxSOCKET_LOST will interpret this as failure
                        m_socket->Close();
                    }
                    else if (type == 0xBE || type == 0xDE)
                    {
                        m_size = chunks * (type == 0xBE ? 1 : 1024);
                        m_inbuf = new char[m_size];
                        m_outbuf = new char[m_size];
                        m_infill = 0;
                        m_outfill = 0;
                        m_written = 0;
                        LogWorker(wxString::Format("Message signature: len: %d, type: %s, size: %d (bytes)",chunks,type == 0xBE ? "b" : "kB",m_size));
                        break;
                    }
                    else
                    {
                        LogWorker(wxString::Format("Unknown test type %x",type));
                        m_socket->Close();
                    }
                }
            }
        }
        while(!m_socket->Error() && (2 - m_infill != 0));
    }

    if (m_inbuf == nullptr)
        return;
    //read message data
    do
    {
        if (m_size == m_infill)
        {
            m_signature[0] = m_signature[1] = 0x0;
            wxDELETEA(m_inbuf);
            m_infill = 0;
            return;
        }
        m_socket->Read(m_inbuf + m_infill,m_size - m_infill);
        if (m_socket->Error())
        {
            if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
            {
                LogWorker(wxString::Format("Read error (%d): %s",
                          m_socket->LastError(),
                          GetSocketErrorMsg(m_socket->LastError())),
                          wxLOG_Error);

                m_socket->Close();
            }
        }
        else
        {
            memcpy(m_outbuf+m_outfill,m_inbuf+m_infill,m_socket->LastCount());
            m_infill += m_socket->LastCount();
            m_outfill += m_socket->LastCount();
            DoWrite();
        }
    }
    while(!m_socket->Error());
};

void EventWorker::OnSocketEvent(wxSocketEvent& pEvent)
{
    switch(pEvent.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            DoRead();
            break;

        case wxSOCKET_OUTPUT:
            if ( m_outbuf )
                DoWrite();
            break;

        case wxSOCKET_CONNECTION:
            LogWorker("Unexpected wxSOCKET_CONNECTION in EventWorker", wxLOG_Error);
            break;

        case wxSOCKET_LOST:
            {
                LogWorker("Connection lost");
                WorkerEvent e(this);
                e.m_workerFailed = m_written != m_size;
                wxGetApp().AddPendingEvent(e);
            }
            break;
    }
}

void  EventWorker::DoWrite()
{
    do
    {
        if (m_written == m_size)
        {
            wxDELETEA(m_outbuf);
            m_outfill = 0;
            LogWorker( "All data written");
            return;
        }
        if (m_outfill - m_written == 0)
        {
            return;
        }
        m_socket->Write(m_outbuf + m_written,m_outfill - m_written);
        if (m_socket->Error())
        {
            if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
            {
                LogWorker(
                            wxString::Format("Write error (%d): %s",
                                            m_socket->LastError(),
                                            GetSocketErrorMsg(m_socket->LastError())
                                            )
                            ,wxLOG_Error
                            );
                m_socket->Close();
            }
            else
            {
                LogWorker("Write would block, waiting for OUTPUT event");
            }
        }
        else
        {
            memmove(m_outbuf,m_outbuf+m_socket->LastCount(),m_outfill-m_socket->LastCount());
            m_written += m_socket->LastCount();
        }
        LogWorker(wxString::Format("Written %d of %d bytes, todo %d",
                  m_written, m_size, m_size - m_written));
    }
    while (!m_socket->Error());
}

wxBEGIN_EVENT_TABLE(EventWorker,wxEvtHandler)
    EVT_SOCKET(wxID_ANY,EventWorker::OnSocketEvent)
wxEND_EVENT_TABLE()
