/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/menu.mm
// Purpose:     wxMenu
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// wxWidgets headers
// -----------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/log.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/menuitem.h"
#include "wx/dialog.h"
#endif

#include "wx/menu.h"

#if wxUSE_MENUS

#include "wx/osx/private.h"

// other standard headers
// ----------------------
#include <string.h>

class wxMenuIPhoneImpl : public wxMenuImpl
{
public :
    wxMenuIPhoneImpl( wxMenu* peer , UIMenu* menu) : wxMenuImpl(peer), m_osxMenu(wxCFRetain(menu))
    {
    }

    virtual ~wxMenuIPhoneImpl();

    virtual void InsertOrAppend(wxMenuItem *pItem, size_t pos) override
    {
        UIMenuElement* uimenuitem = (UIMenuElement*) pItem->GetPeer()->GetHMenuItem();
        m_children.push_back(uimenuitem);
    }

    virtual void Remove( wxMenuItem *pItem ) override
    {
    }

    virtual void MakeRoot() override
    {
    }

    virtual void Enable( bool WXUNUSED(enable) )
    {
    }

    virtual void SetTitle( const wxString& text ) override
    {
    }

    virtual void PopUp( wxWindow *win, int x, int y ) override
    {
#if 0 //  TODO wxIOS
        UIView *view = win->GetPeer()->GetWXWidget();

        wxPoint screenPoint(x,y);
        NSPoint pointInView = wxToNSPoint(view, win->ScreenToClient( screenPoint ));

        // action and validation methods are not called from macos for modal dialogs
        // when using popUpMenuPositioningItem therefore we fall back to the older method
        if ( wxDialog::OSXHasModalDialogsOpen() )
        {
            // we don't want plug-ins interfering
            m_osxMenu.allowsContextMenuPlugIns = NO;

            wxTopLevelWindow* tlw = static_cast<wxTopLevelWindow*>(wxGetTopLevelParent(win));
            NSWindow* nsWindow = tlw->GetWXWindow();
            NSRect nsrect = NSZeroRect;
            nsrect.origin = wxToNSPoint( nullptr, screenPoint );
            nsrect = [nsWindow convertRectFromScreen:nsrect];

            NSEvent* rightClick = [NSEvent mouseEventWithType:NSRightMouseDown
                                                     location:nsrect.origin
                                                modifierFlags:0
                                                    timestamp:0
                                                 windowNumber:[nsWindow windowNumber]
                                                      context:nil
                                                  eventNumber:0
                                                   clickCount:1
                                                     pressure:0];

            [UIMenu popUpContextMenu:m_osxMenu withEvent:rightClick forView:view];
        }
        else
        {
            [m_osxMenu popUpMenuPositioningItem:nil atLocation:pointInView inView:view];
        }
#endif
    }

    virtual void GetMenuBarDimensions(int &x, int &y, int &width, int &height) const override
    {
#if 0 //  TODO wxIOS
        NSRect r = [(NSScreen*)[[NSScreen screens] objectAtIndex:0] frame];
        height = [m_osxMenu menuBarHeight];
        x = r.origin.x;
        y = r.origin.y;
        width = r.size.width;
#endif
    }

    void DisableAutoEnable()
    {
    }

    WXHMENU GetHMenu() override {
        if ( m_osxMenu == nil )
        {
            NSString* title = wxNSStringWithWxString(wxStripMenuCodes(m_peer->GetTitle(), wxStrip_Menu));

            wxCFMutableArrayRef<UIMenu*> groups;
            wxCFMutableArrayRef<UIMenuElement*> currentMenuElements;

            for ( int i = 0 ; i < m_children.size(); i++ )
            {
                UIMenuElement* element = m_children[i];
                if ( element.title.length > 0 )
                {
                    currentMenuElements.push_back(element);
                }
                else // separator, start a new menu group
                {
                    UIMenu* menu = [UIMenu menuWithTitle:@"" image:nil identifier:nil options:UIMenuOptionsDisplayInline children:currentMenuElements];
                    currentMenuElements.clear();
                    groups.push_back(menu);
                }
            }
            if ( !currentMenuElements.empty() )
            {
                UIMenu* menu = [UIMenu menuWithTitle:@"" image:nil identifier:nil options:UIMenuOptionsDisplayInline children:currentMenuElements];
                currentMenuElements.clear();
                groups.push_back(menu);
            }

            m_osxMenu.reset( [UIMenu menuWithTitle:title children:groups] );
        }
        return m_osxMenu;
    }

    static wxMenuImpl* Create( wxMenu* peer, const wxString& title );
    static wxMenuImpl* CreateRootMenu( wxMenu* peer );
protected :
    wxCFRef<UIMenu*> m_osxMenu;
    wxCFMutableArrayRef<UIMenuElement*> m_children;
} ;

wxMenuIPhoneImpl::~wxMenuIPhoneImpl()
{
}

wxMenuImpl* wxMenuImpl::Create( wxMenu* peer, const wxString& title )
{
    wxCFStringRef cfText( title );
    wxMenuImpl* c = new wxMenuIPhoneImpl( peer, nil );
    return c;
 }

#if wxUSE_MENUBAR
void wxMenuBar::OSXOnBuildMenu(WX_NSObject b)
{
    id<UIMenuBuilder> builder = (id<UIMenuBuilder>) b;

    UIMenuIdentifier lastmenuid = UIMenuApplication;

    [builder removeMenuForIdentifier:UIMenuFile];
    [builder removeMenuForIdentifier:UIMenuEdit];
    [builder removeMenuForIdentifier:UIMenuFormat];
    [builder removeMenuForIdentifier:UIMenuView];

    NSString* nsHelpMenuTitle = wxNSStringWithWxString(wxStripMenuCodes(wxApp::s_macHelpMenuTitleName, wxStrip_Menu));
    NSString* nsTranslatedHelpTitle = wxNSStringWithWxString(wxGETTEXT_IN_CONTEXT("macOS menu name", "Help"));

    NSString* nsWindowMenuTitle = wxNSStringWithWxString(wxStripMenuCodes(wxApp::s_macWindowMenuTitleName, wxStrip_Menu));
    NSString* nsTranslatedWindowMenuTitle = wxNSStringWithWxString(wxGETTEXT_IN_CONTEXT("macOS menu name", "Window"));


    for ( wxMenuItemList::compatibility_iterator node = m_rootMenu->GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        const wxMenuItem* const item = node->GetData();
        if ( item->IsSubMenu() )
        {
            UIMenu* menu = (UIMenu *)item->GetSubMenu()->GetHMenu();

            if ( item->GetSubMenu() == m_appleMenu )
            {
                // the default is quite resonable
                // TODO merge non desktop commands into the application menu, discard the rest
            }
            else if ([[menu title] isEqualToString:nsWindowMenuTitle] ||
            [[menu title] isEqualToString:nsTranslatedWindowMenuTitle])
            {
                [builder replaceMenuForIdentifier:UIMenuWindow withMenu:menu];
            }
            else if ([[menu title] isEqualToString:nsHelpMenuTitle] ||
                [[menu title] isEqualToString:nsTranslatedHelpTitle])
            {
                [builder replaceMenuForIdentifier:UIMenuHelp withMenu:menu];
            }
            else
            {
                [builder insertSiblingMenu:menu afterMenuForIdentifier:(UIMenuIdentifier) lastmenuid];
            }
            lastmenuid = menu.identifier;
        }
    }
}
#endif

#endif

