#ifndef _WX_MDI_H_BASE_
#define _WX_MDI_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/mdi.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/mdi.h"
#elif defined(__WXGTK__)
#include "wx/gtk/mdi.h"
#elif defined(__WXQT__)
#include "wx/qt/mdi.h"
#elif defined(__WXMAC__)
#include "wx/mac/mdi.h"
#elif defined(__WXPM__)
#include "wx/os2/mdi.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/mdi.h"
#endif

#endif
    // _WX_MDI_H_BASE_
