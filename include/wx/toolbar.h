/////////////////////////////////////////////////////////////////////////////
// Name:        wx/toolbar.h
// Purpose:     wxToolBar interface declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_BASE_
#define _WX_TOOLBAR_H_BASE_

#include "wx/tbarbase.h"     // the base class for all toolbars

#if wxUSE_TOOLBAR
    #if !wxUSE_TOOLBAR_NATIVE
        // the wxToolBarSimple is *the* toolbar class in this case
        #define wxToolBarSimple wxToolBar
    
        #include "wx/tbarsmpl.h"
    #else // wxUSE_TOOLBAR_NATIVE
        #if defined(__WXMSW__) && defined(__WIN95__)
           #include "wx/msw/tbar95.h"
        #elif defined(__WXMSW__)
           #include "wx/msw/tbarmsw.h"
        #elif defined(__WXMOTIF__)
           #include "wx/motif/toolbar.h"
        #elif defined(__WXGTK__)
           #include "wx/gtk/tbargtk.h"
        #elif defined(__WXQT__)
           #include "wx/qt/tbarqt.h"
        #elif defined(__WXMAC__)
           #include "wx/mac/toolbar.h"
        #elif defined(__WXPM__)
           #include "wx/os2/toolbar.h"
        #elif defined(__WXSTUBS__)
           #include "wx/stubs/toolbar.h"
        #endif
    #endif // !wxUSE_TOOLBAR_NATIVE/wxUSE_TOOLBAR_NATIVE
#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_BASE_
