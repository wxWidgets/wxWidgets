#ifndef _WX_DCSCREEN_H_BASE_
#define _WX_DCSCREEN_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dcscreen.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dcscreen.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcscreen.h"
#elif defined(__WXQT__)
#include "wx/qt/dcscreen.h"
#elif defined(__WXMAC__)
#include "wx/mac/dcscreen.h"
#elif defined(__WXPM__)
#include "wx/os2/dcscreen.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dcscreen.h"
#endif

#endif
    // _WX_DCSCREEN_H_BASE_
