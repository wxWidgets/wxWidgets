/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stbox.h
// Purpose:     XML resource handler for wxStaticBox
// Author:      Brian Gavin
// Created:     2000/09/00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STBOX_H_
#define _WX_XH_STBOX_H_

#ifdef __GNUG__
#pragma interface "xh_stbox.h"
#endif

#include "wx/xml/xmlres.h"


class WXDLLEXPORT wxStaticBoxXmlHandler : public wxXmlResourceHandler
{
    public:
        wxStaticBoxXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_STBOX_H_
