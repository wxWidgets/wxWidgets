#ifndef __DCCLIENTH_BASE__
#define __DCCLIENTH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dcclient.h"
#elif defined(__MOTIF__)
#include "wx/xt/dcclient.h"
#elif defined(__GTK__)
#include "wx/gtk/dcclient.h"
#endif

#endif
    // __DCCLIENTH_BASE__
