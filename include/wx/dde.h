#ifndef _WX_DDE_H_BASE_
#define _WX_DDE_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dde.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dde.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dde.h"
#elif defined(__WXQT__)
#include "wx/qt/dde.h"
#elif defined(__WXMAC__)
#include "wx/mac/dde.h"
#elif defined(__WXPM__)
#include "wx/os2/dde.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dde.h"
#endif

#endif
    // _WX_DDE_H_BASE_
