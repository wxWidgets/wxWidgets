/////////////////////////////////////////////////////////////////////////////
// Name:        xh_panel.h
// Purpose:     XML resource handler for panels
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_PANEL_H_
#define _WX_XH_PANEL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_panel.h"
#endif

#include "wx/xrc/xmlres.h"

class WXDLLIMPEXP_XRC wxPanelXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxPanelXmlHandler)
public:
    wxPanelXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_PANEL_H_
