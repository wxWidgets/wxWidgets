/////////////////////////////////////////////////////////////////////////////
// Name:        xh_slidr.h
// Purpose:     XML resource handler for wxSlider
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SLIDR_H_
#define _WX_XH_SLIDR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_slidr.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/defs.h"

#if wxUSE_SLIDER

class WXDLLIMPEXP_XRC wxSliderXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxSliderXmlHandler)
    enum
    {
        wxSL_DEFAULT_VALUE = 0,
        wxSL_DEFAULT_MIN = 0,
        wxSL_DEFAULT_MAX = 100
    };

public:
    wxSliderXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_SLIDER_H_
