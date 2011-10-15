///////////////////////////////////////////////////////////////////////////////
// Name:        wx/nonownedwnd.h
// Purpose:     declares wxNonTopLevelWindow class
// Author:      Vaclav Slavik
// Modified by:
// Created:     2006-12-24
// RCS-ID:      $Id$
// Copyright:   (c) 2006 TT-Solutions
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NONOWNEDWND_H_
#define _WX_NONOWNEDWND_H_

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxNonOwnedWindow: a window that is not a child window of another one.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindowBase : public wxWindow
{
public:
    // Set the shape of the window to the given region.
    // Returns true if the platform supports this feature (and the
    // operation is successful.)
    virtual bool SetShape(const wxRegion& WXUNUSED(region)) { return false; }

};

#if defined(__WXDFB__)
    #include "wx/dfb/nonownedwnd.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/nonownedwnd.h"
#elif defined(__WXMAC__)
    #include "wx/osx/nonownedwnd.h"
#elif defined(__WXMSW__) && !defined(__WXWINCE__)
    #include "wx/msw/nonownedwnd.h"
#else
    // No special class needed in other ports, they can derive both wxTLW and
    // wxPopupWindow directly from wxWindow and don't implement SetShape().
    class wxNonOwnedWindow : public wxNonOwnedWindowBase
    {
    };
#endif

#endif // _WX_NONOWNEDWND_H_
