#ifndef _WX_TIMER_H_BASE_
#define _WX_TIMER_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/timer.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/timer.h"
#elif defined(__WXGTK__)
#include "wx/gtk/timer.h"
#elif defined(__WXQT__)
#include "wx/qt/timer.h"
#elif defined(__WXMAC__)
#include "wx/mac/timer.h"
#elif defined(__WXPM__)
#include "wx/os2/timer.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/timer.h"
#endif

class WXDLLEXPORT wxChrono
{
public: 
  wxChrono();
  
  void Start(long t=0);
  void Pause();
  void Resume();

  long Time();

private:
  long m_t0;
  long m_pause;
};

// Timer functions (milliseconds)
void WXDLLEXPORT wxStartTimer();

// Gets time since last wxStartTimer or wxGetElapsedTime
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

// EXPERIMENTAL: comment this out if it doesn't compile.
bool WXDLLEXPORT wxGetLocalTime(long *timeZone, int *dstObserved);

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentTime();

// Get number of milliseconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentMTime();

#endif
    // _WX_TIMER_H_BASE_
