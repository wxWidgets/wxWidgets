/////////////////////////////////////////////////////////////////////////////
// Name:        xh_menu.h
// Purpose:     XML resource handler for menus/menubars
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_MENU_H_
#define _WX_XH_MENU_H_

#ifdef __GNUG__
#pragma interface "xh_menu.h"
#endif

#include "wx/xml/xmlres.h"

class WXDLLEXPORT wxMenuXmlHandler : public wxXmlResourceHandler
{
    public:
        wxMenuXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
        
    private:
        bool m_InsideMenu;
};

class WXDLLEXPORT wxMenuBarXmlHandler : public wxXmlResourceHandler
{
    public:
        wxMenuBarXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_MENU_H_
