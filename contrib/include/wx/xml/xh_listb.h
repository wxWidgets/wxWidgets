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

#ifdef __GNUG__
#pragma interface "xh_listb.h"
#endif

#include "wx/xml/xmlres.h"

#if wxUSE_LISTBOX

class WXDLLEXPORT wxListBoxXmlHandler : public wxXmlResourceHandler
{
    public:
        wxListBoxXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
    private:
        bool m_InsideBox;
        wxArrayString strList;
};

#endif

#endif // _WX_XH_LISTB_H_
