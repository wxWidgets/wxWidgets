#ifndef __CONTROLH_BASE__
#define __CONTROLH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/control.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/control.h"
#elif defined(__WXGTK__)
#include "wx/gtk/control.h"
#elif defined(__WXQT__)
#include "wx/qt/control.h"
#endif

#endif
    // __CONTROLH_BASE__
