/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/mbarman.cpp
// Purpose:     wxMenuBarManager implementation
// Author:      David Elliott
// Modified by:
// Created:     2003/09/04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
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
#import <Foundation/NSNotification.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>

// ============================================================================
// wxMenuBarManagerObserver
// ============================================================================
@interface wxMenuBarManagerObserver : NSObject
{
    wxMenuBarManager *m_mbarman;
}

- (id)init;
- (id)initWithWxMenuBarManager: (wxMenuBarManager *)mbarman;
- (void)windowDidBecomeKey: (NSNotification *)notification;
#if 0
- (void)windowDidResignKey: (NSNotification *)notification;
- (void)windowDidBecomeMain: (NSNotification *)notification;
- (void)windowDidResignMain: (NSNotification *)notification;
- (void)windowWillClose: (NSNotification *)notification;
#endif // 0
@end // interface wxMenuBarManagerObserver : NSObject

@implementation wxMenuBarManagerObserver : NSObject
- (id)init
{
    wxFAIL_MSG(wxT("[wxMenuBarManagerObserver -init] should never be called!"));
    m_mbarman = NULL;
    return self;
}

- (id)initWithWxMenuBarManager: (wxMenuBarManager *)mbarman
{
    wxASSERT(mbarman);
    m_mbarman = mbarman;
    return [super init];
}

- (void)windowDidBecomeKey: (NSNotification *)notification
{
    wxASSERT(m_mbarman);
    m_mbarman->WindowDidBecomeKey(notification);
}

#if 0
- (void)windowDidResignKey: (NSNotification *)notification
{
    wxASSERT(m_mbarman);
    m_mbarman->WindowDidResignKey(notification);
}

- (void)windowDidBecomeMain: (NSNotification *)notification
{
    wxASSERT(m_mbarman);
    m_mbarman->WindowDidBecomeMain(notification);
}

- (void)windowDidResignMain: (NSNotification *)notification
{
    wxASSERT(m_mbarman);
    m_mbarman->WindowDidResignMain(notification);
}

- (void)windowWillClose: (NSNotification *)notification
{
    wxASSERT(m_mbarman);
    m_mbarman->WindowWillClose(notification);
}
#endif // 0

@end // implementation wxMenuBarManagerObserver : NSObject

// ============================================================================
// wxMenuBarManager
// ============================================================================
wxMenuBarManager *wxMenuBarManager::sm_mbarmanInstance = NULL;

wxMenuBarManager::wxMenuBarManager()
{
    m_observer = [[wxMenuBarManagerObserver alloc]
            initWithWxMenuBarManager:this];
    [[NSNotificationCenter defaultCenter] addObserver:m_observer
            selector:@selector(windowDidBecomeKey:)
            name:NSWindowDidBecomeKeyNotification object:nil];
#if 0
    [[NSNotificationCenter defaultCenter] addObserver:m_observer
            selector:@selector(windowDidResignKey:)
            name:NSWindowDidResignKeyNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:m_observer
            selector:@selector(windowDidBecomeMain:)
            name:NSWindowDidBecomeMainNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:m_observer
            selector:@selector(windowDidResignMain:)
            name:NSWindowDidResignMainNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:m_observer
            selector:@selector(windowWillClose:)
            name:NSWindowWillCloseNotification object:nil];
#endif // 0
    m_menuApp = nil;
    m_menuServices = nil;
    m_menuWindows = nil;
    m_menuMain = nil;
    m_mainMenuBarInstalled = true;
    m_mainMenuBar = NULL;
    m_currentNSWindow = nil;

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
/**/menuitem = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
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
    [m_observer release];
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

void wxMenuBarManager::SetMenuBar(wxMenuBar* menubar)
{
    m_mainMenuBarInstalled = false;
    if(menubar)
    {
        [[[wxTheApp->GetNSApplication() mainMenu] itemAtIndex:0] setSubmenu:nil];
        [[menubar->GetNSMenu() itemAtIndex:0] setSubmenu:m_menuApp];
        [wxTheApp->GetNSApplication() setMainMenu:menubar->GetNSMenu()];
    }
    else
        InstallMainMenu();
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
        m_mainMenuBarInstalled = true;
        [[[wxTheApp->GetNSApplication() mainMenu] itemAtIndex:0] setSubmenu:nil];
        [[m_menuMain itemAtIndex:0] setSubmenu:m_menuApp];
        [wxTheApp->GetNSApplication() setMainMenu:m_menuMain];
    }
}

void wxMenuBarManager::WindowDidBecomeKey(NSNotification *notification)
{
    /* NOTE: m_currentNSWindow might be destroyed but we only ever use it
       to look it up in the hash table.  Do not send messages to it. */
    m_currentNSWindow = [notification object];
    wxCocoaNSWindow *win = wxCocoaNSWindow::GetFromCocoa(m_currentNSWindow);
    if(win)
        InstallMenuBarForWindow(win);
    else
        SetMenuBar(NULL);
}

#if 0
void wxMenuBarManager::WindowDidResignKey(NSNotification *notification)
{
}

void wxMenuBarManager::WindowDidBecomeMain(NSNotification *notification)
{
}

void wxMenuBarManager::WindowDidResignMain(NSNotification *notification)
{
}

void wxMenuBarManager::WindowWillClose(NSNotification *notification)
{
}
#endif // 0

void wxMenuBarManager::InstallMenuBarForWindow(wxCocoaNSWindow *win)
{
    wxASSERT(win);
    wxMenuBar *menubar = win->GetAppMenuBar(win);
    wxLogTrace(wxTRACE_COCOA,wxT("Found menubar=%p for window=%p."),menubar,win);
    SetMenuBar(menubar);
}

void wxMenuBarManager::UpdateMenuBar()
{
    if(m_currentNSWindow)
    {
        wxCocoaNSWindow *win = wxCocoaNSWindow::GetFromCocoa(m_currentNSWindow);
        if(win)
            InstallMenuBarForWindow(win);
    }
}

#endif // wxUSE_MENUS
