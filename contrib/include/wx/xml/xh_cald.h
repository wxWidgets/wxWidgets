/////////////////////////////////////////////////////////////////////////////
// Name:        xh_cald.h
// Purpose:     XML resource handler for wxCalendarCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CALD_H_
#define _WX_XH_CALD_H_

#ifdef __GNUG__
#pragma interface "xh_cald.h"
#endif

#include "wx/xml/xmlres.h"

class WXDLLEXPORT wxCalendarCtrlXmlHandler : public wxXmlResourceHandler
{
    public:
        wxCalendarCtrlXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_CALD_H_
