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

protected:
    virtual void DoGetSize(int *width, int *height) const;

    void Init();

private:
    DECLARE_ABSTRACT_CLASS(wxScreenDCImpl)
};

#endif // __GTKDCSCREENH__

