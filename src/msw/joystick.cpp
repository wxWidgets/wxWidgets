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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_JOYSTICK

#include <iostream>

#include "wx/joystick.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/window.h"
#endif

#include "wx/msw/private.h"

#include <mmsystem.h>

// Why doesn't BC++ have joyGetPosEx?
#if defined(__BORLANDC__)
#define NO_JOYGETPOSEX
#endif

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
    wxJoystickThread(int joystick);
    void* Entry() wxOVERRIDE;

private:
    void      SendEvent(wxEventType type, long ts, int change = 0);
    int       m_device;
    int       m_joystick;
    wxPoint   m_lastposition;
    int       m_axe[wxJS_MAX_AXES];
    int       m_buttons;
    wxWindow* m_catchwin;
    int       m_polling;
    int       m_threshold;
    JOYINFOEX m_joyInfoEx;
    JOYINFOEX m_lastJoyInfoEx;

    friend class wxJoystick;
};


wxJoystickThread::wxJoystickThread(int joystick)
    : m_joystick(joystick),
      m_lastposition(wxDefaultPosition),
      m_buttons(0),
      m_catchwin(NULL),
      m_polling(0),
      m_threshold(0),
      m_joyInfoEx(),
      m_lastJoyInfoEx()
{
    memset(m_axe, 0, sizeof(m_axe));
    m_joyInfoEx.dwSize = sizeof(JOYINFOEX);
    m_joyInfoEx.dwFlags = JOY_RETURNALL;
    m_lastJoyInfoEx.dwSize = sizeof(JOYINFOEX);
    m_lastJoyInfoEx.dwFlags = JOY_RETURNALL;
}

void wxJoystickThread::SendEvent(wxEventType type, long ts, int change)
{
    wxJoystickEvent jwx_event(type, m_buttons, m_joystick, change);
    if (change)
        std::cout << "Button changed: " << change << std::endl;

    jwx_event.SetTimestamp(ts);
    jwx_event.SetPosition(wxPoint(m_joyInfoEx.dwXpos, m_joyInfoEx.dwYpos));
    jwx_event.SetZPosition(m_joyInfoEx.dwZpos);
    jwx_event.SetEventObject(m_catchwin);

    if (m_catchwin)
        m_catchwin->GetEventHandler()->AddPendingEvent(jwx_event);
}

void* wxJoystickThread::Entry()
{
    std::cout << "Entered polling thread" << std::endl;
    joyGetPosEx(m_joystick, &m_lastJoyInfoEx);
    std::cout << m_lastJoyInfoEx.dwSize << std::endl;

    while (true)
    {
        if (TestDestroy())
            break;

        this->Sleep(m_polling);
        DWORD ts = GetTickCount();

        joyGetPosEx(m_joystick, &m_joyInfoEx);
        DWORD delta = m_joyInfoEx.dwButtons ^ m_lastJoyInfoEx.dwButtons;
        DWORD deltaUp = delta ^ !m_joyInfoEx.dwButtons;
        DWORD deltaDown = delta & m_joyInfoEx.dwButtons;

        for (int i = 0; i < wxJS_MAX_BUTTONS; i++)
        {
            if (deltaUp & (1<<i))
                SendEvent(wxEVT_JOY_BUTTON_UP, ts, i);
            else if (deltaDown & (1<<i))
                SendEvent(wxEVT_JOY_BUTTON_DOWN, ts, i);
        }

        if (not( deltaUp || deltaDown)) // button events also report position.
        {
            if ((m_joyInfoEx.dwXpos != m_lastJoyInfoEx.dwXpos) ||
                (m_joyInfoEx.dwYpos != m_lastJoyInfoEx.dwYpos) ||
                (m_joyInfoEx.dwZpos != m_lastJoyInfoEx.dwZpos)   )
            {
                SendEvent(wxEVT_JOY_MOVE, ts);
            }
        }
        memcpy (&m_lastJoyInfoEx, &m_joyInfoEx, m_lastJoyInfoEx.dwSize);
    }


    std::cout << "Leaving polling thread cleanly." << std::endl;
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
                m_thread->Create();
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
    if (id > sizeof(int) * 8)
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
        return thresh;
    }
    else
        return 0;
}

void wxJoystick::SetMovementThreshold(int threshold)
{
    UINT thresh = threshold;
    joySetThreshold(m_joystick, thresh);
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
        m_thread->m_catchwin = win;
        m_thread->m_polling = pollingFreq;
        return true;
    }
    return false;
}

bool wxJoystick::ReleaseCapture()
{
    if (m_thread)
    {
        m_thread->m_catchwin = NULL;
        m_thread->m_polling = 0;
        return true;
    }
    return false;
}

#endif // wxUSE_JOYSTICK
