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

#if 0
class WXDLLEXPORT wxToolBar : public wxControl
{
};
#endif // 0

#if defined(__WXMSW__) && defined(__WIN95__)
#   include "wx/msw/tbar95.h"
#elif defined(__WXMSW__)
#   include "wx/msw/tbarmsw.h"
#elif defined(__WXMOTIF__)
#   include "wx/motif/toolbar.h"
#elif defined(__WXGTK__)
#   include "wx/gtk/tbargtk.h"
#elif defined(__WXQT__)
#   include "wx/qt/tbarqt.h"
#elif defined(__WXMAC__)
#   include "wx/mac/toolbar.h"
#elif defined(__WXPM__)
#   include "wx/os2/toolbar.h"
#elif defined(__WXSTUBS__)
#   include "wx/stubs/toolbar.h"
#endif

#endif
    // _WX_TOOLBAR_H_BASE_
