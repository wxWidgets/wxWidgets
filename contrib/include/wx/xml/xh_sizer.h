/////////////////////////////////////////////////////////////////////////////
// Name:        xh_sizer.h
// Purpose:     XML resource handler for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/04/24
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SIZER_H_
#define _WX_XH_SIZER_H_

#ifdef __GNUG__
#pragma interface "xh_sizer.h"
#endif

#include "wx/xml/xmlres.h"


class wxSizer;

class wxSizerXmlHandler : public wxXmlResourceHandler
{
    public:
        wxSizerXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
        
    private:
        bool m_IsInside;
        wxSizer *m_ParentSizer;
};


#endif // _WX_XH_BOXSIZER_H_
