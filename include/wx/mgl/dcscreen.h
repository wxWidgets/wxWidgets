/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_DCSCREEN_H__
#define __WX_DCSCREEN_H__

#ifdef __GNUG__
#pragma interface "dcscreen.h"
#endif

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxScreenDC;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

//FIXME_MGL
class WXDLLEXPORT wxScreenDC: public wxPaintDC
{
public:
    wxScreenDC() {}
    ~wxScreenDC() {}

    static bool StartDrawingOnTop( wxWindow *window ) {}
    static bool StartDrawingOnTop( wxRect *rect = (wxRect *) NULL ) {}
    static bool EndDrawingOnTop() {}
  
private:
    DECLARE_DYNAMIC_CLASS(wxScreenDC)
};

#endif

    // __WX_DCSCREEN_H__

