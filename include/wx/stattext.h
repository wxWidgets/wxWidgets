#ifndef __STATTEXTH_BASE__
#define __STATTEXTH_BASE_

#if defined(__WXMSW__)
#include "wx/msw/stattext.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/stattext.h"
#elif defined(__WXGTK__)
#include "wx/gtk/stattext.h"
#endif

#endif
    // __STATTEXTH_BASE__
