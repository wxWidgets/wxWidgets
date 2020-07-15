///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/menuitem.mm
// Purpose:     wxMenuItem implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
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
// as we don't have NSUndoManager support we must not use the native actions
#if 0
    { wxID_UNDO, @selector(undo:) },
    { wxID_REDO, @selector(redo:) },
#endif
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

/*
void wxMacCocoaMenuItemSetAccelerator( UIMenuItem* menuItem, wxAcceleratorEntry* entry )
{
    if ( entry == NULL )
    {
        [menuItem setKeyEquivalent:@""];
        return;
    }

#if 0 //  TODO wxIOS wxUSE_ACCEL
    unsigned int modifiers = 0 ;
    int key = entry->GetKeyCode() ;
    if ( key )
    {
        if (entry->GetFlags() & wxACCEL_CTRL)
            modifiers |= NSCommandKeyMask;

        if (entry->GetFlags() & wxACCEL_RAW_CTRL)
            modifiers |= NSControlKeyMask;

        if (entry->GetFlags() & wxACCEL_ALT)
            modifiers |= NSAlternateKeyMask ;

        // this may be ignored later for alpha chars

        if (entry->GetFlags() & wxACCEL_SHIFT)
            modifiers |= NSShiftKeyMask ;

        unichar shortcut = 0;
        if ( key >= WXK_F1 && key <= WXK_F15 )
        {
            shortcut = NSF1FunctionKey + ( key - WXK_F1 );
        }
        else
        {
            switch ( key )
            {
                case WXK_CLEAR :
                    shortcut = NSDeleteCharacter ;
                    break ;

                case WXK_PAGEUP :
                    shortcut = NSPageUpFunctionKey ;
                    break ;

                case WXK_PAGEDOWN :
                    shortcut = NSPageDownFunctionKey ;
                    break ;

                case WXK_NUMPAD_LEFT :
                    modifiers |= NSNumericPadKeyMask;
                    wxFALLTHROUGH;
                case WXK_LEFT :
                    shortcut = NSLeftArrowFunctionKey ;
                    break ;

                case WXK_NUMPAD_UP :
                    modifiers |= NSNumericPadKeyMask;
                    wxFALLTHROUGH;
                case WXK_UP :
                    shortcut = NSUpArrowFunctionKey ;
                    break ;

                case WXK_NUMPAD_RIGHT :
                    modifiers |= NSNumericPadKeyMask;
                    wxFALLTHROUGH;
                case WXK_RIGHT :
                    shortcut = NSRightArrowFunctionKey ;
                    break ;

                case WXK_NUMPAD_DOWN :
                    modifiers |= NSNumericPadKeyMask;
                    wxFALLTHROUGH;
                case WXK_DOWN :
                    shortcut = NSDownArrowFunctionKey ;
                    break ;

                case WXK_HOME :
                    shortcut = NSHomeFunctionKey ;
                    break ;

                case WXK_END :
                    shortcut = NSEndFunctionKey ;
                    break ;

                case WXK_NUMPAD_ENTER :
                    shortcut = NSEnterCharacter;
                    break;

                case WXK_BACK :
                case WXK_RETURN :
                case WXK_TAB :
                case WXK_ESCAPE :
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
#endif // wxUSE_ACCEL
}

*/

class wxMenuItemCocoaImpl : public wxMenuItemImpl
{
public :
    wxMenuItemCocoaImpl( wxMenuItem* peer, UIMenuElement* item ) : wxMenuItemImpl(peer), m_osxMenuItem(wxCFRetain(item))
    {
    }

    ~wxMenuItemCocoaImpl();

    void SetBitmap( const wxBitmap& bitmap ) wxOVERRIDE
    {
    }

    void Enable( bool enable ) wxOVERRIDE
    {
        UIMenuElement* menuElement = m_osxMenuItem.get();
        if ( [menuElement isKindOfClass:UIAction.class] )
        {
            UIAction* action = (UIAction*) menuElement;
            if ( enable )
                action.attributes = action.attributes & ~UIMenuElementAttributesDisabled;
            else
                action.attributes = action.attributes | UIMenuElementAttributesDisabled;
        }
    }

    void Check( bool check ) wxOVERRIDE
    {
        UIMenuElement* menuElement = m_osxMenuItem.get();
        if ( [menuElement isKindOfClass:UIAction.class] )
        {
            UIAction* action = (UIAction*) menuElement;
            action.state = check ? UIMenuElementStateOn : UIMenuElementStateOff;
        }
    }

    void Hide( bool hide ) wxOVERRIDE
    {
        UIMenuElement* menuElement = m_osxMenuItem.get();
        if ( [menuElement isKindOfClass:UIAction.class] )
        {
            UIAction* action = (UIAction*) menuElement;
            if ( hide )
                action.attributes = action.attributes | UIMenuElementAttributesHidden;
            else
                action.attributes = action.attributes & ~UIMenuElementAttributesHidden;
        }
    }

    void SetLabel( const wxString& text, wxAcceleratorEntry *entry ) wxOVERRIDE
    {
        // recreate, it's readonly
    }

    bool DoDefault() wxOVERRIDE;

    void * GetHMenuItem() wxOVERRIDE { return m_osxMenuItem; }

protected :
    wxCFRef<UIMenuElement*> m_osxMenuItem ;
} ;

wxMenuItemCocoaImpl::~wxMenuItemCocoaImpl()
{
}

bool wxMenuItemCocoaImpl::DoDefault()
{
    bool handled=false;
    int menuid = m_peer->GetId();

#if 0 //  TODO wxIOS
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
    else if (menuid == wxApp::s_macExitMenuItemId)
    {
        wxTheApp->ExitMainLoop();
    }
#endif
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
    UIMenuElement* item = nil;

    if ( kind == wxITEM_SEPARATOR )
    {
        // TODO new version of item = [[UIMenuItem separatorItem] retain];
    }
    else
    {
        wxCFStringRef cfText(text);

        if ( pSubMenu )
        {
            pSubMenu->GetPeer()->SetTitle( text );
            UIMenu* submenu = pSubMenu->GetHMenu();
            item = submenu;
        }
        else
        {
            // currently in the iOS 14 Beta UICommands trigger a layout violation so stick with UIActions for the time being
            if ( entry )
            {
                //  TODO UIKeyCommand* command = [UIKeyCommand commandWithTitle ...]

                UIAction* menuitem = [UIAction actionWithTitle:cfText.AsNSString() image:nil identifier:nil
                                                   handler: ^( UIAction* action) { peer->GetMenu()->HandleCommandProcess(peer);} ];
                item = menuitem;
            }
            else
            {
                /*
                UICommand* command = [UICommand commandWithTitle:cfText.AsNSString() image:nil action:@selector(menuItemAction:) propertyList:nil];
                 */

                UIAction* menuitem = [UIAction actionWithTitle:cfText.AsNSString() image:nil identifier:nil
                                                   handler: ^( UIAction* action) { peer->GetMenu()->HandleCommandProcess(peer);} ];
                item = menuitem;
            }
        }
    }
    c = new wxMenuItemCocoaImpl( peer, item );
    return c;
}
