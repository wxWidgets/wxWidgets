/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.cpp
// Purpose:     wxJoystick class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_JOYSTICK

#if !defined (__WXMAC_OSX__)

#include "wx/joystick.h"

IMPLEMENT_DYNAMIC_CLASS(wxJoystick, wxObject)

// Attributes
////////////////////////////////////////////////////////////////////////////

wxPoint wxJoystick::GetPosition() const
{
    // TODO
    return wxPoint(0, 0);
}

int wxJoystick::GetZPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetButtonState() const
{
    // TODO
    return 0;
}

int wxJoystick::GetPOVPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetPOVCTSPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetRudderPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetUPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetVPosition() const
{
    // TODO
    return 0;
}

int wxJoystick::GetMovementThreshold() const
{
    // TODO
    return 0;
}

void wxJoystick::SetMovementThreshold(int threshold)
{
    // TODO
}

// Capabilities
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::IsOk() const
{
    // TODO
    return FALSE;
}

int wxJoystick::GetNumberJoysticks()
{
    // TODO
    return 0;
}

int wxJoystick::GetManufacturerId() const
{
    // TODO
    return 0;
}

int wxJoystick::GetProductId() const
{
    // TODO
    return 0;
}

wxString wxJoystick::GetProductName() const
{
    // TODO
    return wxString(wxT(""));
}

int wxJoystick::GetXMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetYMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetZMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetXMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetYMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetZMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetNumberButtons() const
{
    // TODO
    return 0;
}

int wxJoystick::GetNumberAxes() const
{
    // TODO
    return 0;
}

int wxJoystick::GetMaxButtons() const
{
    // TODO
    return 0;
}

int wxJoystick::GetMaxAxes() const
{
    // TODO
    return 0;
}

int wxJoystick::GetPollingMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetPollingMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetRudderMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetRudderMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetUMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetUMax() const
{
    // TODO
    return 0;
}

int wxJoystick::GetVMin() const
{
    // TODO
    return 0;
}

int wxJoystick::GetVMax() const
{
    // TODO
    return 0;
}

bool wxJoystick::HasRudder() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasZ() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasU() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasV() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasPOV() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasPOV4Dir() const
{
    // TODO
    return FALSE;
}

bool wxJoystick::HasPOVCTS() const
{
    // TODO
    return FALSE;
}

// Operations
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::SetCapture(wxWindow* win, int pollingFreq)
{
    // TODO
    return FALSE;
}

bool wxJoystick::ReleaseCapture()
{
    // TODO
    return FALSE;
}

#endif 
    // !OSX
#endif
    // wxUSE_JOYSTICK

