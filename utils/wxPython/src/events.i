/////////////////////////////////////////////////////////////////////////////
// Name:        events.i
// Purpose:     SWIGgable Event classes for wxPython
//
// Author:      Robin Dunn
//
// Created:     5/24/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module events

%{
#include "helpers.h"
#include <wx/spinbutt.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i

//---------------------------------------------------------------------------

class wxEvent {
public:
    wxEvent(int id = 0);
    ~wxEvent();

    wxObject* GetEventObject();
    wxEventType GetEventType();
    int GetId();
    bool GetSkipped();
    long GetTimestamp();
    void SetEventObject(wxObject* object);
    void SetEventType(wxEventType typ);
    void SetId(int id);
    void SetTimestamp(long timeStamp);
    void Skip(bool skip = TRUE);
};

//---------------------------------------------------------------------------

class wxSizeEvent : public wxEvent {
public:
    wxSizeEvent(const wxSize& sz, int id = 0);
    wxSize GetSize();
};

//---------------------------------------------------------------------------

class wxCloseEvent : public wxEvent {
public:
    wxCloseEvent(int commandEventType = 0, int id = 0);

    void SetLoggingOff(bool loggingOff);
    bool GetLoggingOff();
    void Veto(bool veto = TRUE);
    bool CanVeto();
    bool GetVeto();
    void SetCanVeto(bool canVeto);
};

//---------------------------------------------------------------------------

class wxCommandEvent : public wxEvent {
public:
    wxCommandEvent(int commandEventType = 0, int id = 0);

    bool Checked();
    long GetExtraLong();
    int GetInt();
    int GetSelection();
    wxString GetString();
    bool IsSelection();
};


//---------------------------------------------------------------------------

class wxScrollEvent: public wxCommandEvent {
public:
    wxScrollEvent(int commandType = 0, int id = 0, int pos = 0,
                  int orientation = 0);

    int GetOrientation();
    int GetPosition();
};

//---------------------------------------------------------------------------

class wxScrollWinEvent: public wxEvent {
public:
    wxScrollWinEvent(int commandType = 0, int pos = 0,
                     int orientation = 0);

    int GetOrientation();
    int GetPosition();
};

//---------------------------------------------------------------------------

class wxSpinEvent : public wxScrollEvent {
public:
    wxSpinEvent(int commandType = 0, int id = 0);

};

//---------------------------------------------------------------------------

class wxMouseEvent: public wxEvent {
public:
    wxMouseEvent(int mouseEventType = 0);

    bool IsButton();
    bool ButtonDown(int but = -1);
    bool ButtonDClick(int but = -1);
    bool ButtonUp(int but = -1);
    bool Button(int but);
    bool ButtonIsDown(int but);
    bool ControlDown();
    bool MetaDown();
    bool AltDown();
    bool ShiftDown();
    bool LeftDown();
    bool MiddleDown();
    bool RightDown();
    bool LeftUp();
    bool MiddleUp();
    bool RightUp();
    bool LeftDClick();
    bool MiddleDClick();
    bool RightDClick();
    bool LeftIsDown();
    bool MiddleIsDown();
    bool RightIsDown();
    bool Dragging();
    bool Moving();
    bool Entering();
    bool Leaving();
    wxPoint GetPosition();
    %name(GetPositionTuple)void GetPosition(long *OUTPUT, long *OUTPUT);
    wxPoint GetLogicalPosition(const wxDC& dc);
    long GetX();
    long GetY();
};

//---------------------------------------------------------------------------

class wxKeyEvent: public wxEvent {
public:
    wxKeyEvent(int keyEventType);

    bool ControlDown();
    bool MetaDown();
    bool AltDown();
    bool ShiftDown();
    long KeyCode();

    long GetX();
    long GetY();
    wxPoint GetPosition();
    %name(GetPositionTuple) void GetPosition(long* OUTPUT, long* OUTPUT);
};

//---------------------------------------------------------------------------

class wxNavigationKeyEvent : public wxCommandEvent {
public:
    wxNavigationKeyEvent();

    bool GetDirection();
    void SetDirection(bool bForward);
    bool IsWindowChange();
    void SetWindowChange(bool bIs);
    wxWindow* GetCurrentFocus();
    void SetCurrentFocus(wxWindow *win);
};


//---------------------------------------------------------------------------

class wxMoveEvent: public wxEvent {
public:
    wxMoveEvent(const wxPoint& pt, int id = 0);

    wxPoint GetPosition();
};

//---------------------------------------------------------------------------

class wxPaintEvent: public wxEvent {
public:
    wxPaintEvent(int id = 0);

};

//---------------------------------------------------------------------------

class wxEraseEvent: public wxEvent {
public:
    wxEraseEvent(int id = 0, wxDC* dc = NULL);

    wxDC *GetDC();
};

//---------------------------------------------------------------------------

class wxFocusEvent: public wxEvent {
public:
    wxFocusEvent(WXTYPE eventType = 0, int id = 0);
};

//---------------------------------------------------------------------------

class wxActivateEvent: public wxEvent{
public:
    wxActivateEvent(WXTYPE eventType = 0, int active = TRUE, int id = 0);
    bool GetActive();
};

//---------------------------------------------------------------------------

class wxInitDialogEvent: public wxEvent {
public:
    wxInitDialogEvent(int id = 0);
};

//---------------------------------------------------------------------------

class wxMenuEvent: public wxEvent {
public:
    wxMenuEvent(WXTYPE id = 0, int id = 0);
    int GetMenuId();
};

//---------------------------------------------------------------------------

class wxShowEvent: public wxEvent {
public:
    wxShowEvent(int id = 0, int show = FALSE);
    void SetShow(bool show);
    bool GetShow();
};

//---------------------------------------------------------------------------

class wxIconizeEvent: public wxEvent {
public:
    wxIconizeEvent(int id = 0);
};

//---------------------------------------------------------------------------

class wxMaximizeEvent: public wxEvent {
public:
    wxMaximizeEvent(int id = 0);
};

//---------------------------------------------------------------------------

class wxJoystickEvent: public wxEvent {
public:
    wxJoystickEvent(int type = wxEVT_NULL,
                    int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0);
    wxPoint GetPosition();
    int GetZPosition();
    int GetButtonState();
    int GetButtonChange();
    int GetJoystick();
    void SetJoystick(int stick);
    void SetButtonState(int state);
    void SetButtonChange(int change);
    void SetPosition(const wxPoint& pos);
    void SetZPosition(int zPos);
    bool IsButton();
    bool IsMove();
    bool IsZMove();
    bool ButtonDown(int but = wxJOY_BUTTON_ANY);
    bool ButtonUp(int but = wxJOY_BUTTON_ANY);
    bool ButtonIsDown(int but =  wxJOY_BUTTON_ANY);
};

//---------------------------------------------------------------------------

class wxDropFilesEvent: public wxEvent {
public:
    wxPoint GetPosition();
    int GetNumberOfFiles();

    %addmethods {
        PyObject* GetFiles() {
            int         count = self->GetNumberOfFiles();
            wxString*   files = self->GetFiles();
            PyObject*   list  = PyList_New(count);

            if (!list) {
                PyErr_SetString(PyExc_MemoryError, "Can't allocate list of files!");
                return NULL;
            }

            for (int i=0; i<count; i++) {
                PyList_SetItem(list, i, PyString_FromString((const char*)files[i]));
            }
            return list;
        }
    }
};

//---------------------------------------------------------------------------

class wxIdleEvent: public wxEvent {
public:
    wxIdleEvent();
    void RequestMore(bool needMore = TRUE);
    bool MoreRequested();
};

//---------------------------------------------------------------------------

class wxUpdateUIEvent: public wxEvent {
public:
    wxUpdateUIEvent(wxWindowID commandId = 0);
    bool GetChecked();
    bool GetEnabled();
    wxString GetText();
    bool GetSetText();
    bool GetSetChecked();
    bool GetSetEnabled();

    void Check(bool check);
    void Enable(bool enable);
    void SetText(const wxString& text);
};

//---------------------------------------------------------------------------

class wxSysColourChangedEvent: public wxEvent {
public:
    wxSysColourChangedEvent();
};

//---------------------------------------------------------------------------


class wxNotifyEvent : public wxCommandEvent {
public:
    wxNotifyEvent(int commandType = wxEVT_NULL, int id = 0);
    bool IsAllowed();
    void Veto();
};


//---------------------------------------------------------------------------

class  wxPaletteChangedEvent : public wxEvent {
public:
    wxPaletteChangedEvent(wxWindowID id = 0);

    void SetChangedWindow(wxWindow* win);
    wxWindow* GetChangedWindow();

};

//---------------------------------------------------------------------------

class  wxQueryNewPaletteEvent : public wxEvent {
public:
    wxQueryNewPaletteEvent(wxWindowID id = 0);

    void SetPaletteRealized(bool realized);
    bool GetPaletteRealized();
};


//---------------------------------------------------------------------------

class wxWindowCreateEvent : public wxEvent {
public:
    wxWindowCreateEvent(wxWindow *win = NULL);

    wxWindow *GetWindow();
};

class wxWindowDestroyEvent : public wxEvent {
public:
    wxWindowDestroyEvent(wxWindow *win = NULL);

    wxWindow *GetWindow();
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// This one can be derived from in Python and passed through the event
// system without loosing anything so long as the Python data is saved with
// SetPyData...

%{
class wxPyEvent : public wxEvent {
    DECLARE_DYNAMIC_CLASS(wxPyEvent)
public:
    wxPyEvent(int id=0, PyObject* userData = Py_None)
        : wxEvent(id) {
        m_userData = userData;
        Py_INCREF(m_userData);
    }

    ~wxPyEvent() {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_userData);
        wxPySaveThread(doSave);
    }

    void SetPyData(PyObject* userData) {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_userData);
        m_userData = userData;
        Py_INCREF(m_userData);
        wxPySaveThread(doSave);
    }

    PyObject* GetPyData() const {
        Py_INCREF(m_userData);
        return m_userData;
    }

    // This one is so the event object can be Cloned...
    void CopyObject(wxObject& dest) const {
        wxEvent::CopyObject(dest);
        ((wxPyEvent*)&dest)->SetPyData(m_userData);
    }

private:
    PyObject* m_userData;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyEvent, wxEvent)

%}


class wxPyEvent : public wxEvent {
public:
    wxPyEvent(int id=0, PyObject* userData = Py_None);
    ~wxPyEvent();

    void SetPyData(PyObject* userData);
    PyObject* GetPyData();
};

//---------------------------------------------------------------------------
// Same for this one except it is a wxCommandEvent and so will get passed up the
// containment heirarchy.

%{
class wxPyCommandEvent : public wxCommandEvent {
    DECLARE_DYNAMIC_CLASS(wxPyCommandEvent)
public:
    wxPyCommandEvent(wxEventType commandType = wxEVT_NULL, int id=0, PyObject* userData = Py_None)
        : wxCommandEvent(commandType, id) {
        m_userData = userData;
        Py_INCREF(m_userData);
    }

    ~wxPyCommandEvent() {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_userData);
        wxPySaveThread(doSave);
    }

    void SetPyData(PyObject* userData) {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_userData);
        m_userData = userData;
        Py_INCREF(m_userData);
        wxPySaveThread(doSave);
    }

    PyObject* GetPyData() const {
        Py_INCREF(m_userData);
        return m_userData;
    }

    // This one is so the event object can be Cloned...
    void CopyObject(wxObject& dest) const {
        wxCommandEvent::CopyObject(dest);
        ((wxPyCommandEvent*)&dest)->SetPyData(m_userData);
    }

private:
    PyObject* m_userData;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyCommandEvent, wxCommandEvent)

%}


class wxPyCommandEvent : public wxCommandEvent {
public:
    wxPyCommandEvent(wxEventType commandType = wxEVT_NULL, int id=0, PyObject* userData = Py_None);
    ~wxPyCommandEvent();

    void SetPyData(PyObject* userData);
    PyObject* GetPyData();
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

