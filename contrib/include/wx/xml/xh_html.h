/////////////////////////////////////////////////////////////////////////////
// Name:        xh_html.h
// Purpose:     XML resource handler for wxHtmlWindow
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_HTML_H_
#define _WX_XH_HTML_H_

#ifdef __GNUG__
#pragma interface "xh_html.h"
#endif

#include "wx/xml/xmlres.h"

#include "wx/defs.h"

#if wxUSE_HTML

class wxHtmlWindowXmlHandler : public wxXmlResourceHandler
{
    public:
        wxHtmlWindowXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_SLIDER_H_
