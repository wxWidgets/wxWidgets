#ifndef __CHECKBOXH_BASE__
#define __CHECKBOXH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/checkbox.h"
#elif defined(__MOTIF__)
#include "wx/xt/checkbox.h"
#elif defined(__GTK__)
#include "wx/gtk/checkbox.h"
#endif

#endif
    // __CHECKBOXH_BASE__
