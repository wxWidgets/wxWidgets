#ifndef _WX_MENU_H_BASE_
#define _WX_MENU_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/menu.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/menu.h"
#elif defined(__WXGTK__)
#include "wx/gtk/menu.h"
#elif defined(__WXQT__)
#include "wx/qt/menu.h"
#elif defined(__WXMAC__)
#include "wx/mac/menu.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/menu.h"
#endif

#endif
    // _WX_MENU_H_BASE_
