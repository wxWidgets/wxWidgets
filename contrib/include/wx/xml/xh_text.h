/////////////////////////////////////////////////////////////////////////////
// Name:        xh_text.h
// Purpose:     XML resource handler for wxTextCtrl
// Author:      Aleksandras Gluchovas
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_TEXT_H_
#define _WX_XH_TEXT_H_

#ifdef __GNUG__
#pragma interface "xh_text.h"
#endif

#include "wx/xml/xmlres.h"


class WXDLLEXPORT wxTextCtrlXmlHandler : public wxXmlResourceHandler
{
    public:
        wxTextCtrlXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_TEXT_H_
