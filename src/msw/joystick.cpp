/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/joystick.cpp
// Purpose:     wxJoystick class
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_JOYSTICK

#include "wx/joystick.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/window.h"
#endif

#include "wx/msw/private.h"

#include <mmsystem.h>

#include "wx/msw/registry.h"

#include <regstr.h>


enum {
    wxJS_AXIS_X = 0,
    wxJS_AXIS_Y,
    wxJS_AXIS_Z,
    wxJS_AXIS_RUDDER,
    wxJS_AXIS_U,
    wxJS_AXIS_V,

    wxJS_AXIS_MAX = 32767,
    wxJS_AXIS_MIN = -32767,
    wxJS_MAX_AXES = 6,      // WinMM supports up to 6 axes.
    wxJS_MAX_BUTTONS = 32,  // WinMM supports up to 32 buttons.
};

wxIMPLEMENT_DYNAMIC_CLASS(wxJoystick, wxObject);

////////////////////////////////////////////////////////////////////////////
// Background thread for reading the joystick device
////////////////////////////////////////////////////////////////////////////

class wxJoystickThread : public wxThread
{
public:
    explicit wxJoystickThread(int joystick);
    void* Entry() wxOVERRIDE;
    void SetPolling(wxWindow* win, int pollingFreq)
    {
        m_catchwin = win;
        m_polling = pollingFreq;
    }


private:
    void      SendEvent(wxEventType type, long ts, int change = 0);
    int       m_joystick;
    int       m_buttons;
    wxWindow* m_catchwin;
    int       m_polling;
    JOYINFO   m_joyInfo;
    JOYINFO   m_lastJoyInfo;
};


wxJoystickThread::wxJoystickThread(int joystick)
    : m_joystick(joystick),
      m_buttons(0),
      m_catchwin(NULL),
      m_polling(0),
      m_joyInfo(),
      m_lastJoyInfo()
{
}

void wxJoystickThread::SendEvent(wxEventType type, long ts, int change)
{
    wxJoystickEvent joystickEvent(type, (int)m_buttons, m_joystick, change);

    joystickEvent.SetTimestamp(ts);
    joystickEvent.SetPosition(wxPoint( (int)m_joyInfo.wXpos, (int)m_joyInfo.wYpos) );
    joystickEvent.SetZPosition( (int)m_joyInfo.wZpos );
    joystickEvent.SetEventObject(m_catchwin);

    if (m_catchwin)
        m_catchwin->GetEventHandler()->ProcessThreadEvent(joystickEvent);
}

void* wxJoystickThread::Entry()
{
    joyGetPos(m_joystick, &m_lastJoyInfo);

    while (!TestDestroy())
    {
        Sleep(m_polling);
        long ts = GetTickCount();

        joyGetPos(m_joystick, &m_joyInfo);
        m_buttons = m_joyInfo.wButtons;
        UINT delta = m_buttons ^ m_lastJoyInfo.wButtons;
        UINT deltaUp = delta & ~m_buttons;
        UINT deltaDown = delta & m_buttons;

        // Use count trailing zeros to determine which button changed.
        // Was using JOYINFOEX.dwButtons, because the docs state this is
        // "Current button number that is pressed.", but it turns out
        // it is the *total* number of buttons pressed.
        if (deltaUp)
            SendEvent(wxEVT_JOY_BUTTON_UP, ts, deltaUp);
        if (deltaDown)
            SendEvent(wxEVT_JOY_BUTTON_DOWN, ts, deltaDown);

        if ((m_joyInfo.wXpos != m_lastJoyInfo.wXpos) ||
            (m_joyInfo.wYpos != m_lastJoyInfo.wYpos) ||
            (m_joyInfo.wZpos != m_lastJoyInfo.wZpos)   )
        {
            SendEvent(wxEVT_JOY_MOVE, ts);
        }

        m_lastJoyInfo = m_joyInfo;
    }

    return NULL;
}



// Attributes
////////////////////////////////////////////////////////////////////////////

/**
    johan@linkdata.se 2002-08-20:
    Now returns only valid, functioning
    joysticks, counting from the first
    available and upwards.
*/
wxJoystick::wxJoystick(int joystick)
{
    JOYINFO joyInfo;
    int i, maxsticks;

    m_thread = NULL;
    maxsticks = joyGetNumDevs();
    for( i=0; i<maxsticks; i++ )
    {
        if( joyGetPos(i, & joyInfo) == JOYERR_NOERROR )
        {
            if( !joystick )
            {
                /* Found the one we want, store actual OS id and return */
                m_joystick = i;
                m_thread = new wxJoystickThread(m_joystick);
                m_thread->Run();
                return;
            }
            joystick --;
        }
    }

    /* No such joystick, return ID 0 */
    m_joystick = 0;
    return;
}


wxJoystick::~wxJoystick()
{
    ReleaseCapture();
    if (m_thread)
        m_thread->Delete();  // It's detached so it will delete itself
}


wxPoint wxJoystick::GetPosition() const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
        return wxPoint(joyInfo.wXpos, joyInfo.wYpos);
    else
        return wxPoint(0,0);
}

int wxJoystick::GetPosition(unsigned axis) const
{
    switch (axis) {
    case 0:
        return GetPosition().x;
    case 1:
        return GetPosition().y;
    case 2:
        return GetZPosition();
    case 3:
        return GetRudderPosition();
    case 4:
        return GetUPosition();
    case 5:
        return GetVPosition();
    default:
        return 0;
    }
}

int wxJoystick::GetZPosition() const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
        return joyInfo.wZpos;
    else
        return 0;
}

/**
    johan@linkdata.se 2002-08-20:
    Return a bitmap with all button states in it,
    like the GTK version does and Win32 does.
*/
int wxJoystick::GetButtonState() const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.wButtons;
#if 0
        int buttons = 0;

        if (joyInfo.wButtons & JOY_BUTTON1)
            buttons |= wxJOY_BUTTON1;
        if (joyInfo.wButtons & JOY_BUTTON2)
            buttons |= wxJOY_BUTTON2;
        if (joyInfo.wButtons & JOY_BUTTON3)
            buttons |= wxJOY_BUTTON3;
        if (joyInfo.wButtons & JOY_BUTTON4)
            buttons |= wxJOY_BUTTON4;

        return buttons;
#endif
    }
    else
        return 0;
}

bool wxJoystick::GetButtonState(unsigned id) const
{
    if (id >= sizeof(int) * 8)
        return false;

    return (GetButtonState() & (1 << id)) != 0;
}

/**
    JLI 2002-08-20:
    Returns -1 to signify error.
*/
int wxJoystick::GetPOVPosition() const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNPOV;
    joyInfo.dwSize = sizeof(joyInfo);
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwPOV;
    }
    else
        return -1;
#else
    return -1;
#endif
}

/**
    johan@linkdata.se 2002-08-20:
    Returns -1 to signify error.
*/
int wxJoystick::GetPOVCTSPosition() const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNPOVCTS;
    joyInfo.dwSize = sizeof(joyInfo);
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwPOV;
    }
    else
        return -1;
#else
    return -1;
#endif
}

int wxJoystick::GetRudderPosition() const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNR;
    joyInfo.dwSize = sizeof(joyInfo);
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwRpos;
    }
    else
        return 0;
#else
    return 0;
#endif
}

int wxJoystick::GetUPosition() const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNU;
    joyInfo.dwSize = sizeof(joyInfo);
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwUpos;
    }
    else
        return 0;
#else
    return 0;
#endif
}

int wxJoystick::GetVPosition() const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNV;
    joyInfo.dwSize = sizeof(joyInfo);
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwVpos;
    }
    else
        return 0;
#else
    return 0;
#endif
}

int wxJoystick::GetMovementThreshold() const
{
    UINT thresh = 0;
    MMRESULT res = joyGetThreshold(m_joystick, & thresh);
    if (res == JOYERR_NOERROR )
    {
        return (int)thresh;
    }
    else
        return 0;
}

void wxJoystick::SetMovementThreshold(int threshold)
{
    joySetThreshold(m_joystick, (UINT)threshold);
}

// Capabilities
////////////////////////////////////////////////////////////////////////////

/**
    johan@linkdata.se 2002-08-20:
    Now returns the number of connected, functioning
    joysticks, as intended.
*/
int wxJoystick::GetNumberJoysticks()
{
    JOYINFO joyInfo;
    int i, maxsticks, actualsticks;
    maxsticks = joyGetNumDevs();
    actualsticks = 0;
    for( i=0; i<maxsticks; i++ )
    {
        if( joyGetPos( i, & joyInfo ) == JOYERR_NOERROR )
        {
            actualsticks ++;
        }
    }
    return actualsticks;
}

/**
    johan@linkdata.se 2002-08-20:
    The old code returned true if there were any
    joystick capable drivers loaded (=always).
*/
bool wxJoystick::IsOk() const
{
    JOYINFO joyInfo;
    return (joyGetPos(m_joystick, & joyInfo) == JOYERR_NOERROR);
}

int wxJoystick::GetManufacturerId() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMid;
}

int wxJoystick::GetProductId() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPid;
}

wxString wxJoystick::GetProductName() const
{
    wxString str;
#ifndef __WINE__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, &joyCaps, sizeof(joyCaps)) != JOYERR_NOERROR)
        return wxEmptyString;

    wxRegKey key1(wxString::Format(wxT("HKEY_LOCAL_MACHINE\\%s\\%s\\%s"),
                   REGSTR_PATH_JOYCONFIG, joyCaps.szRegKey, REGSTR_KEY_JOYCURR));

    if ( key1.Exists() )
    {
        key1.QueryValue(wxString::Format(wxT("Joystick%d%s"),
                                        m_joystick + 1, REGSTR_VAL_JOYOEMNAME),
                        str);
    }

    if ( !str.empty() )
    {
        wxRegKey key2(wxString::Format(wxT("HKEY_LOCAL_MACHINE\\%s\\%s"),
                                       REGSTR_PATH_JOYOEM, str.c_str()));
        if ( key2.Exists() )
        {
            key2.QueryValue(REGSTR_VAL_JOYOEMNAME, str);
        }
    }
#endif
    return str;
}

int wxJoystick::GetXMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wXmin;
}

int wxJoystick::GetYMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wYmin;
}

int wxJoystick::GetZMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wZmin;
}

int wxJoystick::GetXMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wXmax;
}

int wxJoystick::GetYMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wYmax;
}

int wxJoystick::GetZMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wZmax;
}

int wxJoystick::GetNumberButtons() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wNumButtons;
}

int wxJoystick::GetNumberAxes() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wNumAxes;
}

int wxJoystick::GetMaxButtons() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMaxButtons;
}

int wxJoystick::GetMaxAxes() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMaxAxes;
}

int wxJoystick::GetPollingMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPeriodMin;
}

int wxJoystick::GetPollingMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPeriodMax;
}

int wxJoystick::GetRudderMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wRmin;
}

int wxJoystick::GetRudderMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wRmax;
}

int wxJoystick::GetUMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wUmin;
}

int wxJoystick::GetUMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wUmax;
}

int wxJoystick::GetVMin() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wVmin;
}

int wxJoystick::GetVMax() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wVmax;
}


bool wxJoystick::HasRudder() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASR) == JOYCAPS_HASR);
}

bool wxJoystick::HasZ() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASZ) == JOYCAPS_HASZ);
}

bool wxJoystick::HasU() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASU) == JOYCAPS_HASU);
}

bool wxJoystick::HasV() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASV) == JOYCAPS_HASV);
}

bool wxJoystick::HasPOV() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASPOV) == JOYCAPS_HASPOV);
}

bool wxJoystick::HasPOV4Dir() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_POV4DIR) == JOYCAPS_POV4DIR);
}

bool wxJoystick::HasPOVCTS() const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return false;
    else
        return ((joyCaps.wCaps & JOYCAPS_POVCTS) == JOYCAPS_POVCTS);
}

////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::SetCapture(wxWindow* win, int pollingFreq)
{
    if (m_thread)
    {
        m_thread->SetPolling(win, pollingFreq);
        return true;
    }
    return false;
}

bool wxJoystick::ReleaseCapture()
{
    if (m_thread)
    {
        m_thread->SetPolling(NULL, 0);
        return true;
    }
    return false;
}

#endif // wxUSE_JOYSTICK
