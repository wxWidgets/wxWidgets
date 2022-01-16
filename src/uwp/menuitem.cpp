///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/menuitem.cpp
// Purpose:     wxMenuItem class
// Author:      Yann Clotioloman Yéo
// Modified by: 
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if wxUSE_MENUS

#include "wx/menuitem.h"

wxMenuItem::wxMenuItem(wxMenu* pParentMenu,
    int id,
    const wxString& text,
    const wxString& strHelp,
    wxItemKind kind,
    wxMenu* pSubMenu)
    : wxMenuItemBase(pParentMenu, id, text, strHelp, kind, pSubMenu)
{
}

wxMenuItem::~wxMenuItem()
{
}


#endif // wxUSE_MENUS
