#ifndef __FILEDLGH_BASE__
#define __FILEDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/filedlg.h"
#elif defined(__MOTIF__)
#include "wx/xt/filedlg.h"
#elif defined(__GTK__)
#include "wx/gtk/filedlg.h"
#endif

#endif
    // __FILEDLGH_BASE__
