/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "menu.h"
    #pragma implementation "menuitem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menu.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/app.h"
#include "wx/bitmap.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "wx/gtk/private.h"

#include <gdk/gdkkeysyms.h>

// FIXME: is this right? somehow I don't think so (VZ)
#ifdef __WXGTK20__
    #include <glib-object.h>

    #define gtk_accel_group_attach(g, o) gtk_window_add_accel_group((o), (g))
    #define gtk_accel_group_detach(g, o) gtk_window_remove_accel_group((o), (g))
    #define gtk_menu_ensure_uline_accel_group(m) gtk_menu_get_accel_group(m)

    #define ACCEL_OBJECT        GtkWindow
    #define ACCEL_OBJECTS(a)    (a)->acceleratables
    #define ACCEL_OBJ_CAST(obj) ((GtkWindow*) obj)
#else // GTK+ 1.x
    #define ACCEL_OBJECT        GtkObject
    #define ACCEL_OBJECTS(a)    (a)->attach_objects
    #define ACCEL_OBJ_CAST(obj) GTK_OBJECT(obj)
#endif

// we use normal item but with a special id for the menu title
static const int wxGTK_TITLE_ID = -3;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

#if wxUSE_ACCEL
static wxString GetHotKey( const wxMenuItem& item );
#endif

//-----------------------------------------------------------------------------
// substitute for missing GtkPixmapMenuItem
//-----------------------------------------------------------------------------

#ifndef __WXGTK20__

#define GTK_TYPE_PIXMAP_MENU_ITEM            (gtk_pixmap_menu_item_get_type ())
#define GTK_PIXMAP_MENU_ITEM(obj)            (GTK_CHECK_CAST ((obj), GTK_TYPE_PIXMAP_MENU_ITEM, GtkPixmapMenuItem))
#define GTK_PIXMAP_MENU_ITEM_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_PIXMAP_MENU_ITEM, GtkPixmapMenuItemClass))
#define GTK_IS_PIXMAP_MENU_ITEM(obj)         (GTK_CHECK_TYPE ((obj), GTK_TYPE_PIXMAP_MENU_ITEM))
#define GTK_IS_PIXMAP_MENU_ITEM_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_PIXMAP_MENU_ITEM))
//#define GTK_PIXMAP_MENU_ITEM_GET_CLASS(obj)  (GTK_CHECK_GET_CLASS ((obj), GTK_TYPE_PIXMAP_MENU_ITEM))
#define GTK_PIXMAP_MENU_ITEM_GET_CLASS(obj)  (GTK_PIXMAP_MENU_ITEM_CLASS( GTK_OBJECT_GET_CLASS(obj)))

#ifndef GTK_MENU_ITEM_GET_CLASS
#define GTK_MENU_ITEM_GET_CLASS(obj) (GTK_MENU_ITEM_CLASS( GTK_OBJECT_GET_CLASS(obj)))
#endif

typedef struct _GtkPixmapMenuItem       GtkPixmapMenuItem;
typedef struct _GtkPixmapMenuItemClass  GtkPixmapMenuItemClass;

struct _GtkPixmapMenuItem
{
    GtkMenuItem menu_item;

    GtkWidget *pixmap;
};

struct _GtkPixmapMenuItemClass
{
    GtkMenuItemClass parent_class;

    guint orig_toggle_size;
    guint have_pixmap_count;
};


GtkType    gtk_pixmap_menu_item_get_type       (void);
GtkWidget* gtk_pixmap_menu_item_new            (void);
void       gtk_pixmap_menu_item_set_pixmap     (GtkPixmapMenuItem *menu_item,
                                                                    GtkWidget *pixmap);
#endif // GTK 2.0

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

static wxString wxReplaceUnderscore( const wxString& title )
{
    const wxChar *pc;

    // GTK 1.2 wants to have "_" instead of "&" for accelerators
    wxString str;
    pc = title;
    while (*pc != wxT('\0'))
    {
        if ((*pc == wxT('&')) && (*(pc+1) == wxT('&')))
        {
            // "&" is doubled to indicate "&" instead of accelerator
            ++pc;
            str << wxT('&');
        }
        else if (*pc == wxT('&'))
        {
            str << wxT('_');
        }
        else
        {
            if ( *pc == wxT('_') )
            {
                // underscores must be doubled to prevent them from being
                // interpreted as accelerator character prefix by GTK
                str << *pc;
            }

            str << *pc;
        }
        ++pc;
    }

    // wxPrintf( wxT("before %s after %s\n"), title.c_str(), str.c_str() );

    return str;
}

//-----------------------------------------------------------------------------
// activate message from GTK
//-----------------------------------------------------------------------------

static void gtk_menu_open_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle) wxapp_install_idle_handler();

    wxMenuEvent event( wxEVT_MENU_OPEN, -1, menu );
    event.SetEventObject( menu );

    wxEvtHandler* handler = menu->GetEventHandler();
    if (handler && handler->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win) win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar( long style )
{
    // the parent window is known after wxFrame::SetMenu()
    m_needParent = FALSE;
    m_style = style;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menubar = gtk_menu_bar_new();
#ifndef __WXGTK20__
    m_accel = gtk_accel_group_new();
#endif

    if (style & wxMB_DOCKABLE)
    {
        m_widget = gtk_handle_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_menubar) );
        gtk_widget_show( GTK_WIDGET(m_menubar) );
    }
    else
    {
        m_widget = GTK_WIDGET(m_menubar);
    }

    PostCreation();

    ApplyWidgetStyle();
}

wxMenuBar::wxMenuBar()
{
    // the parent window is known after wxFrame::SetMenu()
    m_needParent = FALSE;
    m_style = 0;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menubar = gtk_menu_bar_new();
#ifndef __WXGTK20__
    m_accel = gtk_accel_group_new();
#endif

    m_widget = GTK_WIDGET(m_menubar);

    PostCreation();

    ApplyWidgetStyle();
}

wxMenuBar::~wxMenuBar()
{
}

static void wxMenubarUnsetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( (wxWindow*) NULL );

    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

#ifndef __WXGTK20__
    // support for native hot keys
    gtk_accel_group_detach( menu->m_accel, ACCEL_OBJ_CAST(top_frame->m_widget) );
#endif

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarUnsetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->GetNext();
    }
}

static void wxMenubarSetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( win );

    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    // support for native hot keys
    ACCEL_OBJECT *obj = ACCEL_OBJ_CAST(top_frame->m_widget);
    if ( !g_slist_find( ACCEL_OBJECTS(menu->m_accel), obj ) )
        gtk_accel_group_attach( menu->m_accel, obj );

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarSetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->GetNext();
    }
}

void wxMenuBar::SetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = win;
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

#ifndef __WXGTK20__
    // support for native key accelerators indicated by underscroes
    ACCEL_OBJECT *obj = ACCEL_OBJ_CAST(top_frame->m_widget);
    if ( !g_slist_find( ACCEL_OBJECTS(m_accel), obj ) )
        gtk_accel_group_attach( m_accel, obj );
#endif

    wxMenuList::compatibility_iterator node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        wxMenubarSetInvokingWindow( menu, win );
        node = node->GetNext();
    }
}

void wxMenuBar::UnsetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = (wxWindow*) NULL;
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

#ifndef __WXGTK20__
    // support for native key accelerators indicated by underscroes
    gtk_accel_group_detach( m_accel, ACCEL_OBJ_CAST(top_frame->m_widget) );
#endif

    wxMenuList::compatibility_iterator node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        wxMenubarUnsetInvokingWindow( menu, win );
        node = node->GetNext();
    }
}

bool wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
    if ( !wxMenuBarBase::Append( menu, title ) )
        return FALSE;

    return GtkAppend(menu, title);
}

bool wxMenuBar::GtkAppend(wxMenu *menu, const wxString& title, int pos)
{
    wxString str( wxReplaceUnderscore( title ) );

    // This doesn't have much effect right now.
    menu->SetTitle( str );

    // The "m_owner" is the "menu item"
#ifdef __WXGTK20__
    menu->m_owner = gtk_menu_item_new_with_mnemonic( wxGTK_CONV( str ) );
#else
    menu->m_owner = gtk_menu_item_new_with_label( wxGTK_CONV( str ) );
    GtkLabel *label = GTK_LABEL( GTK_BIN(menu->m_owner)->child );
    // set new text
    gtk_label_set_text( label, wxGTK_CONV( str ) );
    // reparse key accel
    guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( str ) );
    if (accel_key != GDK_VoidSymbol)
    {
        gtk_widget_add_accelerator (menu->m_owner,
                                       "activate_item",
                                        m_accel,//gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menubar)),
                                        accel_key,
                                        GDK_MOD1_MASK,
                                        GTK_ACCEL_LOCKED);
    }
#endif

    gtk_widget_show( menu->m_owner );

    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );

    if (pos == -1)
        gtk_menu_shell_append( GTK_MENU_SHELL(m_menubar), menu->m_owner );
    else
        gtk_menu_shell_insert( GTK_MENU_SHELL(m_menubar), menu->m_owner, pos );

    gtk_signal_connect( GTK_OBJECT(menu->m_owner), "activate",
                        GTK_SIGNAL_FUNC(gtk_menu_open_callback),
                        (gpointer)menu );

    // m_invokingWindow is set after wxFrame::SetMenuBar(). This call enables
    // addings menu later on.
    if (m_invokingWindow)
    {
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

            // OPTIMISE ME:  we should probably cache this, or pass it
            //               directly, but for now this is a minimal
            //               change to validate the new dynamic sizing.
            //               see (and refactor :) similar code in Remove
            //               below.

        wxFrame *frame = wxDynamicCast( m_invokingWindow, wxFrame );

        if( frame )
            frame->UpdateMenuBarSize();
    }

    return TRUE;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    // TODO

    if ( !GtkAppend(menu, title, (int)pos) )
        return FALSE;

    return TRUE;
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    // remove the old item and insert a new one
    wxMenu *menuOld = Remove(pos);
    if ( menuOld && !Insert(pos, menu, title) )
    {
        return (wxMenu*) NULL;
    }

    // either Insert() succeeded or Remove() failed and menuOld is NULL
    return menuOld;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return (wxMenu*) NULL;

    gtk_menu_item_remove_submenu( GTK_MENU_ITEM(menu->m_owner) );
    gtk_container_remove(GTK_CONTAINER(m_menubar), menu->m_owner);

    gtk_widget_destroy( menu->m_owner );
    menu->m_owner = NULL;

    if (m_invokingWindow)
    {
        // OPTIMISE ME:  see comment in GtkAppend
        wxFrame *frame = wxDynamicCast( m_invokingWindow, wxFrame );

        if( frame )
            frame->UpdateMenuBarSize();
    }

    return menu;
}

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
    if (wxMenuItem::GetLabelFromText(menu->GetTitle()) == wxMenuItem::GetLabelFromText(menuString))
    {
        int res = menu->FindItem( itemString );
        if (res != wxNOT_FOUND)
            return res;
    }

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *item = node->GetData();
        if (item->IsSubMenu())
            return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);

        node = node->GetNext();
    }

    return wxNOT_FOUND;
}

int wxMenuBar::FindMenuItem( const wxString &menuString, const wxString &itemString ) const
{
    wxMenuList::compatibility_iterator node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        int res = FindMenuItemRecursive( menu, menuString, itemString);
        if (res != -1)
            return res;
        node = node->GetNext();
    }

    return wxNOT_FOUND;
}

// Find a wxMenuItem using its id. Recurses down into sub-menus
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
    wxMenuItem* result = menu->FindChildItem(id);

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();
    while ( node && result == NULL )
    {
        wxMenuItem *item = node->GetData();
        if (item->IsSubMenu())
        {
            result = FindMenuItemByIdRecursive( item->GetSubMenu(), id );
        }
        node = node->GetNext();
    }

    return result;
}

wxMenuItem* wxMenuBar::FindItem( int id, wxMenu **menuForItem ) const
{
    wxMenuItem* result = 0;
    wxMenuList::compatibility_iterator node = m_menus.GetFirst();
    while (node && result == 0)
    {
        wxMenu *menu = node->GetData();
        result = FindMenuItemByIdRecursive( menu, id );
        node = node->GetNext();
    }

    if ( menuForItem )
    {
        *menuForItem = result ? result->GetMenu() : (wxMenu *)NULL;
    }

    return result;
}

void wxMenuBar::EnableTop( size_t pos, bool flag )
{
    wxMenuList::compatibility_iterator node = m_menus.Item( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = node->GetData();

    if (menu->m_owner)
        gtk_widget_set_sensitive( menu->m_owner, flag );
}

wxString wxMenuBar::GetLabelTop( size_t pos ) const
{
    wxMenuList::compatibility_iterator node = m_menus.Item( pos );

    wxCHECK_MSG( node, wxT("invalid"), wxT("menu not found") );

    wxMenu* menu = node->GetData();

    wxString label;
    wxString text( menu->GetTitle() );
    for ( const wxChar *pc = text.c_str(); *pc; pc++ )
    {
        if ( *pc == wxT('_') )
        {
            // '_' is the escape character for GTK+
            continue;
        }

        // don't remove ampersands '&' since if we have them in the menu title
        // it means that they were doubled to indicate "&" instead of accelerator

        label += *pc;
    }

    return label;
}

void wxMenuBar::SetLabelTop( size_t pos, const wxString& label )
{
    wxMenuList::compatibility_iterator node = m_menus.Item( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = node->GetData();

    wxString str( wxReplaceUnderscore( label ) );

    menu->SetTitle( str );

    if (menu->m_owner)
    {
        GtkLabel *label = GTK_LABEL( GTK_BIN(menu->m_owner)->child );

        /* set new text */
        gtk_label_set( label, wxGTK_CONV( str ) );

        /* reparse key accel */
        (void)gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( str ) );
        gtk_accel_label_refetch( GTK_ACCEL_LABEL(label) );
    }

}

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_menu_clicked_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    int id = menu->FindMenuIdByMenuItem(widget);

    /* should find it for normal (not popup) menu */
    wxASSERT_MSG( (id != -1) || (menu->GetInvokingWindow() != NULL),
                  _T("menu item not found in gtk_menu_clicked_callback") );

    if (!menu->IsEnabled(id))
        return;

    wxMenuItem* item = menu->FindChildItem( id );
    wxCHECK_RET( item, wxT("error in menu item callback") );

    if ( item->GetId() == wxGTK_TITLE_ID )
    {
        // ignore events from the menu title
        return;
    }

    if (item->IsCheckable())
    {
        bool isReallyChecked = item->IsChecked(),
            isInternallyChecked = item->wxMenuItemBase::IsChecked();

        // ensure that the internal state is always consistent with what is
        // shown on the screen
        item->wxMenuItemBase::Check(isReallyChecked);

        // we must not report the events for the radio button going up nor the
        // events resulting from the calls to wxMenuItem::Check()
        if ( (item->GetKind() == wxITEM_RADIO && !isReallyChecked) ||
             (isInternallyChecked == isReallyChecked) )
        {
            return;
        }
    }


    // Is this menu on a menubar?  (possibly nested)
    wxFrame* frame = NULL;
    if(menu->IsAttached())
        frame = menu->GetMenuBar()->GetFrame();

    // FIXME: why do we have to call wxFrame::GetEventHandler() directly here?
    //        normally wxMenu::SendEvent() should be enough, if it doesn't work
    //        in wxGTK then we have a bug in wxMenu::GetInvokingWindow() which
    //        should be fixed instead of working around it here...
    if (frame)
    {
        // If it is attached then let the frame send the event.
        // Don't call frame->ProcessCommand(id) because it toggles
        // checkable items and we've already done that above.
        wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
        commandEvent.SetEventObject(frame);
        if (item->IsCheckable())
            commandEvent.SetInt(item->IsChecked());
        commandEvent.SetEventObject(menu);

        frame->GetEventHandler()->ProcessEvent(commandEvent);
    }
    else
    {
        // otherwise let the menu have it
        menu->SendEvent(id, item->IsCheckable() ? item->IsChecked() : -1);
    }
}

//-----------------------------------------------------------------------------
// "select"
//-----------------------------------------------------------------------------

static void gtk_menu_hilight_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle) wxapp_install_idle_handler();

    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id))
        return;

    wxMenuEvent event( wxEVT_MENU_HIGHLIGHT, id );
    event.SetEventObject( menu );

    wxEvtHandler* handler = menu->GetEventHandler();
    if (handler && handler->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win) win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "deselect"
//-----------------------------------------------------------------------------

static void gtk_menu_nolight_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle) wxapp_install_idle_handler();

    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id))
        return;

    wxMenuEvent event( wxEVT_MENU_HIGHLIGHT, -1 );
    event.SetEventObject( menu );

    wxEvtHandler* handler = menu->GetEventHandler();
    if (handler && handler->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win)
        win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                wxItemKind kind,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       wxItemKind kind,
                       wxMenu *subMenu)
          : wxMenuItemBase(parentMenu, id, text, help, kind, subMenu)
{
    Init(text);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
          : wxMenuItemBase(parentMenu, id, text, help,
                           isCheckable ? wxITEM_CHECK : wxITEM_NORMAL, subMenu)
{
    Init(text);
}

void wxMenuItem::Init(const wxString& text)
{
    m_labelWidget = (GtkWidget *) NULL;
    m_menuItem = (GtkWidget *) NULL;

    DoSetText(text);
}

wxMenuItem::~wxMenuItem()
{
   // don't delete menu items, the menus take care of that
}

// return the menu item text without any menu accels
/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    wxString label;

    for ( const wxChar *pc = text.c_str(); *pc; pc++ )
    {
        if ( *pc == wxT('\t'))
            break;

        if ( *pc == wxT('_') )
        {
            // GTK 1.2 escapes "xxx_xxx" to "xxx__xxx"
            pc++;
            label += *pc;
            continue;
        }

#ifdef __WXGTK20__
        if ( *pc == wxT('\\')  )
        {
            // GTK 2.0 escapes "xxx/xxx" to "xxx\/xxx"
            pc++;
            label += *pc;
            continue;
        }
#endif

        if ( (*pc == wxT('&')) && (*(pc+1) != wxT('&')) )
        {
            // wxMSW escapes "&"
            // "&" is doubled to indicate "&" instead of accelerator
            continue;
        }

        label += *pc;
    }

    // wxPrintf( wxT("GetLabelFromText(): text %s label %s\n"), text.c_str(), label.c_str() );

    return label;
}

void wxMenuItem::SetText( const wxString& str )
{
    // Some optimization to avoid flicker
    wxString oldLabel = m_text;
    oldLabel = wxStripMenuCodes(oldLabel.BeforeFirst('\t'));
    oldLabel.Replace(wxT("_"), wxT(""));
    wxString label1 = wxStripMenuCodes(str.BeforeFirst('\t'));
    if (oldLabel == label1)
        return;

    DoSetText(str);

    if (m_menuItem)
    {
        GtkLabel *label;
        if (m_labelWidget)
            label = (GtkLabel*) m_labelWidget;
        else
            label = GTK_LABEL( GTK_BIN(m_menuItem)->child );

#ifdef __WXGTK20__
        gtk_label_set_text_with_mnemonic( GTK_LABEL(label), wxGTK_CONV(m_text) );
#else
        // set new text
        gtk_label_set( label, wxGTK_CONV( m_text ) );

        // reparse key accel
        (void)gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV(m_text) );
        gtk_accel_label_refetch( GTK_ACCEL_LABEL(label) );
#endif
    }
}

// it's valid for this function to be called even if m_menuItem == NULL
void wxMenuItem::DoSetText( const wxString& str )
{
    // '\t' is the deliminator indicating a hot key
    m_text.Empty();
    const wxChar *pc = str;
    while ( (*pc != wxT('\0')) && (*pc != wxT('\t')) )
    {
        if ((*pc == wxT('&')) && (*(pc+1) == wxT('&')))
        {
            // "&" is doubled to indicate "&" instead of accelerator
            ++pc;
            m_text << wxT('&');
        }
        else if (*pc == wxT('&'))
        {
            m_text << wxT('_');
        }
        else if ( *pc == wxT('_') )    // escape underscores
        {
            m_text << wxT("__");
        }
        else
        {
            m_text << *pc;
        }
        ++pc;
    }

    // wxPrintf( wxT("DoSetText(): str %s m_text %s\n"), str.c_str(), m_text.c_str() );

    m_hotKey = wxT("");

    if(*pc == wxT('\t'))
    {
       pc++;
       m_hotKey = pc;
    }
}

#if wxUSE_ACCEL

wxAcceleratorEntry *wxMenuItem::GetAccel() const
{
    if ( !GetHotKey() )
    {
        // nothing
        return (wxAcceleratorEntry *)NULL;
    }

    // as wxGetAccelFromString() looks for TAB, insert a dummy one here
    wxString label;
    label << wxT('\t') << GetHotKey();

    return wxGetAccelFromString(label);
}

#endif // wxUSE_ACCEL

void wxMenuItem::Check( bool check )
{
    wxCHECK_RET( m_menuItem, wxT("invalid menu item") );

    if (check == m_isChecked)
        return;

    wxMenuItemBase::Check( check );

    switch ( GetKind() )
    {
        case wxITEM_CHECK:
        case wxITEM_RADIO:
            gtk_check_menu_item_set_state( (GtkCheckMenuItem*)m_menuItem, (gint)check );
            break;

        default:
            wxFAIL_MSG( _T("can't check this item") );
    }
}

void wxMenuItem::Enable( bool enable )
{
    wxCHECK_RET( m_menuItem, wxT("invalid menu item") );

    gtk_widget_set_sensitive( m_menuItem, enable );
    wxMenuItemBase::Enable( enable );
}

bool wxMenuItem::IsChecked() const
{
    wxCHECK_MSG( m_menuItem, FALSE, wxT("invalid menu item") );

    wxCHECK_MSG( IsCheckable(), FALSE,
                 wxT("can't get state of uncheckable item!") );

    return ((GtkCheckMenuItem*)m_menuItem)->active != 0;
}

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

void wxMenu::Init()
{
    m_accel = gtk_accel_group_new();
    m_menu = gtk_menu_new();
    // NB: keep reference to the menu so that it is not destroyed behind
    //     our back by GTK+ e.g. when it is removed from menubar:
    gtk_widget_ref(m_menu);

    m_owner = (GtkWidget*) NULL;

    // Tearoffs are entries, just like separators. So if we want this
    // menu to be a tear-off one, we just append a tearoff entry
    // immediately.
    if ( m_style & wxMENU_TEAROFF )
    {
        GtkWidget *tearoff = gtk_tearoff_menu_item_new();

        gtk_menu_append(GTK_MENU(m_menu), tearoff);
    }

    m_prevRadio = NULL;

    // append the title as the very first entry if we have it
    if ( !m_title.empty() )
    {
        Append(wxGTK_TITLE_ID, m_title);
        AppendSeparator();
    }
}

wxMenu::~wxMenu()
{
   WX_CLEAR_LIST(wxMenuItemList, m_items);

   if ( GTK_IS_WIDGET( m_menu ))
   {
       // see wxMenu::Init
       gtk_widget_unref( m_menu ); 
       // if the menu is inserted in another menu at this time, there was
       // one more reference to it:
       if ( m_owner )
           gtk_widget_destroy( m_menu );
   }
}

bool wxMenu::GtkAppend(wxMenuItem *mitem, int pos)
{
    GtkWidget *menuItem;

    if ( mitem->IsSeparator() )
    {
#ifdef __WXGTK20__
        menuItem = gtk_separator_menu_item_new();
#else
        // TODO
        menuItem = gtk_menu_item_new();
#endif
        if (pos == -1)
            gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), menuItem);
        else
            gtk_menu_shell_insert(GTK_MENU_SHELL(m_menu), menuItem, pos);
    }
    else if ( mitem->IsSubMenu() )
    {
        // text has "_" instead of "&" after mitem->SetText()
        wxString text( mitem->GetText() );

#ifdef __WXGTK20__
        menuItem = gtk_menu_item_new_with_mnemonic( wxGTK_CONV( text ) );
#else
        menuItem = gtk_menu_item_new_with_label( wxGTK_CONV( text ) );
        GtkLabel *label = GTK_LABEL( GTK_BIN(menuItem)->child );
        // set new text
        gtk_label_set_text( label, wxGTK_CONV( text ) );
        // reparse key accel
        guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
        if (accel_key != GDK_VoidSymbol)
        {
            gtk_widget_add_accelerator (menuItem,
                                        "activate_item",
                                        gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menu)),
                                        accel_key,
                                        GDK_MOD1_MASK,
                                        GTK_ACCEL_LOCKED);
        }
#endif

        gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), mitem->GetSubMenu()->m_menu );
        if (pos == -1)
            gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), menuItem);
        else
            gtk_menu_shell_insert(GTK_MENU_SHELL(m_menu), menuItem, pos);

        gtk_widget_show( mitem->GetSubMenu()->m_menu );

        // if adding a submenu to a menu already existing in the menu bar, we
        // must set invoking window to allow processing events from this
        // submenu
        if ( m_invokingWindow )
            wxMenubarSetInvokingWindow(mitem->GetSubMenu(), m_invokingWindow);

        m_prevRadio = NULL;
    }
    else if (mitem->GetBitmap().Ok())
    {
        wxString text = mitem->GetText();
        const wxBitmap *bitmap = &mitem->GetBitmap();

#ifdef __WXGTK20__
        menuItem = gtk_image_menu_item_new_with_mnemonic( wxGTK_CONV( text ) );

        GtkWidget *image;
        if (bitmap->HasPixbuf())
        {
            image = gtk_image_new_from_pixbuf(bitmap->GetPixbuf());
        }
        else
        {
            GdkPixmap *gdk_pixmap = bitmap->GetPixmap();
            GdkBitmap *gdk_bitmap = bitmap->GetMask() ? 
                                        bitmap->GetMask()->GetBitmap() :
                                        (GdkBitmap*) NULL;
            image = gtk_image_new_from_pixmap( gdk_pixmap, gdk_bitmap );
        }
        
        gtk_widget_show(image);

        gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(menuItem), image );

        gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                            GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                            (gpointer)this );

        if (pos == -1)
            gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), menuItem);
        else
            gtk_menu_shell_insert(GTK_MENU_SHELL(m_menu), menuItem, pos);
#else
        GdkPixmap *gdk_pixmap = bitmap->GetPixmap();
        GdkBitmap *gdk_bitmap = bitmap->GetMask() ? bitmap->GetMask()->GetBitmap() : (GdkBitmap*) NULL;

        menuItem = gtk_pixmap_menu_item_new ();
        GtkWidget *label = gtk_accel_label_new ( wxGTK_CONV( text ) );
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_container_add (GTK_CONTAINER (menuItem), label);

        gtk_accel_label_set_accel_widget (GTK_ACCEL_LABEL (label), menuItem);
        guint accel_key;
        GdkModifierType accel_mods;

        // accelerator for the item, as specified by its label
        // (ex. Ctrl+O for open)
        gtk_accelerator_parse(GetHotKey(*mitem).c_str(), &accel_key,
                              &accel_mods);
        if (accel_key != GDK_VoidSymbol)
        {
            gtk_widget_add_accelerator (menuItem,
                                        "activate_item",
                                        m_accel,
                                        accel_key, accel_mods,
                                        GTK_ACCEL_VISIBLE);
        }

        // accelerator for the underlined char (ex ALT+F for the File menu)
        accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
        if (accel_key != GDK_VoidSymbol)
        {
            gtk_widget_add_accelerator (menuItem,
                                        "activate_item",
                                        gtk_menu_ensure_uline_accel_group(GTK_MENU (m_menu)),
                                        accel_key,
                                        GDK_MOD1_MASK,
                                        GTK_ACCEL_LOCKED);
        }

        gtk_widget_show (label);

        mitem->SetLabelWidget(label);

        GtkWidget* pixmap = gtk_pixmap_new( gdk_pixmap, gdk_bitmap );
        gtk_widget_show(pixmap);
        gtk_pixmap_menu_item_set_pixmap(GTK_PIXMAP_MENU_ITEM( menuItem ), pixmap);

        gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                            GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                            (gpointer)this );

        if (pos == -1)
            gtk_menu_append( GTK_MENU(m_menu), menuItem );
        else
            gtk_menu_insert( GTK_MENU(m_menu), menuItem, pos );
        gtk_widget_show( menuItem );
#endif

        m_prevRadio = NULL;
    }
    else // a normal item
    {
        // text has "_" instead of "&" after mitem->SetText() so don't use it
        wxString text( mitem->GetText() );

        switch ( mitem->GetKind() )
        {
            case wxITEM_CHECK:
            {
#ifdef __WXGTK20__
                menuItem = gtk_check_menu_item_new_with_mnemonic( wxGTK_CONV( text ) );
#else
                menuItem = gtk_check_menu_item_new_with_label( wxGTK_CONV( text ) );
                GtkLabel *label = GTK_LABEL( GTK_BIN(menuItem)->child );
                // set new text
                gtk_label_set_text( label, wxGTK_CONV( text ) );
                // reparse key accel
                guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
                if (accel_key != GDK_VoidSymbol)
                {
                    gtk_widget_add_accelerator (menuItem,
                                                "activate_item",
                                                gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menu)),
                                                accel_key,
                                                GDK_MOD1_MASK,
                                                GTK_ACCEL_LOCKED);
                }
#endif
                m_prevRadio = NULL;
                break;
            }

            case wxITEM_RADIO:
            {
                GSList *group = NULL;
                if ( m_prevRadio == NULL )
                {
                    // start of a new radio group
#ifdef __WXGTK20__
                    m_prevRadio = menuItem = gtk_radio_menu_item_new_with_mnemonic( group, wxGTK_CONV( text ) );
#else
                    m_prevRadio = menuItem = gtk_radio_menu_item_new_with_label( group, wxGTK_CONV( text ) );
                    GtkLabel *label = GTK_LABEL( GTK_BIN(menuItem)->child );
                    // set new text
                    gtk_label_set_text( label, wxGTK_CONV( text ) );
                    // reparse key accel
                    guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
                    if (accel_key != GDK_VoidSymbol)
                    {
                        gtk_widget_add_accelerator (menuItem,
                                                    "activate_item",
                                                    gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menu)),
                                                    accel_key,
                                                    GDK_MOD1_MASK,
                                                    GTK_ACCEL_LOCKED);
                    }
#endif
                }
                else // continue the radio group
                {
#ifdef __WXGTK20__
                    group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (m_prevRadio));
                    m_prevRadio = menuItem = gtk_radio_menu_item_new_with_mnemonic( group, wxGTK_CONV( text ) );
#else
                    group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (m_prevRadio));
                    m_prevRadio = menuItem = gtk_radio_menu_item_new_with_label( group, wxGTK_CONV( text ) );
                    GtkLabel *label = GTK_LABEL( GTK_BIN(menuItem)->child );
                    // set new text
                    gtk_label_set_text( label, wxGTK_CONV( text ) );
                    // reparse key accel
                    guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
                    if (accel_key != GDK_VoidSymbol)
                    {
                        gtk_widget_add_accelerator (menuItem,
                                                    "activate_item",
                                                    gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menu)),
                                                    accel_key,
                                                    GDK_MOD1_MASK,
                                                    GTK_ACCEL_LOCKED);
                    }
#endif
                }
                break;
            }

            default:
                wxFAIL_MSG( _T("unexpected menu item kind") );
                // fall through

            case wxITEM_NORMAL:
            {
#ifdef __WXGTK20__
                menuItem = gtk_menu_item_new_with_mnemonic( wxGTK_CONV( text ) );
#else
                menuItem = gtk_menu_item_new_with_label( wxGTK_CONV( text ) );
                GtkLabel *label = GTK_LABEL( GTK_BIN(menuItem)->child );
                // set new text
                gtk_label_set_text( label, wxGTK_CONV( text ) );
                // reparse key accel
                guint accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
                if (accel_key != GDK_VoidSymbol)
                {
                    gtk_widget_add_accelerator (menuItem,
                                                    "activate_item",
                                                    gtk_menu_ensure_uline_accel_group(GTK_MENU(m_menu)),
                                                    accel_key,
                                                    GDK_MOD1_MASK,
                                                    GTK_ACCEL_LOCKED);
                }
#endif
                m_prevRadio = NULL;
                break;
            }
        }

        gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                            GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                            (gpointer)this );

        if (pos == -1)
            gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), menuItem);
        else
            gtk_menu_shell_insert(GTK_MENU_SHELL(m_menu), menuItem, pos);
    }

    guint accel_key;
    GdkModifierType accel_mods;
    wxCharBuffer buf = wxGTK_CONV( GetHotKey(*mitem) );

    // wxPrintf( wxT("item: %s hotkey %s\n"), mitem->GetText().c_str(), GetHotKey(*mitem).c_str() );

    gtk_accelerator_parse( (const char*) buf, &accel_key, &accel_mods);
    if (accel_key != 0)
    {
        gtk_widget_add_accelerator (GTK_WIDGET(menuItem),
                                    "activate",
                                    m_accel,
                                    accel_key,
                                    accel_mods,
                                    GTK_ACCEL_VISIBLE);
    }

    gtk_widget_show( menuItem );

    if ( !mitem->IsSeparator() )
    {
        wxASSERT_MSG( menuItem, wxT("invalid menuitem") );

        gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                            GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                            (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                            GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                            (gpointer)this );
    }

    mitem->SetMenuItem(menuItem);

    if (ms_locked)
    {
        // This doesn't even exist!
        // gtk_widget_lock_accelerators(mitem->GetMenuItem());
    }

    return TRUE;
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *mitem)
{
    if (!GtkAppend(mitem))
        return NULL;

    return wxMenuBase::DoAppend(mitem);
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return NULL;

    // TODO
    if ( !GtkAppend(item, (int)pos) )
        return NULL;

    return item;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    if ( !wxMenuBase::DoRemove(item) )
        return (wxMenuItem *)NULL;

    // TODO: this code doesn't delete the item factory item and this seems
    //       impossible as of GTK 1.2.6.
    gtk_widget_destroy( item->GetMenuItem() );

    return item;
}

int wxMenu::FindMenuIdByMenuItem( GtkWidget *menuItem ) const
{
    wxMenuItemList::compatibility_iterator node = m_items.GetFirst();
    while (node)
    {
        wxMenuItem *item = node->GetData();
        if (item->GetMenuItem() == menuItem)
           return item->GetId();
        node = node->GetNext();
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if wxUSE_ACCEL

static wxString GetHotKey( const wxMenuItem& item )
{
    wxString hotkey;

    wxAcceleratorEntry *accel = item.GetAccel();
    if ( accel )
    {
        int flags = accel->GetFlags();
        if ( flags & wxACCEL_ALT )
            hotkey += wxT("<alt>");
        if ( flags & wxACCEL_CTRL )
            hotkey += wxT("<control>");
        if ( flags & wxACCEL_SHIFT )
            hotkey += wxT("<shift>");

        int code = accel->GetKeyCode();
        switch ( code )
        {
            case WXK_F1:
            case WXK_F2:
            case WXK_F3:
            case WXK_F4:
            case WXK_F5:
            case WXK_F6:
            case WXK_F7:
            case WXK_F8:
            case WXK_F9:
            case WXK_F10:
            case WXK_F11:
            case WXK_F12:
                hotkey += wxString::Format(wxT("F%d"), code - WXK_F1 + 1);
                break;

                // TODO: we should use gdk_keyval_name() (a.k.a.
                //       XKeysymToString) here as well as hardcoding the keysym
                //       names this might be not portable
            case WXK_NUMPAD_INSERT:
                hotkey << wxT("KP_Insert" );
                break;
            case WXK_NUMPAD_DELETE:
                hotkey << wxT("KP_Delete" );
                break;
            case WXK_INSERT:
                hotkey << wxT("Insert" );
                break;
            case WXK_DELETE:
                hotkey << wxT("Delete" );
                break;
            case WXK_UP:
                hotkey << wxT("Up" );
                break;
            case WXK_DOWN:
                hotkey << wxT("Down" );
                break;
            case WXK_PAGEUP:
            case WXK_PRIOR:
                hotkey << wxT("Prior" );
                break;
            case WXK_PAGEDOWN:
            case WXK_NEXT:
                hotkey << wxT("Next" );
                break;
            case WXK_LEFT:
                hotkey << wxT("Left" );
                break;
            case WXK_RIGHT:
                hotkey << wxT("Right" );
                break;
            case WXK_HOME:
                hotkey << wxT("Home" );
                break;
            case WXK_END:
                hotkey << wxT("End" );
                break;
            case WXK_RETURN:
                hotkey << wxT("Return" );
                break;

                // if there are any other keys wxGetAccelFromString() may
                // return, we should process them here

            default:
                if ( code < 127 )
                {
                    wxString name = wxGTK_CONV_BACK( gdk_keyval_name((guint)code) );
                    if ( name )
                    {
                        hotkey << name;
                        break;
                    }
                }

                wxFAIL_MSG( wxT("unknown keyboard accel") );
        }

        delete accel;
    }

    return hotkey;
}

#endif // wxUSE_ACCEL


//-----------------------------------------------------------------------------
// substitute for missing GtkPixmapMenuItem
//-----------------------------------------------------------------------------

#ifndef __WXGTK20__

/*
 * Copyright (C) 1998, 1999, 2000 Free Software Foundation
 * All rights reserved.
 *
 * This file is part of the Gnome Library.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/*
  @NOTATION@
 */

/* Author: Dietmar Maurer <dm@vlsivie.tuwien.ac.at> */

#include <gtk/gtkaccellabel.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkcontainer.h>

extern "C"
{

static void gtk_pixmap_menu_item_class_init    (GtkPixmapMenuItemClass *klass);
static void gtk_pixmap_menu_item_init          (GtkPixmapMenuItem      *menu_item);
static void gtk_pixmap_menu_item_draw          (GtkWidget              *widget,
                                                GdkRectangle           *area);
static gint gtk_pixmap_menu_item_expose        (GtkWidget              *widget,
                                                GdkEventExpose         *event);

/* we must override the following functions */

static void gtk_pixmap_menu_item_map           (GtkWidget        *widget);
static void gtk_pixmap_menu_item_size_allocate (GtkWidget        *widget,
                                                GtkAllocation    *allocation);
static void gtk_pixmap_menu_item_forall        (GtkContainer    *container,
                                                gboolean         include_internals,
                                                GtkCallback      callback,
                                                gpointer         callback_data);
static void gtk_pixmap_menu_item_size_request  (GtkWidget        *widget,
                                                GtkRequisition   *requisition);
static void gtk_pixmap_menu_item_remove        (GtkContainer *container,
                                                GtkWidget    *child);

static void changed_have_pixmap_status         (GtkPixmapMenuItem *menu_item);

static GtkMenuItemClass *parent_class = NULL;

}

#define BORDER_SPACING  3
#define PMAP_WIDTH 20

GtkType
gtk_pixmap_menu_item_get_type (void)
{
  static GtkType pixmap_menu_item_type = 0;

  if (!pixmap_menu_item_type)
    {
      GtkTypeInfo pixmap_menu_item_info =
      {
        (char *)"GtkPixmapMenuItem",
        sizeof (GtkPixmapMenuItem),
        sizeof (GtkPixmapMenuItemClass),
        (GtkClassInitFunc) gtk_pixmap_menu_item_class_init,
        (GtkObjectInitFunc) gtk_pixmap_menu_item_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      pixmap_menu_item_type = gtk_type_unique (gtk_menu_item_get_type (),
                                               &pixmap_menu_item_info);
    }

  return pixmap_menu_item_type;
}

/**
 * gtk_pixmap_menu_item_new
 *
 * Creates a new pixmap menu item. Use gtk_pixmap_menu_item_set_pixmap()
 * to set the pixmap wich is displayed at the left side.
 *
 * Returns:
 * &GtkWidget pointer to new menu item
 **/

GtkWidget*
gtk_pixmap_menu_item_new (void)
{
  return GTK_WIDGET (gtk_type_new (gtk_pixmap_menu_item_get_type ()));
}

static void
gtk_pixmap_menu_item_class_init (GtkPixmapMenuItemClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkMenuItemClass *menu_item_class;
  GtkContainerClass *container_class;

  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;
  menu_item_class = (GtkMenuItemClass*) klass;
  container_class = (GtkContainerClass*) klass;

  parent_class = (GtkMenuItemClass*) gtk_type_class (gtk_menu_item_get_type ());

  widget_class->draw = gtk_pixmap_menu_item_draw;
  widget_class->expose_event = gtk_pixmap_menu_item_expose;
  widget_class->map = gtk_pixmap_menu_item_map;
  widget_class->size_allocate = gtk_pixmap_menu_item_size_allocate;
  widget_class->size_request = gtk_pixmap_menu_item_size_request;

  container_class->forall = gtk_pixmap_menu_item_forall;
  container_class->remove = gtk_pixmap_menu_item_remove;

  klass->orig_toggle_size = menu_item_class->toggle_size;
  klass->have_pixmap_count = 0;
}

static void
gtk_pixmap_menu_item_init (GtkPixmapMenuItem *menu_item)
{
  GtkMenuItem *mi;

  mi = GTK_MENU_ITEM (menu_item);

  menu_item->pixmap = NULL;
}

static void
gtk_pixmap_menu_item_draw (GtkWidget    *widget,
                           GdkRectangle *area)
{
  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_PIXMAP_MENU_ITEM (widget));
  g_return_if_fail (area != NULL);

  if (GTK_WIDGET_CLASS (parent_class)->draw)
    (* GTK_WIDGET_CLASS (parent_class)->draw) (widget, area);

  if (GTK_WIDGET_DRAWABLE (widget) &&
      GTK_PIXMAP_MENU_ITEM(widget)->pixmap) {
    gtk_widget_draw(GTK_WIDGET(GTK_PIXMAP_MENU_ITEM(widget)->pixmap),NULL);
  }
}

static gint
gtk_pixmap_menu_item_expose (GtkWidget      *widget,
                             GdkEventExpose *event)
{
  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_PIXMAP_MENU_ITEM (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_CLASS (parent_class)->expose_event)
    (* GTK_WIDGET_CLASS (parent_class)->expose_event) (widget, event);

  if (GTK_WIDGET_DRAWABLE (widget) &&
      GTK_PIXMAP_MENU_ITEM(widget)->pixmap) {
    gtk_widget_draw(GTK_WIDGET(GTK_PIXMAP_MENU_ITEM(widget)->pixmap),NULL);
  }

  return FALSE;
}

/**
 * gtk_pixmap_menu_item_set_pixmap
 * @menu_item: Pointer to the pixmap menu item
 * @pixmap: Pointer to a pixmap widget
 *
 * Set the pixmap of the menu item.
 *
 **/

void
gtk_pixmap_menu_item_set_pixmap (GtkPixmapMenuItem *menu_item,
                                 GtkWidget         *pixmap)
{
  g_return_if_fail (menu_item != NULL);
  g_return_if_fail (pixmap != NULL);
  g_return_if_fail (GTK_IS_PIXMAP_MENU_ITEM (menu_item));
  g_return_if_fail (GTK_IS_WIDGET (pixmap));
  g_return_if_fail (menu_item->pixmap == NULL);

  gtk_widget_set_parent (pixmap, GTK_WIDGET (menu_item));
  menu_item->pixmap = pixmap;

  if (GTK_WIDGET_REALIZED (pixmap->parent) &&
      !GTK_WIDGET_REALIZED (pixmap))
    gtk_widget_realize (pixmap);

  if (GTK_WIDGET_VISIBLE (pixmap->parent)) {
    if (GTK_WIDGET_MAPPED (pixmap->parent) &&
        GTK_WIDGET_VISIBLE(pixmap) &&
        !GTK_WIDGET_MAPPED (pixmap))
      gtk_widget_map (pixmap);
  }

  changed_have_pixmap_status(menu_item);

  if (GTK_WIDGET_VISIBLE (pixmap) && GTK_WIDGET_VISIBLE (menu_item))
    gtk_widget_queue_resize (pixmap);
}

static void
gtk_pixmap_menu_item_map (GtkWidget *widget)
{
  GtkPixmapMenuItem *menu_item;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_PIXMAP_MENU_ITEM (widget));

  menu_item = GTK_PIXMAP_MENU_ITEM(widget);

  GTK_WIDGET_CLASS(parent_class)->map(widget);

  if (menu_item->pixmap &&
      GTK_WIDGET_VISIBLE (menu_item->pixmap) &&
      !GTK_WIDGET_MAPPED (menu_item->pixmap))
    gtk_widget_map (menu_item->pixmap);
}

static void
gtk_pixmap_menu_item_size_allocate (GtkWidget        *widget,
                                    GtkAllocation    *allocation)
{
  GtkPixmapMenuItem *pmenu_item;

  pmenu_item = GTK_PIXMAP_MENU_ITEM(widget);

  if (pmenu_item->pixmap && GTK_WIDGET_VISIBLE(pmenu_item))
    {
      GtkAllocation child_allocation;
      int border_width;

      border_width = GTK_CONTAINER (widget)->border_width;

      child_allocation.width = pmenu_item->pixmap->requisition.width;
      child_allocation.height = pmenu_item->pixmap->requisition.height;
      child_allocation.x = border_width + BORDER_SPACING;
      child_allocation.y = (border_width + BORDER_SPACING
                            + (((allocation->height - child_allocation.height) - child_allocation.x)
                               / 2)); /* center pixmaps vertically */
      gtk_widget_size_allocate (pmenu_item->pixmap, &child_allocation);
    }

  if (GTK_WIDGET_CLASS (parent_class)->size_allocate)
    GTK_WIDGET_CLASS(parent_class)->size_allocate (widget, allocation);
}

static void
gtk_pixmap_menu_item_forall (GtkContainer    *container,
                             gboolean         include_internals,
                             GtkCallback      callback,
                             gpointer         callback_data)
{
  GtkPixmapMenuItem *menu_item;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_PIXMAP_MENU_ITEM (container));
  g_return_if_fail (callback != NULL);

  menu_item = GTK_PIXMAP_MENU_ITEM (container);

  if (menu_item->pixmap)
    (* callback) (menu_item->pixmap, callback_data);

  GTK_CONTAINER_CLASS(parent_class)->forall(container,include_internals,
                                            callback,callback_data);
}

static void
gtk_pixmap_menu_item_size_request (GtkWidget      *widget,
                                   GtkRequisition *requisition)
{
  GtkPixmapMenuItem *menu_item;
  GtkRequisition req = {0, 0};

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_MENU_ITEM (widget));
  g_return_if_fail (requisition != NULL);

  GTK_WIDGET_CLASS(parent_class)->size_request(widget,requisition);

  menu_item = GTK_PIXMAP_MENU_ITEM (widget);

  if (menu_item->pixmap)
    gtk_widget_size_request(menu_item->pixmap, &req);

  requisition->height = MAX(req.height + GTK_CONTAINER(widget)->border_width + BORDER_SPACING, (unsigned int) requisition->height);
  requisition->width += (req.width + GTK_CONTAINER(widget)->border_width + BORDER_SPACING);
}

static void
gtk_pixmap_menu_item_remove (GtkContainer *container,
                             GtkWidget    *child)
{
  GtkBin *bin;
  gboolean widget_was_visible;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_PIXMAP_MENU_ITEM (container));
  g_return_if_fail (child != NULL);
  g_return_if_fail (GTK_IS_WIDGET (child));

  bin = GTK_BIN (container);
  g_return_if_fail ((bin->child == child ||
                     (GTK_PIXMAP_MENU_ITEM(container)->pixmap == child)));

  widget_was_visible = GTK_WIDGET_VISIBLE (child);

  gtk_widget_unparent (child);
  if (bin->child == child)
    bin->child = NULL;
  else {
    GTK_PIXMAP_MENU_ITEM(container)->pixmap = NULL;
    changed_have_pixmap_status(GTK_PIXMAP_MENU_ITEM(container));
  }

  if (widget_was_visible)
    gtk_widget_queue_resize (GTK_WIDGET (container));
}


/* important to only call this if there was actually a _change_ in pixmap == NULL */
static void
changed_have_pixmap_status (GtkPixmapMenuItem *menu_item)
{
  if (menu_item->pixmap != NULL) {
    GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->have_pixmap_count += 1;

    if (GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->have_pixmap_count == 1) {
      /* Install pixmap toggle size */
      GTK_MENU_ITEM_GET_CLASS(menu_item)->toggle_size = MAX(GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->orig_toggle_size, PMAP_WIDTH);
    }
  } else {
    GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->have_pixmap_count -= 1;

    if (GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->have_pixmap_count == 0) {
      /* Install normal toggle size */
      GTK_MENU_ITEM_GET_CLASS(menu_item)->toggle_size = GTK_PIXMAP_MENU_ITEM_GET_CLASS(menu_item)->orig_toggle_size;
    }
  }

  /* Note that we actually need to do this for _all_ GtkPixmapMenuItem
     whenever the klass->toggle_size changes; but by doing it anytime
     this function is called, we get the same effect, just because of
     how the preferences option to show pixmaps works. Bogus, broken.
  */
  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(menu_item)))
    gtk_widget_queue_resize(GTK_WIDGET(menu_item));
}

#endif

