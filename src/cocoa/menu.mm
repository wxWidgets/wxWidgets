/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/menu.cpp
// Purpose:     wxMenu and wxMenuBar implementation
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#import <Foundation/NSString.h>
#import <AppKit/NSMenu.h>

#if wxUSE_MENUS

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// ============================================================================
// wxMenu implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

bool wxMenu::Create(const wxString& title, long style)
{
#if 0
    if(!title)
        return CocoaCreate("wxMenu");
#endif
    return CocoaCreate(title);
}

wxMenu::~wxMenu()
{
}

bool wxMenu::DoAppend(wxMenuItem *item)
{
    if(!wxMenuBase::DoAppend(item))
        return false;
    [m_cocoaNSMenu addItem: item->GetNSMenuItem()];
    return true;
}

bool wxMenu::DoInsert(unsigned long pos, wxMenuItem *item)
{
    if(!wxMenuBase::DoInsert(pos,item))
        return false;
    [m_cocoaNSMenu insertItem:item->GetNSMenuItem() atIndex:pos];
    return true;
}

wxMenuItem* wxMenu::DoRemove(wxMenuItem *item)
{
    wxMenuItem *retitem = wxMenuBase::DoRemove(item);
    wxASSERT(retitem->GetNSMenuItem());
    [m_cocoaNSMenu removeItem:retitem->GetNSMenuItem()];
    return retitem;
}

// ============================================================================
// wxMenuBar implementation
// ============================================================================
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

bool wxMenuBar::Create(long style)
{
    if(!CocoaCreate("wxMenuBar"))
        return false;
    return true;
}

wxMenuBar::~wxMenuBar()
{
}

bool wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
    wxLogDebug("append menu=%p, title=%s",menu,title.c_str());
    if(!wxMenuBarBase::Append(menu,title))
        return false;
    wxASSERT(menu);
    wxASSERT(menu->GetNSMenu());
    NSString *menuTitle = [[NSString alloc] initWithCString: wxStripMenuCodes(title).c_str()];
    NSMenuItem *newItem = [[NSMenuItem alloc] initWithTitle:menuTitle action:NULL keyEquivalent:@""];
    [menu->GetNSMenu() setTitle:menuTitle];
    [newItem setSubmenu:menu->GetNSMenu()];

    [m_cocoaNSMenu addItem:newItem];

    [menuTitle release];
    [newItem release];
    return true;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    wxLogDebug("insert pos=%lu, menu=%p, title=%s",pos,menu,title.c_str());
    if(!wxMenuBarBase::Insert(pos,menu,title))
        return false;
    wxASSERT(menu);
    wxASSERT(menu->GetNSMenu());
    NSString *menuTitle = [[NSString alloc] initWithCString: title.c_str()];
    NSMenuItem *newItem = [[NSMenuItem alloc] initWithTitle:menuTitle action:NULL keyEquivalent:@""];
    [menu->GetNSMenu() setTitle:menuTitle];
    [newItem setSubmenu:menu->GetNSMenu()];

    [m_cocoaNSMenu insertItem:newItem atIndex:pos];

    [menuTitle release];
    [newItem release];
    return true;
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    return NULL;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    return NULL;
}


void wxMenuBar::EnableTop(size_t pos, bool enable)
{
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    return false;
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    return wxEmptyString;
}

void wxMenuBar::Attach(wxFrame *frame)
{
}

void wxMenuBar::Detach()
{
}

wxSize wxMenuBar::DoGetBestClientSize() const
{
    return wxDefaultSize;
}

#endif // wxUSE_MENUS
