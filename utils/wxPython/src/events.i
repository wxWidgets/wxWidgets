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
    bool GetSessionEnding();
    bool GetLoggingOff();
    void Veto(bool veto = TRUE);
    bool GetVeto();
    void SetForce(bool force);
    bool GetForce();
};

//---------------------------------------------------------------------------

class wxCommandEvent : public wxEvent {
public:
    bool Checked();
    long GetExtraLong();
    int GetInt();
    int GetSelection();
    char* GetString();
    bool IsSelection();

};


//---------------------------------------------------------------------------

class wxScrollEvent: public wxCommandEvent {
public:
    int GetOrientation();
    int GetPosition();
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
    void Position(float *OUTPUT, float *OUTPUT);
    float GetX();
    float GetY();
};

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


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.2  1998/08/15 07:36:33  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:50  RD
// Initial version
//
//

