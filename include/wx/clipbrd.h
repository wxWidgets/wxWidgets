#ifndef _WX_CLIPBRD_H_BASE_
#define _WX_CLIPBRD_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/clipbrd.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/clipbrd.h"
#elif defined(__WXGTK__)
#include "wx/gtk/clipbrd.h"
#elif defined(__WXQT__)
#include "wx/gtk/clipbrd.h"
#elif defined(__WXMAC__)
#include "wx/mac/clipbrd.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/clipbrd.h"
#endif

#endif
    // _WX_CLIPBRD_H_BASE_
