/////////////////////////////////////////////////////////////////////////////
// Name:        xh_toolb.h
// Purpose:     XML resource handler for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_TOOLB_H_
#define _WX_XH_TOOLB_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xh_toolb.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_TOOLBAR

class WXDLLEXPORT wxToolBar;

class WXXMLDLLEXPORT wxToolBarXmlHandler : public wxXmlResourceHandler
{
public:
    wxToolBarXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    wxToolBar *m_toolbar;
};

#endif

#endif // _WX_XH_TOOLBAR_H_
