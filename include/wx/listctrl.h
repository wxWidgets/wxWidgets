#ifndef _WX_LISTCTRL_H_BASE_
#define _WX_LISTCTRL_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/listctrl.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/listctrl.h"
#elif defined(__WXGTK__)
#include "wx/generic/listctrl.h"
#elif defined(__WXQT__)
#include "wx/generic/listctrl.h"
#elif defined(__WXMAC__)
#include "wx/generic/listctrl.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/listctrl.h"
#endif

#endif
    // _WX_LISTCTRL_H_BASE_
