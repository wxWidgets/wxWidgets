#ifndef _WX_STATLINE_H_BASE_
#define _WX_STATLINE_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/statline.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/statline.h"
#elif defined(__WXGTK__)
#include "wx/gtk/statline.h"
#elif defined(__WXQT__)
#include "wx/qt/statline.h"
#elif defined(__WXMAC__)
#include "wx/mac/statline.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/statline.h"
#endif

#endif
    // _WX_STATLINE_H_BASE_
