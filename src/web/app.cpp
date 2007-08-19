// Copyright (c) 2007 John Wilmes

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/log.h"
#include <string.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxAppBase::OnIdle)
END_EVENT_TABLE()

wxApp::wxApp() {
}

wxApp::~wxApp() {
}

void wxApp::CleanUp() {
    // We have responsibility to delete FIFOs on exit.
    // Always remove response FIFO first!
    if (0 != remove(m_responseFifoPath)) {
#if wxUSE_LOG
        wxLogSysError("Unable to remove FIFO during cleanup at '%s'", m_responseFifoPath);
#endif //wxUSE_LOG
    }
    if (0 != remove(m_requestFifoPath)) {
#if wxUSE_LOG
        wxLogSysError("Unable to remove FIFO during cleanup at '%s'", m_requestFifoPath);
#endif //wxUSE_LOG
    }
}

void wxApp::Exit() {
    exit(0);
}

bool wxApp::Initialize(int& argc, wxChar **argv) {
    if (argc < 7) {
        return false;
    }
    if (!wxAppBase::Initialize(argc, argv)) {
        return false;
    }
    m_sessionId = argv[1];
    m_remoteIp = argv[2];
    m_requestFifoPath = argv[3];
    m_responseFifoPath = argv[4];
    m_resourcePath = argv[5];
    m_resourceUrl = argv[5];
    return WriteTemplate();
}

void wxApp::WakeUpIdle() {
    int fd = open(m_requestFifoPath, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENXIO) {
            // Not currently waiting in dispatch, no need to wake up
            return;
        }
        // We should be able to open the FIFO, but we can't
        // No way to recover from this error
        // TODO: try notifying the client of a 500 error
#if wxUSE_LOG
        wxLogSysError("Unable to open request FIFO for wakeup event at '%s'", m_requestFifoPath);
#endif // wxUSE_LOG
        CleanUp();
        wxExit();
        return;
    }
    if (0 != flock(fd, LOCK_EX)) {
        // Unable to obtain the needed lock for the request fifo
        // No way to recover from this error
        // TODO: try notifying the client of a 500 error
#if wxUSE_LOG
        wxLogSysError("Unable to lock request FIFO for wakeup event at '%s'", m_requestFifoPath);
#endif // wxUSE_LOG
        CleanUp();
        wxExit();
        return;
    }
    //TODO: figure out the appropriate real wakeup message
    char * wakeupMsg = "Wakeup message";
    if (-1 == write(fd, wakeupMsg, strlen(wakeupMsg))) {
        int e = errno;
        close(fd);
        if (e == EAGAIN) {
            // We already woke up?
            return;
        }
        // Unable to obtain the needed lock for the request fifo
        // TODO: try notifying the client of a 500 error
#if wxUSE_LOG
        wxLogSysError("Unable to write wakeup event to request FIFO at '%s'", m_requestFifoPath);
#endif // wxUSE_LOG
        return;
    }
    if (-1 == close(fd)) {
#if wxUSE_LOG
        wxLogSysError("Unable to close request FIFO at '%s'", m_requestFifoPath);
#endif // wxUSE_LOG
    }
}

static bool wxIsInsideYield = false;

bool wxApp::Yield(bool onlyIfNeeded) {
    if (wxIsInsideYield) {
        if (!onlyIfNeeded) {
            wxFAIL_MSG( wxT("wxYield called recursively") );
        }
        return false;
    }

#if wxUSE_THREADS
    if (!wxThread::IsMain()) {
        // Can't read FIFO from two threads simultaneously, right?
        return true;
    }
#endif // wxUSE_THREADS
    
    wxIsInsideYield = true;

#if wxUSE_LOG
    // Log flushing is disabled in Yield, see declaration
    wxLog::Suspend();
#endif // wxUSE_LOG

    if (wxEventLoop::GetActive()) {
        while (wxEventLoop::GetActive()->Pending()) {
            wxEventLoop::GetActive()->Dispatch();
        }
    }

    while (ProcessIdle()) {}

#if wxUSE_LOG
    wxLog::Resume();
#endif //wxUSE_LOG

    wxIsInsideYield = false;
    return true;
}

bool wxApp::WriteTemplate() {
     FILE* fd = fopen(m_responseFifoPath, "w");
     if (NULL == fd) {
         //can't open response FIFO, even though we should block until someone
         //reads
 #if wxUSE_LOG
         wxLogSysError("Unable to open response FIFO to write application template '%s'",
                      m_responseFifoPath);
 #endif //wxUSE_LOG
         return false;
     }
     if (EOF == fputs(GetTemplate(), fd)) {
         //can't write to response FIFO, even though it was successfully opened
 #if wxUSE_LOG
         wxLogSysError("Unable to write to response FIFO to write application template at '%s'",
                      m_responseFifoPath);
 #endif //wxUSE_LOG
         return false;
     }
     if (EOF == fclose(fd)) {
         //can't close response FIFO
 #if wxUSE_LOG
         wxLogSysError("Unable to close response FIFO to write application template at '%s'",
                      m_responseFifoPath);
 #endif //wxUSE_LOG
     }
     return true;
}

wxString wxApp::GetTemplate() {
    //TODO
    wxString tpl;
    tpl.Append("<html><head><title>");
    return tpl;
}

const wxString& wxApp::GetResourcePath() const {
    return m_resourcePath;
}

const wxString& wxApp::GetResourceUrl() const {
    return m_resourceUrl;
}
