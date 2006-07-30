/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.h
// Purpose:     wxJoystick class
// Author:      Guilhem Lavaux
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __JOYSTICKH__
#define __JOYSTICKH__

#ifdef __GNUG__
#pragma interface "joystick.h"
#endif

#include "wx/event.h"
#include "wx/thread.h"

class WXDLLEXPORT wxJoystick: public wxObject, public wxThread
{
  DECLARE_DYNAMIC_CLASS(wxJoystick)
 public:
  /*
   * Public interface
   */

  wxJoystick(int joystick = wxJOYSTICK1);

  // Attributes
  ////////////////////////////////////////////////////////////////////////////

  wxPoint GetPosition(void) const;
  int GetZPosition(void) const;
  int GetButtonState(void) const;
  int GetPOVPosition(void) const;
  int GetPOVCTSPosition(void) const;
  int GetRudderPosition(void) const;
  int GetUPosition(void) const;
  int GetVPosition(void) const;
  int GetMovementThreshold(void) const;
  void SetMovementThreshold(int threshold) ;

  // Capabilities
  ////////////////////////////////////////////////////////////////////////////

  bool IsOk(void) const; // Checks that the joystick is functioning
  int GetNumberJoysticks(void) const ;
  int GetManufacturerId(void) const ;
  int GetProductId(void) const ;
  wxString GetProductName(void) const ;
  int GetXMin(void) const;
  int GetYMin(void) const;
  int GetZMin(void) const;
  int GetXMax(void) const;
  int GetYMax(void) const;
  int GetZMax(void) const;
  int GetNumberButtons(void) const;
  int GetNumberAxes(void) const;
  int GetMaxButtons(void) const;
  int GetMaxAxes(void) const;
  int GetPollingMin(void) const;
  int GetPollingMax(void) const;
  int GetRudderMin(void) const;
  int GetRudderMax(void) const;
  int GetUMin(void) const;
  int GetUMax(void) const;
  int GetVMin(void) const;
  int GetVMax(void) const;

  bool HasRudder(void) const;
  bool HasZ(void) const;
  bool HasU(void) const;
  bool HasV(void) const;
  bool HasPOV(void) const;
  bool HasPOV4Dir(void) const;
  bool HasPOVCTS(void) const;

  // Operations
  ////////////////////////////////////////////////////////////////////////////

  // pollingFreq = 0 means that movement events are sent when above the threshold.
  // If pollingFreq > 0, events are received every this many milliseconds.
  bool SetCapture(wxWindow* win, int pollingFreq = 0);
  bool ReleaseCapture(void);

protected:
  int       m_joystick;
  wxPoint   m_lastposition;
  int	    m_axe[15];
  int	    m_buttons;
  wxWindow *m_catchwin;
  int	    m_polling;

  void	   *Entry(void);
};

#endif
    // __JOYSTICKH__

