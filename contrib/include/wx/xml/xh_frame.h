/////////////////////////////////////////////////////////////////////////////
// Name:        xh_frame.h
// Purpose:     XML resource handler for dialogs
// Author:      Vaclav Slavik & Aleks.
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_FRAME_H_
#define _WX_XH_FRAME_H_

#ifdef __GNUG__
#pragma interface "xh_frame.h"
#endif

#include "wx/xml/xmlres.h"

class WXDLLEXPORT wxFrameXmlHandler : public wxXmlResourceHandler
{
    public:
        wxFrameXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_FRAME_H_
