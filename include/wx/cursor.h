#ifndef __CURSORH_BASE__
#define __CURSORH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/cursor.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/cursor.h"
#elif defined(__WXGTK__)
#include "wx/gtk/cursor.h"
#elif defined(__WXQT__)
#include "wx/qt/cursor.h"
#endif

#endif
    // __CURSORH_BASE__
