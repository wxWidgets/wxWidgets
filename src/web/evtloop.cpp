// Copyright (c) 2007 John Wilmes

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/log.h"
#include "wx/ptr_scpd.h"

#include <fcntl.h>
#include <errno.h>

//TODO redefine, put somewhere else
#define BUFFER_SIZE 1024

class WXDLLEXPORT wxEventLoopImpl {
public:
    wxEventLoopImpl() {
        SetExitCode(0);
        SetKeepLooping(true);
    }

    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

    void SetKeepLooping(bool keepLooping) { m_keepLooping = keepLooping; }
    int GetKeepLooping() const { return m_keepLooping; }

private:
    int m_exitcode;
    bool m_keepLooping;
};
        
wxDEFINE_TIED_SCOPED_PTR_TYPE(wxEventLoopImpl)

int wxGUIEventLoop::Run() {
    // event loops are not recursive, you need to create another loop
    wxCHECK_MSG(!IsRunning(), -1, _T("can't reenter a message loop"));

    wxEventLoopActivator activate(this);
    wxEventLoopImplTiedPtr impl(&m_impl, new wxEventLoopImpl);

    for (;;) {
        // generate and process idle events for as long as we don't have
        // anything else to do
        while (!Pending() && wxTheApp->ProcessIdle()) {}

        // a message came or no more idle processing to do, sit in Dispatch()
        // waiting for the next message
        if (!Dispatch()) {
            // app terminated
            break;
        }
    }

    return m_impl->GetExitCode();
}

void wxGUIEventLoop::Exit(int rc) {
    wxCHECK_RET(!IsRunning(), _T("can't call Exit() if not running"));
    m_impl->SetExitCode(rc);
    m_impl->SetKeepLooping(false);
    wxTheApp->WakeUpIdle();
}

bool wxGUIEventLoop::Pending() const {
    int fd = open(wxTheApp->m_requestFifoPath, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        // can't open request fifo, time to panic
#if wxUSE_LOG
        wxLogSysError("Unable to open request FIFO for Pending() check at '%s'", wxTheApp->m_requestFifoPath);
#endif // wxUSE_LOG
        wxTheApp->CleanUp();
        wxExit();
    }
    char c;
    int r = read(fd, &c, 1);
    if (-1 == close(fd)) {
#if wxUSE_LOG
        wxLogSysError("Unable to close request FIFO for Pending() check at '%s'", wxTheApp->m_requestFifoPath);
#endif // wxUSE_LOG
    }
    wxASSERT_MSG(r <= 0, "Non-blocking read from FIFO was unexpectedly successful");
    if (r < 0) {
        if (errno == EAGAIN) {
            return true;
        }
        // Reading failed, but not because of O_NONBLOCK, time to panic
#if wxUSE_LOG
        wxLogSysError("Unable to read request FIFO for Pending() check at '%s'", wxTheApp->m_requestFifoPath);
#endif // wxUSE_LOG
        wxTheApp->CleanUp();
        wxExit();
    }
    return false;
}

bool wxGUIEventLoop::Dispatch() {
    wxCHECK_MSG( !IsRunning(), false, _T("can't call Dispatch() if not running") );
    FILE* fd = fopen(wxTheApp->m_requestFifoPath, "r");
    if (fd < 0) {
        // can't open request fifo, time to panic
#if wxUSE_LOG
        wxLogSysError("Unable to open request FIFO for dispatch '%s'", wxTheApp->m_requestFifoPath);
#endif // wxUSE_LOG
        wxTheApp->CleanUp();
        wxExit();
    }

    wxString req;
    char buf[BUFFER_SIZE + 1];
    do {
        // block if no data is available
        int end = fread(buf, sizeof(char), BUFFER_SIZE, fd);
        buf[end] = '\0';
        req.Append(buf);
    } while (0 == feof(fd));
    if (EOF == fclose(fd)) {
#if wxUSE_LOG
        wxLogSysError("Unable to close request FIFO for dispatch '%s'", wxTheApp->m_requestFifoPath);
#endif // wxUSE_LOG
    }
    return m_impl->GetKeepLooping();
}
