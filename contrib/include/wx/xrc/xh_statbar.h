/////////////////////////////////////////////////////////////////////////////
// Name:        xh_statbar.h
// Purpose:     XML resource handler for wxStatusBar
// Author:      Brian Ravnsgaard Riis
// Created:     2004/01/21
// RCS-ID:
// Copyright:   (c) 2004 Brian Ravnsgaard Riis
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STATBAR_H_
#define _WX_XH_STATBAR_H_

#include "wx/xrc/xmlres.h"

class WXXMLDLLEXPORT wxStatusBarXmlHandler : public wxXmlResourceHandler
{
    public:
        wxStatusBarXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif // _WX_XH_STATBAR_H_

