#ifndef _WX_BITMAP_H_BASE_
#define _WX_BITMAP_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/bitmap.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/bitmap.h"
#elif defined(__WXGTK__)
#include "wx/gtk/bitmap.h"
#elif defined(__WXQT__)
#include "wx/qt/bitmap.h"
#elif defined(__WXMAC__)
#include "wx/mac/bitmap.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/bitmap.h"
#endif

#endif
    // _WX_BITMAP_H_BASE_
