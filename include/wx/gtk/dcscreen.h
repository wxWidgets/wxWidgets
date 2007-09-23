/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCSCREENH__
#define __GTKDCSCREENH__

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

#if wxUSE_NEW_DC
class WXDLLIMPEXP_CORE wxGTKScreenImplDC : public wxGTKWindowImplDC
#else
#define wxGTKScreenImplDC wxScreenDC
class WXDLLIMPEXP_CORE wxScreenDC : public wxWindowDC
#endif
{
public:

#if wxUSE_NEW_DC
    wxGTKScreenImplDC( wxScreenDC *owner );
#else
    wxScreenDC();
#endif

    ~wxGTKScreenImplDC();

    static bool StartDrawingOnTop( wxWindow *window ) { return true; }
    static bool StartDrawingOnTop( wxRect *rect = (wxRect *) NULL ) { return true; }
    static bool EndDrawingOnTop() { return true; }


protected:
    virtual void DoGetSize(int *width, int *height) const;
    
    void Init();

private:
    DECLARE_ABSTRACT_CLASS(wxGTKScreenImplDC)
};

#endif

    // __GTKDCSCREENH__

