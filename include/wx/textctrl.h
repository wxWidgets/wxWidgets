#ifndef __TEXTCTRLH_BASE__
#define __TEXTCTRLH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/textctrl.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/textctrl.h"
#elif defined(__WXGTK__)
#include "wx/gtk/textctrl.h"
#elif defined(__WXQT__)
#include "wx/qt/textctrl.h"
#endif

#endif
    // __TEXTCTRLH_BASE__
