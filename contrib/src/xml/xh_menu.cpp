/////////////////////////////////////////////////////////////////////////////
// Name:        xh_menu.cpp
// Purpose:     XML resource for menus and menubars
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_menu.h"
#include "wx/menu.h"


wxMenuXmlHandler::wxMenuXmlHandler() : 
        wxXmlResourceHandler(), m_InsideMenu(FALSE)
{
    ADD_STYLE(wxMENU_TEAROFF);
}



wxObject *wxMenuXmlHandler::DoCreateResource()
{
    if (m_Node->GetName() == _T("menu"))
    {
        wxMenu *menu = new wxMenu(GetStyle());
        wxString title = GetText(_T("label"));
        wxString help = GetText(_T("help"));
    
        bool oldins = m_InsideMenu;
        m_InsideMenu = TRUE;
        CreateChildren(menu, TRUE/*only this handler*/);
        m_InsideMenu = oldins;

        wxMenuBar *p_bar = wxDynamicCast(m_Parent, wxMenuBar);
        if (p_bar)
            p_bar->Append(menu, title);
        else
        {
            wxMenu *p_menu = wxDynamicCast(m_Parent, wxMenu);
            if (p_menu)
                p_menu->Append(GetID(), title, menu, help);
        }

        return menu;
    }

    else
    {
        wxMenu *p_menu = wxDynamicCast(m_Parent, wxMenu);
        
        if (m_Node->GetName() == _T("separator"))
            p_menu->AppendSeparator();
        else if (m_Node->GetName() == _T("break"))
            p_menu->Break();
        else /*menuitem*/
        {
            int id = GetID();
            bool checkable = GetBool(_T("checkable"));

            p_menu->Append(id, GetText(_T("label")), 
                           GetText(_T("help")), checkable);
            if (id != -1)
            {
                p_menu->Enable(id, GetBool(_T("enabled"), TRUE));
                if (checkable) p_menu->Check(id, GetBool(_T("checked")));
                
#if wxCHECK_VERSION(2,3,0) || defined(__WXMSW__)
                if (HasParam(_T("bitmap")))
                {
                    wxMenuItem *mitem = p_menu->FindItem(id);
                    mitem->SetBitmap(GetBitmap(_T("bitmap")));
                }
#endif
            }
        }
        return NULL;
    }
}



bool wxMenuXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("menu") ||
           (m_InsideMenu && 
               (node->GetName() == _T("menuitem") ||
                node->GetName() == _T("break") ||
                node->GetName() == _T("separator"))
           );
}











wxMenuBarXmlHandler::wxMenuBarXmlHandler() : wxXmlResourceHandler()
{
    ADD_STYLE(wxMB_DOCKABLE);
}



wxObject *wxMenuBarXmlHandler::DoCreateResource()
{
    wxMenuBar *menubar = new wxMenuBar(GetStyle());
    CreateChildren(menubar);
    return menubar;
}



bool wxMenuBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("menubar");
}

