#ifndef _WX_TABCTRL_H_BASE_
#define _WX_TABCTRL_H_BASE_

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED, 800)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING, 801)
END_DECLARE_EVENT_TYPES()

#if defined(__WXMSW__)
#include "wx/msw/tabctrl.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/tabctrl.h"
#elif defined(__WXGTK__)
#elif defined(__WXMAC__)
#include "wx/mac/tabctrl.h"
#elif defined(__WXPM__)
#include "wx/os2/tabctrl.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/tabctrl.h"
#endif

#endif
    // _WX_TABCTRL_H_BASE_
