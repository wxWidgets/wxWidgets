///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/menuitem.mm
// Purpose:     wxMenuItem implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: menuitem.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/stockitem.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

// a mapping from wx ids to standard osx actions in order to support the native menu item handling
// if a new mapping is added, make sure the wxNonOwnedWindowController has a handler for this action as well

struct Mapping
{
    int menuid;
    SEL action;
};

Mapping sActionToWXMapping[] =
{
    { wxID_UNDO, @selector(undo:) },
    { wxID_REDO, @selector(redo:) },
    { wxID_CUT, @selector(cut:) },
    { wxID_COPY, @selector(copy:) },
    { wxID_PASTE, @selector(paste:) },
    { wxID_CLEAR, @selector(delete:) },
    { wxID_SELECTALL, @selector(selectAll:) },
    { 0, NULL }
};

int wxOSXGetIdFromSelector(SEL action )
{
    int i = 0 ;
    while ( sActionToWXMapping[i].action != nil )
    {
        if ( sActionToWXMapping[i].action == action )
            return sActionToWXMapping[i].menuid;
        ++i;
    }
    
    return 0;
}

SEL wxOSXGetSelectorFromID(int menuId )
{
    int i = 0 ;
    while ( sActionToWXMapping[i].action != nil )
    {
        if ( sActionToWXMapping[i].menuid == menuId )
            return sActionToWXMapping[i].action;
        ++i;
    }
    
    return nil;
}


@implementation wxNSMenuItem

- (id) initWithTitle:(NSString *)aString action:(SEL)aSelector keyEquivalent:(NSString *)charCode
{
    [super initWithTitle:aString action:aSelector keyEquivalent:charCode];
     return self;
}

- (void) clickedAction: (id) sender
{
    wxUnusedVar(sender);
    if ( impl )
    {
        wxMenuItem* menuitem = impl->GetWXPeer();
        if ( menuitem->GetMenu()->HandleCommandProcess(menuitem) == false )
        {
        }
     }
}

- (void) setEnabled:(BOOL) flag
{
    [super setEnabled:flag];
}

- (BOOL)validateMenuItem:(NSMenuItem *) menuItem
{
    wxUnusedVar(menuItem);
    if( impl )
    {
        if ( impl->GetWXPeer()->GetMenu()->HandleCommandUpdateStatus(impl->GetWXPeer()) )
            return impl->GetWXPeer()->IsEnabled();
    }
    return YES ;
}

- (void)setImplementation: (wxMenuItemImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxMenuItemImpl*) implementation
{
    return impl;
}

@end

void wxMacCocoaMenuItemSetAccelerator( NSMenuItem* menuItem, wxAcceleratorEntry* entry )
{
    unsigned int modifiers = 0 ;
    int key = entry->GetKeyCode() ;
    if ( key )
    {
        if (entry->GetFlags() & wxACCEL_CTRL)
            modifiers |= NSCommandKeyMask;

        if (entry->GetFlags() & wxACCEL_ALT)
            modifiers |= NSAlternateKeyMask ;

        // this may be ignored later for alpha chars

        if (entry->GetFlags() & wxACCEL_SHIFT)
            modifiers |= NSShiftKeyMask ;

        unichar shortcut = 0;
        if ( key >= WXK_F1 && key <= WXK_F15 )
        {
            modifiers |= NSFunctionKeyMask ;
            shortcut = NSF1FunctionKey + ( key - WXK_F1 );
        }
        else
        {
            switch ( key )
            {
/*
                // standard function keys from here
                case WXK_TAB :
                    modifiers |= NSFunctionKeyMask ;
                    shortcut = NSTabCharacter ;
                    break ;

                case kEnterCharCode :
                    modifiers |= NSFunctionKeyMask ;
                    cocoaKey = NSTabCharacter ;
                    break ;

                case WXK_RETURN :
                    modifiers |= NSFunctionKeyMask ;
                    cocoaKey = NSTabCharacter ;
                    break ;

                case WXK_ESCAPE :
                    modifiers |= NSFunctionKeyMask ;
                    cocoaKey = kEscapeCharCode ;
                    break ;

                case WXK_SPACE :
                    shortcut = ' ' ;
                    break ;


                case WXK_CLEAR :
                    cocoaKey = kClearCharCode ;
                    break ;

                case WXK_PAGEUP :
                    cocoaKey = kPageUpCharCode ;
                    break ;

                case WXK_PAGEDOWN :
                    cocoaKey = kPageDownCharCode ;
                    break ;

                case WXK_LEFT :
                    cocoaKey = kLeftArrowCharCode ;
                    break ;

                case WXK_UP :
                    cocoaKey = kUpArrowCharCode ;
                    break ;

                case WXK_RIGHT :
                    cocoaKey = kRightArrowCharCode ;
                    break ;

                case WXK_DOWN :
                    cocoaKey = kDownArrowCharCode ;
                    break ;

                case WXK_HOME :
                    cocoaKey = kHomeCharCode ;
                    break ;

                case WXK_END :
                    cocoaKey = kEndCharCode ;
                    break ;
*/
                // TODO Test all above with their function key equiv.
                // from NSEvent.h
                default :
                    if(entry->GetFlags() & wxACCEL_SHIFT)
                        shortcut = toupper(key);
                    else
                        shortcut = tolower(key);
                    break ;
            }
        }

        [menuItem setKeyEquivalent:[NSString stringWithCharacters:&shortcut length:1]];
        [menuItem setKeyEquivalentModifierMask:modifiers];
    }
}

class wxMenuItemCocoaImpl : public wxMenuItemImpl
{
public :
    wxMenuItemCocoaImpl( wxMenuItem* peer, NSMenuItem* item ) : wxMenuItemImpl(peer), m_osxMenuItem(item)
    {
        if ( ![m_osxMenuItem isSeparatorItem] )
            [(wxNSMenuItem*)m_osxMenuItem setImplementation:this];
    }

    ~wxMenuItemCocoaImpl();

    void SetBitmap( const wxBitmap& bitmap )
    {
        [m_osxMenuItem setImage:bitmap.GetNSImage()];
    }

    void Enable( bool enable )
    {
        [m_osxMenuItem setEnabled:enable];
    }

    void Check( bool check )
    {
        [m_osxMenuItem setState:( check ?  NSOnState :  NSOffState) ];
    }

    void Hide( bool hide )
    {
        // NB: setHidden is new as of 10.5 so we should not call it below there
        if ([m_osxMenuItem respondsToSelector:@selector(setHidden:)])
            [m_osxMenuItem setHidden:hide ];
        else
            wxLogDebug("wxMenuItemCocoaImpl::Hide not yet supported under OS X < 10.5");
    }

    void SetLabel( const wxString& text, wxAcceleratorEntry *entry )
    {
        wxCFStringRef cfText(text);
        [m_osxMenuItem setTitle:cfText.AsNSString()];

        if ( entry )
            wxMacCocoaMenuItemSetAccelerator( m_osxMenuItem, entry );

    }
    
    bool DoDefault();

    void * GetHMenuItem() { return m_osxMenuItem; }

protected :
    NSMenuItem* m_osxMenuItem ;
} ;

wxMenuItemCocoaImpl::~wxMenuItemCocoaImpl()
{
    if ( ![m_osxMenuItem isSeparatorItem] )
        [(wxNSMenuItem*)m_osxMenuItem setImplementation:nil];
    [m_osxMenuItem release];
}

bool wxMenuItemCocoaImpl::DoDefault()
{
    bool handled=false;
    int menuid = m_peer->GetId();
    
    NSApplication *theNSApplication = [NSApplication sharedApplication];
    if (menuid == wxID_OSX_HIDE)
    {
        [theNSApplication hide:nil];
        handled=true;
    }
    else if (menuid == wxID_OSX_HIDEOTHERS)
    {
        [theNSApplication hideOtherApplications:nil];
        handled=true;
    }
    else if (menuid == wxID_OSX_SHOWALL)
    {
        [theNSApplication unhideAllApplications:nil];
        handled=true;
    }
    return handled;
}

wxMenuItemImpl* wxMenuItemImpl::Create( wxMenuItem* peer, wxMenu *pParentMenu,
                       int menuid,
                       const wxString& text,
                       wxAcceleratorEntry *entry,
                       const wxString& WXUNUSED(strHelp),
                       wxItemKind kind,
                       wxMenu *pSubMenu )
{
    wxMenuItemImpl* c = NULL;
    NSMenuItem* item = nil;

    if ( kind == wxITEM_SEPARATOR )
    {
        item = [[NSMenuItem separatorItem] retain];
    }
    else
    {
        wxCFStringRef cfText(text);
        SEL selector = nil;
        bool targetSelf = false;
        if ( ! pParentMenu->GetNoEventsMode() && pSubMenu == NULL )
        {
            selector = wxOSXGetSelectorFromID(menuid);
            
            if ( selector == nil )
            {
                selector = @selector(clickedAction:);
                targetSelf = true;
            }
        }
        
        wxNSMenuItem* menuitem = [ [ wxNSMenuItem alloc ] initWithTitle:cfText.AsNSString() action:selector keyEquivalent:@""];
        if ( targetSelf )
            [menuitem setTarget:menuitem];
        
        if ( pSubMenu )
        {
            pSubMenu->GetPeer()->SetTitle( text );
            [menuitem setSubmenu:pSubMenu->GetHMenu()];
        }
        else
        {
            if ( entry )
                wxMacCocoaMenuItemSetAccelerator( menuitem, entry );
        }
        item = menuitem;
    }
    c = new wxMenuItemCocoaImpl( peer, item );
    return c;
}
