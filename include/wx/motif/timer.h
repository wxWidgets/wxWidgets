/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#ifdef __GNUG__
#pragma interface "timer.h"
#endif

#include "wx/object.h"

class WXDLLEXPORT wxTimer: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxTimer)

 friend void wxTimerCallback (wxTimer * timer);

 public:
  wxTimer();
  ~wxTimer();
  virtual bool Start(int milliseconds = -1,bool one_shot = FALSE); // Start timer
  virtual void Stop();                   // Stop timer
  virtual void Notify() = 0;             // Override this member
  inline int Interval() { return m_milli ; }; // Returns the current interval time (0 if stop)

protected:
  bool      m_oneShot ;
  int       m_milli ;
  int       m_lastMilli ;
  long      m_id;

};

/* Note: these are implemented in common/timercmn.cpp, so need to implement them separately.
 * But you may need to modify timercmn.cpp.
 */

#endif
    // _WX_TIMER_H_
