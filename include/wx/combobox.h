#ifndef __COMBOBOXH_BASE__
#define __COMBOBOXH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/combobox.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/combobox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/combobox.h"
#elif defined(__WXQT__)
#include "wx/qt/combobox.h"
#endif

#endif
    // __COMBOBOXH_BASE__
