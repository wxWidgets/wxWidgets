/////////////////////////////////////////////////////////////////////////////
// Name:        xh_gauge.h
// Purpose:     XML resource handler for wxGauge
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_GAUGE_H_
#define _WX_XH_GAUGE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_gauge.h"
#endif

#include "wx/defs.h"

#if wxUSE_GAUGE

#include "wx/xrc/xmlres.h"


class WXDLLIMPEXP_XRC wxGaugeXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxGaugeXmlHandler)
    enum
    {
        wxGAUGE_DEFAULT_RANGE = 100
    };

public:
    wxGaugeXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_GAUGE_H_
