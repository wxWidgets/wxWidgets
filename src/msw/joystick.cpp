/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.cpp
// Purpose:     wxJoystick class
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "joystick.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include <windows.h>

#ifndef __GNUWIN32__
#include <mmsystem.h>
#endif

#if !defined(__WIN32__) && !defined(_MMRESULT_)
typedef UINT MMRESULT;
#endif

#ifdef __GNUWIN32__
#include <wx/msw/gnuwin32/extra.h>
#endif

// Why doesn't BC++ have joyGetPosEx?
#if !defined(__WIN32__) || defined(__BORLANDC__)
#define NO_JOYGETPOSEX
#endif

#include <wx/msw/joystick.h>

IMPLEMENT_DYNAMIC_CLASS(wxJoystick, wxObject)

// Attributes
////////////////////////////////////////////////////////////////////////////

wxPoint wxJoystick::GetPosition(void) const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
        return wxPoint(joyInfo.wXpos, joyInfo.wYpos);
    else
        return wxPoint(0, 0);
}

int wxJoystick::GetZPosition(void) const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
        return joyInfo.wZpos;
    else
        return 0;
}

int wxJoystick::GetButtonState(void) const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
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
    }
    else
        return 0;
}

int wxJoystick::GetPOVPosition(void) const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNPOV;
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwPOV;
    }
    else
        return 0;
#else
    return 0;
#endif
}

int wxJoystick::GetPOVCTSPosition(void) const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNPOVCTS;
    MMRESULT res = joyGetPosEx(m_joystick, & joyInfo);
    if (res == JOYERR_NOERROR )
    {
        return joyInfo.dwPOV;
    }
    else
        return 0;
#else
    return 0;
#endif
}

int wxJoystick::GetRudderPosition(void) const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNR;
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

int wxJoystick::GetUPosition(void) const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNU;
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

int wxJoystick::GetVPosition(void) const
{
#ifndef NO_JOYGETPOSEX
    JOYINFOEX joyInfo;
    joyInfo.dwFlags = JOY_RETURNV;
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

int wxJoystick::GetMovementThreshold(void) const
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

bool wxJoystick::IsOk(void) const
{
    JOYINFO joyInfo;
    MMRESULT res = joyGetPos(m_joystick, & joyInfo);
    return ((joyGetNumDevs() > 0) || (res == JOYERR_NOERROR));
}

int wxJoystick::GetNumberJoysticks(void) const
{
    return joyGetNumDevs();
}

int wxJoystick::GetManufacturerId(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMid;
}

int wxJoystick::GetProductId(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPid;
}

wxString wxJoystick::GetProductName(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return wxString("");
    else
        return wxString(joyCaps.szPname);
}

int wxJoystick::GetXMin(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wXmin;
}

int wxJoystick::GetYMin(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wYmin;
}

int wxJoystick::GetZMin(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wZmin;
}

int wxJoystick::GetXMax(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wXmax;
}

int wxJoystick::GetYMax(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wYmax;
}

int wxJoystick::GetZMax(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wZmax;
}

int wxJoystick::GetNumberButtons(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wNumButtons;
}

int wxJoystick::GetNumberAxes(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wNumAxes;
#else
    return 0;
#endif
}

int wxJoystick::GetMaxButtons(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMaxButtons;
#else
    return 0;
#endif
}

int wxJoystick::GetMaxAxes(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wMaxAxes;
#else
    return 0;
#endif
}

int wxJoystick::GetPollingMin(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPeriodMin;
}

int wxJoystick::GetPollingMax(void) const
{
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wPeriodMax;
}

int wxJoystick::GetRudderMin(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wRmin;
#else
    return 0;
#endif
}

int wxJoystick::GetRudderMax(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wRmax;
#else
    return 0;
#endif
}

int wxJoystick::GetUMin(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wUmin;
#else
    return 0;
#endif
}

int wxJoystick::GetUMax(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wUmax;
#else
    return 0;
#endif
}

int wxJoystick::GetVMin(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wVmin;
#else
    return 0;
#endif
}

int wxJoystick::GetVMax(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return 0;
    else
        return joyCaps.wVmax;
#else
    return 0;
#endif
}


bool wxJoystick::HasRudder(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASR) == JOYCAPS_HASR);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasZ(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASZ) == JOYCAPS_HASZ);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasU(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASU) == JOYCAPS_HASU);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasV(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASV) == JOYCAPS_HASV);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasPOV(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_HASPOV) == JOYCAPS_HASPOV);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasPOV4Dir(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_POV4DIR) == JOYCAPS_POV4DIR);
#else
    return FALSE;
#endif
}

bool wxJoystick::HasPOVCTS(void) const
{
#ifdef __WIN32__
    JOYCAPS joyCaps;
    if (joyGetDevCaps(m_joystick, & joyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        return FALSE;
    else
        return ((joyCaps.wCaps & JOYCAPS_POVCTS) == JOYCAPS_POVCTS);
#else
    return FALSE;
#endif
}

// Operations
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::SetCapture(wxWindow* win, int pollingFreq)
{
    BOOL changed = (pollingFreq == 0);
    MMRESULT res = joySetCapture((HWND) win->GetHWND(), m_joystick, pollingFreq, changed);
    return (res == JOYERR_NOERROR);
}

bool wxJoystick::ReleaseCapture(void)
{
    MMRESULT res = joyReleaseCapture(m_joystick);
    return (res == JOYERR_NOERROR);
}

