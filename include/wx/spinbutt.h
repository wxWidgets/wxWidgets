#ifndef _WX_SPINBUTT_H_BASE_
#define _WX_SPINBUTT_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/spinbutt.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/spinbutt.h"
#elif defined(__WXGTK__)
#include "wx/gtk/spinbutt.h"
#elif defined(__WXQT__)
#include "wx/qt/spinbutt.h"
#elif defined(__WXMAC__)
#include "wx/mac/spinbutt.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/spinbutt.h"
#endif

#endif
    // _WX_SPINBUTT_H_BASE_
