#ifndef _WX_FRAME_H_BASE_
#define _WX_FRAME_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/frame.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/frame.h"
#elif defined(__WXGTK__)
#include "wx/gtk/frame.h"
#elif defined(__WXQT__)
#include "wx/qt/frame.h"
#elif defined(__WXMAC__)
#include "wx/mac/frame.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/frame.h"
#endif

#endif
    // _WX_FRAME_H_BASE_
