/////////////////////////////////////////////////////////////////////////////
// Name:        xh_mdi.h
// Purpose:     XML resource handler for dialogs
// Author:      David M. Falkinder & Vaclav Slavik
// Created:     14/02/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_MDI_H_
#define _WX_XH_MDI_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_mdi.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_MDI

class WXDLLIMPEXP_CORE wxWindow;

class WXDLLIMPEXP_XRC wxMdiXmlHandler : public wxXmlResourceHandler
{
public:
    wxMdiXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    wxWindow *CreateFrame();

    DECLARE_DYNAMIC_CLASS(wxMdiXmlHandler)
};

#endif // wxUSE_MDI

#endif // _WX_XH_MDI_H_
