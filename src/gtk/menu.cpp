/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "menu.h"
#pragma implementation "menuitem.h"
#endif

#include "wx/log.h"
#include "wx/intl.h"
#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/menu.h"

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

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

#if GTK_CHECK_VERSION(1, 2, 0) && wxUSE_ACCEL
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

    /* GTK 1.2 wants to have "_" instead of "&" for accelerators */
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
#if GTK_CHECK_VERSION(1, 2, 0)
            str << wxT('_');
#endif
        }
#if GTK_CHECK_VERSION(2, 0, 0)
        else if (*pc == wxT('/'))
        {
            str << wxT("\\/");
        }
        else if (*pc == wxT('\\'))
        {
            str << wxT("\\\\");
        }
#elif GTK_CHECK_VERSION(1, 2, 0)
        else if (*pc == wxT('/'))
        {
            str << wxT('\\');
        }
#endif
        else
        {
#ifdef __WXGTK12__
            if ( *pc == wxT('_') )
            {
                // underscores must be doubled to prevent them from being
                // interpreted as accelerator character prefix by GTK
                str << *pc;
            }
#endif // GTK+ 1.2

            str << *pc;
        }
        ++pc;
    }
    return str;
}

//-----------------------------------------------------------------------------
// activate message from GTK
//-----------------------------------------------------------------------------

static void gtk_menu_open_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle) wxapp_install_idle_handler();

    wxMenuEvent event( wxEVT_MENU_OPEN, -1 );
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
    /* the parent window is known after wxFrame::SetMenu() */
    m_needParent = FALSE;
    m_style = style;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menus.DeleteContents( TRUE );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if GTK_CHECK_VERSION(1, 2, 1)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", m_accel );
    m_menubar = gtk_item_factory_get_widget( m_factory, "<main>" );
#else
    m_menubar = gtk_menu_bar_new();
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
    /* the parent window is known after wxFrame::SetMenu() */
    m_needParent = FALSE;
    m_style = 0;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menus.DeleteContents( TRUE );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if GTK_CHECK_VERSION(1, 2, 1)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", m_accel );
    m_menubar = gtk_item_factory_get_widget( m_factory, "<main>" );
#else
    m_menubar = gtk_menu_bar_new();
#endif

    m_widget = GTK_WIDGET(m_menubar);

    PostCreation();

    ApplyWidgetStyle();
}

wxMenuBar::~wxMenuBar()
{
//    gtk_object_unref( GTK_OBJECT(m_factory) );  why not ?
}

static void wxMenubarUnsetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( (wxWindow*) NULL );

    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native hot keys */
    gtk_accel_group_detach( menu->m_accel, ACCEL_OBJ_CAST(top_frame->m_widget) );

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
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

#if GTK_CHECK_VERSION(1, 2, 1)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native hot keys  */
    ACCEL_OBJECT *obj = ACCEL_OBJ_CAST(top_frame->m_widget);
    if ( !g_slist_find( ACCEL_OBJECTS(menu->m_accel), obj ) )
        gtk_accel_group_attach( menu->m_accel, obj );
#endif // GTK+ 1.2.1+

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
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
#if GTK_CHECK_VERSION(1, 2, 1)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    ACCEL_OBJECT *obj = ACCEL_OBJ_CAST(top_frame->m_widget);
    if ( !g_slist_find( ACCEL_OBJECTS(m_accel), obj ) )
        gtk_accel_group_attach( m_accel, obj );
#endif // GTK+ 1.2.1+

    wxMenuList::Node *node = m_menus.GetFirst();
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
#if GTK_CHECK_VERSION(1, 2, 1)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    gtk_accel_group_detach( m_accel, ACCEL_OBJ_CAST(top_frame->m_widget) );
#endif // GTK+ 1.2.1+

    wxMenuList::Node *node = m_menus.GetFirst();
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

bool wxMenuBar::GtkAppend(wxMenu *menu, const wxString& title)
{
    wxString str( wxReplaceUnderscore( title ) );

    /* this doesn't have much effect right now */
    menu->SetTitle( str );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if GTK_CHECK_VERSION(1, 2, 1)

    wxString buf;
    buf << wxT('/') << str.c_str();

    /* local buffer in multibyte form */
    char cbuf[400];
    strcpy(cbuf, wxGTK_CONV(buf) );

    GtkItemFactoryEntry entry;
    entry.path = (gchar *)cbuf;  // const_cast
    entry.accelerator = (gchar*) NULL;
    entry.callback = (GtkItemFactoryCallback) NULL;
    entry.callback_action = 0;
    entry.item_type = (char *)"<Branch>";

    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */
    /* in order to get the pointer to the item we need the item text _without_ underscores */
    wxString tmp = wxT("<main>/");
    const wxChar *pc;
    for ( pc = str; *pc != wxT('\0'); pc++ )
    {
       // contrary to the common sense, we must throw out _all_ underscores,
       // (i.e. "Hello__World" => "HelloWorld" and not "Hello_World" as we
       // might naively think). IMHO it's a bug in GTK+ (VZ)
       while (*pc == wxT('_'))
           pc++;
       tmp << *pc;
    }
    menu->m_owner = gtk_item_factory_get_item( m_factory, wxGTK_CONV( tmp ) );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );
#else

    menu->m_owner = gtk_menu_item_new_with_label( wxGTK_CONV( str ) );
    gtk_widget_show( menu->m_owner );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );

    gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), menu->m_owner );

#endif

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

        wxFrame	*frame = wxDynamicCast( m_invokingWindow, wxFrame );

        if( frame )
            frame->UpdateMenuBarSize();
    }

    return TRUE;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    // GTK+ doesn't have a function to insert a menu using GtkItemFactory (as
    // of version 1.2.6), so we first append the item and then change its
    // index
    if ( !GtkAppend(menu, title) )
        return FALSE;

    if (pos+1 >= m_menus.GetCount())
        return TRUE;

    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);
    gpointer data = g_list_last(menu_shell->children)->data;
    menu_shell->children = g_list_remove(menu_shell->children, data);
    menu_shell->children = g_list_insert(menu_shell->children, data, pos);

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

static wxMenu *CopyMenu (wxMenu *menu)
{
    wxMenu *menucopy = new wxMenu ();
    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *item = node->GetData();
        int itemid = item->GetId();
        wxString text = item->GetText();
        text.Replace(wxT("_"), wxT("&"));
        wxMenu *submenu = item->GetSubMenu();
        if (!submenu)
        {
            wxMenuItem* itemcopy = new wxMenuItem(menucopy,
                                        itemid, text,
                                        menu->GetHelpString(itemid));
            itemcopy->SetBitmap(item->GetBitmap());
            itemcopy->SetCheckable(item->IsCheckable());
            menucopy->Append(itemcopy);
        }
        else
          menucopy->Append (itemid, text, CopyMenu(submenu),
                            menu->GetHelpString(itemid));

        node = node->GetNext();
    }

    return menucopy;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return (wxMenu*) NULL;

/*
    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);

    printf( "factory entries before %d\n", (int)g_slist_length(m_factory->items) );
    printf( "menu shell entries before %d\n", (int)g_list_length( menu_shell->children ) );
*/

    wxMenu *menucopy = CopyMenu( menu );

    // unparent calls unref() and that would delete the widget so we raise
    // the ref count to 2 artificially before invoking unparent.
    gtk_widget_ref( menu->m_menu );
    gtk_widget_unparent( menu->m_menu );

    gtk_widget_destroy( menu->m_owner );
    delete menu;

    menu = menucopy;
/*
    printf( "factory entries after %d\n", (int)g_slist_length(m_factory->items) );
    printf( "menu shell entries after %d\n", (int)g_list_length( menu_shell->children ) );
*/

    if (m_invokingWindow)
    {
            // OPTIMISE ME:  see comment in GtkAppend

	wxFrame	*frame = wxDynamicCast( m_invokingWindow, wxFrame );

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

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
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
    wxMenuList::Node *node = m_menus.GetFirst();
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

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
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
    wxMenuList::Node *node = m_menus.GetFirst();
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
    wxMenuList::Node *node = m_menus.Item( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = node->GetData();

    if (menu->m_owner)
        gtk_widget_set_sensitive( menu->m_owner, flag );
}

wxString wxMenuBar::GetLabelTop( size_t pos ) const
{
    wxMenuList::Node *node = m_menus.Item( pos );

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
    wxMenuList::Node *node = m_menus.Item( pos );

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
    wxMenu*  pm = menu;
    while ( pm && !frame )
    {
        if ( pm->IsAttached() )
            frame = pm->GetMenuBar()->GetFrame();
        pm = pm->GetParent();
    }

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
        if ( *pc == wxT('_') )
        {
            // GTK 1.2 escapes "xxx_xxx" to "xxx__xxx"
            pc++;
            label += *pc;
            continue;
        }

#if GTK_CHECK_VERSION(2, 0, 0)
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

#if GTK_CHECK_VERSION(2, 0, 0)
        // We have to imitate item_factory_unescape_label here
        wxString tmp;
        for (size_t n = 0; n < m_text.Len(); n++)
        {
            if (m_text[n] != wxT('\\'))
                tmp += m_text[n];
        }

        gtk_label_set_text_with_mnemonic( GTK_LABEL(label), wxGTK_CONV(tmp) );
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
#if GTK_CHECK_VERSION(2, 0, 0)
        else if ( *pc == wxT('_') )    // escape underscores
        {
            m_text << wxT("__");
        }
        else if (*pc == wxT('/'))      // we have to escape slashes
        {
            m_text << wxT("\\/");
        }
        else if (*pc == wxT('\\'))     // we have to double backslashes
        {
            m_text << wxT("\\\\");
        }
#else
        else if ( *pc == wxT('_') )    // escape underscores
        {
            m_text << wxT("__");
        }
        else if (*pc == wxT('/'))      /* we have to filter out slashes ... */
        {
            m_text << wxT('\\');  /* ... and replace them with back slashes */
        }
#endif
        else {
            m_text << *pc;
        }
        ++pc;
    }

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

wxString wxMenuItem::GetFactoryPath() const
{
    /* in order to get the pointer to the item we need the item text
       _without_ underscores */
    wxString path( wxT("<main>/") );

    for ( const wxChar *pc = m_text.c_str(); *pc; pc++ )
    {
        if ( *pc == wxT('_') )
        {
#ifdef __WXGTK20__
            pc++;
#else
            // remove '_' unconditionally
            continue;
#endif
        }

        // don't remove ampersands '&' since if we have them in the menu item title
        // it means that they were doubled to indicate "&" instead of accelerator

        path += *pc;
    }

    return path;
}

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

void wxMenu::Init()
{
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU, "<main>", m_accel );
    m_menu = gtk_item_factory_get_widget( m_factory, "<main>" );

    m_owner = (GtkWidget*) NULL;

    /* Tearoffs are entries, just like separators. So if we want this
       menu to be a tear-off one, we just append a tearoff entry
       immediately. */
    if(m_style & wxMENU_TEAROFF)
    {
       GtkItemFactoryEntry entry;
       entry.path = (char *)"/tearoff";
       entry.callback = (GtkItemFactoryCallback) NULL;
       entry.callback_action = 0;
       entry.item_type = (char *)"<Tearoff>";
       entry.accelerator = (gchar*) NULL;
       gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */
       //GtkWidget *menuItem = gtk_item_factory_get_widget( m_factory, "<main>/tearoff" );
    }

    // append the title as the very first entry if we have it
    if ( !!m_title )
    {
        Append(-2, m_title);
        AppendSeparator();
    }
}

wxMenu::~wxMenu()
{
   m_items.Clear();

   if ( GTK_IS_WIDGET( m_menu ))
       gtk_widget_destroy( m_menu );

   gtk_object_unref( GTK_OBJECT(m_factory) );
}

bool wxMenu::GtkAppend(wxMenuItem *mitem)
{
    GtkWidget *menuItem;

    // does this item terminate the current radio group?
    bool endOfRadioGroup = TRUE;

    if ( mitem->IsSeparator() )
    {
        GtkItemFactoryEntry entry;
        entry.path = (char *)"/sep";
        entry.callback = (GtkItemFactoryCallback) NULL;
        entry.callback_action = 0;
        entry.item_type = (char *)"<Separator>";
        entry.accelerator = (gchar*) NULL;

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        /* this will be wrong for more than one separator. do we care? */
        menuItem = gtk_item_factory_get_widget( m_factory, "<main>/sep" );

        // we might have a separator inside a radio group
        endOfRadioGroup = FALSE;
    }
    else if ( mitem->IsSubMenu() )
    {
        /* text has "_" instead of "&" after mitem->SetText() */
        wxString text( mitem->GetText() );

        /* local buffer in multibyte form */
        char buf[200];
        strcpy( buf, "/" );
        strcat( buf, wxGTK_CONV( text ) );

        GtkItemFactoryEntry entry;
        entry.path = buf;
        entry.callback = (GtkItemFactoryCallback) 0;
        entry.callback_action = 0;
        entry.item_type = (char *)"<Branch>";
        entry.accelerator = (gchar*) NULL;

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        wxString path( mitem->GetFactoryPath() );
        menuItem = gtk_item_factory_get_item( m_factory, wxGTK_CONV( path ) );

        gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), mitem->GetSubMenu()->m_menu );

        // if adding a submenu to a menu already existing in the menu bar, we
        // must set invoking window to allow processing events from this
        // submenu
        if ( m_invokingWindow )
            wxMenubarSetInvokingWindow(mitem->GetSubMenu(), m_invokingWindow);
    }
#ifndef __WXGTK20__
    else if (mitem->GetBitmap().Ok())
    {
        // Our extra code for Bitmaps in GTK 1.2
        wxString text( mitem->GetText() );
        const wxBitmap *bitmap = &mitem->GetBitmap();

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
                                        gtk_menu_get_accel_group(
                                            GTK_MENU(m_menu)),
                                        accel_key, accel_mods,
                                        GTK_ACCEL_VISIBLE);
        }

        // accelerator for the underlined char (ex ALT+F for the File menu)
        accel_key = gtk_label_parse_uline (GTK_LABEL(label), wxGTK_CONV( text ) );
        if (accel_key != GDK_VoidSymbol)
        {
            gtk_widget_add_accelerator (menuItem,
                                        "activate_item",
                                        gtk_menu_ensure_uline_accel_group (
                                            GTK_MENU (m_menu)),
                                        accel_key, (GdkModifierType) 0,
                                        GTK_ACCEL_LOCKED);
        }

        gtk_widget_show (label);

        mitem->SetLabelWidget(label);

        GtkWidget* pixmap = gtk_pixmap_new( bitmap->GetPixmap(), bitmap->GetMask() ? bitmap->GetMask()->GetBitmap() : (GdkBitmap* )NULL);
        gtk_widget_show(pixmap);
        gtk_pixmap_menu_item_set_pixmap(GTK_PIXMAP_MENU_ITEM( menuItem ), pixmap);

        gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                            GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                            (gpointer)this );

        gtk_menu_append( GTK_MENU(m_menu), menuItem );
        gtk_widget_show( menuItem );
    }
#endif
    else // a normal item
    {
        // text has "_" instead of "&" after mitem->SetText() so don't use it
        wxString text( mitem->GetText() );

        // buffers containing the menu item path and type in multibyte form
        char bufPath[256],
             bufType[256];

        strcpy( bufPath, "/" );
        strncat( bufPath, wxGTK_CONV(text), WXSIZEOF(bufPath) - 2 );
        bufPath[WXSIZEOF(bufPath) - 1] = '\0';

        GtkItemFactoryEntry entry;
        entry.path = bufPath;
        entry.callback = (GtkItemFactoryCallback) gtk_menu_clicked_callback;
        entry.callback_action = 0;

        wxString pathRadio;
        const char *item_type;
        switch ( mitem->GetKind() )
        {
            case wxITEM_CHECK:
                item_type = "<CheckItem>";
                break;

            case wxITEM_RADIO:
                if ( m_pathLastRadio.empty() )
                {
                    // start of a new radio group
                    item_type = "<RadioItem>";
                    wxString tmp( wxGTK_CONV_BACK( bufPath ) );
                    tmp.Remove(0,1);
                    m_pathLastRadio = tmp;
                }
                else // continue the radio group
                {
                    pathRadio = m_pathLastRadio;
                    pathRadio.Replace(wxT("_"), wxT(""));
                    pathRadio.Prepend(wxT("<main>/"));

                    strncpy(bufType, wxGTK_CONV(pathRadio), WXSIZEOF(bufType));
                    bufType[WXSIZEOF(bufType) - 1] = '\0';
                    item_type = bufType;
                }

                // continue the existing radio group, if any
                endOfRadioGroup = FALSE;
                break;


            default:
                wxFAIL_MSG( _T("unexpected menu item kind") );
                // fall through

            case wxITEM_NORMAL:
                item_type = "<Item>";
#if defined(__WXGTK20__) && wxUSE_IMAGE
                if (mitem->GetBitmap().Ok())
                {
                    item_type = "<ImageItem>";
                    // GTK2's image factory know about image items, but they need to
                    // get a GdkPixbuf structure, which we need to create on the fly.
                    // This Pixbuf structure needs to be static so we create it and
                    // just make it a memory leak...
                    wxImage image( mitem->GetBitmap().ConvertToImage() );
                    size_t size = 4 +   // magic
                                  20 +  // header
                                  image.GetHeight() * image.GetWidth() * 4; // RGBA

                    unsigned char *dest = new unsigned char[size];
                    entry.extra_data = dest;

                    unsigned char *source = image.GetData();
                    bool has_mask = image.HasMask();
                    unsigned char mask_r = image.GetMaskRed();
                    unsigned char mask_b = image.GetMaskBlue();
                    unsigned char mask_g = image.GetMaskGreen();
                    wxUint32 tmp;

                    // Magic
                    *dest = 'G'; dest++; *dest = 'd'; dest++; *dest = 'k'; dest++; *dest = 'P'; dest++;
                    // Data size
                    tmp = size;
                    *dest = tmp >> 24; dest++; *dest = tmp >> 16; dest++; *dest = tmp >> 8; dest++; *dest = tmp; dest++;
                    // Pixdata type
                    *dest = 1; dest++; *dest = 1; dest++; *dest = 0; dest++; *dest = 2; dest++;
                    // Rowstride
                    tmp = image.GetWidth()*4;
                    *dest = tmp >> 24; dest++; *dest = tmp >> 16; dest++; *dest = tmp >> 8; dest++; *dest = tmp; dest++;
                    // Width
                    tmp = image.GetWidth();
                    *dest = tmp >> 24; dest++; *dest = tmp >> 16; dest++; *dest = tmp >> 8; dest++; *dest = tmp; dest++;
                    // Height
                    tmp = image.GetHeight();
                    *dest = tmp >> 24; dest++; *dest = tmp >> 16; dest++; *dest = tmp >> 8; dest++; *dest = tmp; dest++;

                    for (int i = 0; i < image.GetWidth()*image.GetHeight(); i++)
                    {
                        unsigned char r = *source; source++;
                        unsigned char g = *source; source++;
                        unsigned char b = *source; source++;
                        *dest = r; dest++;
                        *dest = g; dest++;
                        *dest = b; dest++;
                        if (has_mask && (r == mask_r)  && (g == mask_g)  && (b == mask_b))
                             *dest = 0;
                        else
                             *dest = 255;
                        dest++;
                    }
                    break;
                }
#endif // GTK 2.0+
                break;
        }

        entry.item_type = (char *)item_type; // cast needed for GTK+
        entry.accelerator = (gchar*) NULL;

#if wxUSE_ACCEL
        // due to an apparent bug in GTK+, we have to use a static buffer here -
        // otherwise GTK+ 1.2.2 manages to override the memory we pass to it
        // somehow! (VZ)
        char s_accel[50]; // should be big enough, we check for overruns
        wxString tmp( GetHotKey(*mitem) );
        strncpy(s_accel, wxGTK_CONV( tmp ), WXSIZEOF(s_accel));
        s_accel[WXSIZEOF(s_accel) - 1] = '\0';
        entry.accelerator = s_accel;
#else // !wxUSE_ACCEL
        entry.accelerator = (char*) NULL;
#endif // wxUSE_ACCEL/!wxUSE_ACCEL

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        wxString path( mitem->GetFactoryPath() );
        menuItem = gtk_item_factory_get_widget( m_factory, wxGTK_CONV( path ) );

        if (!menuItem)
            wxLogError( wxT("Wrong menu path: %s\n"), path.c_str() );
    }

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

    if ( endOfRadioGroup )
    {
        m_pathLastRadio.clear();
    }

    return TRUE;
}

bool wxMenu::DoAppend(wxMenuItem *mitem)
{
    return GtkAppend(mitem) && wxMenuBase::DoAppend(mitem);
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return FALSE;

#ifdef __WXGTK12__
    // GTK+ doesn't have a function to insert a menu using GtkItemFactory (as
    // of version 1.2.6), so we first append the item and then change its
    // index
    if ( !GtkAppend(item) )
        return FALSE;

    if ( m_style & wxMENU_TEAROFF )
    {
        // change the position as the first item is the tear-off marker
        pos++;
    }

    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);
    gpointer data = g_list_last(menu_shell->children)->data;
    menu_shell->children = g_list_remove(menu_shell->children, data);
    menu_shell->children = g_list_insert(menu_shell->children, data, pos);

    return TRUE;
#else // GTK < 1.2
    // this should be easy to do...
    wxFAIL_MSG( wxT("not implemented") );

    return FALSE;
#endif // GTK 1.2/1.0
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
    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->GetMenuItem() == menuItem)
           return item->GetId();
        node = node->Next();
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if GTK_CHECK_VERSION(1, 2, 0) && wxUSE_ACCEL

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
                hotkey << wxT('F') << code - WXK_F1 + 1;
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
      GTK_PIXMAP_MENU_ITEM(widget)->pixmap)
  {
    // Use GtkPixmap for drawing
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

#endif // __WXGTK20__

