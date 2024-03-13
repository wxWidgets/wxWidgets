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
/*
@implementation wxUIMenu

- (id) initWithTitle:(NSString*) title
{
    if ( self = [super initWithTitle:title] )
    {
        impl = nullptr;
    }
    return self;
}

- (void)setImplementation: (wxMenuImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxMenuImpl*) implementation
{
    return impl;
}

@end
*/
// this is more compatible, as it is also called for command-key shortcuts
// and under 10.4, we are not getting a 'close' event however...
#define wxOSX_USE_NEEDSUPDATE_HOOK 1

@interface wxUIMenuController : NSObject //  TODO wxIOS <UIMenuDelegate>
{
}

#if wxOSX_USE_NEEDSUPDATE_HOOK
- (void)menuNeedsUpdate:(UIMenu*)smenu;
#else
- (void)menuWillOpen:(UIMenu *)menu;
#endif
- (void)menuDidClose:(UIMenu *)menu;
- (void)menu:(UIMenu *)menu willHighlightItem:(UIMenuItem *)item;

@end

@implementation wxUIMenuController

- (id) init
{
    self = [super init];
    return self;
}

/*
#if wxOSX_USE_NEEDSUPDATE_HOOK
- (void)menuNeedsUpdate:(UIMenu*)smenu
{
    wxUIMenu* menu = (wxUIMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMenuOpened();
    }
}
#else
- (void)menuWillOpen:(UIMenu *)smenu
{
    wxUIMenu* menu = (wxUIMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMenuOpened();
    }
}
#endif

- (void)menuDidClose:(UIMenu *)smenu
{
    wxUIMenu* menu = (wxUIMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMenuClosed();
    }
}

- (void)menu:(UIMenu *)smenu willHighlightItem:(UIMenuItem *)item
{
    wxUIMenu* menu = (wxUIMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenuItem* menuitem = nullptr;
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();

        if ( [ item isKindOfClass:[wxUIMenuItem class] ] )
        {
            wxMenuItemImpl* menuitemimpl = (wxMenuItemImpl*) [ (wxUIMenuItem*) item implementation ];
            if ( menuitemimpl )
            {
                menuitem = menuitemimpl->GetWXPeer();
            }
        }

        if ( wxpeer )
        {
            wxpeer->HandleMenuItemHighlighted( menuitem );
        }
    }
}
*/

@end

class wxMenuCocoaImpl : public wxMenuImpl
{
public :
    wxMenuCocoaImpl( wxMenu* peer , UIMenu* menu) : wxMenuImpl(peer), m_osxMenu(wxCFRetain(menu))
    {
        /*
        static wxUIMenuController* controller = nullptr;
        if ( controller == nullptr )
        {
            controller = [[wxUIMenuController alloc] init];
        }
        [menu setDelegate:controller];
        [m_osxMenu setImplementation:this];
        // gc aware
        if ( m_osxMenu )
            CFRetain(m_osxMenu);
        [m_osxMenu release];
         */
    }

    virtual ~wxMenuCocoaImpl();

    virtual void InsertOrAppend(wxMenuItem *pItem, size_t pos) override
    {
        UIMenuElement* uimenuitem = (UIMenuElement*) pItem->GetPeer()->GetHMenuItem();
        m_children.push_back(uimenuitem);
        /*
        // make sure a call of SetSubMenu is also reflected (occurring after Create)
        // update the native menu item accordingly

        if ( pItem->IsSubMenu() )
        {
            wxMenu* wxsubmenu = pItem->GetSubMenu();
            WXHMENU nssubmenu = wxsubmenu->GetHMenu();
            if ( [nsmenuitem submenu] != nssubmenu )
            {
                wxsubmenu->GetPeer()->SetTitle( pItem->GetItemLabelText() );
                [nsmenuitem setSubmenu:nssubmenu];
            }
        }

        if ( pos == (size_t) -1 )
            [m_osxMenu addItem:nsmenuitem ];
        else
            [m_osxMenu insertItem:nsmenuitem atIndex:pos];
        */
    }

    virtual void Remove( wxMenuItem *pItem ) override
    {
        // [m_osxMenu removeItem:(UIMenuItem*) pItem->GetPeer()->GetHMenuItem()];
    }

    virtual void MakeRoot() override
    {
    }

    virtual void Enable( bool WXUNUSED(enable) )
    {
    }

    virtual void SetTitle( const wxString& text ) override
    {
     //   wxCFStringRef cfText(text);
     //   [m_osxMenu setTitle:cfText.AsNSString()];
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
        /*
        [m_osxMenu setAutoenablesItems:NO];

        wxMenu* menu = GetWXPeer();
        for ( wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();
              node;
              node = node->GetNext() )
        {
            const wxMenuItem* const item = node->GetData();
            if ( item->IsSubMenu() )
            {
                wxMenuCocoaImpl* subimpl = dynamic_cast<wxMenuCocoaImpl*>(item->GetSubMenu()->GetPeer());
                if ( subimpl )
                    subimpl->DisableAutoEnable();
            }
        }
*/
    }

    WXHMENU GetHMenu() override {
        if ( m_osxMenu == nil )
        {
            m_osxMenu.reset( [UIMenu menuWithTitle:wxCFStringRef(m_peer->GetTitle()).AsNSString() children:m_children] );
        }
        return m_osxMenu;
    }

    static wxMenuImpl* Create( wxMenu* peer, const wxString& title );
    static wxMenuImpl* CreateRootMenu( wxMenu* peer );
protected :
    wxCFRef<UIMenu*> m_osxMenu;
    wxCFMutableArrayRef<UIMenuElement*> m_children;

/*
 UIMenu* menu = [[UIMenu menuWithTitle:cfText.AsNSString() children:];
*/

} ;

wxMenuCocoaImpl::~wxMenuCocoaImpl()
{
    // [m_osxMenu setDelegate:nil];
    // [m_osxMenu setImplementation:nil];
    // gc aware
}

wxMenuImpl* wxMenuImpl::Create( wxMenu* peer, const wxString& title )
{
    wxCFStringRef cfText( title );
    wxMenuImpl* c = new wxMenuCocoaImpl( peer, nil );
    return c;
 }

#endif
