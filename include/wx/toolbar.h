#ifndef _TOOLBAR_H_BASE_
#define _TOOLBAR_H_BASE_

#if defined(__WXMSW__) && defined(__WIN95__)
#   include "wx/msw/tbar95.h"
#elif defined(__WXMSW__)
#   include "wx/msw/tbarmsw.h"
#elif defined(__WXMOTIF__)
#   include "wx/motif/toolbar.h"
#elif defined(__WXGTK__)
#   include "wx/gtk/tbargtk.h"
#elif defined(__WXQT__)
#   include "wx/qt/tbarqt.h"
#elif defined(__WXMAC__)
#   include "wx/mac/toolbar.h"
#elif defined(__WXPM__)
#   include "wx/os2/toolbar.h"
#elif defined(__WXSTUBS__)
#   include "wx/stubs/toolbar.h"
#endif

#endif
    // _TOOLBAR_H_BASE_
