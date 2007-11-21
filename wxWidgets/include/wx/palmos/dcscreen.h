/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/dcscreen.h
// Purpose:     wxScreenDC class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxScreenDC : public wxWindowDC
{
public:
    // Create a DC representing the whole screen
    wxScreenDC();

    // Compatibility with X's requirements for drawing on top of all windows
    static bool StartDrawingOnTop(wxWindow* WXUNUSED(window)) { return TRUE; }
    static bool StartDrawingOnTop(wxRect* WXUNUSED(rect) = NULL) { return TRUE; }
    static bool EndDrawingOnTop() { return TRUE; }

protected:
    virtual void DoGetSize(int *width, int *height) const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxScreenDC)
};

#endif
    // _WX_DCSCREEN_H_

