#ifndef __STATBOXH_BASE__
#define __STATBOXH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/statbox.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/statbox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/statbox.h"
#elif defined(__WXQT__)
#include "wx/qt/statbox.h"
#endif

#endif
    // __STATBOXH_BASE__
