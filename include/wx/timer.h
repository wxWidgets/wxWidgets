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

#endif
    // _WX_TIMER_H_BASE_
