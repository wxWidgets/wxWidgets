// Copyright (c) 2007 John Wilmes

#include "wx/app.h"
#include "wx/window.h"
#include "wx/evtloop.h"
#include "wx/log.h"
#include "wx/arrimpl.cpp"
#include <string.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

WX_DEFINE_OBJARRAY(wxStringArray)
WX_DEFINE_OBJARRAY(wxWindowArray)

const char* wxApp::DEFAULT_CANVAS_ID = "MainCanvas";

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
    //TODO delete resource files
}

void wxApp::Exit() {
    exit(0);
}

bool wxApp::Initialize(int& argc, wxChar **argv) {
    if (argc < 2) {
        return false;
    }
    if (!wxAppBase::Initialize(argc, argv)) {
        return false;
    }
    if (!InitFromFifo(argv[1])) {
        return false;
    }
    return WriteTemplate();
}

bool wxApp::InitFromFifo(const wxString& path) {
    FILE* fd = fopen(path, "r");
    if (NULL == fd) {
        // can't open request fifo, time to panic
#if wxUSE_LOG
        wxLogSysError("Unable to open FIFO for initialization at '%s'", path);
#endif // wxUSE_LOG
        wxTheApp->CleanUp();
        wxExit();
    }

    wxString init;
    //TODO get rid of magic numbers
    char buf[1024 + 1];
    do {
        // block if no data is available yet
        int end = fread(buf, sizeof(char), 1024, fd);
        buf[end] = '\0';
        init.Append(buf);
    } while (0 == feof(fd));
    if (EOF == fclose(fd)) {
#if wxUSE_LOG
        wxLogSysError("Unable to close FIFO for initialization at '%s'", path);
#endif // wxUSE_LOG
    }

    wxStringArray lines;
    while (!init.IsEmpty()) {
        lines.Add(init.BeforeFirst('\n'));
        init = init.AfterFirst('\n');
    }

    if (lines.GetCount() < 6) {
        return false;
    }
    m_sessionId = lines[0];
    m_remoteIp = lines[1];
    m_requestFifoPath = lines[2];
    m_responseFifoPath = lines[3];
    m_resourcePath = lines[4];
    m_resourceUrl = lines[5];
    return true;
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
    //TODO: get rid of magic string
    char * wakeupMsg = "123";
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

void wxApp::FlushBuffers() {
    wxString buffer;
    while (!m_flushTargets.IsEmpty()) {
        wxWindowWeb** next = m_flushTargets.Detach(m_flushTargets.GetCount()-1);
        buffer += (*next)->FlushClientEvalBuffer();
    }
    // This method should be called exactly once per request, so we can assume
    // that someone is listening on the other end of the FIFO, and not bother
    // with open/O_NONBLOCK
    FILE* fd = fopen(m_responseFifoPath, "w");
    if (NULL == fd) {
        //can't open response FIFO, even though we should block until someone
        //reads
#if wxUSE_LOG
        wxLogSysError("Unable to open response FIFO to flush eval buffer at '%s'",
                      m_responseFifoPath);
#endif //wxUSE_LOG
        return;
    }
    if (EOF == fputs(buffer.fn_str(), fd)) {
        //can't write to response FIFO, even though it was successfully opened
#if wxUSE_LOG
        wxLogSysError("Unable to write to response FIFO to flush eval buffer at '%s'",
                      m_responseFifoPath);
#endif //wxUSE_LOG
        return;
    }
    if (EOF == fclose(fd)) {
        //can't close response FIFO
#if wxUSE_LOG
        wxLogSysError("Unable to close response FIFO to flush eval buffer at '%s'",
                      m_responseFifoPath);
#endif //wxUSE_LOG
    }

}

void wxApp::RequestFlush(wxWindowWeb* win) {
    m_flushTargets.Add(win);
}

wxString wxApp::GetTemplate() const {
    wxString tpl;
    //TODO - do something with the title
    tpl.Printf("<html><head><title>%s</title>"
               "<script type=\"text/javascript\" src=\"%s\"></script><script type=\"text/javascript\"><!--"
                "   App.initialize(\"%s\", \"%s\");"
                "--></script><noscript>%s</noscript></head><body><div id=\"%s\"></div></body></html>",
              GetTitle(), "scriptUrl", "appUrl", DEFAULT_CANVAS_ID, GetNoScript(), DEFAULT_CANVAS_ID);
    return tpl;
}

wxString wxApp::GetNoScript() const {
    return "<b>This page requires Javascript in order to function</b>"
           "<p>Please enable Javascript and reload this page in order to continue</p>";
}

const wxString& wxApp::GetResourcePath() const {
    return m_resourcePath;
}

const wxString& wxApp::GetResourceUrl() const {
    return m_resourceUrl;
}

wxString wxApp::GetResourceFile() {
    wxString file;
    file.Printf("%d", wxNewId());
    m_resourceFiles.Add(file);
    return file;
}
