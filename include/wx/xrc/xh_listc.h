/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_listc.h
// Purpose:     XML resource handler for wxListCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_LISTC_H_
#define _WX_XH_LISTC_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_LISTCTRL

class WXDLLIMPEXP_XRC wxListCtrlXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxListCtrlXmlHandler)

public:
    wxListCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC && wxUSE_LISTCTRL

#endif // _WX_XH_LISTC_H_
