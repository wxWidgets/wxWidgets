/////////////////////////////////////////////////////////////////////////////
// Name:        _joystick.i
// Purpose:     SWIG interface stuff for wxJoystick
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/joystick.h>
%}    

//---------------------------------------------------------------------------



// Which joystick? Same as Windows ids so no conversion necessary.
enum
{
    wxJOYSTICK1,
    wxJOYSTICK2
};

// Which button is down?
enum
{
    wxJOY_BUTTON_ANY,
    wxJOY_BUTTON1,
    wxJOY_BUTTON2,
    wxJOY_BUTTON3,
    wxJOY_BUTTON4,
};


%{
#if !wxUSE_JOYSTICK && !defined(__WXMSW__)
// A C++ stub class for wxJoystick for platforms that don't have it.
class wxJoystick : public wxObject {
public:
    wxJoystick(int joystick = wxJOYSTICK1) {
        bool blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxJoystick is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    wxPoint GetPosition() { return wxPoint(-1,-1); }
    int GetZPosition() { return -1; }
    int GetButtonState() { return -1; }
    int GetPOVPosition() { return -1; }
    int GetPOVCTSPosition() { return -1; }
    int GetRudderPosition() { return -1; }
    int GetUPosition() { return -1; }
    int GetVPosition() { return -1; }
    int GetMovementThreshold() { return -1; }
    void SetMovementThreshold(int threshold) {}

    bool IsOk(void) { return false; }
    int GetNumberJoysticks() { return -1; }
    int GetManufacturerId() { return -1; }
    int GetProductId() { return -1; }
    wxString GetProductName() { return wxEmptyString; }
    int GetXMin() { return -1; }
    int GetYMin() { return -1; }
    int GetZMin() { return -1; }
    int GetXMax() { return -1; }
    int GetYMax() { return -1; }
    int GetZMax() { return -1; }
    int GetNumberButtons() { return -1; }
    int GetNumberAxes() { return -1; }
    int GetMaxButtons() { return -1; }
    int GetMaxAxes() { return -1; }
    int GetPollingMin() { return -1; }
    int GetPollingMax() { return -1; }
    int GetRudderMin() { return -1; }
    int GetRudderMax() { return -1; }
    int GetUMin() { return -1; }
    int GetUMax() { return -1; }
    int GetVMin() { return -1; }
    int GetVMax() { return -1; }

    bool HasRudder() { return false; }
    bool HasZ() { return false; }
    bool HasU() { return false; }
    bool HasV() { return false; }
    bool HasPOV() { return false; }
    bool HasPOV4Dir() { return false; }
    bool HasPOVCTS() { return false; }

    bool SetCapture(wxWindow* win, int pollingFreq = 0) { return false; }
    bool ReleaseCapture() { return false; }
};
#endif
%}


MustHaveApp(wxJoystick);

class wxJoystick /* : public wxObject */
{
public:
    wxJoystick(int joystick = wxJOYSTICK1);
    ~wxJoystick();

    wxPoint GetPosition();
    int GetZPosition();
    int GetButtonState();
    int GetPOVPosition();
    int GetPOVCTSPosition();
    int GetRudderPosition();
    int GetUPosition();
    int GetVPosition();
    int GetMovementThreshold();
    void SetMovementThreshold(int threshold) ;

    bool IsOk(void);
    int GetNumberJoysticks();
    int GetManufacturerId();
    int GetProductId();
    wxString GetProductName();
    int GetXMin();
    int GetYMin();
    int GetZMin();
    int GetXMax();
    int GetYMax();
    int GetZMax();
    int GetNumberButtons();
    int GetNumberAxes();
    int GetMaxButtons();
    int GetMaxAxes();
    int GetPollingMin();
    int GetPollingMax();
    int GetRudderMin();
    int GetRudderMax();
    int GetUMin();
    int GetUMax();
    int GetVMin();
    int GetVMax();

    bool HasRudder();
    bool HasZ();
    bool HasU();
    bool HasV();
    bool HasPOV();
    bool HasPOV4Dir();
    bool HasPOVCTS();

    bool SetCapture(wxWindow* win, int pollingFreq = 0);
    bool ReleaseCapture();

    %pythoncode { def __nonzero__(self): return self.IsOk() }
};


//---------------------------------------------------------------------------

%constant wxEventType wxEVT_JOY_BUTTON_DOWN;
%constant wxEventType wxEVT_JOY_BUTTON_UP;
%constant wxEventType wxEVT_JOY_MOVE;
%constant wxEventType wxEVT_JOY_ZMOVE;


class wxJoystickEvent : public wxEvent
{
public:
    wxPoint   m_pos;
    int       m_zPosition;
    int       m_buttonChange;   // Which button changed?
    int       m_buttonState;    // Which buttons are down?
    int       m_joyStick;       // Which joystick?

    wxJoystickEvent(wxEventType type = wxEVT_NULL,
                    int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0);

    wxPoint GetPosition() const;
    int GetZPosition() const;
    int GetButtonState() const;
    int GetButtonChange() const;
    int GetJoystick() const;

    void SetJoystick(int stick);
    void SetButtonState(int state);
    void SetButtonChange(int change);
    void SetPosition(const wxPoint& pos);
    void SetZPosition(int zPos);

    // Was it a button event? (*doesn't* mean: is any button *down*?)
    bool IsButton() const;

    // Was it a move event?
    bool IsMove() const;

    // Was it a zmove event?
    bool IsZMove() const;

    // Was it a down event from button 1, 2, 3, 4 or any?
    bool ButtonDown(int but = wxJOY_BUTTON_ANY) const;

    // Was it a up event from button 1, 2, 3 or any?
    bool ButtonUp(int but = wxJOY_BUTTON_ANY) const;

    // Was the given button 1,2,3,4 or any in Down state?
    bool ButtonIsDown(int but =  wxJOY_BUTTON_ANY) const;
};


%pythoncode {
EVT_JOY_BUTTON_DOWN = wx.PyEventBinder( wxEVT_JOY_BUTTON_DOWN )
EVT_JOY_BUTTON_UP = wx.PyEventBinder( wxEVT_JOY_BUTTON_UP )
EVT_JOY_MOVE = wx.PyEventBinder( wxEVT_JOY_MOVE )
EVT_JOY_ZMOVE = wx.PyEventBinder( wxEVT_JOY_ZMOVE )

EVT_JOYSTICK_EVENTS = wx.PyEventBinder([ wxEVT_JOY_BUTTON_DOWN,
                                        wxEVT_JOY_BUTTON_UP, 
                                        wxEVT_JOY_MOVE, 
                                        wxEVT_JOY_ZMOVE,
                                        ])

}    

//---------------------------------------------------------------------------
