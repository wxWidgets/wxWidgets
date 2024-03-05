/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_listbk.h
// Purpose:     XML resource handler for wxListbook
// Author:      Vaclav Slavik
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_LISTBK_H_
#define _WX_XH_LISTBK_H_

#include "wx/xrc/xh_bookctrlbase.h"

#if wxUSE_XRC && wxUSE_LISTBOOK

class WXDLLIMPEXP_FWD_CORE wxListbook;

class WXDLLIMPEXP_XRC wxListbookXmlHandler : public wxBookCtrlXmlHandlerBase
{
    wxDECLARE_DYNAMIC_CLASS(wxListbookXmlHandler);

public:
    wxListbookXmlHandler();
    virtual wxObject *DoCreateResource() override;
    virtual bool CanHandle(wxXmlNode *node) override;

private:
    wxListbook *m_listbook;
};

#endif // wxUSE_XRC && wxUSE_LISTBOOK

#endif // _WX_XH_LISTBK_H_
