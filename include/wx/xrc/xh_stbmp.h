/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stbmp.h
// Purpose:     XML resource handler for wxStaticBitmap
// Author:      Vaclav Slavik
// Created:     2000/04/22
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STBMP_H_
#define _WX_XH_STBMP_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xh_stbmp.h"
#endif

#include "wx/xrc/xmlres.h"


class WXXMLDLLEXPORT wxStaticBitmapXmlHandler : public wxXmlResourceHandler
{
public:
    wxStaticBitmapXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_STBMP_H_
