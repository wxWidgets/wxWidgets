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
    wxSize GetSize();
};

//---------------------------------------------------------------------------

class wxCloseEvent : public wxEvent {
public:
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
    int GetOrientation();
    int GetPosition();
};

//---------------------------------------------------------------------------

class wxScrollWinEvent: public wxEvent {
public:
    int GetOrientation();
    int GetPosition();
};

//---------------------------------------------------------------------------

class wxSpinEvent : public wxScrollEvent {
public:

};

//---------------------------------------------------------------------------

class wxMouseEvent: public wxEvent {
public:
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
    void Position(long *OUTPUT, long *OUTPUT);
    wxPoint GetPosition();
    wxPoint GetLogicalPosition(const wxDC& dc);
    long GetX();
    long GetY();
};

//---------------------------------------------------------------------------

class wxKeyEvent: public wxEvent {
public:
    bool ControlDown();
    bool MetaDown();
    bool AltDown();
    bool ShiftDown();
    long KeyCode();

};

//---------------------------------------------------------------------------

//  class wxNavigationKeyEvent : public wxCommandEvent {
//  public:
//      wxNavigationKeyEvent();

//      bool GetDirection();
//      void SetDirection(bool bForward);
//      bool IsWindowChange();
//      void SetWindowChange(bool bIs);
//      wxWindow* GetCurrentFocus();
//      void SetCurrentFocus(wxWindow *win);
//  };


//---------------------------------------------------------------------------

class wxMoveEvent: public wxEvent {
public:
    wxPoint GetPosition();
};

//---------------------------------------------------------------------------

class wxPaintEvent: public wxEvent {
public:

};

//---------------------------------------------------------------------------

class wxEraseEvent: public wxEvent {
public:
    wxDC *GetDC();
};

//---------------------------------------------------------------------------

class wxFocusEvent: public wxEvent {
public:

};

//---------------------------------------------------------------------------

class wxActivateEvent: public wxEvent{
public:
    bool GetActive();
};

//---------------------------------------------------------------------------

class wxInitDialogEvent: public wxEvent {
public:

};

//---------------------------------------------------------------------------

class wxMenuEvent: public wxEvent {
public:
    int GetMenuId();
};

//---------------------------------------------------------------------------

class wxShowEvent: public wxEvent {
public:
    void SetShow(bool show);
    bool GetShow();
};

//---------------------------------------------------------------------------

class wxIconizeEvent: public wxEvent {
public:
};

//---------------------------------------------------------------------------

class wxMaximizeEvent: public wxEvent {
public:

};

//---------------------------------------------------------------------------

class wxJoystickEvent: public wxEvent {
public:
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
    void RequestMore(bool needMore = TRUE);
    bool MoreRequested();
};

//---------------------------------------------------------------------------

class wxUpdateUIEvent: public wxEvent {
public:
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

};

//---------------------------------------------------------------------------


class wxNotifyEvent : public wxCommandEvent {
public:
    bool IsAllowed();
    void Veto();
};


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

