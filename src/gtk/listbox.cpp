/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Modified By: Ryan Norton (GtkTreeView implementation)
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"
#include "wx/dynarray.h"
#include "wx/arrstr.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/checklst.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/gtk/private.h"
#include "wx/gtk/treeentry_gtk.h"

#if wxUSE_TOOLTIPS
#include "wx/tooltip.h"
#endif

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool           g_blockEventsOnDrag;
extern bool           g_blockEventsOnScroll;
extern wxCursor       g_globalCursor;


//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// Macro to tell which row the strings are in (1 if native checklist, 0 if not)
//-----------------------------------------------------------------------------

#if wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST
#   define WXLISTBOX_DATACOLUMN_ARG(x)  (x->m_hasCheckBoxes ? 1 : 0)
#else
#   define WXLISTBOX_DATACOLUMN_ARG(x)  (0)
#endif // wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST

#define WXLISTBOX_DATACOLUMN    WXLISTBOX_DATACOLUMN_ARG(this)

//-----------------------------------------------------------------------------
// "row-activated"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_listbox_row_activated_callback(GtkTreeView        *treeview,
                                   GtkTreePath        *path,
                                   GtkTreeViewColumn  *col,
                                   wxListBox          *listbox)
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return;
    if (g_blockEventsOnScroll) return;

    if (!listbox->m_hasVMT) return;

    //Notes:
    //1) This is triggered by either a double-click or a space press
    //2) We handle both here because
    //2a) in the case of a space/keypress we can't really know
    //    which item was pressed on because we can't get coords
    //    from a keyevent
    //2b) It seems more correct

    int sel = gtk_tree_path_get_indices(path)[0];

    if(!listbox->m_spacePressed)
    {
        //Assume it was double-click
        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, listbox->GetId() );
        event.SetEventObject( listbox );

        if(listbox->IsSelected(sel))
        {
            GtkTreeEntry* entry = listbox->GtkGetEntry(sel);

            if(entry)
            {
                event.SetInt(sel);
                event.SetString(wxConvUTF8.cMB2WX(gtk_tree_entry_get_label(entry)));

                if ( listbox->HasClientObjectData() )
                    event.SetClientObject( 
                    (wxClientData*) gtk_tree_entry_get_userdata(entry) );
                else if ( listbox->HasClientUntypedData() )
                    event.SetClientData( gtk_tree_entry_get_userdata(entry) );
                g_object_unref(G_OBJECT(entry));
            }
            else
            {
                wxLogSysError(wxT("Internal error - could not get entry for double-click"));
                event.SetInt(-1);
            }
        }
        else
            event.SetInt(-1);

        listbox->GetEventHandler()->ProcessEvent( event );
    }
    else
    {
        listbox->m_spacePressed = false; //don't block selection behaviour anymore

        //Space was pressed - toggle the appropriate checkbox and the selection
#if wxUSE_CHECKLISTBOX //Do it for both native and non-native
        if (listbox->m_hasCheckBoxes)
        {
            wxCheckListBox *clb = (wxCheckListBox *)listbox;

            clb->Check( sel, !clb->IsChecked(sel) );

            wxCommandEvent new_event( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, listbox->GetId() );
            new_event.SetEventObject( listbox );
            new_event.SetInt( sel );
            listbox->GetEventHandler()->ProcessEvent( new_event );
        }
#endif // wxUSE_CHECKLISTBOX

        if(  (((listbox->GetWindowStyleFlag() & wxLB_MULTIPLE) != 0) ||
              ((listbox->GetWindowStyleFlag() & wxLB_EXTENDED) != 0)) )
    {
            //toggle the selection + send event
            if(listbox->IsSelected( sel ))
                listbox->GtkSetSelection(sel, FALSE, FALSE);
    else
                listbox->GtkSetSelection(sel, TRUE, FALSE);
        }
    }
}
}

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_listbox_button_press_callback( GtkWidget *widget,
                                   GdkEventButton *gdk_event,
                                   wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;
    if (g_blockEventsOnScroll) return FALSE;

    if (!listbox->m_hasVMT) return FALSE;

    //Just to be on the safe side - it should be unset in the activate callback
    //but we don't want any obscure bugs if it doesn't get called somehow...
    listbox->m_spacePressed = false;

#if wxUSE_CHECKLISTBOX && !wxUSE_NATIVEGTKCHECKLIST
    if ((listbox->m_hasCheckBoxes) && (gdk_event->x < 15) && (gdk_event->type != GDK_2BUTTON_PRESS))
    {
        GtkTreePath* path;
        gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), 
                                  (gint)gdk_event->x, (gint)gdk_event->y,
                                  &path, NULL, NULL, NULL);
        int sel = gtk_tree_path_get_indices(path)[0];
        gtk_tree_path_free(path);

        wxCheckListBox *clb = (wxCheckListBox *)listbox;

        clb->Check( sel, !clb->IsChecked(sel) );

        wxCommandEvent event( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, listbox->GetId() );
        event.SetEventObject( listbox );
        event.SetInt( sel );
        listbox->GetEventHandler()->ProcessEvent( event );
    }
#endif // wxUSE_CHECKLISTBOX && !wxUSE_NATIVEGTKCHECKLIST

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_listbox_key_press_callback( GtkWidget *widget, 
                                GdkEventKey *gdk_event, 
                                wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;


    bool ret = FALSE;

    if ((gdk_event->keyval == GDK_Tab) || (gdk_event->keyval == GDK_ISO_Left_Tab))
    {
        wxNavigationKeyEvent new_event;
        /* GDK reports GDK_ISO_Left_Tab for SHIFT-TAB */
        new_event.SetDirection( (gdk_event->keyval == GDK_Tab) );
        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
        new_event.SetWindowChange( (gdk_event->state & GDK_CONTROL_MASK) );
        new_event.SetCurrentFocus( listbox );
        ret = listbox->GetEventHandler()->ProcessEvent( new_event );
    }

    if ((gdk_event->keyval == GDK_Return) && (!ret))
    {
        // eat return in all modes (RN:WHY?)
        ret = TRUE;
    }

    // Check or uncheck item with SPACE
    if (gdk_event->keyval == ' ')
        {
        //In the keyevent we don't know the index of the item
        //and the activated event gets called anyway...
        //
        //Also, activating with the space causes the treeview to 
        //unselect all the items and then select the item in question
        //wx's behaviour is to just toggle the item's selection state
        //and leave the others alone
        listbox->m_spacePressed = true;
    }

    if (ret)
    {
        g_signal_stop_emission_by_name (widget, "key_press_event");
        return TRUE;
    }

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "select" and "deselect"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_listitem_select_cb( GtkTreeSelection* selection,
                                        GtkTreeModel* model,
                                        GtkTreePath* path,
                                        gboolean is_selected,
                                        wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!listbox->m_hasVMT) return TRUE;
    if (g_blockEventsOnDrag) return TRUE;

    if (listbox->m_spacePressed) return FALSE; //see keyevent callback
    if (listbox->m_blockEvent) return TRUE;

    // NB: wxdocs explicitly say that this event only gets sent when
    // something is actually selected, plus the controls example
    // assumes so and passes -1 to the dogetclientdata funcs if not

    // OK, so basically we need to do a bit of a run-around here as
    // 1) is_selected says whether the item(s?) are CURRENTLY selected -    
    //    i.e. if is_selected is FALSE then the item is going to be 
    //    selected right now!
    // 2) However, since it is not already selected and the user 
    //    will expect it to be we need to manually select it and
    //    return FALSE telling GTK we handled the selection
    if (is_selected) return TRUE;

    int nIndex = gtk_tree_path_get_indices(path)[0];
    GtkTreeEntry* entry = listbox->GtkGetEntry(nIndex);

    if(entry)
    {
        //Now, as mentioned above, we manually select the row that is/was going
        //to be selected anyway by GTK
        listbox->m_blockEvent = TRUE; //if we don't block events we will lock the
                                      //app due to recursion!!

        GtkTreeSelection* selection = 
            gtk_tree_view_get_selection(listbox->m_treeview);
        GtkTreeIter iter;
        gtk_tree_model_get_iter(GTK_TREE_MODEL(listbox->m_liststore), &iter, path);
        gtk_tree_selection_select_iter(selection, &iter);

        listbox->m_blockEvent = FALSE;

        //Finally, send the wx event
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, listbox->GetId() );
    event.SetEventObject( listbox );

    // indicate whether this is a selection or a deselection
        event.SetExtraLong( 1 );

        event.SetInt(nIndex);
        event.SetString(wxConvUTF8.cMB2WX(gtk_tree_entry_get_label(entry)));

        if ( listbox->HasClientObjectData() )
            event.SetClientObject( 
                    (wxClientData*) gtk_tree_entry_get_userdata(entry) 
                                 );
        else if ( listbox->HasClientUntypedData() )
            event.SetClientData( gtk_tree_entry_get_userdata(entry) );

    listbox->GetEventHandler()->ProcessEvent( event );

        g_object_unref(G_OBJECT(entry));
        return FALSE;  //We handled it/did it manually
    }

    return TRUE; //allow selection to change
}
}

//-----------------------------------------------------------------------------
// GtkTreeEntry destruction (to destroy client data)
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_tree_entry_destroy_cb(GtkTreeEntry* entry, 
                                      wxListBox* listbox)
{
    if(listbox->HasClientObjectData())
    {
        gpointer userdata = gtk_tree_entry_get_userdata(entry);
        if(userdata)
            delete (wxClientData *)userdata;
    }
}
}

//-----------------------------------------------------------------------------
// Sorting callback (standard CmpNoCase return value)
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_listbox_sort_callback(GtkTreeModel *model,
                                      GtkTreeIter  *a,
                                      GtkTreeIter  *b,
                                      wxListBox    *listbox)
{
    GtkTreeEntry* entry;
    GtkTreeEntry* entry2;

    gtk_tree_model_get(GTK_TREE_MODEL(listbox->m_liststore),
                             a,
                             WXLISTBOX_DATACOLUMN_ARG(listbox), 
                             &entry, -1);
    gtk_tree_model_get(GTK_TREE_MODEL(listbox->m_liststore),
                             b,
                             WXLISTBOX_DATACOLUMN_ARG(listbox), 
                             &entry2, -1);
    wxCHECK_MSG(entry, 0, wxT("Could not get entry"));
    wxCHECK_MSG(entry2, 0, wxT("Could not get entry2"));

    //We compare collate keys here instead of calling g_utf8_collate
    //as it is rather slow (and even the docs reccommend this)
    int ret = strcasecmp(gtk_tree_entry_get_collate_key(entry),
                         gtk_tree_entry_get_collate_key(entry2));

    g_object_unref(G_OBJECT(entry));
    g_object_unref(G_OBJECT(entry2));

    return ret;
}
}

//-----------------------------------------------------------------------------
// Searching callback (TRUE == not equal, FALSE == equal)
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_listbox_searchequal_callback(GtkTreeModel* model,
                                                 gint column,
                                                 const gchar* key,
                                                 GtkTreeIter* iter,
                                                 wxListBox* listbox)
{
    GtkTreeEntry* entry;

    gtk_tree_model_get(GTK_TREE_MODEL(listbox->m_liststore),
                             iter,
                             WXLISTBOX_DATACOLUMN_ARG(listbox), 
                             &entry, -1);
    wxCHECK_MSG(entry, 0, wxT("Could not get entry"));
    gchar* keycollatekey = g_utf8_collate_key(key, -1);

    int ret = strcasecmp(keycollatekey,
                         gtk_tree_entry_get_collate_key(entry));

    g_free(keycollatekey);
    g_object_unref(G_OBJECT(entry));

    return ret != 0;
}
}

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxListBox::wxListBox()
{
    m_treeview = (GtkTreeView*) NULL;
#if wxUSE_CHECKLISTBOX
    m_hasCheckBoxes = FALSE;
#endif // wxUSE_CHECKLISTBOX
    m_spacePressed = false;
}

bool wxListBox::Create( wxWindow *parent, wxWindowID id,
                        const wxPoint &pos, const wxSize &size,
                        const wxArrayString& choices,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    wxCArrayString chs(choices);

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                   style, validator, name );
}

bool wxListBox::Create( wxWindow *parent, wxWindowID id,
                        const wxPoint &pos, const wxSize &size,
                        int n, const wxString choices[],
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
    m_blockEvent = FALSE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxListBox creation failed") );
        return FALSE;
    }

    m_widget = gtk_scrolled_window_new( (GtkAdjustment*) NULL, (GtkAdjustment*) NULL );
    if (style & wxLB_ALWAYS_SB)
    {
      gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
    }
    else
    {
      gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    }

    m_treeview = GTK_TREE_VIEW( gtk_tree_view_new( ) );

    //wxListBox doesn't have a header :)
    //NB: If enabled SetFirstItem doesn't work correctly
    gtk_tree_view_set_headers_visible(m_treeview, FALSE);

#if wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST
    if(m_hasCheckBoxes)
        ((wxCheckListBox*)this)->DoCreateCheckList();
#endif // wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST

    // Create the data column
    gtk_tree_view_insert_column_with_attributes(m_treeview, -1, "", 
                                                gtk_cell_renderer_text_new(),
                                                "text", 
                                                WXLISTBOX_DATACOLUMN, NULL);

    // Now create+set the model (GtkListStore) - first argument # of columns

#if wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST
    if(m_hasCheckBoxes)
        m_liststore = gtk_list_store_new(2, G_TYPE_BOOLEAN,
                                            GTK_TYPE_TREE_ENTRY);
    else
#endif
        m_liststore = gtk_list_store_new(1, GTK_TYPE_TREE_ENTRY);

    gtk_tree_view_set_model(m_treeview, GTK_TREE_MODEL(m_liststore));
    
    g_object_unref(G_OBJECT(m_liststore)); //free on treeview destruction
    
    // Disable the pop-up textctrl that enables searching - note that
    // the docs specify that even if this disabled (which we are doing)
    // the user can still have it through the start-interactive-search
    // key binding...either way we want to provide a searchequal callback
    // NB: If this is enabled a doubleclick event (activate) gets sent 
    //     on a successful search
    gtk_tree_view_set_search_column(m_treeview, WXLISTBOX_DATACOLUMN);
    gtk_tree_view_set_search_equal_func(m_treeview, 
       (GtkTreeViewSearchEqualFunc) gtk_listbox_searchequal_callback,
                                        this,
                                        NULL);

    gtk_tree_view_set_enable_search(m_treeview, FALSE);


    GtkTreeSelection* selection = gtk_tree_view_get_selection( m_treeview );
    gtk_tree_selection_set_select_function(selection, 
                     (GtkTreeSelectionFunc)gtk_listitem_select_cb,
                                           this, NULL); //NULL == destroycb         

    GtkSelectionMode mode;
    if (style & wxLB_MULTIPLE)
    {
        mode = GTK_SELECTION_MULTIPLE;
    }
    else if (style & wxLB_EXTENDED)
    {
        mode = GTK_SELECTION_EXTENDED;
    }
    else
    {
        // if style was 0 set single mode
        m_windowStyle |= wxLB_SINGLE;
        mode = GTK_SELECTION_SINGLE;
    }

    gtk_tree_selection_set_mode( selection, mode );

    //Handle sortable stuff 
    if(style & wxLB_SORT)
    {
        //Setup sorting in ascending (wx) order
        gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(m_liststore), 
                                             WXLISTBOX_DATACOLUMN, 
                                             GTK_SORT_ASCENDING);

        //Set the sort callback
        gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(m_liststore),
                                        WXLISTBOX_DATACOLUMN,
                   (GtkTreeIterCompareFunc) gtk_listbox_sort_callback,
                                        this, //userdata
                                        NULL //"destroy notifier"
                                       );
    }


    gtk_container_add (GTK_CONTAINER (m_widget), GTK_WIDGET(m_treeview) );
    
    gtk_widget_show( GTK_WIDGET(m_treeview) );

    wxListBox::DoInsertItems(wxArrayString(n, choices), 0); // insert initial items

    //treeview-specific events
    g_signal_connect(m_treeview, "row-activated",
                     G_CALLBACK(gtk_listbox_row_activated_callback), this);

    // other events
    g_signal_connect (m_treeview, "button_press_event",
                      G_CALLBACK (gtk_listbox_button_press_callback),
                      this);
    g_signal_connect (m_treeview, "key_press_event",
                      G_CALLBACK (gtk_listbox_key_press_callback),
                           this);

    m_parent->DoAddChild( this );

    PostCreation(size);
    SetBestSize(size); // need this too because this is a wxControlWithItems

    return TRUE;
}

wxListBox::~wxListBox()
{
    m_hasVMT = FALSE;

    Clear();
}

// ----------------------------------------------------------------------------
// adding items
// ----------------------------------------------------------------------------

void wxListBox::GtkInsertItems(const wxArrayString& items, 
                        void** clientData, int pos)
{
    wxCHECK_RET( m_treeview != NULL, wxT("invalid listbox") );

    InvalidateBestSize();

    // Create and set column ids and GValues

    size_t nNum = items.GetCount();
    int nCurCount = wxListBox::GetCount();
    wxASSERT_MSG(pos <= nCurCount, wxT("Invalid index passed to wxListBox"));

    GtkTreeIter* pIter;
    if (pos == nCurCount)
    {
        pIter = NULL; // append
    }
    else
    {
        GtkTreeIter iter;
        gboolean res = gtk_tree_model_iter_nth_child(
                        GTK_TREE_MODEL(m_liststore),
                        &iter, NULL, //NULL = parent = get first
                        pos );
        if(!res)
        {
            wxLogSysError(wxT("internal wxListBox error in insertion"));
            return;   
        }

        pIter = &iter;
    }

    for (size_t i = 0; i < nNum; ++i)
    {
        wxString label = items[i];

#if wxUSE_CHECKLISTBOX && !wxUSE_NATIVEGTKCHECKLIST
    if (m_hasCheckBoxes)
    {
        label.Prepend(wxCHECKLBOX_STRING);
    }
#endif // wxUSE_CHECKLISTBOX


        GtkTreeEntry* entry = gtk_tree_entry_new();
        gtk_tree_entry_set_label(entry, wxConvUTF8.cWX2MB(label));
        gtk_tree_entry_set_destroy_func(entry, 
                (GtkTreeEntryDestroy)gtk_tree_entry_destroy_cb,
                            this);

        if (clientData)
            gtk_tree_entry_set_userdata(entry, clientData[i]);

        GtkTreeIter itercur;
        gtk_list_store_insert_before(m_liststore, &itercur, pIter);

#if wxUSE_CHECKLISTBOX && wxUSE_NATIVEGTKCHECKLIST
        if (m_hasCheckBoxes)
        {
            gtk_list_store_set(m_liststore, &itercur, 
                                 0, FALSE, //FALSE == not toggled
                                 1, entry, -1);
        }
        else
#endif
            gtk_list_store_set(m_liststore, &itercur, 
                                 0, entry, -1);

        g_object_unref(G_OBJECT(entry)); //liststore always refs :)
    }
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    GtkInsertItems(items, NULL, pos);
}

int wxListBox::DoAppend( const wxString& item )
{
    InvalidateBestSize();

    //Just call DoInsertItems for now
    //RN: Originally I had gtk_list_store_append etc.
    //    here as an optimization but now the insert
    //    has been streamlined and its quite a bit of code duplication
    int nWhere = wxListBox::GetCount();
    wxArrayString aItems;
    aItems.Add(item);
    wxListBox::DoInsertItems(aItems, nWhere);
    return nWhere;
}

void wxListBox::DoSetItems( const wxArrayString& items,
                            void **clientData)
{
    Clear();
    GtkInsertItems(items, clientData, 0);
}

// ----------------------------------------------------------------------------
// deleting items
// ----------------------------------------------------------------------------

void wxListBox::Clear()
{
    wxCHECK_RET( m_treeview != NULL, wxT("invalid listbox") );

    gtk_list_store_clear( m_liststore ); /* well, THAT was easy :) */
}

void wxListBox::Delete( int n )
{
    wxCHECK_RET( m_treeview != NULL, wxT("invalid listbox") );

    GtkTreeIter iter;
    gboolean res = gtk_tree_model_iter_nth_child(
                        GTK_TREE_MODEL(m_liststore),
                        &iter, NULL, //NULL = parent = get first
                        n
                                                );

    wxCHECK_RET( res, wxT("wrong listbox index") );

    //this returns false if iter is invalid (i.e. deleting item
    //at end) but since we don't use iter, well... :)
    gtk_list_store_remove(m_liststore, &iter);
}

// ----------------------------------------------------------------------------
// get GtkTreeEntry from position (note: you need to g_unref it if valid)
// ----------------------------------------------------------------------------

struct _GtkTreeEntry* wxListBox::GtkGetEntry(int n) const
{
    GtkTreeIter iter;
    gboolean res = gtk_tree_model_iter_nth_child(
                        GTK_TREE_MODEL(m_liststore),
                        &iter, NULL, //NULL = parent = get first
                        n );

    if (!res)
    {
        wxLogDebug(wxT("gtk_tree_model_iter_nth_child failed\n")
                   wxT("Passed in value was:[%i]  List size:[%i]"),
                   n, wxListBox::GetCount() );
        return NULL;
    }


    GtkTreeEntry* entry = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(m_liststore), &iter,
                       WXLISTBOX_DATACOLUMN, &entry, -1);

    return entry;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void* wxListBox::DoGetItemClientData( int n ) const
{
    wxCHECK_MSG( n >= 0 && n < wxListBox::GetCount(), NULL, 
                 wxT("Invalid index passed to GetItemClientData") );

    GtkTreeEntry* entry = GtkGetEntry(n);
    wxCHECK_MSG(entry, NULL, wxT("could not get entry"));

    void* userdata = gtk_tree_entry_get_userdata( entry );
    g_object_unref(G_OBJECT(entry));
    return userdata;
}

wxClientData* wxListBox::DoGetItemClientObject( int n ) const
{
    return (wxClientData*) wxListBox::DoGetItemClientData(n);
}

void wxListBox::DoSetItemClientData( int n, void* clientData )
{
    wxCHECK_RET( n >= 0 && n < wxListBox::GetCount(), 
                 wxT("Invalid index passed to SetItemClientData") );

    GtkTreeEntry* entry = GtkGetEntry(n);
    wxCHECK_RET(entry, wxT("could not get entry"));

    gtk_tree_entry_set_userdata( entry, clientData );
    g_object_unref(G_OBJECT(entry));
}

void wxListBox::DoSetItemClientObject( int n, wxClientData* clientData )
{
    // wxItemContainer already deletes data for us
    wxListBox::DoSetItemClientData(n, (void*) clientData);
}

// ----------------------------------------------------------------------------
// string list access
// ----------------------------------------------------------------------------

void wxListBox::SetString( int n, const wxString &string )
{
    wxCHECK_RET( m_treeview != NULL, wxT("invalid listbox") );

    GtkTreeEntry* entry = GtkGetEntry(n);
    wxCHECK_RET( entry, wxT("wrong listbox index") );

    wxString label = string;

#if wxUSE_CHECKLISTBOX && !wxUSE_NATIVEGTKCHECKLIST
        if (m_hasCheckBoxes)
            label.Prepend(wxCHECKLBOX_STRING);
#endif // wxUSE_CHECKLISTBOX

    // RN: This may look wierd but the problem is that the TreeView 
    // doesn't resort or update when changed above and there is no real
    // notification function...
    void* userdata = gtk_tree_entry_get_userdata(entry);
    gtk_tree_entry_set_userdata(entry, NULL); //don't delete on destroy
    g_object_unref(G_OBJECT(entry));

    bool bWasSelected = wxListBox::IsSelected(n);
    wxListBox::Delete(n);

    wxArrayString aItems;
    aItems.Add(label);
    GtkInsertItems(aItems, &userdata, n);
    if (bWasSelected)
        wxListBox::GtkSetSelection(n, TRUE, TRUE);
}

wxString wxListBox::GetString( int n ) const
{
    wxCHECK_MSG( m_treeview != NULL, wxEmptyString, wxT("invalid listbox") );

    GtkTreeEntry* entry = GtkGetEntry(n);
    wxCHECK_MSG( entry, wxEmptyString, wxT("wrong listbox index") );

    wxString label = wxGTK_CONV_BACK( gtk_tree_entry_get_label(entry) );

#if wxUSE_CHECKLISTBOX && !wxUSE_NATIVEGTKCHECKLIST
    // checklistboxes have "[±] " prepended to their lables, remove it
    //
    // NB: 4 below is the length of wxCHECKLBOX_STRING from wx/gtk/checklst.h
    if ( m_hasCheckBoxes )
        label.erase(0, 4);
#endif // wxUSE_CHECKLISTBOX

    g_object_unref(G_OBJECT(entry));
    return label;
}

int wxListBox::GetCount() const
{
    wxCHECK_MSG( m_treeview != NULL, -1, wxT("invalid listbox") );

    return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(m_liststore), NULL);
}

int wxListBox::FindString( const wxString &item, bool bCase ) const
{
    wxCHECK_MSG( m_treeview != NULL, wxNOT_FOUND, wxT("invalid listbox") );

    //Sort of hackish - maybe there is a faster way
    int nCount = wxListBox::GetCount();

    for(int i = 0; i < nCount; ++i)
    {
        if( item.IsSameAs( wxListBox::GetString(i), bCase ) )
            return i;
    }


    // it's not an error if the string is not found -> no wxCHECK
    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( m_treeview != NULL, -1, wxT("invalid listbox"));
    wxCHECK_MSG( HasFlag(wxLB_SINGLE), -1, 
                    wxT("must be single selection listbox"));

    GtkTreeIter iter;    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview);

    // only works on single-sel    
    if (!gtk_tree_selection_get_selected(selection, NULL, &iter))
        return -1;

    GtkTreePath* path =
        gtk_tree_model_get_path(GTK_TREE_MODEL(m_liststore), &iter);

    int sel = gtk_tree_path_get_indices(path)[0];

    gtk_tree_path_free(path);

    return sel;
}

int wxListBox::GetSelections( wxArrayInt& aSelections ) const
{
    wxCHECK_MSG( m_treeview != NULL, -1, wxT("invalid listbox") );

    aSelections.Empty();

        int i = 0;
    GtkTreeIter iter;    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview);

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(m_liststore), &iter))
    { //gtk_tree_selection_get_selected_rows is GTK 2.2+ so iter instead
        do
        {
            if (gtk_tree_selection_iter_is_selected(selection, &iter))
                aSelections.Add(i);

            i++;
        } while(gtk_tree_model_iter_next(GTK_TREE_MODEL(m_liststore), &iter));
    }

    return aSelections.GetCount();
}

bool wxListBox::IsSelected( int n ) const
{
    wxCHECK_MSG( m_treeview != NULL, FALSE, wxT("invalid listbox") );

    GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview);

    GtkTreeIter iter;
    gboolean res = gtk_tree_model_iter_nth_child(
                        GTK_TREE_MODEL(m_liststore),
                        &iter, NULL, //NULL = parent = get first
                        n );
                        
    wxCHECK_MSG( res, FALSE, wxT("Invalid index") );

    return gtk_tree_selection_iter_is_selected(selection, &iter);
}

void wxListBox::DoSetSelection( int n, bool select )
{
    return GtkSetSelection(n, select, TRUE); //docs say no events here
}

void wxListBox::GtkSetSelection(int n, const bool select, const bool blockEvent)
{
    wxCHECK_RET( m_treeview != NULL, wxT("invalid listbox") );

    GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview);

    GtkTreeIter iter;
    gboolean res = gtk_tree_model_iter_nth_child(
                        GTK_TREE_MODEL(m_liststore),
                        &iter, NULL, //NULL = parent = get first
                        n
                                                );
    wxCHECK_RET( res, wxT("Invalid index") );

    m_blockEvent = blockEvent;

    if (select)
        gtk_tree_selection_select_iter(selection, &iter);
    else
        gtk_tree_selection_unselect_iter(selection, &iter);

    m_blockEvent = FALSE;
}

void wxListBox::DoSetFirstItem( int n )
{
    wxCHECK_RET( m_treeview, wxT("invalid listbox") );
    wxCHECK_RET( n >= 0 && n < wxListBox::GetCount(), wxT("invalid index"));

    //RN: I have no idea why this line is needed...
    if (gdk_pointer_is_grabbed () && GTK_WIDGET_HAS_GRAB (m_treeview))
        return;

    // terribly efficient (RN:???)
    // RN: Note that evidently the vadjustment property "vadjustment" from
    // GtkTreeView is different from the "gtk-vadjustment"...
    // (i.e. gtk_tree_view_get_vadjustment)
    const gchar *vadjustment_key = "gtk-vadjustment";
    guint vadjustment_key_id = g_quark_from_static_string (vadjustment_key);

    GtkAdjustment *adjustment =
       (GtkAdjustment*) g_object_get_qdata(G_OBJECT (m_treeview), vadjustment_key_id);
    wxCHECK_RET( adjustment, wxT("invalid listbox code") );

    // Get the greater of the item heights from each column
    gint cellheight = 0, cellheightcur;
    GList* columnlist = gtk_tree_view_get_columns(m_treeview);
    GList* curlist = columnlist;

    while(curlist)
    {
        gtk_tree_view_column_cell_get_size(
            GTK_TREE_VIEW_COLUMN(curlist->data),
            NULL, NULL, NULL, NULL,
            &cellheightcur);

        cellheight = cellheightcur > cellheight ? 
                      cellheightcur : cellheight;

        curlist = curlist->next;
    }

    g_list_free(columnlist);

    float y = (float) (cellheight * n);
    if (y > adjustment->upper - adjustment->page_size)
        y = adjustment->upper - adjustment->page_size;
    gtk_adjustment_set_value( adjustment, y );
}


// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS
void wxListBox::ApplyToolTip( GtkTooltips *tips, const wxChar *tip )
{
    // RN: Is this needed anymore?
    gtk_tooltips_set_tip( tips, GTK_WIDGET( m_treeview ), wxGTK_CONV(tip), (gchar*) NULL );
}
#endif // wxUSE_TOOLTIPS

GtkWidget *wxListBox::GetConnectWidget()
{
    // return GTK_WIDGET(m_treeview);
    return m_widget;
}

bool wxListBox::IsOwnGtkWindow( GdkWindow *window )
{
    return (window == GetGtkBinWindow());
}

GdkWindow* wxListBox::GetGtkBinWindow()
{
    // GtkTreeView has 2 GdkWindows (just like GtkPizza and
    // any other GtkWidget that can scroll). The one on top
    // is usually called "->bin_window" and is the one we are
    // looking for here. Since GtkTreeView hides this in the
    // private data section, we assume that "->window" only has
    // one child window, namely, the "->bin_window". 
    
    GdkWindow *window = GTK_WIDGET(m_treeview)->window;
    if (window == NULL)
        return NULL;
    
    GList *children = gdk_window_peek_children( window );
    if (g_list_length(children) == 0)
        return NULL;
        
    GdkWindow *bin_window = (GdkWindow*) g_list_nth_data( children, 0 );
    return bin_window;
}

void wxListBox::DoApplyWidgetStyle(GtkRcStyle *style)
{
    if (m_hasBgCol && m_backgroundColour.Ok())
    {
        GdkWindow *window = GetGtkBinWindow();
        if (window)
        {
            m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
            gdk_window_set_background( window, m_backgroundColour.GetColor() );
            gdk_window_clear( window );
        }
    }

    gtk_widget_modify_style( GTK_WIDGET(m_treeview), style );
}

void wxListBox::OnInternalIdle()
{
    //RN: Is this needed anymore?
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (GTK_WIDGET(m_treeview)->window && cursor.Ok())
    {
        /* I now set the cursor the anew in every OnInternalIdle call
           as setting the cursor in a parent window also effects the
           windows above so that checking for the current cursor is
           not possible. */
        GdkWindow *window = GetGtkBinWindow();
        gdk_window_set_cursor( window, cursor.GetCursor() );
    }

    if (wxUpdateUIEvent::CanUpdate(this))
        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
}

wxSize wxListBox::DoGetBestSize() const
{
    wxCHECK_MSG(m_treeview, wxDefaultSize, wxT("invalid tree view"));

    int lbWidth;
    int lbHeight;

    // Get the visible area of the tree view
    GdkRectangle rect;
    gtk_tree_view_get_visible_rect(m_treeview, &rect);
    lbWidth = rect.width;
    lbHeight = rect.height;

    // Add room for the scrollbar
    lbWidth += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // And just a bit more
    int cx, cy;
    GetTextExtent( wxT("X"), &cx, &cy);
    lbWidth += 3 * cx;

    // don't make the listbox too tall (limit height to around 10 items) but don't
    // make it too small neither
    lbHeight = (cy+4) * wxMin(wxMax(GetCount(), 3), 10);

    wxSize best(lbWidth, lbHeight);
    CacheBestSize(best);
    return best;
}

// static
wxVisualAttributes
wxListBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_tree_view_new, true);
}

#endif // wxUSE_LISTBOX
