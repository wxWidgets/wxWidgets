/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.cpp
// Purpose:     wxJoystick class
// Author:      Ported to Linux by Guilhem Lavaux
// Modified by:
// Created:     05/23/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "joystick.h"
#endif
#if wxUSE_JOYSTICK

#include <linux/joystick.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "wx/event.h"
#include "wx/window.h"
#include "wx/gtk/joystick.h"

#define JOYSTICK_AXE_MAX 32767
#define JOYSTICK_AXE_MIN -32767

IMPLEMENT_DYNAMIC_CLASS(wxJoystick, wxObject)

wxJoystick::wxJoystick(int joystick)
{
  wxString dev_name;
  // Assume it's the same device name on all Linux systems ...
  dev_name.Printf("/dev/js%d", (joystick == wxJOYSTICK1) ? 0 : 1);

  m_joystick = open(dev_name, O_RDWR);
  m_lastposition = wxPoint(-1, -1);
  for (int i=0;i<15;i++)
    m_axe[i] = 0;
  if (m_joystick != -1)
    Create();
}

////////////////////////////////////////////////////////////////////////////
// Background thread
////////////////////////////////////////////////////////////////////////////
void *wxJoystick::Entry(void)
{
  struct js_event j_evt;
  wxJoystickEvent jwx_event;
  fd_set read_fds;
  struct timeval time_out = {0, 0};

  FD_ZERO(&read_fds);
  while (1) {
    TestDestroy();

    if (m_polling) {
      FD_SET(m_joystick, &read_fds);
      select(m_joystick+1, &read_fds, NULL, NULL, &time_out);
      if (FD_ISSET(m_joystick, &read_fds))
        read(m_joystick, &j_evt, sizeof(j_evt));
      else
        j_evt.type = 0;
    } else {
      read(m_joystick, &j_evt, sizeof(j_evt));
    }

    if ((j_evt.type & JS_EVENT_AXIS) == JS_EVENT_AXIS) {
      switch (j_evt.number) {
      case 1:
        m_lastposition.x = j_evt.value;
        jwx_event.SetEventType(wxEVT_JOY_MOVE);
        break;
      case 2:
        m_lastposition.y = j_evt.value;
        jwx_event.SetEventType(wxEVT_JOY_MOVE);
        break;
      case 3:
        m_axe[3] = j_evt.value;
        jwx_event.SetEventType(wxEVT_JOY_ZMOVE);
        break;
      default:
        m_axe[j_evt.number] = j_evt.value;
        jwx_event.SetEventType(wxEVT_JOY_MOVE);
        break;
      }
      jwx_event.SetPosition(m_lastposition);
      jwx_event.SetZPosition(m_axe[3]);
    }
    if ((j_evt.type & JS_EVENT_BUTTON) == JS_EVENT_BUTTON) {
      register int mask = 1 << j_evt.number;
      char button = m_buttons & mask;

      m_buttons &= ~mask;
      if (button) {
        jwx_event.SetEventType(wxEVT_JOY_BUTTON_UP);
      } else {
        jwx_event.SetEventType(wxEVT_JOY_BUTTON_DOWN);
        m_buttons |= mask;
      }

      jwx_event.SetButtonState(m_buttons);
      jwx_event.SetButtonChange(j_evt.number);
    }
  }
  if (m_catchwin)
    m_catchwin->ProcessEvent(jwx_event);
  if (m_polling)
    usleep(m_polling*1000);
}

////////////////////////////////////////////////////////////////////////////
// State
////////////////////////////////////////////////////////////////////////////

wxPoint wxJoystick::GetPosition(void) const
{
  return m_lastposition;
}

int wxJoystick::GetZPosition(void) const
{
  return m_axe[3];
}

int wxJoystick::GetButtonState(void) const
{
  return m_buttons;
}

int wxJoystick::GetPOVPosition(void) const
{
  return 0;
}

int wxJoystick::GetPOVCTSPosition(void) const
{
  return 0;
}

int wxJoystick::GetRudderPosition(void) const
{
  return m_axe[4];
}

int wxJoystick::GetUPosition(void) const
{
  return m_axe[5];
}

int wxJoystick::GetVPosition(void) const
{
  return m_axe[6];
}

int wxJoystick::GetMovementThreshold(void) const
{
  return 0;
}

void wxJoystick::SetMovementThreshold(int threshold)
{
}

////////////////////////////////////////////////////////////////////////////
// Capabilities
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::IsOk(void) const
{
  return (m_joystick != -1);
}

int wxJoystick::GetNumberJoysticks(void) const
{
  wxString dev_name;
  int fd, j;

  for (j=0;j<2;j++) {
    dev_name.Printf("/dev/js%d", j);
    fd = open(dev_name, O_RDONLY);
    if (fd == -1)
      return j;
    close(fd);
  }
  return j;
}

int wxJoystick::GetManufacturerId(void) const
{
  return 0;
}

int wxJoystick::GetProductId(void) const
{
  return 0;
}

wxString wxJoystick::GetProductName(void) const
{
  return wxT("");
}

int wxJoystick::GetXMin(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetYMin(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetZMin(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetXMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetYMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetZMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetNumberButtons(void) const
{
  int nb;

  ioctl(m_joystick, JSIOCGBUTTONS, &nb);

  return nb;
}

int wxJoystick::GetNumberAxes(void) const
{
  int nb;

  ioctl(m_joystick, JSIOCGAXES, &nb);

  return nb;
}

int wxJoystick::GetMaxButtons(void) const
{
  return 15; // internal
}

int wxJoystick::GetMaxAxes(void) const
{
  return 15; // internal
}

int wxJoystick::GetPollingMin(void) const
{
  return -1;
}

int wxJoystick::GetPollingMax(void) const
{
  return -1;
}

int wxJoystick::GetRudderMin(void) const
{
  return JOYSTICK_AXE_MIN;
}

int wxJoystick::GetRudderMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetUMin(void) const
{
  return JOYSTICK_AXE_MIN;
}

int wxJoystick::GetUMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

int wxJoystick::GetVMin(void) const
{
  return JOYSTICK_AXE_MIN;
}

int wxJoystick::GetVMax(void) const
{
  return JOYSTICK_AXE_MAX;
}

bool wxJoystick::HasRudder(void) const
{
  return GetNumberAxes() >= 4;
}

bool wxJoystick::HasZ(void) const
{
  return GetNumberAxes() >= 3;
}

bool wxJoystick::HasU(void) const
{
  return GetNumberAxes() >= 5;
}

bool wxJoystick::HasV(void) const
{
  return GetNumberAxes() >= 6;
}

bool wxJoystick::HasPOV(void) const
{
  return FALSE;
}

bool wxJoystick::HasPOV4Dir(void) const
{
  return FALSE;
}

bool wxJoystick::HasPOVCTS(void) const
{
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::SetCapture(wxWindow* win, int pollingFreq = 0)
{
  m_catchwin = win;
  m_polling = pollingFreq;
  return TRUE;
}

bool wxJoystick::ReleaseCapture(void)
{
  m_catchwin = NULL;
  m_polling = 0;
  return TRUE;
}
#endif

