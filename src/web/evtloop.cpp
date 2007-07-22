///////////////////////////////////////////////////////////////////////////////
// Name:        src/web/evtloop.cpp
// Purpose:     implements wxEventLoop for wxWeb
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.07.01
// RCS-ID:      $Id: evtloop.cpp 46029 2007-05-14 23:43:39Z VZ $
// Copyright:   (c) 2001 Vadim Zeitlin, John Wilmes
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/evtloop.h"
#include "wx/ptr_scpd.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP


#include <json/json.h>

#include <fcntl.h>
#include <errno.h>

//TODO redefine, put somewhere else
#define BUFFER_SIZE 1024

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

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

    bool DispatchJson(wxString& events);
    wxEvent* JsonToWxEvent(json_object* jevt);
    void JsonToWxKeyEvent(json_object* jevt, wxKeyEvent& wxevt);
    wxWindow* GetEventWindow(json_object* jevt);

private:
    int m_exitcode;
    bool m_keepLooping;
};


bool wxEventLoopImpl::DispatchJson(wxString& events) {
    json_object *root = json_tokener_parse(events.char_str());
    wxEvent* wxevt = NULL;
    wxWindow* win = NULL;

    if (is_error(root)) {
        //TODO error - failed to parse JSON
        return false;
    }

    if (json_type_array != json_object_get_type(root)) {
        //TODO error - JSON in wrong format
        return false;
    }

    int len = json_object_array_length(root);
    for (int i = 0; i < len; ++i) {
        json_object *jevt = json_object_array_get_idx(root, i);
        if (jevt == NULL ) {
            //TODO error - array index skipped
            continue;
        }
        wxevt = JsonToWxEvent(jevt);
        if (wxevt == NULL) {
            //TODO error - failed to parse event
            continue;
        }
        win = GetEventWindow(jevt);
        if (win == NULL) {
            //TODO error - event does not contain window data
            continue;
        }
        win->GetEventHandler()->ProcessEvent(*wxevt);
        delete wxevt;
    }
    return true;
}


wxEvent* wxEventLoopImpl::JsonToWxEvent(json_object* jevt) {
    wxEvent* wxevt = NULL;
    json_object* type = json_object_object_get(jevt, "eventType");
    int evtType = json_object_get_int(type);
    if (evtType == wxEVT_KEY_DOWN || //corresponding to Javascript onKeyDown
        evtType == wxEVT_KEY_UP || //corresponding to Javascript onKeyUp
        evtType == wxEVT_CHAR) //corresponding to Javascript onKeyPress
    {
        wxevt = new wxKeyEvent(evtType);
        JsonToWxKeyEvent(jevt, *(wxKeyEvent*)wxevt);
    }
    return wxevt;
}

void wxEventLoopImpl::JsonToWxKeyEvent(json_object* jevt, wxKeyEvent& wxevt) {
    wxevt.m_shiftDown = json_object_get_boolean(
                            json_object_object_get(jevt, "shiftDown"));
    wxevt.m_controlDown = json_object_get_boolean(
                            json_object_object_get(jevt, "controlDown"));
    wxevt.m_metaDown = json_object_get_boolean(
                            json_object_object_get(jevt, "metaDown"));
    wxevt.m_altDown = json_object_get_boolean(
                            json_object_object_get(jevt, "altDown"));
    wxevt.m_keyCode = json_object_get_int(
                            json_object_object_get(jevt, "keyCode"));
    wxevt.m_x = json_object_get_int(
                            json_object_object_get(jevt, "x"));
    wxevt.m_y = json_object_get_int(
                            json_object_object_get(jevt, "y"));
}

wxWindow* wxEventLoopImpl::GetEventWindow(json_object* jevt) {
    wxWindow* win = NULL;
    return win;
}

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxDEFINE_TIED_SCOPED_PTR_TYPE(wxEventLoopImpl)

wxGUIEventLoop::~wxGUIEventLoop()
{
    wxASSERT_MSG(!m_impl, _T("should have been deleted in Run()"));
}

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
    if (NULL == fd) {
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
    m_impl->DispatchJson(req);
    return m_impl->GetKeepLooping();
}
