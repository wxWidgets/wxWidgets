#ifndef __LISTBOXH_BASE__
#define __LISTBOXH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/listbox.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/listbox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/listbox.h"
#elif defined(__WXQT__)
#include "wx/qt/listbox.h"
#endif

#endif
    // __LISTBOXH_BASE__
