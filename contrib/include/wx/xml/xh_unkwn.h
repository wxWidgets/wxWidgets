/////////////////////////////////////////////////////////////////////////////
// Name:        xh_unkwn.h
// Purpose:     XML resource handler for unkown widget
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_UNKWN_H_
#define _WX_XH_UNKWN_H_

#ifdef __GNUG__
#pragma interface "xh_unkwn.h"
#endif

#include "wx/xml/xmlres.h"


class WXDLLEXPORT wxUnknownWidgetXmlHandler : public wxXmlResourceHandler
{
    public:
        wxUnknownWidgetXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_UNKWN_H_
