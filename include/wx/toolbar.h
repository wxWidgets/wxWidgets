#ifndef _TOOLBAR_H_BASE_
#define _TOOLBAR_H_BASE_

#if defined(__WINDOWS__) && defined(__WIN95__)
#   include "wx/tbar95.h"
#   define wxToolBar wxToolBar95
#   define classwxToolBar classwxToolBar95
#elif defined(__WINDOWS__)
#   include "wx/tbarmsw.h"
#   define wxToolBar wxToolBarMSW
#   define classwxToolBar classwxToolBarMSW
#elif defined(__GTK__)
#   include "wx/gtk/tbargtk.h"
#   define classwxToolBar wxToolBarGTK
#endif

#endif
    // _TOOLBAR_H_BASE_
