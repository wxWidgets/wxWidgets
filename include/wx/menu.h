#ifndef __MENUH_BASE__
#define __MENUH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/menu.h"
#elif defined(__MOTIF__)
#include "wx/xt/menu.h"
#elif defined(__GTK__)
#include "wx/gtk/menu.h"
#endif

#endif
    // __MENUH_BASE__
