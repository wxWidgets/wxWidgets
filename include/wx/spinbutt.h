#ifndef __SPINBUTTH_BASE__
#define __SPINBUTTH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/spinbutt.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/spinbutt.h"
#elif defined(__WXGTK__)
#include "wx/gtk/spinbutt.h"
#elif defined(__WXQT__)
#include "wx/qt/spinbutt.h"
#endif

#endif
    // __SPINBUTTH_BASE__
