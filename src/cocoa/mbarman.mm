/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/mbarman.cpp
// Purpose:     wxMenuBarManager implementation
// Author:      David Elliott
// Modified by:
// Created:     2003/09/04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_MENUS
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/menu.h"
    #include "wx/toplevel.h"
#endif // WX_PRECOMP

#include "wx/cocoa/mbarman.h"
#include "wx/cocoa/autorelease.h"

#import <Foundation/NSString.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSApplication.h>

// ============================================================================
// wxMenuBarManager
// ============================================================================
wxMenuBarManager *wxMenuBarManager::sm_mbarmanInstance = NULL;

wxMenuBarManager::wxMenuBarManager()
{
    m_menuApp = nil;
    m_menuServices = nil;
    m_menuWindows = nil;
    m_menuMain = nil;
    m_needMenuBar = true;
    m_mainMenuBarInstalled = true;
    m_mainMenuBar = NULL;
    m_windowKey = NULL;
    m_windowMain = NULL;

    NSApplication *theNSApplication = wxTheApp->GetNSApplication();
    // Create the services menu.
    m_menuServices = [[NSMenu alloc] initWithTitle: @"Services"];
    [theNSApplication setServicesMenu:m_menuServices];

    NSMenuItem *menuitem;
    // Create the application (Apple) menu.
    m_menuApp = [[NSMenu alloc] initWithTitle: @"Apple Menu"];

/**/[m_menuApp addItemWithTitle:@"Preferences..." action:nil keyEquivalent:@""];
/**/[m_menuApp addItem: [NSMenuItem separatorItem]];
/**/menuitem = [[NSMenuItem alloc] initWithTitle: @"Services" action:nil keyEquivalent:@""];
    [menuitem setSubmenu:m_menuServices];
    [m_menuApp addItem: menuitem];
    [menuitem release];
/**/[m_menuApp addItem: [NSMenuItem separatorItem]];
/**/menuitem = [[NSMenuItem alloc] initWithTitle:@"Hide" action:@selector(hide:) keyEquivalent:@""];
    [menuitem setTarget: theNSApplication];
    [m_menuApp addItem: menuitem];
    [menuitem release];
/**/menuitem = [[NSMenuItem alloc] initWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@""];
    [menuitem setTarget: theNSApplication];
    [m_menuApp addItem: menuitem];
    [menuitem release];
/**/menuitem = [[NSMenuItem alloc] initWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [menuitem setTarget: theNSApplication];
    [m_menuApp addItem: menuitem];
    [menuitem release];
/**/[m_menuApp addItem: [NSMenuItem separatorItem]];
/**/menuitem = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"Q"];
    [menuitem setTarget: theNSApplication];
    [m_menuApp addItem: menuitem];
    [menuitem release];

    [theNSApplication setAppleMenu:m_menuApp];

    // Create the Windows menu
    m_menuWindows = [[NSMenu alloc] initWithTitle: @"Window"];

/**/[m_menuWindows addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@""];
/**/[m_menuWindows addItem: [NSMenuItem separatorItem]];
/**/[m_menuWindows addItemWithTitle:@"Bring All to Front" action:@selector(arrangeInFront:) keyEquivalent:@""];

    [theNSApplication setWindowsMenu:m_menuWindows];

    // Create the main menubar
    m_menuMain = [[NSMenu alloc] initWithTitle: @"wxApp Menu"];
/**/NSMenuItem *dummyItem = [[NSMenuItem alloc] initWithTitle:@"App menu"
        /* Note: title gets clobbered by app name anyway */
        action:nil keyEquivalent:@""];
    [dummyItem setSubmenu:m_menuApp];
    [m_menuMain addItem:dummyItem];
    [dummyItem release];
/**/dummyItem = [[NSMenuItem alloc] initWithTitle:@"Window"
        action:nil keyEquivalent:@""];
    [dummyItem setSubmenu:m_menuWindows];
    [m_menuMain addItem:dummyItem];
    [dummyItem release];

    [theNSApplication setMainMenu: m_menuMain];

}

wxMenuBarManager::~wxMenuBarManager()
{
}

void wxMenuBarManager::CreateInstance()
{
    sm_mbarmanInstance = new wxMenuBarManager;
}

void wxMenuBarManager::DestroyInstance()
{
    delete sm_mbarmanInstance;
    sm_mbarmanInstance = NULL;
}

void wxMenuBarManager::CocoaInternalIdle()
{
    if(m_needMenuBar)
        InstallMainMenu();
}

void wxMenuBarManager::SetMenuBar(wxMenuBar* menubar)
{
    m_mainMenuBarInstalled = false;
    m_needMenuBar = !menubar;
    if(menubar)
    {
        [[[wxTheApp->GetNSApplication() mainMenu] itemAtIndex:0] setSubmenu:nil];
        [[menubar->GetNSMenu() itemAtIndex:0] setSubmenu:m_menuApp];
        [wxTheApp->GetNSApplication() setMainMenu:menubar->GetNSMenu()];
    }
}

void wxMenuBarManager::SetMainMenuBar(wxMenuBar* menubar)
{
    m_mainMenuBar = menubar;
    if(m_mainMenuBarInstalled)
        InstallMainMenu();
}

void wxMenuBarManager::InstallMainMenu()
{
    if(m_mainMenuBar)
        SetMenuBar(m_mainMenuBar);
    else
    {
        m_needMenuBar = false;
        m_mainMenuBarInstalled = true;
        [[[wxTheApp->GetNSApplication() mainMenu] itemAtIndex:0] setSubmenu:nil];
        [[m_menuMain itemAtIndex:0] setSubmenu:m_menuApp];
        [wxTheApp->GetNSApplication() setMainMenu:m_menuMain];
    }
}

void wxMenuBarManager::WindowDidBecomeKey(wxTopLevelWindowNative *win)
{
    wxASSERT(!m_windowKey);
    m_windowKey = win;
    InstallMenuBarForWindow(win);
}

void wxMenuBarManager::WindowDidResignKey(wxTopLevelWindowNative *win, bool uninstallMenuBar)
{
    wxASSERT(m_windowKey==win);
    m_windowKey = NULL;
    if(uninstallMenuBar)
        SetMenuBar(NULL);
}

void wxMenuBarManager::WindowDidBecomeMain(wxTopLevelWindowNative *win)
{
    wxASSERT(!m_windowMain);
    m_windowMain = win;
}

void wxMenuBarManager::WindowDidResignMain(wxTopLevelWindowNative *win)
{
    wxASSERT(m_windowMain==win);
    m_windowMain = NULL;
}

void wxMenuBarManager::InstallMenuBarForWindow(wxTopLevelWindowNative *win)
{
    wxMenuBar *menubar = NULL;
    for(wxTopLevelWindowNative *destwin = win;
        !menubar && destwin;
        destwin = wxDynamicCast(destwin->GetParent(), wxTopLevelWindow))
    {
        menubar = destwin->GetAppMenuBar();
    }
    SetMenuBar(menubar);
}

void wxMenuBarManager::UpdateWindowMenuBar(wxTopLevelWindowNative *win)
{
    InstallMenuBarForWindow(m_windowKey);
}

#endif // wxUSE_MENUS
