#ifndef _WX_MENUITEM_H_BASE_
#define _WX_MENUITEM_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/menuitem.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/menuitem.h"
#elif defined(__WXGTK__)
#include "wx/gtk/menuitem.h"
#elif defined(__WXQT__)
#include "wx/qt/menuitem.h"
#elif defined(__WXMAC__)
#include "wx/mac/menuitem.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/menuitem.h"
#endif

#endif
    // _WX_MENUITEM_H_BASE_
