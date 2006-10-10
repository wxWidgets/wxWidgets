/////////////////////////////////////////////////////////////////////////////
// Name:        _taskbar.i
// Purpose:     SWIG interface defs for wxTaskBarIcon
//
// Author:      Robin Dunn
//
// Created:     2-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------
%newgroup;


%{
#ifndef wxHAS_TASK_BAR_ICON
// implement dummy classes for platforms that don't have it

class wxTaskBarIcon : public wxEvtHandler
{
public:
    wxTaskBarIcon()  { wxPyRaiseNotImplemented(); }
};


class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType, wxTaskBarIcon *)
        { wxPyRaiseNotImplemented(); }
    virtual wxEvent* Clone() const { return NULL; }
    bool IsOk() const { return false; }
    bool IsIconInstalled() const { return false; }
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxPyEmptyString) { return false; }
    bool RemoveIcon() { return false; }
    bool PopupMenu(wxMenu *menu) { return false; }
};

enum {
    wxEVT_TASKBAR_MOVE = 0,
    wxEVT_TASKBAR_LEFT_DOWN = 0,
    wxEVT_TASKBAR_LEFT_UP = 0,
    wxEVT_TASKBAR_RIGHT_DOWN = 0,
    wxEVT_TASKBAR_RIGHT_UP = 0,
    wxEVT_TASKBAR_LEFT_DCLICK = 0,
    wxEVT_TASKBAR_RIGHT_DCLICK = 0,
};


#else
// Otherwise make a class that can virtualize CreatePopupMenu
class wxPyTaskBarIcon : public wxTaskBarIcon
{
    DECLARE_ABSTRACT_CLASS(wxPyTaskBarIcon)
public:
    wxPyTaskBarIcon() : wxTaskBarIcon()
    {}

    wxMenu* CreatePopupMenu() {
        wxMenu *rval = NULL;
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "CreatePopupMenu"))) {
            PyObject* ro;
            wxMenu* ptr;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
            if (ro) {
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxMenu")))
                    rval = ptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxTaskBarIcon::CreatePopupMenu();
        return rval;
    }

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyTaskBarIcon, wxTaskBarIcon);

#endif
%}


MustHaveApp(wxPyTaskBarIcon);

%rename(TaskBarIcon) wxPyTaskBarIcon;
class wxPyTaskBarIcon : public wxEvtHandler
{
public:
    %pythonAppend wxPyTaskBarIcon   "self._setCallbackInfo(self, TaskBarIcon, 0)"

    wxPyTaskBarIcon();
    ~wxPyTaskBarIcon();

    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref);

    %pythonPrepend Destroy "args[0].this.own(False)";
    %extend {
        void Destroy() {
            self->RemoveIcon();
            delete self;
        }
    }

    bool IsOk() const;
    %pythoncode { def __nonzero__(self): return self.IsOk() }

    bool IsIconInstalled() const;

    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxPyEmptyString);
    bool RemoveIcon();
    bool PopupMenu(wxMenu *menu);
};



class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon);
};



%constant wxEventType wxEVT_TASKBAR_MOVE;
%constant wxEventType wxEVT_TASKBAR_LEFT_DOWN;
%constant wxEventType wxEVT_TASKBAR_LEFT_UP;
%constant wxEventType wxEVT_TASKBAR_RIGHT_DOWN;
%constant wxEventType wxEVT_TASKBAR_RIGHT_UP;
%constant wxEventType wxEVT_TASKBAR_LEFT_DCLICK;
%constant wxEventType wxEVT_TASKBAR_RIGHT_DCLICK;


%pythoncode {
EVT_TASKBAR_MOVE = wx.PyEventBinder (         wxEVT_TASKBAR_MOVE )
EVT_TASKBAR_LEFT_DOWN = wx.PyEventBinder (    wxEVT_TASKBAR_LEFT_DOWN )
EVT_TASKBAR_LEFT_UP = wx.PyEventBinder (      wxEVT_TASKBAR_LEFT_UP )
EVT_TASKBAR_RIGHT_DOWN = wx.PyEventBinder (   wxEVT_TASKBAR_RIGHT_DOWN )
EVT_TASKBAR_RIGHT_UP = wx.PyEventBinder (     wxEVT_TASKBAR_RIGHT_UP )
EVT_TASKBAR_LEFT_DCLICK = wx.PyEventBinder (  wxEVT_TASKBAR_LEFT_DCLICK )
EVT_TASKBAR_RIGHT_DCLICK = wx.PyEventBinder ( wxEVT_TASKBAR_RIGHT_DCLICK )
}

//---------------------------------------------------------------------------
