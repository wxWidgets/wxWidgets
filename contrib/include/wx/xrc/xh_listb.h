/////////////////////////////////////////////////////////////////////////////
// Name:        xh_listb.h
// Purpose:     XML resource handler for wxListbox
// Author:      Bob Mitchell & Vaclav Slavik
// Created:     2000/07/29
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell & Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_LISTB_H_
#define _WX_XH_LISTB_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xh_listb.h"
#endif

#include "wx/xrc/xmlres.h"

#if wxUSE_LISTBOX

class WXXMLDLLEXPORT wxListBoxXmlHandler : public wxXmlResourceHandler
{
public:
    wxListBoxXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
private:
    bool m_insideBox;
    wxArrayString strList;
};

#endif

#endif // _WX_XH_LISTB_H_
