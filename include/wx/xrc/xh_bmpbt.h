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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xh_bmpbt.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC


class WXDLLIMPEXP_XRC wxBitmapButtonXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxBitmapButtonXmlHandler)
public:
    wxBitmapButtonXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC


#endif // _WX_XH_BMPBT_H_
