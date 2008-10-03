/////////////////////////////////////////////////////////////////////////////
// Name:        samples/sockbase/client.cpp
// Purpose:     Sockets sample for wxBase
// Author:      Lukasz Michalski
// Modified by:
// Created:     27.06.2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Lukasz Michalski <lmichalski@user.sourceforge.net>
// Licence:     wxWindows license
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

wxChar* GetSocketErrorMsg(int pSockError)
{
    switch(pSockError)
    {
        case wxSOCKET_NOERROR:
            return wxT("wxSOCKET_NOERROR");
        break;
        case wxSOCKET_INVOP:
            return wxT("wxSOCKET_INVOP");
        break;
        case wxSOCKET_IOERR:
            return wxT("wxSOCKET_IOERR");
        break;
        case wxSOCKET_INVADDR:
            return wxT("wxSOCKET_INVADDR");
        break;
        case wxSOCKET_NOHOST:
            return wxT("wxSOCKET_NOHOST");
        break;
        case wxSOCKET_INVPORT:
            return wxT("wxSOCKET_INVPORT");
        break;
        case wxSOCKET_WOULDBLOCK:
            return wxT("wxSOCKET_WOULDBLOCK");
        break;
        case wxSOCKET_TIMEDOUT:
            return wxT("wxSOCKET_TIMEDOUT");
        break;
        case wxSOCKET_MEMERR:
            return wxT("wxSOCKET_MEMERR");
        break;
        default:
            return wxT("Unknown");
        break;
    }
}

//log output types for LogWorker helper function
typedef enum
{
    LOG_MESSAGE,
    LOG_ERROR,
    LOG_VERBOSE
} logWorker_t;

//outputs log message with IP and TCP port number prepended
void
LogWorker(const wxIPV4address& pAddr, const wxString& pMessage, logWorker_t pType = LOG_VERBOSE)
{
    wxString msg(wxString::Format(wxT("%s:%d "),pAddr.IPAddress().c_str(),pAddr.Service()));
    msg += pMessage;
    switch (pType)
    {
        case LOG_VERBOSE:
            wxLogVerbose(msg);
        break;
        case LOG_MESSAGE:
            wxLogMessage(msg);
        break;
        case LOG_ERROR:
            wxLogError(msg);
        break;
    }
}

//event sent by workers to server class
//after client is served
const wxEventType wxEVT_WORKER = wxNewEventType();
#define EVT_WORKER(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_WORKER, -1, -1, (wxObjectEventFunction) (wxEventFunction) (WorkerEventFunction) & func, (wxObject *) NULL ),

class WorkerEvent : public wxEvent {
public:
    WorkerEvent(void* pSender)
    {
        SetId(-1);
        SetEventType(wxEVT_WORKER);
        m_sender = pSender;
        m_exit = false;
        m_workerFailed = false;
    }

    virtual wxEvent* Clone() const
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
    DECLARE_EVENT_TABLE();
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

    virtual bool OnInit();
    virtual int OnExit();

    void OnInitCmdLine(wxCmdLineParser& pParser);
    bool OnCmdLineParsed(wxCmdLineParser& pParser);

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

    long m_port;

    wxTimer mTimer;
};

DECLARE_APP(Server);

//thread based worker reads signature and all data first from connected client
//and resends data to client after reading
class ThreadWorker : public wxThread
{
public:
    ThreadWorker(wxSocketBase* pSocket);
    virtual ExitCode Entry();
private:
    wxSocketBase* m_socket;
    wxIPV4address m_peer;
};

//event based worker reads signature and creates buffer for incoming data.
//When part of data arrives this worker resends it as soon as possible.
class EventWorker : public wxEvtHandler
{
    DECLARE_EVENT_TABLE();
public:
    EventWorker(wxSocketBase* pSock);
    ~EventWorker();
private:
    wxSocketBase* m_socket;
    wxIPV4address m_peer;

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
};

/******************* Implementation ******************/
IMPLEMENT_APP_CONSOLE(Server)

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TList);
WX_DEFINE_LIST(EList);


void
Server::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxApp::OnInitCmdLine(pParser);
    pParser.AddSwitch(wxT("t"),wxT("threads"),_("Use thread based workers only"));
    pParser.AddSwitch(wxT("e"),wxT("events"),_("Use event based workers only"));
    pParser.AddOption(wxT("m"),wxT("max"),_("Exit after <n> connections"),wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL);
    pParser.AddOption(wxT("p"),wxT("port"),_("listen on given port (default 3000)"),wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL);
}

void
Server::DumpStatistics()
{
    wxString mode;
    switch(m_workMode)
    {
        case EVENTS:
            mode = _("Event based workers");
            break;
        case THREADS:
            mode = _("Thread based workers");
            break;
        case MIXED:
            mode = _("Event and thread based workers");
            break;
    }
    wxLogMessage(wxString::Format(wxT("Server mode: %s"),mode.c_str()));
    wxLogMessage(wxString::Format(wxT("\t\t\t\tThreads\tEvents\tTotal")));
    wxLogMessage(wxString::Format(wxT("Workers created:\t\t%d\t%d\t%d"),m_threadWorkersCreated,m_eventWorkersCreated,m_threadWorkersCreated+m_eventWorkersCreated));
    wxLogMessage(wxString::Format(wxT("Max concurrent workers:\t%d\t%d\t%d"),m_maxThreadWorkers,m_maxEventWorkers,m_maxThreadWorkers+m_maxEventWorkers));
    wxLogMessage(wxString::Format(wxT("Workers failed:\t\t%d\t%d\t%d"),m_threadWorkersFailed,m_eventWorkersFailed,m_threadWorkersFailed+m_eventWorkersFailed));
    wxLogMessage(wxString::Format(wxT("Workers done:\t\t%d\t%d\t%d"),m_threadWorkersDone,m_eventWorkersDone,m_threadWorkersDone+m_eventWorkersDone));

    if ((int)(m_threadWorkersDone+m_eventWorkersDone) == m_maxConnections)
    {
        wxLogMessage(wxT("%d connection(s) served, exiting"),m_maxConnections);
        ExitMainLoop();
    }
}


bool
Server::OnCmdLineParsed(wxCmdLineParser& pParser)
{
    if (pParser.Found(_("verbose")))
    {
        wxLog::AddTraceMask(wxT("wxSocket"));
        wxLog::AddTraceMask(wxT("epolldispatcher"));
        wxLog::AddTraceMask(wxT("selectdispatcher"));
        wxLog::AddTraceMask(wxT("thread"));
        wxLog::AddTraceMask(wxT("events"));
        wxLog::AddTraceMask(wxT("timer"));
    }

    if (pParser.Found(wxT("m"),&m_maxConnections))
    {
        wxLogMessage(wxT("%d connection(s) to exit"),m_maxConnections);
    }

    if (pParser.Found(wxT("p"),&m_port))
    {
        wxLogMessage(wxT("%d connection(s) to exit"),m_maxConnections);
    }

    if (pParser.Found(wxT("t")))
        m_workMode = THREADS;
    else if (pParser.Found(wxT("e")))
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
    if (!m_listeningSocket->Ok())
    {
        wxLogError(wxT("Cannot bind listening socket"));
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

    wxLogMessage(wxT("Server listening at port %d, waiting for connections"), m_port);
    return true;
}


int Server::OnExit()
{
    for(TList::compatibility_iterator it = m_threadWorkers.GetFirst(); it ; it = it->GetNext()) {
        it->GetData()->Wait();
        delete it->GetData();
    }

    for(EList::compatibility_iterator it2 = m_eventWorkers.GetFirst(); it2 ; it2->GetNext()) {
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
            wxLogError(wxT("Unexpected wxSOCKET_INPUT in wxSocketServer"));
        break;
        case wxSOCKET_OUTPUT:
            wxLogError(wxT("Unexpected wxSOCKET_OUTPUT in wxSocketServer"));
        break;
        case wxSOCKET_CONNECTION:
        {
            wxSocketBase* sock = m_listeningSocket->Accept();
            wxIPV4address addr;
            if (!sock->GetPeer(addr))
            {
                wxLogError(wxT("Server: cannot get peer info"));
            } else {
                wxLogMessage(wxT("Got connection from %s:%d"),addr.IPAddress().c_str(), addr.Service());
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
                    wxLogError(wxT("Server: cannot create next thread (current threads: %d"), m_threadWorkers.size());
                };
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
            wxLogError(wxT("Unexpected wxSOCKET_LOST in wxSocketServer"));
        break;
    }
}

void  Server::OnWorkerEvent(WorkerEvent& pEvent)
{
    //wxLogMessage(wxT("Got worker event"));
    for(TList::compatibility_iterator it = m_threadWorkers.GetFirst(); it ; it = it->GetNext())
    {
        if (it->GetData() == pEvent.m_sender)
        {
            wxLogVerbose(wxT("Deleting thread worker (%d left)"),m_threadWorkers.GetCount());
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
            wxLogVerbose(wxT("Deleting event worker (%d left)"),m_eventWorkers.GetCount());
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


BEGIN_EVENT_TABLE(Server,wxEvtHandler)
  EVT_SOCKET(wxID_ANY,Server::OnSocketEvent)
  EVT_WORKER(Server::OnWorkerEvent)
  EVT_TIMER(wxID_ANY,Server::OnTimerEvent)
END_EVENT_TABLE()


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
        LogWorker(m_peer,wxT("ThreadWorker: not connected"),LOG_ERROR);
        return 0;
    }
    int to_process = -1;
    if (m_socket->IsConnected())
    {
        unsigned char signature[2];
        LogWorker(m_peer,wxT("ThreadWorker: reading for data"));
        to_process = 2;
        do
        {
            m_socket->Read(&signature,to_process);
            if (m_socket->Error())
            {
                LogWorker(m_peer,wxT("ThreadWorker: Read error"),LOG_ERROR);
                wxGetApp().AddPendingEvent(e);
                return 0;
            }
            to_process -= m_socket->LastCount();
            LogWorker(m_peer,wxString::Format(wxT("to_process: %d"),to_process));

        }
        while (!m_socket->Error() && to_process != 0);

        if (signature[0] == 0)
        {
            e.m_exit = true;
            return 0;
        }

        if (signature[0] == 0xCE)
        {
            LogWorker(m_peer,_("This server does not support test2 from GUI client"),LOG_ERROR);
            e.m_workerFailed = true;
            e.m_exit = true;
            return 0;
        }
        int size = signature[1] * (signature[0] == 0xBE ? 1 : 1024);
        char* buf = new char[size];
        LogWorker(m_peer,wxString::Format(wxT("Message signature: chunks: %d, kilobytes: %d, size: %d (bytes)"),signature[0],signature[1],size));

        to_process = size;
        LogWorker(m_peer,wxString::Format(wxT("ThreadWorker: reading %d bytes of data"),to_process));

        do
        {
            m_socket->Read(buf,to_process);
            if (m_socket->Error())
            {
                LogWorker(m_peer,wxT("ThreadWorker: Read error"),LOG_ERROR);
                wxGetApp().AddPendingEvent(e);
                return 0;
            }
            to_process -= m_socket->LastCount();
            LogWorker(m_peer,wxString::Format(wxT("ThreadWorker: %d bytes readed, %d todo"),m_socket->LastCount(),to_process));

        }
        while(!m_socket->Error() && to_process != 0);

        to_process = size;

        do
        {
            m_socket->Write(buf,to_process);
            if (m_socket->Error()) {
                LogWorker(m_peer,wxT("ThreadWorker: Write error"),LOG_ERROR);
                break;
           }
           to_process -= m_socket->LastCount();
           LogWorker(m_peer,wxString::Format(wxT("ThreadWorker: %d bytes written, %d todo"),m_socket->LastCount(),to_process));
        }
        while(!m_socket->Error() && to_process != 0);
    }

    LogWorker(m_peer,wxT("ThreadWorker: done"));
    e.m_workerFailed = to_process != 0;
    m_socket->Destroy();
    wxGetApp().AddPendingEvent(e);
    return 0;
}

EventWorker::EventWorker(wxSocketBase* pSock)
  : m_socket(pSock),
    m_inbuf(NULL),
    m_infill(0),
    m_outbuf(NULL),
    m_outfill(0)
{
    m_socket->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG);
    m_socket->Notify(true);
    m_socket->SetEventHandler(*this);
    m_socket->SetFlags(wxSOCKET_NOWAIT);
    m_socket->GetPeer(m_peer);
}

EventWorker::~EventWorker() {
    m_socket->Destroy();
    delete [] m_inbuf;
    delete [] m_outbuf;
}

void
EventWorker::DoRead() 
{
    if (m_inbuf == NULL)
    {
        //read message header
        do
        {
            m_socket->Read(m_signature,2 - m_infill);
            if (m_socket->Error()) {
                if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
                {
                    LogWorker(m_peer,wxString::Format(wxT("Read error (%d): %s"),m_socket->LastError(),GetSocketErrorMsg(m_socket->LastError())),LOG_ERROR);
                    m_socket->Close();
                }
            }
            else
            {
                m_infill += m_socket->LastCount();
                if (m_infill == 2) {
                    unsigned char chunks = m_signature[1];
                    unsigned char type = m_signature[0];
                    if (type == 0xCE)
                    {
                        LogWorker(m_peer,_("This server does not support test2 from GUI client"),LOG_ERROR);
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
                        LogWorker(m_peer,wxString::Format(wxT("Message signature: len: %d, type: %s, size: %d (bytes)"),chunks,type == 0xBE ? wxT("b") : wxT("kB"),m_size));
                        break;
                    } else 
                    {
                        LogWorker(m_peer,wxString::Format(wxT("Unknown test type %x"),type));
                        m_socket->Close();
                    }
                }
            }
        }
        while(!m_socket->Error() && (2 - m_infill != 0));
    }

    if (m_inbuf == NULL)
        return;
    //read message data
    do {
        if (m_size == m_infill) {
            m_signature[0] = m_signature[1] = 0x0;
            delete [] m_inbuf;
            m_inbuf = NULL;
            m_infill = 0;
            return;
        }
        m_socket->Read(m_inbuf + m_infill,m_size - m_infill);
        if (m_socket->Error()) {
            if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
            {
                LogWorker(
                        m_peer,
                        wxString::Format(wxT("Read error (%d): %s"),
                                        m_socket->LastError(),
                                        GetSocketErrorMsg(m_socket->LastError())
                                    ),
                        LOG_ERROR);

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
            if (m_inbuf != NULL)
                DoWrite();
        break;
        case wxSOCKET_CONNECTION:
            LogWorker(m_peer,wxString::Format(wxT("Unexpected wxSOCKET_CONNECTION in EventWorker")),LOG_ERROR);
        break;
        case wxSOCKET_LOST:
        {
            LogWorker(m_peer,wxString::Format(wxT("Connection lost")));
            WorkerEvent e(this);
            e.m_workerFailed = m_written != m_size;
            wxGetApp().AddPendingEvent(e);
        }
        break;
    }
}

void  EventWorker::DoWrite() {
    do {
        if (m_written == m_size)
        {
                delete [] m_outbuf;
                m_outbuf = NULL;
                m_outfill = 0;
                LogWorker(m_peer,wxString::Format(wxT("All data written")));
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
                LogWorker(m_peer,
                            wxString::Format(wxT("Write error (%d): %s"),
                                            m_socket->LastError(),
                                            GetSocketErrorMsg(m_socket->LastError())
                                            )
                            ,LOG_ERROR
                            );
                m_socket->Close();
            }
            else
            {
                LogWorker(m_peer,wxString::Format(wxT("Write would block, waiting for OUTPUT event")));
            }
        }
        else
        {
            memmove(m_outbuf,m_outbuf+m_socket->LastCount(),m_outfill-m_socket->LastCount());
            m_written += m_socket->LastCount();
        }
        LogWorker(m_peer,wxString::Format(wxT("Written %d of %d bytes, todo %d"),m_socket->LastCount(),m_size,m_size - m_written));
    }
    while (!m_socket->Error());
}

BEGIN_EVENT_TABLE(EventWorker,wxEvtHandler)
  EVT_SOCKET(wxID_ANY,EventWorker::OnSocketEvent)
END_EVENT_TABLE()
