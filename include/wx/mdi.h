#ifndef _WX_MDI_H_BASE_
#define _WX_MDI_H_BASE_

#include "wx/setup.h"

#if wxUSE_MDI

#if defined(__WXUNIVERSAL__)
    #include "wx/generic/mdig.h"
#elif defined(__WXMSW__)
    #include "wx/msw/mdi.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/mdi.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/mdi.h"
#elif defined(__WXMAC__)
    #include "wx/mac/mdi.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/mdi.h"
#elif defined(__WXPM__)
    #include "wx/generic/mdig.h"
#endif

#endif // wxUSE_MDI

#endif
    // _WX_MDI_H_BASE_
