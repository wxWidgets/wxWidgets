/////////////////////////////////////////////////////////////////////////////
// Name:        xh_bmpbt.h
// Purpose:     XML resource handler for bitmap buttons
// Author:      Brian Gavin
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_BMPBT_H_
#define _WX_XH_BMPBT_H_

#ifdef __GNUG__
#pragma interface "xh_bmpbt.h"
#endif

#include "wx/xml/xmlres.h"


class WXDLLEXPORT wxBitmapButtonXmlHandler : public wxXmlResourceHandler
{
    public:
        wxBitmapButtonXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_BMPBT_H_
