#ifndef __RADIOBOXH_BASE__
#define __RADIOBOXH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/radiobox.h"
#elif defined(__MOTIF__)
#include "wx/xt/radiobox.h"
#elif defined(__GTK__)
#include "wx/gtk/radiobox.h"
#endif

#endif
    // __RADIOBOXH_BASE__
