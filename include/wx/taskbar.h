#ifndef _WX_TASKBAR_H_BASE_
#define _WX_TASKBAR_H_BASE_

#include "wx/defs.h"

#ifdef wxHAS_TASK_BAR_ICON

#include "wx/event.h"

class WXDLLIMPEXP_ADV wxTaskBarIconEvent;

// ----------------------------------------------------------------------------
// wxTaskBarIconBase: define wxTaskBarIcon interface
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxTaskBarIconBase : public wxEvtHandler
{
public:
    wxTaskBarIconBase() { }

    // Operations:
    virtual bool SetIcon(const wxIcon& icon,
                         const wxString& tooltip = wxEmptyString) = 0;
    virtual bool RemoveIcon() = 0;
    virtual bool PopupMenu(wxMenu *menu) = 0;

protected:
    // creates menu to be displayed when user clicks on the icon
    virtual wxMenu *CreatePopupMenu() { return NULL; }

private:
    // default events handling, calls CreatePopupMenu:
    void OnRightButtonDown(wxTaskBarIconEvent& event);

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTaskBarIconBase)
};


// ----------------------------------------------------------------------------
// now include the actual class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/taskbar.h"
#elif defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #include "wx/unix/taskbarx11.h"
#elif defined(__DARWIN__)
    #include "wx/mac/taskbarosx.h"
#endif

// ----------------------------------------------------------------------------
// wxTaskBarIcon events
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon)
        : wxEvent(wxID_ANY, evtType)
    {
        SetEventObject(tbIcon);
    }

    virtual wxEvent *Clone() const { return new wxTaskBarIconEvent(*this); }

private:
    DECLARE_NO_ASSIGN_CLASS(wxTaskBarIconEvent)
};

typedef void (wxEvtHandler::*wxTaskBarIconEventFunction)(wxTaskBarIconEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_MOVE,1550)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_LEFT_DOWN,1551)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_LEFT_UP,1552)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_RIGHT_DOWN,1553)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_RIGHT_UP,1554)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_LEFT_DCLICK,1555)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV,wxEVT_TASKBAR_RIGHT_DCLICK,1556)
END_DECLARE_EVENT_TYPES()

#define EVT_TASKBAR_MOVE(fn)         DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_MOVE, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_LEFT_DOWN(fn)    DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_DOWN, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_LEFT_UP(fn)      DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_UP, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_RIGHT_DOWN(fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_DOWN, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_RIGHT_UP(fn)     DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_UP, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_LEFT_DCLICK(fn)  DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_LEFT_DCLICK, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),
#define EVT_TASKBAR_RIGHT_DCLICK(fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_TASKBAR_RIGHT_DCLICK, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTaskBarIconEventFunction, &fn ), NULL),

#endif
    // wxHAS_TASK_BAR_ICON

#endif
    // _WX_TASKBAR_H_BASE_
