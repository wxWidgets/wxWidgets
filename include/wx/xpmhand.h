#ifndef _WX_XPMHAND_H_BASE_
#define _WX_XPMHAND_H_BASE_

// Only wxMSW and wxPM currently defines a separate XPM handler, since
// mostly Windows and Presentation Manager apps won't need XPMs.
#if defined(__WXMSW__)
#include "wx/msw/xpmhand.h"
#endif
#if defined(__WXPM__)
#include "wx/os2/xpmhand.h"
#endif

#endif
    // _WX_XPMHAND_H_BASE_
