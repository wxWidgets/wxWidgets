#ifndef __MSGDLGH_BASE__
#define __MSGDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/msgdlg.h"
#elif defined(__MOTIF__)
#include "wx/generic/msgdlgg.h"
#elif defined(__GTK__)
#include "wx/generic/msgdlgg.h"
#endif

#endif
    // __MSGDLGH_BASE__
