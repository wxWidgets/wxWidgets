/////////////////////////////////////////////////////////////////////////////
// Name:        xh_dlg.h
// Purpose:     XML resource handler for dialogs
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_DLG_H_
#define _WX_XH_DLG_H_

#ifdef __GNUG__
#pragma interface "xh_dlg.h"
#endif

#include "wx/xml/xmlres.h"

class WXDLLEXPORT wxDialogXmlHandler : public wxXmlResourceHandler
{
    public:
        wxDialogXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_DLG_H_
