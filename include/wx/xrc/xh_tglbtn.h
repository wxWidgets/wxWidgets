/////////////////////////////////////////////////////////////////////////////
// Name:        xh_tglbtn.h
// Purpose:     XML resource handler for wxToggleButton
// Author:      Julian Smart
// Created:     2004-08-30
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_TGLBTN_H_
#define _WX_XH_TGLBTN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xh_tglbtn.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/defs.h"

#if wxUSE_TOGGLEBTN

class WXDLLIMPEXP_XRC wxToggleButtonXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxToggleButtonXmlHandler)
public:
    wxToggleButtonXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif


#endif // _WX_XH_TGLBTN_H_

