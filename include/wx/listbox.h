#ifndef _WX_LISTBOX_H_BASE_
#define _WX_LISTBOX_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/listbox.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/listbox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/listbox.h"
#elif defined(__WXQT__)
#include "wx/qt/listbox.h"
#elif defined(__WXMAC__)
#include "wx/mac/listbox.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/listbox.h"
#endif

#endif
    // _WX_LISTBOX_H_BASE_
