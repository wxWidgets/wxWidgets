#ifndef _WX_MINIFRAM_H_BASE_
#define _WX_MINIFRAMH_BASE_

#if defined(__WXMSW__)
#include "wx/msw/minifram.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/minifram.h"
#elif defined(__WXGTK__)
#include "wx/gtk/minifram.h"
#elif defined(__WXMAC__)
#include "wx/mac/minifram.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/minifram.h"
#endif

#endif
    // _WX_MINIFRAM_H_BASE_
