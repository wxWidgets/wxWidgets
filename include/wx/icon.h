#ifndef __ICONH_BASE__
#define __ICONH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/icon.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/icon.h"
#elif defined(__WXGTK__)
#include "wx/gtk/icon.h"
#endif

#endif
    // __ICONH_BASE__
