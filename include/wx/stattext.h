#ifndef __STATTEXTH_BASE__
#define __STATTEXTH_BASE_

#if defined(__WINDOWS__)
#include "wx/msw/stattext.h"
#elif defined(__MOTIF__)
#include "wx/xt/stattext.h"
#elif defined(__GTK__)
#include "wx/gtk/stattext.h"
#endif

#endif
    // __STATTEXTH_BASE__
