/////////////////////////////////////////////////////////////////////////////
// Name:        xh_scrol.h
// Purpose:     XML resource handler for wxScrollBar
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SCROL_H_
#define _WX_XH_SCROL_H_

#ifdef __GNUG__
#pragma interface "xh_scrol.h"
#endif

#include "wx/xml/xmlres.h"
#include "wx/defs.h"



class WXDLLEXPORT wxScrollBarXmlHandler : public wxXmlResourceHandler
{
    enum 
    {
        wxSL_DEFAULT_VALUE = 0,
        wxSL_DEFAULT_MIN = 0,
        wxSL_DEFAULT_MAX = 100
    };

    public:
        wxScrollBarXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_SCROL_H_
