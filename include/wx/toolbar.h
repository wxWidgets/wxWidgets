#ifndef _TOOLBAR_H_BASE_
#define _TOOLBAR_H_BASE_

// the application code should use only wxToolBar which is #define'd to be the
// native implementation for each platform
#if defined(__WXMSW__) && defined(__WIN95__)
#   include "wx/msw/tbar95.h"
#   define wxToolBar wxToolBar95
#   define classwxToolBar classwxToolBar95
#elif defined(__WXMSW__)
#   include "wx/msw/tbarmsw.h"
#   define wxToolBar wxToolBarMSW
#   define classwxToolBar classwxToolBarMSW
#elif defined(__WXGTK__)
#   include "wx/gtk/tbargtk.h"
#endif

#endif
    // _TOOLBAR_H_BASE_
