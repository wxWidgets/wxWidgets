#ifndef _WX_DC_H_BASE_
#define _WX_DC_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dc.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dc.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dc.h"
#elif defined(__WXQT__)
#include "wx/qt/dc.h"
#elif defined(__WXMAC__)
#include "wx/mac/dc.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dc.h"
#endif

#endif
    // _WX_DC_H_BASE_
