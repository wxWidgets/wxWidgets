#ifndef __GAUGEH_BASE__
#define __GAUGEH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/gauge.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/gauge.h"
#elif defined(__WXGTK__)
#include "wx/gtk/gauge.h"
#elif defined(__WXQT__)
#include "wx/qt/gauge.h"
#endif

#endif
    // __GAUGEH_BASE__
