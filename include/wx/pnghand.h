#ifndef _WX_PNGHAND_H_BASE_
#define _WX_PNGHAND_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/pnghand.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/pnghand.h"
#elif defined(__WXGTK__)
#include "wx/gtk/pnghand.h"
#elif defined(__WXMAC__)
#include "wx/mac/pnghand.h"
#elif defined(__WXPM__)
#include "wx/os2/pnghand.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/pnghand.h"
#endif

#endif
    // _WX_PNGHAND_H_BASE_
