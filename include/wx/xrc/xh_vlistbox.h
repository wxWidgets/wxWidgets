/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_vlistbox.h
// Purpose:     XML resource handler for wxVListBox
// Author:      Vaclav Slavik, Bill Su
// Created:     2000/03/05, 2024/10/07
// Copyright:   (c) 2000 Vaclav Slavik, 2024 Bill Su
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_VLISTBOX_H_
#define _WX_XH_VLISTBOX_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_LISTBOX

class WXDLLIMPEXP_XRC wxVListBoxXmlHandler : public wxXmlResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxVListBoxXmlHandler);

public:
    wxVListBoxXmlHandler();
    virtual wxObject *DoCreateResource() override;
    virtual bool CanHandle(wxXmlNode *node) override;
};

#endif // wxUSE_XRC && wxUSE_LISTBOX

#endif // _WX_XH_VLISTBOX_H_
