#ifndef __MSGDLGH_BASE__
#define __MSGDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/msgdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXQT__)
#include "wx/generic/msgdlgg.h"
#endif

#endif
    // __MSGDLGH_BASE__
