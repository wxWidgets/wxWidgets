#ifndef __TEXTCTRLH_BASE__
#define __TEXTCTRLH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/textctrl.h"
#elif defined(__MOTIF__)
#include "wx/xt/textctrl.h"
#elif defined(__GTK__)
#include "wx/gtk/textctrl.h"
#endif

#endif
    // __TEXTCTRLH_BASE__
