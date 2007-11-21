/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/dcscreen.h
// Purpose:     wxScreenDC declaration
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_DCSCREEN_H_
#define _WX_DFB_DCSCREEN_H_

#include "wx/dc.h"

class WXDLLIMPEXP_CORE wxScreenDC: public wxDC
{
public:
    wxScreenDC();

    static bool StartDrawingOnTop(wxWindow *WXUNUSED(window))
        { return true; }
    static bool StartDrawingOnTop(wxRect *WXUNUSED(rect) = NULL)
        { return true; }
    static bool EndDrawingOnTop()
        { return true; }

    DECLARE_DYNAMIC_CLASS(wxScreenDC)
};

#endif // _WX_DFB_DCSCREEN_H_
