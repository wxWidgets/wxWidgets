#ifndef __CHECKBOXH_BASE__
#define __CHECKBOXH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/checkbox.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/checkbox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/checkbox.h"
#elif defined(__WXQT__)
#include "wx/qt/checkbox.h"
#endif

#endif
    // __CHECKBOXH_BASE__
