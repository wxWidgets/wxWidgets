#ifndef __FILEDLGH_BASE__
#define __FILEDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/filedlg.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/filedlg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/filedlg.h"
#elif defined(__WXQT__)
#include "wx/qt/filedlg.h"
#endif

#endif
    // __FILEDLGH_BASE__
