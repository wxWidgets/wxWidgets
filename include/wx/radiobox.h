#ifndef __RADIOBOXH_BASE__
#define __RADIOBOXH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/radiobox.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/radiobox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/radiobox.h"
#endif

#endif
    // __RADIOBOXH_BASE__
