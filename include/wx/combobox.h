#ifndef __COMBOBOXH_BASE__
#define __COMBOBOXH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/combobox.h"
#elif defined(__MOTIF__)
#include "wx/xt/combobox.h"
#elif defined(__GTK__)
#include "wx/gtk/combobox.h"
#endif

#endif
    // __COMBOBOXH_BASE__
