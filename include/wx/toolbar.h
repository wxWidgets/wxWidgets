#ifndef _TOOLBAR_H_BASE_
#define _TOOLBAR_H_BASE_

#if defined(__WXMSW__) && defined(__WIN95__)
#   include "wx/tbar95.h"
#   define wxToolBar wxToolBar95
#   define classwxToolBar classwxToolBar95
#elif defined(__WXMSW__)
#   include "wx/tbarmsw.h"
#   define wxToolBar wxToolBarMSW
#   define classwxToolBar classwxToolBarMSW
#elif defined(__WXGTK__)
#   include "wx/gtk/tbargtk.h"
#   define wxToolBar wxToolBarGTK
#   define classwxToolBar classwxToolBarGTK
#endif

#endif
    // _TOOLBAR_H_BASE_
