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

#include "wx/web/private/utils.h"

#include <json/json.h>

#include <fcntl.h>
#include <errno.h>

//TODO redefine, put somewhere else
#define BUFFER_SIZE 1024

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

enum WebEventClass {
    WEB_EVENT_CLASS_KEY,
    WEB_EVENT_CLASS_MOUSE,
    WEB_EVENT_CLASS_OTHER
};

WebEventClass GetEventClass(int evtType) {
    if (evtType == wxEVT_KEY_DOWN || //corresponding to Javascript onKeyDown
        evtType == wxEVT_KEY_UP || //corresponding to Javascript onKeyUp
        evtType == wxEVT_CHAR) //corresponding to Javascript onKeyPress
    {
        return WEB_EVENT_CLASS_KEY;
    } else if (evtType == wxEVT_LEFT_DOWN ||
               evtType == wxEVT_LEFT_UP ||
               evtType == wxEVT_MIDDLE_DOWN ||
               evtType == wxEVT_MIDDLE_UP ||
               evtType == wxEVT_RIGHT_UP ||
               evtType == wxEVT_RIGHT_DOWN ||
               evtType == wxEVT_LEFT_DCLICK ||
               evtType == wxEVT_MIDDLE_DCLICK ||
               evtType == wxEVT_RIGHT_DCLICK ||
               evtType == wxEVT_MOTION) {
        return WEB_EVENT_CLASS_MOUSE;
    }
    return WEB_EVENT_CLASS_OTHER;
}
    
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

    bool DispatchEvents(wxString& events);
    void DispatchJsonEvent(json_object* jevt);
    wxKeyEvent* JsonToWxKeyEvent(json_object* jevt, int type);
    wxMouseEvent* JsonToWxMouseEvent(json_object* jevt, int type);
    wxWindow* GetEventWindow(json_object* jevt);

private:
    int m_exitcode;
    bool m_keepLooping;
};


bool wxEventLoopImpl::DispatchEvents(wxString& events) {
    json_object *root = json_tokener_parse(events.char_str());

    if (is_error(root)) {
        //TODO error - failed to parse JSON
        return false;
    }

    if (json_type_array != json_object_get_type(root)) {
        //JSON in wrong format - probably a wakeup message
        return true;
    }

    int len = json_object_array_length(root);
    for (int i = 0; i < len; ++i) {
        json_object *jevt = json_object_array_get_idx(root, i);
        if (jevt == NULL || is_error(jevt)) {
            //TODO error - array index skipped
            continue;
        }
        DispatchJsonEvent(jevt);
    }
    wxTheApp->FlushBuffers();
    return true;
}


void wxEventLoopImpl::DispatchJsonEvent(json_object* jevt) {
    wxEvent* wxevt = NULL;
    wxWindow* win = NULL;
    json_object* type = json_object_object_get(jevt, "eventType");
    int evtType = json_object_get_int(type);
    switch (GetEventClass(evtType)) {
        case WEB_EVENT_CLASS_KEY:
            wxevt = JsonToWxKeyEvent(jevt, evtType);
            WebStateManager::UpdateKeyState(*(wxKeyEvent*)wxevt);
            break;
        case WEB_EVENT_CLASS_MOUSE:
            wxevt = JsonToWxMouseEvent(jevt, evtType);
            WebStateManager::UpdateMouseState(*(wxMouseEvent*)wxevt);
            break;
        default:
            break;
    }
    if (wxevt == NULL) {
        //TODO error - failed to parse event
        return;
    }
    win = GetEventWindow(jevt);
    if (win == NULL) {
        //TODO error - event does not contain window data
        return;
    }
    win->GetEventHandler()->ProcessEvent(*wxevt);
    delete wxevt;
 }

wxKeyEvent* wxEventLoopImpl::JsonToWxKeyEvent(json_object* jevt, int type) {
    wxKeyEvent* wxevt = new wxKeyEvent(type);
    //Set attributes based on state
    json_object* state = json_object_object_get(jevt, "eventState");
    wxevt->m_altDown = json_object_get_boolean(
                            json_object_object_get(state, "altDown"));
    wxevt->m_controlDown = json_object_get_boolean(
                            json_object_object_get(state, "controlDown"));
    wxevt->m_metaDown = json_object_get_boolean(
                            json_object_object_get(state, "metaDown"));
    wxevt->m_shiftDown = json_object_get_boolean(
                            json_object_object_get(state, "shiftDown"));
    wxevt->m_x = json_object_get_int(
                            json_object_object_get(state, "x"));
    wxevt->m_y = json_object_get_int(
                            json_object_object_get(state, "y"));

    //Set keycode
    wxevt->m_keyCode = json_object_get_int(
                            json_object_object_get(jevt, "keyCode"));
}

wxMouseEvent* wxEventLoopImpl::JsonToWxMouseEvent(json_object* jevt, int type) {
    wxMouseEvent *wxevt = new wxMouseEvent(type);
    //State
    json_object* state = json_object_object_get(jevt, "eventState");
    wxevt->m_altDown = json_object_get_boolean(
                            json_object_object_get(state, "altDown"));
    wxevt->m_controlDown = json_object_get_boolean(
                            json_object_object_get(state, "controlDown"));
    wxevt->m_metaDown = json_object_get_boolean(
                            json_object_object_get(state, "metaDown"));
    wxevt->m_shiftDown = json_object_get_boolean(
                            json_object_object_get(state, "shiftDown"));
    wxevt->m_leftDown = json_object_get_boolean(
                            json_object_object_get(state, "leftDown"));
    wxevt->m_middleDown = json_object_get_boolean(
                            json_object_object_get(state, "middleDown"));
    wxevt->m_rightDown = json_object_get_boolean(
                            json_object_object_get(state, "rightDown"));
    wxevt->m_x = json_object_get_int(
                            json_object_object_get(state, "x"));
    wxevt->m_y = json_object_get_int(
                            json_object_object_get(state, "y"));
    return wxevt;
}

wxWindow* wxEventLoopImpl::GetEventWindow(json_object* jevt) {
    json_object* state = json_object_object_get(jevt, "eventState");
    wxPoint pt;
    pt.x = json_object_get_int(
                            json_object_object_get(state, "x"));
    pt.y = json_object_get_int(
                            json_object_object_get(state, "y"));
    return wxFindWindowAtPoint(pt);
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
    m_impl->DispatchEvents(req);
    return m_impl->GetKeepLooping();
}
