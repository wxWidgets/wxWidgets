/////////////////////////////////////////////////////////////////////////////
// Name:        xh_choicbk.h
// Purpose:     XML resource handler for wxChoicebook
// Author:      Vaclav Slavik
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CHOICEBK_H_
#define _WX_XH_CHOICEBK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_choicbk.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_CHOICEBOOK

class WXDLLEXPORT wxChoicebook;

class WXDLLIMPEXP_XRC wxChoicebookXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxChoicebookXmlHandler)
public:
    wxChoicebookXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    wxChoicebook *m_choicebook;
};

#endif

#endif // _WX_XH_CHOICEBK_H_
