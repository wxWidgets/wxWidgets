#ifndef __LISTBOXH_BASE__
#define __LISTBOXH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/listbox.h"
#elif defined(__MOTIF__)
#include "wx/xt/listbox.h"
#elif defined(__GTK__)
#include "wx/gtk/listbox.h"
#endif

#endif
    // __LISTBOXH_BASE__
