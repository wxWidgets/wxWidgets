/////////////////////////////////////////////////////////////////////////////
// Name:        xh_bttn.h
// Purpose:     XML resource handler for buttons
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_BTTN_H_
#define _WX_XH_BTTN_H_

#ifdef __GNUG__
#pragma interface "xh_bttn.h"
#endif

#include "wx/xml/xmlres.h"

class WXDLLEXPORT wxButtonXmlHandler : public wxXmlResourceHandler
{
    public:
        wxButtonXmlHandler() : wxXmlResourceHandler() {}
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_BTTN_H_
