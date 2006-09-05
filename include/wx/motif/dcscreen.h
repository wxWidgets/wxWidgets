/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/dcscreen.h
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxScreenDC: public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxScreenDC)

public:
    // Create a DC representing the whole screen
    wxScreenDC();
    virtual ~wxScreenDC();

    // Compatibility with X's requirements for
    // drawing on top of all windows
    static bool StartDrawingOnTop(wxWindow* window);
    static bool StartDrawingOnTop(wxRect* rect = NULL);
    static bool EndDrawingOnTop();

private:
    static WXWindow sm_overlayWindow;

    // If we have started transparent drawing at a non-(0,0) point
    // then we will have to adjust the device origin in the
    // constructor.
    static int sm_overlayWindowX;
    static int sm_overlayWindowY;
};

#endif
// _WX_DCSCREEN_H_
