#ifndef __DIRDLGH_BASE__
#define __DIRDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dirdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dirdlg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dirdlg.h"
#endif

#endif
    // __DIRDLGH_BASE__
