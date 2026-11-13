/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/menu.cpp
// Purpose:     Native menu bar support for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/window.h"
#endif

#include "wx/retro68/private/qd.h"

// Implemented in src/retro68/app.cpp.
extern void wxMacQuitApplication();

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

// IDs of the native menus.
enum
{
    wxMAC_APPLE_MENU_ID = 128,
    wxMAC_FILE_MENU_ID = 129
};

// Items of the Apple menu.
enum
{
    wxMAC_APPLE_ABOUT_ITEM = 1
    // the separator and desk accessories follow
};

// Items of the File menu.
enum
{
    wxMAC_FILE_QUIT_ITEM = 1
};

MenuHandle gs_appleMenu = nullptr;
MenuHandle gs_fileMenu = nullptr;

// Send the menu command event with the given ID to the active window or the
// application itself, return true if it was processed.
bool SendMenuCommand(int id)
{
    wxCommandEvent event(wxEVT_MENU, id);

    wxWindow* win = wxGetActiveWindow();
    if ( !win && wxTheApp )
        win = wxTheApp->GetTopWindow();

    if ( win )
    {
        event.SetEventObject(win);
        return win->ProcessWindowEvent(event);
    }

    return wxTheApp && wxTheApp->ProcessEvent(event);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

void wxMacSetupMenuBar()
{
    if ( gs_appleMenu )
        return;

    // The Apple menu, with the "About" item and the desk accessories.
    const unsigned char appleTitle[2] = { 1, 0x14 };
    gs_appleMenu = NewMenu(wxMAC_APPLE_MENU_ID, appleTitle);

    // Use a placeholder first and set the real text later to avoid the
    // interpretation of the meta-characters by AppendMenu().
    const unsigned char placeholder[2] = { 1, 'x' };
    AppendMenu(gs_appleMenu, placeholder);

    wxString about = _("About");
    if ( wxTheApp && !wxTheApp->GetAppDisplayName().empty() )
        about << ' ' << wxTheApp->GetAppDisplayName();
    about << wxString::FromUTF8("\xe2\x80\xa6");

    Str255 aboutText;
    wxMacStringToPascal(about, aboutText);
    SetMenuItemText(gs_appleMenu, wxMAC_APPLE_ABOUT_ITEM, aboutText);

    const unsigned char separator[3] = { 2, '(', '-' };
    AppendMenu(gs_appleMenu, separator);

    AppendResMenu(gs_appleMenu, 'DRVR');

    InsertMenu(gs_appleMenu, 0);

    // The File menu with just the Quit item: the application can also use
    // its own menus inside its windows (provided by wxUniversal).
    Str255 fileTitle;
    wxMacStringToPascal(_("File"), fileTitle);
    gs_fileMenu = NewMenu(wxMAC_FILE_MENU_ID, fileTitle);

    // "/Q" is the meta-character syntax for Command-Q shortcut.
    const unsigned char quitItem[] = { 7, 'Q', 'u', 'i', 't', ' ', '/', 'Q' };
    AppendMenu(gs_fileMenu, quitItem);

    Str255 quitText;
    wxMacStringToPascal(_("Quit"), quitText);
    SetMenuItemText(gs_fileMenu, wxMAC_FILE_QUIT_ITEM, quitText);

    InsertMenu(gs_fileMenu, 0);

    DrawMenuBar();
}

bool wxMacHandleMenuSelection(long menuResult)
{
    const short menuID = static_cast<short>((menuResult >> 16) & 0xffff);
    const short item = static_cast<short>(menuResult & 0xffff);

    switch ( menuID )
    {
        case wxMAC_APPLE_MENU_ID:
            if ( item == wxMAC_APPLE_ABOUT_ITEM )
            {
                SendMenuCommand(wxID_ABOUT);
            }
#if !TARGET_API_MAC_CARBON
            else
            {
                // Open the desk accessory.
                Str255 name;
                GetMenuItemText(gs_appleMenu, item, name);
                OpenDeskAcc(name);
            }
#endif // !TARGET_API_MAC_CARBON
            return true;

        case wxMAC_FILE_MENU_ID:
            if ( item == wxMAC_FILE_QUIT_ITEM )
            {
                // Let the application handle the command if it wants to,
                // otherwise just close all the windows.
                if ( !SendMenuCommand(wxID_EXIT) )
                    wxMacQuitApplication();
            }
            return true;
    }

    return false;
}
