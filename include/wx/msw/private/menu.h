///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/menu.h
// Purpose:     Structs used to custom draw and measure menu bar in wxMSW
// Author:      Vadim Zeitlin
// Created:     2025-01-24
// Copyright:   (c) 2025 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_MENU_H_
#define _WX_MSW_PRIVATE_MENU_H_

namespace wxMSWMenuImpl
{

// Definitions for undocumented messages and structs used in this code.
const int WM_MENUBAR_DRAWMENU = 0x91;
const int WM_MENUBAR_DRAWMENUITEM = 0x92;
const int WM_MENUBAR_MEASUREMENUITEM = 0x94;

// This is passed via LPARAM of WM_MENUBAR_DRAWMENU.
struct MenuBarDrawMenu
{
    HMENU hmenu;
    HDC hdc;
    DWORD dwReserved;
};

struct MenuBarMenuItem
{
    int iPosition;

    // There are more fields in this (undocumented) struct but we don't
    // currently need them, so don't bother with declaring them.
};

struct MenuBarDrawMenuItem
{
    DRAWITEMSTRUCT dis;
    MenuBarDrawMenu mbdm;
    MenuBarMenuItem mbmi;
};

struct MenuBarMeasureMenuItem
{
    MEASUREITEMSTRUCT mis;
    MenuBarDrawMenu mbdm;
    MenuBarMenuItem mbmi;
};

} // namespace wxMSWMenuImpl

#endif // _WX_MSW_PRIVATE_MENU_H_
