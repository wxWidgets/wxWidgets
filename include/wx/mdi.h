#ifndef __MDIH_BASE__
#define __MDIH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/mdi.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/mdi.h"
#elif defined(__WXGTK__)
#include "wx/gtk/mdi.h"
#elif defined(__WXQT__)
#include "wx/qt/mdi.h"
#endif

#endif
    // __MDIH_BASE__
