#ifndef __DCCLIENTH_BASE__
#define __DCCLIENTH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dcclient.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dcclient.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcclient.h"
#elif defined(__WXQT__)
#include "wx/qt/dcclient.h"
#endif

#endif
    // __DCCLIENTH_BASE__
