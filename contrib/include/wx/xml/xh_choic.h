/////////////////////////////////////////////////////////////////////////////
// Name:        xh_choic.h
// Purpose:     XML resource handler for wxChoice
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CHOIC_H_
#define _WX_XH_CHOIC_H_

#ifdef __GNUG__
#pragma interface "xh_choic.h"
#endif

#include "wx/xml/xmlres.h"

class wxChoiceXmlHandler : public wxXmlResourceHandler
{
    public:
        wxChoiceXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
    private:
        bool m_InsideBox;
        wxArrayString strList;
};


#endif // _WX_XH_CHOIC_H_
