/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_frame.h
// Purpose:     XML resource handler for wxFrame
// Author:      Vaclav Slavik & Aleks.
// Created:     2000/03/05
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_XH_FRAME_H_
#define WX_XH_FRAME_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC

class WXDLLIMPEXP_XRC wxFrameXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxFrameXmlHandler)

public:
    wxFrameXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC

#endif // _WX_XH_FRAME_H_
