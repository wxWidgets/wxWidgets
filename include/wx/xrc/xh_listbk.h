/////////////////////////////////////////////////////////////////////////////
// Name:        xh_listbk.h
// Purpose:     XML resource handler for wxListbook
// Author:      Vaclav Slavik
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_LISTBK_H_
#define _WX_XH_LISTBK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_listbk.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_LISTBOOK

class WXDLLEXPORT wxListbook;

class WXDLLIMPEXP_XRC wxListbookXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxListbookXmlHandler)
public:
    wxListbookXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    wxListbook *m_listbook;
};

#endif

#endif // _WX_XH_LISTBK_H_
