#ifndef __MENUH_BASE__
#define __MENUH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/menu.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/menu.h"
#elif defined(__WXGTK__)
#include "wx/gtk/menu.h"
#elif defined(__WXQT__)
#include "wx/qt/menu.h"
#endif

#endif
    // __MENUH_BASE__
