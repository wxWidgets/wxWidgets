#ifndef __DIRDLGH_BASE__
#define __DIRDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dirdlg.h"
#elif defined(__MOTIF__)
#include "wx/xt/dirdlg.h"
#elif defined(__GTK__)
#include "wx/gtk/dirdlg.h"
#endif

#endif
    // __DIRDLGH_BASE__
