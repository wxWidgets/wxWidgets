/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TIMERH__
#define __TIMERH_

#ifdef __GNUG__
#pragma interface "timer.h"
#endif

#include "wx/object.h"

class WXDLLEXPORT wxTimer: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxTimer)

 public:
  bool oneShot ;
  int  milli ;
  int  lastMilli ;

  long id;
 public:
  wxTimer(void);
  ~wxTimer(void);
  virtual bool Start(int milliseconds = -1,bool one_shot = FALSE); // Start timer
  virtual void Stop(void);                   // Stop timer
  virtual void Notify(void) = 0;             // Override this member
  inline int Interval(void) { return milli ; }; // Returns the current interval time (0 if stop)
};

// Timer functions (milliseconds)
void WXDLLEXPORT wxStartTimer(void);
// Gets time since last wxStartTimer or wxGetElapsedTime
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

// EXPERIMENTAL: comment this out if it doesn't compile.
bool WXDLLEXPORT wxGetLocalTime(long *timeZone, int *dstObserved);

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentTime(void);

#endif
    // __TIMERH_
