#ifndef _WX_TEXTCTRL_H_BASE_
#define _WX_TEXTCTRL_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/textctrl.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/textctrl.h"
#elif defined(__WXGTK__)
#include "wx/gtk/textctrl.h"
#elif defined(__WXQT__)
#include "wx/qt/textctrl.h"
#elif defined(__WXMAC__)
#include "wx/mac/textctrl.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/textctrl.h"
#endif

#endif
    // _WX_TEXTCTRL_H_BASE_
