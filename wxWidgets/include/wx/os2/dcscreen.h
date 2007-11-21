/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:     wxScreenDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxScreenDC: public wxWindowDC
{
 public:
    //
    // Create a DC representing the whole screen
    //
    wxScreenDC();

    //
    // Compatibility with X's requirements for
    // drawing on top of all windows
    //
    static bool StartDrawingOnTop(wxWindow* WXUNUSED(pWindow)) { return TRUE; }
    static bool StartDrawingOnTop(wxRect* WXUNUSED(prect) = NULL) { return TRUE; }
    static bool EndDrawingOnTop() { return TRUE; }

protected:
    virtual void DoGetSize( int* pnWidth
                           ,int* pnHeight
                          ) const;

private:
  DECLARE_DYNAMIC_CLASS(wxScreenDC)
}; // end of CLASS wxScreenDC

#endif
    // _WX_DCSCREEN_H_

