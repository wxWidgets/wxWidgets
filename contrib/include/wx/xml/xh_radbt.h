/////////////////////////////////////////////////////////////////////////////
// Name:        xh_radbt.h
// Purpose:     XML resource handler for radio buttons
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_RADBT_H_
#define _WX_XH_RADBT_H_

#ifdef __GNUG__
#pragma interface "xh_radbt.h"
#endif

#include "wx/xml/xmlres.h"
#include "wx/defs.h"

#if wxUSE_RADIOBOX

class WXDLLEXPORT wxRadioButtonXmlHandler : public wxXmlResourceHandler
{
    public:
        wxRadioButtonXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_RADIOBUTTON_H_
