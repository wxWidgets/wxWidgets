#ifndef _WX_SCROLBAR_H_BASE_
#define _WX_SCROLBAR_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/scrolbar.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/scrolbar.h"
#elif defined(__WXGTK__)
#include "wx/gtk/scrolbar.h"
#elif defined(__WXQT__)
#include "wx/qt/scrolbar.h"
#elif defined(__WXMAC__)
#include "wx/mac/scrolbar.h"
#elif defined(__WXPM__)
#include "wx/os2/scrolbar.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/scrolbar.h"
#endif

#endif
    // _WX_SCROLBAR_H_BASE_
