/////////////////////////////////////////////////////////////////////////////
// Name:        xh_chckb.h
// Purpose:     XML resource handler for wxCheckBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CHCKB_H_
#define _WX_XH_CHCKB_H_

#ifdef __GNUG__
#pragma interface "xh_chckb.h"
#endif

#include "wx/xml/xmlres.h"
#include "wx/defs.h"

#if wxUSE_CHECKBOX

class wxCheckBoxXmlHandler : public wxXmlResourceHandler
{
    public:
        wxCheckBoxXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif


#endif // _WX_XH_CHECKBOX_H_

