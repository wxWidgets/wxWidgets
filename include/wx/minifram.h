#ifndef _WX_MINIFRAM_H_BASE_
#define _WX_MINIFRAMH_BASE_

#if defined(__WXMSW__)
#include "wx/msw/minifram.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/minifram.h"
#elif defined(__WXGTK__)
#include "wx/gtk/minifram.h"
#elif defined(__WXX11__)
#include "wx/x11/minifram.h"
#elif defined(__WXMAC__)
#include "wx/mac/minifram.h"
#elif defined(__WXPM__)
#include "wx/os2/minifram.h"
#endif

#endif
    // _WX_MINIFRAM_H_BASE_
