#ifndef _WX_STATBMP_H_BASE_
#define _WX_STATBMP_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/statbmp.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/statbmp.h"
#elif defined(__WXGTK__)
#include "wx/gtk/statbmp.h"
#elif defined(__WXQT__)
#include "wx/qt/statbmp.h"
#elif defined(__WXMAC__)
#include "wx/mac/statbmp.h"
#elif defined(__WXPM__)
#include "wx/os2/statbmp.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/statbmp.h"
#endif

#endif
    // _WX_STATBMP_H_BASE_
