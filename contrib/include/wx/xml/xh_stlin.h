/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stlin.h
// Purpose:     XML resource handler for wxStaticLine
// Author:      Vaclav Slavik
// Created:     2000/09/00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STLIN_H_
#define _WX_XH_STLIN_H_

#ifdef __GNUG__
#pragma interface "xh_stlin.h"
#endif

#include "wx/xml/xmlres.h"

#if wxUSE_STATLINE

class WXDLLEXPORT wxStaticLineXmlHandler : public wxXmlResourceHandler
{
    public:
        wxStaticLineXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_STLIN_H_
