#ifndef _WX_TASKBAR_H_BASE_
#define _WX_TASKBAR_H_BASE_

#include "wx/defs.h"

#ifdef wxHAS_TASK_BAR_ICON

#include "wx/event.h"

// ----------------------------------------------------------------------------
// wxTaskBarIconBase: define wxTaskBarIcon interface
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTaskBarIconBase : public wxEvtHandler
{
public:
    wxTaskBarIconBase() { }

private:
    DECLARE_NO_COPY_CLASS(wxTaskBarIconBase)
};


// ----------------------------------------------------------------------------
// now include the actual class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/taskbar.h"
#elif defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #include "wx/unix/taskbarx11.h"
#endif

// ----------------------------------------------------------------------------
// wxTaskBarIcon events
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon)
        : wxEvent(-1, evtType)
    {
        SetEventObject(tbIcon);
    }

    virtual wxEvent *Clone() const { return new wxTaskBarIconEvent(*this); }

private:
    DECLARE_NO_COPY_CLASS(wxTaskBarIconEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_MOVE, 1550 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_DOWN, 1551 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_UP, 1552 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_DOWN, 1553 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_UP, 1554 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_DCLICK, 1555 )
    DECLARE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_DCLICK, 1556 )
END_DECLARE_EVENT_TYPES()

#define EVT_TASKBAR_MOVE(fn)         DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_MOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_DOWN(fn)    DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_UP(fn)      DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_DOWN(fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_UP(fn)     DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_DCLICK(fn)  DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_DCLICK(fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),

#endif
    // wxHAS_TASK_BAR_ICON

#endif
    // _WX_TASKBAR_H_BASE_
