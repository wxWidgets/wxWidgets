/////////////////////////////////////////////////////////////////////////////
// Name:        xh_chckl.h
// Purpose:     XML resource handler for wxCheckListBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CHCKL_H_
#define _WX_XH_CHCKL_H_

#ifdef __GNUG__
#pragma interface "xh_chckl.h"
#endif

#include "wx/xml/xmlres.h"

class wxCheckListXmlHandler : public wxXmlResourceHandler
{
    public:
        wxCheckListXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
    private:
        bool m_InsideBox;
        wxArrayString strList;
};



#endif // _WX_XH_CHECKLIST_H_