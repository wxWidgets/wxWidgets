/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_GTKDCSCREEN_H_
#define WX_GTKDCSCREEN_H_

#include "wx/dcscreen.h"
#include "wx/gtk/dcclient.h"

//-----------------------------------------------------------------------------
// wxScreenDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxScreenDCImpl : public wxWindowDCImpl
{
public:
    wxScreenDCImpl( wxScreenDC *owner );
    ~wxScreenDCImpl();

    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;

private:
    void Init();

    DECLARE_ABSTRACT_CLASS(wxScreenDCImpl)
};

#endif // _WX_GTKDCSCREEN_H_
