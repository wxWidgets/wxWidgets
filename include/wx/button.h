#ifndef _WX_BUTTON_H_BASE_
#define _WX_BUTTON_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/button.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/button.h"
#elif defined(__WXGTK__)
#include "wx/gtk/button.h"
#elif defined(__WXQT__)
#include "wx/qt/button.h"
#elif defined(__WXMAC__)
#include "wx/mac/button.h"
#elif defined(__WXPM__)
#include "wx/os2/button.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/button.h"
#endif

#endif
    // _WX_BUTTON_H_BASE_
