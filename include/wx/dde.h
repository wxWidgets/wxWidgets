#ifndef __DDEH_BASE__
#define __DDEH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dde.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dde.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dde.h"
#elif defined(__WXQT__)
#include "wx/qt/dde.h"
#endif

#endif
    // __DDEH_BASE__
