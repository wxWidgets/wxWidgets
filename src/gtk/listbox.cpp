/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/listbox.h"

#if wxUSE_LISTBOX

#include "wx/dynarray.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/checklst.h"
#include "wx/settings.h"

#if wxUSE_TOOLTIPS
#include "wx/tooltip.h"
#endif

#if wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-------------------------------------------------------------------------
// conditional compilation
//-------------------------------------------------------------------------

#if (GTK_MINOR_VERSION > 0)
    #define NEW_GTK_SCROLL_CODE
#endif

//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

#if wxUSE_CHECKLISTBOX

#define CHECKBOX_STRING "[-] "

// checklistboxes have "[±] " prepended to their lables, this macro removes it
// (NB: 4 below is the length of CHECKBOX_STRING above)
//
// the argument to it is a "const char *" pointer
#define GET_REAL_LABEL(label) ((m_hasCheckBoxes)?(label)+4 : (label))

#else // !wxUSE_CHECKLISTBOX

#define GET_REAL_LABEL(label) (label)

#endif // wxUSE_CHECKLISTBOX

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;
extern bool       g_blockEventsOnScroll;
extern wxCursor   g_globalCursor;

static bool       g_hasDoubleClicked = FALSE;

//-----------------------------------------------------------------------------
// "button_release_event"
//-----------------------------------------------------------------------------

/* we would normally emit a wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event once
   a GDK_2BUTTON_PRESS occurs, but this has the particular problem of the
   listbox keeping the focus until it receives a GDK_BUTTON_RELEASE event.
   this can lead to race conditions so that we emit the dclick event
   after the GDK_BUTTON_RELEASE event after the GDK_2BUTTON_PRESS event */

static gint
gtk_listbox_button_release_callback( GtkWidget * WXUNUSED(widget),
                                     GdkEventButton * WXUNUSED(gdk_event),
                                     wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;
    if (g_blockEventsOnScroll) return FALSE;

    if (!listbox->m_hasVMT) return FALSE;

    if (!g_hasDoubleClicked) return FALSE;

        wxCommandEvent event( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, listbox->GetId() );
        event.SetEventObject( listbox );

        wxArrayInt aSelections;
        int count = listbox->GetSelections(aSelections);
        if ( count > 0 )
        {
            event.m_commandInt = aSelections[0] ;
            event.m_clientData = listbox->GetClientData( event.m_commandInt );
            wxString str(listbox->GetString(event.m_commandInt));
            if (!str.IsEmpty()) event.m_commandString = str;
        }
        else
        {
            event.m_commandInt = -1 ;
            event.m_commandString.Empty();
        }

        listbox->GetEventHandler()->ProcessEvent( event );

    return FALSE;
}

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

static gint
gtk_listbox_button_press_callback( GtkWidget *widget,
                                   GdkEventButton *gdk_event,
                                   wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;
    if (g_blockEventsOnScroll) return FALSE;

    if (!listbox->m_hasVMT) return FALSE;

    int sel = listbox->GetIndex( widget );

#if wxUSE_CHECKLISTBOX
    if ((listbox->m_hasCheckBoxes) && (gdk_event->x < 15) && (gdk_event->type != GDK_2BUTTON_PRESS))
    {
        wxCheckListBox *clb = (wxCheckListBox *)listbox;

        clb->Check( sel, !clb->IsChecked(sel) );

        wxCommandEvent event( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, listbox->GetId() );
        event.SetEventObject( listbox );
        event.SetInt( sel );
        listbox->GetEventHandler()->ProcessEvent( event );
    }
#endif // wxUSE_CHECKLISTBOX

    /* emit wxEVT_COMMAND_LISTBOX_DOUBLECLICKED later */
    g_hasDoubleClicked = (gdk_event->type == GDK_2BUTTON_PRESS);

    return FALSE;
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

#if wxUSE_CHECKLISTBOX
static gint
gtk_listbox_key_press_callback( GtkWidget *widget, GdkEventKey *gdk_event, wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;

    if (!listbox->m_hasVMT) return FALSE;

    if (gdk_event->keyval != ' ') return FALSE;

    int sel = listbox->GetIndex( widget );

    wxCheckListBox *clb = (wxCheckListBox *)listbox;

    clb->Check( sel, !clb->IsChecked(sel) );

    wxCommandEvent event( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, listbox->GetId() );
    event.SetEventObject( listbox );
    event.SetInt( sel );
    listbox->GetEventHandler()->ProcessEvent( event );

    return FALSE;
}
#endif // wxUSE_CHECKLISTBOX

//-----------------------------------------------------------------------------
// "select" and "deselect"
//-----------------------------------------------------------------------------

static void gtk_listitem_select_callback( GtkWidget *WXUNUSED(widget), wxListBox *listbox );

static void gtk_listitem_deselect_callback( GtkWidget *widget, wxListBox *listbox )
{
    gtk_listitem_select_callback( widget, listbox );
}

static void gtk_listitem_select_callback( GtkWidget *WXUNUSED(widget), wxListBox *listbox )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!listbox->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, listbox->GetId() );

    wxArrayInt aSelections;
    int count = listbox->GetSelections(aSelections);
    if ( count > 0 )
    {
        event.m_commandInt = aSelections[0] ;
        event.m_clientData = listbox->GetClientData( event.m_commandInt );
        wxString str(listbox->GetString(event.m_commandInt));
        if (!str.IsEmpty()) event.m_commandString = str;
    }
    else
    {
        event.m_commandInt = -1 ;
        event.m_commandString.Empty();
    }

    event.SetEventObject( listbox );

    listbox->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox()
{
    m_list = (GtkList *) NULL;
#if wxUSE_CHECKLISTBOX
    m_hasCheckBoxes = FALSE;
#endif // wxUSE_CHECKLISTBOX
}

bool wxListBox::Create( wxWindow *parent, wxWindowID id,
                        const wxPoint &pos, const wxSize &size,
                        int n, const wxString choices[],
                        long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( _T("wxListBox creation failed") );
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

    m_list = GTK_LIST( gtk_list_new() );

    GtkSelectionMode mode = GTK_SELECTION_BROWSE;
    if (style & wxLB_MULTIPLE)
        mode = GTK_SELECTION_MULTIPLE;
    else if (style & wxLB_EXTENDED)
        mode = GTK_SELECTION_EXTENDED;

    gtk_list_set_selection_mode( GTK_LIST(m_list), mode );

#ifdef NEW_GTK_SCROLL_CODE
    gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(m_widget), GTK_WIDGET(m_list) );
#else
    gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_list) );
#endif

    gtk_widget_show( GTK_WIDGET(m_list) );

    wxSize newSize = size;
    if (newSize.x == -1) newSize.x = 100;
    if (newSize.y == -1) newSize.y = 110;
    SetSize( newSize.x, newSize.y );

    for (int i = 0; i < n; i++)
    {
        m_clientDataList.Append( (wxObject*) NULL );
        m_clientObjectList.Append( (wxObject*) NULL );

        GtkWidget *list_item;

        wxString str(choices[i]);
#if wxUSE_CHECKLISTBOX
        if (m_hasCheckBoxes)
        {
            str.Prepend(CHECKBOX_STRING);
        }
#endif // wxUSE_CHECKLISTBOX

        list_item = gtk_list_item_new_with_label( str.mbc_str() );

        gtk_container_add( GTK_CONTAINER(m_list), list_item );

        gtk_signal_connect( GTK_OBJECT(list_item), "select",
          GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

        if (style & wxLB_MULTIPLE)
            gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
              GTK_SIGNAL_FUNC(gtk_listitem_deselect_callback), (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(list_item),
                            "button_press_event",
                            (GtkSignalFunc)gtk_listbox_button_press_callback,
                            (gpointer) this );

        gtk_signal_connect_after( GTK_OBJECT(list_item),
                            "button_release_event",
                            (GtkSignalFunc)gtk_listbox_button_release_callback,
                            (gpointer) this );

#if wxUSE_CHECKLISTBOX
        if (m_hasCheckBoxes)
        {
            gtk_signal_connect( GTK_OBJECT(list_item),
                            "key_press_event",
                            (GtkSignalFunc)gtk_listbox_key_press_callback,
                            (gpointer)this );
        }
#endif // wxUSE_CHECKLISTBOX

        ConnectWidget( list_item );

        gtk_widget_show( list_item );
    }

    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_WINDOW ) );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );

    Show( TRUE );

    return TRUE;
}

wxListBox::~wxListBox()
{
    Clear();
}

void wxListBox::InsertItems(int nItems, const wxString items[], int pos)
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    GList *children = m_list->children;
    int length = g_list_length(children);
    wxCHECK_RET( pos <= length, _T("invalid index in wxListBox::InsertItems") );

    // VZ: it seems that GTK 1.0.6 doesn't has a function to insert an item
    //     into a listbox at the given position, this is why we first delete
    //     all items after this position, then append these items and then
    //     reappend back the old ones.

    // first detach the old items
    int n; // loop var

    if ( pos == length )
    {
        // no need to do anything complicated
        for ( n = 0; n < nItems; n++ )
        {
            Append(items[n]);
        }

        return;
    }

    wxArrayString deletedLabels;
    wxArrayPtrVoid deletedData;
    wxArrayInt deletedChecks;   // only for check list boxes

    GList *child = g_list_nth( children, pos );
    for ( n = 0; child != NULL; n++, child = child->next )
    {
        // save label
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str(GET_REAL_LABEL(label->label),*wxConvCurrent);
        deletedLabels.Add(str);

        // save data
        void *clientData = NULL;
        wxNode *node = NULL;

        if ( n < (int)m_clientObjectList.GetCount() )
            node = m_clientObjectList.Nth( n );

        if ( node )
        {
            clientData = node->GetData();
            m_clientObjectList.DeleteNode( node );
        }

        if ( !clientData )
        {
            if ( n < (int)m_clientDataList.GetCount() )
                node = m_clientDataList.Nth( n );

            if ( node )
            {
                clientData = node->GetData();
                node = m_clientDataList.Nth( n );
            }
        }

        deletedData.Add(clientData);

#if wxUSE_CHECKLISTBOX
        // save check state
        if ( m_hasCheckBoxes )
        {
            deletedChecks.Add(((wxCheckListBox *)this)->IsChecked(pos + n));
        }
#endif // wxUSE_CHECKLISTBOX
    }

    int nDeletedCount = n;

    gtk_list_clear_items( m_list, pos, length );

    // now append the new items
    for ( n = 0; n < nItems; n++ )
    {
        Append(items[n]);
    }

    // and append the old items too
    pos += nItems;  // now the indices are shifter
    for ( n = 0; n < nDeletedCount; n++ )
    {
        Append(deletedLabels[n], deletedData[n]);

#if wxUSE_CHECKLISTBOX
        if ( m_hasCheckBoxes )
        {
            ((wxCheckListBox *)this)->Check(pos + n, (bool)deletedChecks[n]);
        }
#endif // wxUSE_CHECKLISTBOX
    }
}

void wxListBox::AppendCommon( const wxString &item )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    GtkWidget *list_item;

    wxString label(item);
#if wxUSE_CHECKLISTBOX
    if (m_hasCheckBoxes)
    {
        label.Prepend(CHECKBOX_STRING);
    }
#endif // wxUSE_CHECKLISTBOX

    list_item = gtk_list_item_new_with_label( label.mbc_str() );

    gtk_container_add( GTK_CONTAINER(m_list), list_item );

    gtk_signal_connect( GTK_OBJECT(list_item), "select",
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

    if (HasFlag(wxLB_MULTIPLE))
        gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
          GTK_SIGNAL_FUNC(gtk_listitem_deselect_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(list_item),
                        "button_press_event",
                        (GtkSignalFunc)gtk_listbox_button_press_callback,
                        (gpointer) this );

    gtk_signal_connect_after( GTK_OBJECT(list_item),
                        "button_release_event",
                        (GtkSignalFunc)gtk_listbox_button_release_callback,
                        (gpointer) this );

#if wxUSE_CHECKLISTBOX
    if (m_hasCheckBoxes)
    {
       gtk_signal_connect( GTK_OBJECT(list_item),
                           "key_press_event",
                           (GtkSignalFunc)gtk_listbox_key_press_callback,
                           (gpointer)this );
    }
#endif // wxUSE_CHECKLISTBOX

    gtk_widget_show( list_item );

    ConnectWidget( list_item );

    if (GTK_WIDGET_REALIZED(m_widget))
    {
        gtk_widget_realize( list_item );
        gtk_widget_realize( GTK_BIN(list_item)->child );

        //if (m_widgetStyle) ApplyWidgetStyle();
        if (m_widgetStyle) {
            // Apply current widget style to the new list_item
            gtk_widget_set_style( GTK_WIDGET( list_item ), m_widgetStyle );
            GtkBin *bin = GTK_BIN( list_item );
            GtkWidget *label = GTK_WIDGET( bin->child );
            gtk_widget_set_style( label, m_widgetStyle );
        }

#if wxUSE_DRAG_AND_DROP
#ifndef NEW_GTK_DND_CODE
        if (m_dropTarget) m_dropTarget->RegisterWidget( list_item );
#endif
#endif

#if wxUSE_TOOLTIPS
        if (m_tooltip) m_tooltip->Apply( this );
#endif
    }
}

void wxListBox::Append( const wxString &item )
{
    m_clientDataList.Append( (wxObject*) NULL );
    m_clientObjectList.Append( (wxObject*) NULL );

    AppendCommon( item );
}

void wxListBox::Append( const wxString &item, void *clientData )
{
    m_clientDataList.Append( (wxObject*) clientData );
    m_clientObjectList.Append( (wxObject*) NULL );

    AppendCommon( item );
}

void wxListBox::Append( const wxString &item, wxClientData *clientData )
{
    m_clientObjectList.Append( (wxObject*) clientData );
    m_clientDataList.Append( (wxObject*) NULL );

    AppendCommon( item );
}

void wxListBox::SetClientData( int n, void* clientData )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid combobox") );

    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return;

    node->SetData( (wxObject*) clientData );
}

void* wxListBox::GetClientData( int n )
{
    wxCHECK_MSG( m_widget != NULL, NULL, _T("invalid combobox") );

    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return NULL;

    return node->Data();
}

void wxListBox::SetClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid combobox") );

    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return;

    wxClientData *cd = (wxClientData*) node->Data();
    if (cd) delete cd;

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxListBox::GetClientObject( int n )
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*)NULL, _T("invalid combobox") );

    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return (wxClientData*) NULL;

    return (wxClientData*) node->Data();
}

void wxListBox::Clear()
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    gtk_list_clear_items( m_list, 0, Number() );

    wxNode *node = m_clientObjectList.First();
    while (node)
    {
        wxClientData *cd = (wxClientData*)node->Data();
        if (cd) delete cd;
        node = node->Next();
    }
    m_clientObjectList.Clear();

    m_clientDataList.Clear();
}

void wxListBox::Delete( int n )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    GList *child = g_list_nth( m_list->children, n );

    wxCHECK_RET( child, _T("wrong listbox index") );

    GList *list = g_list_append( (GList*) NULL, child->data );
    gtk_list_remove_items( m_list, list );
    g_list_free( list );

    wxNode *node = m_clientObjectList.Nth( n );
    if (node)
    {
        wxClientData *cd = (wxClientData*)node->Data();
        if (cd) delete cd;
        m_clientObjectList.DeleteNode( node );
    }

    node = m_clientDataList.Nth( n );
    if (node)
    {
        m_clientDataList.DeleteNode( node );
    }
}

void wxListBox::Deselect( int n )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    DisableEvents();

    gtk_list_unselect_item( m_list, n );

    EnableEvents();
}

int wxListBox::FindString( const wxString &item ) const
{
    wxCHECK_MSG( m_list != NULL, -1, _T("invalid listbox") );

    GList *child = m_list->children;
    int count = 0;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str = wxString(GET_REAL_LABEL(label->label),*wxConvCurrent);

        if (str == item)
            return count;

        count++;
        child = child->next;
    }

  // it's not an error if the string is not found -> no wxCHECK

  return -1;
}

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( m_list != NULL, -1, _T("invalid listbox") );

    GList *child = m_list->children;
    int count = 0;
    while (child)
    {
        if (GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED) return count;
        count++;
        child = child->next;
    }
    return -1;
}

int wxListBox::GetSelections( wxArrayInt& aSelections ) const
{
    wxCHECK_MSG( m_list != NULL, -1, _T("invalid listbox") );

    // get the number of selected items first
    GList *child = m_list->children;
    int count = 0;
    for (child = m_list->children; child != NULL; child = child->next)
    {
        if (GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED)
            count++;
    }

    aSelections.Empty();

    if (count > 0)
    {
        // now fill the list
        aSelections.Alloc(count); // optimization attempt
        int i = 0;
        for (child = m_list->children; child != NULL; child = child->next, i++)
        {
            if (GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED)
                 aSelections.Add(i);
        }
    }

    return count;
}

wxString wxListBox::GetString( int n ) const
{
    wxCHECK_MSG( m_list != NULL, _T(""), _T("invalid listbox") );

    GList *child = g_list_nth( m_list->children, n );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str = wxString(GET_REAL_LABEL(label->label),*wxConvCurrent);

        return str;
    }

    wxFAIL_MSG(_T("wrong listbox index"));

    return _T("");
}

wxString wxListBox::GetStringSelection() const
{
    wxCHECK_MSG( m_list != NULL, _T(""), _T("invalid listbox") );

    GList *selection = m_list->selection;
    if (selection)
    {
        GtkBin *bin = GTK_BIN( selection->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str = wxString(GET_REAL_LABEL(label->label),*wxConvCurrent);

        return str;
    }

    wxFAIL_MSG(_T("no listbox selection available"));
    return _T("");
}

int wxListBox::Number()
{
    wxCHECK_MSG( m_list != NULL, -1, _T("invalid listbox") );

    GList *child = m_list->children;
    int count = 0;
    while (child) { count++; child = child->next; }
    return count;
}

bool wxListBox::Selected( int n )
{
    wxCHECK_MSG( m_list != NULL, FALSE, _T("invalid listbox") );

    GList *target = g_list_nth( m_list->children, n );
    if (target)
    {
        GList *child = m_list->selection;
        while (child)
        {
            if (child->data == target->data) return TRUE;
            child = child->next;
        }
    }
    wxFAIL_MSG(_T("wrong listbox index"));
    return FALSE;
}

void wxListBox::Set( int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
    wxFAIL_MSG(_T("wxListBox::Set not implemented"));
}

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
    wxFAIL_MSG(_T("wxListBox::SetFirstItem not implemented"));
}

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
    wxFAIL_MSG(_T("wxListBox::SetFirstItem not implemented"));
}

void wxListBox::SetSelection( int n, bool select )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    DisableEvents();

    if (select)
        gtk_list_select_item( m_list, n );
    else
        gtk_list_unselect_item( m_list, n );

    EnableEvents();
}

void wxListBox::SetString( int n, const wxString &string )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    GList *child = g_list_nth( m_list->children, n );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str;
#if wxUSE_CHECKLISTBOX
        if (m_hasCheckBoxes)
            str += CHECKBOX_STRING;
#endif // wxUSE_CHECKLISTBOX
        str += string;

        gtk_label_set( label, str.mbc_str() );
    }
    else
    {
        wxFAIL_MSG(_T("wrong listbox index"));
    }
}

void wxListBox::SetStringSelection( const wxString &string, bool select )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

    SetSelection( FindString(string), select );
}

int wxListBox::GetIndex( GtkWidget *item ) const
{
    if (item)
    {
        GList *child = m_list->children;
        int count = 0;
        while (child)
        {
            if (GTK_WIDGET(child->data) == item) return count;
            count++;
            child = child->next;
        }
    }
    return -1;
}

#if wxUSE_TOOLTIPS
void wxListBox::ApplyToolTip( GtkTooltips *tips, const wxChar *tip )
{
    GList *child = m_list->children;
    while (child)
    {
        gtk_tooltips_set_tip( tips, GTK_WIDGET( child->data ), wxConvCurrent->cWX2MB(tip), (gchar*) NULL );
        child = child->next;
    }
}
#endif // wxUSE_TOOLTIPS

#if wxUSE_DRAG_AND_DROP
void wxListBox::SetDropTarget( wxDropTarget *dropTarget )
{
    wxCHECK_RET( m_list != NULL, _T("invalid listbox") );

#ifndef NEW_GTK_DND_CODE
    if (m_dropTarget)
    {
        GList *child = m_list->children;
        while (child)
        {
            m_dropTarget->UnregisterWidget( GTK_WIDGET( child->data ) );
            child = child->next;
        }
    }
#endif

    wxWindow::SetDropTarget( dropTarget );

#ifndef NEW_GTK_DND_CODE
    if (m_dropTarget)
    {
        GList *child = m_list->children;
        while (child)
        {
            m_dropTarget->RegisterWidget( GTK_WIDGET( child->data ) );
            child = child->next;
        }
    }
#endif
}
#endif

void wxListBox::DisableEvents()
{
    GList *child = m_list->children;
    while (child)
    {
        gtk_signal_disconnect_by_func( GTK_OBJECT(child->data),
          GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

        if (HasFlag(wxLB_MULTIPLE))
            gtk_signal_disconnect_by_func( GTK_OBJECT(child->data),
              GTK_SIGNAL_FUNC(gtk_listitem_deselect_callback), (gpointer)this );

        child = child->next;
    }
}

void wxListBox::EnableEvents()
{
    GList *child = m_list->children;
    while (child)
    {
        gtk_signal_connect( GTK_OBJECT(child->data), "select",
          GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

        if (HasFlag(wxLB_MULTIPLE))
            gtk_signal_connect( GTK_OBJECT(child->data), "deselect",
              GTK_SIGNAL_FUNC(gtk_listitem_deselect_callback), (gpointer)this );

        child = child->next;
    }
}

GtkWidget *wxListBox::GetConnectWidget()
{
    return GTK_WIDGET(m_list);
}

bool wxListBox::IsOwnGtkWindow( GdkWindow *window )
{
    if (wxWindow::IsOwnGtkWindow( window )) return TRUE;

    GList *child = m_list->children;
    while (child)
    {
        GtkWidget *bin = GTK_WIDGET( child->data );
        if (bin->window == window) return TRUE;
        child = child->next;
    }

    return FALSE;
}

void wxListBox::ApplyWidgetStyle()
{
    SetWidgetStyle();

    if (m_backgroundColour.Ok())
    {
        GdkWindow *window = GTK_WIDGET(m_list)->window;
        if ( window )
        {
            m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
            gdk_window_set_background( window, m_backgroundColour.GetColor() );
            gdk_window_clear( window );
        }
    }

    GList *child = m_list->children;
    while (child)
    {
        gtk_widget_set_style( GTK_WIDGET(child->data), m_widgetStyle );

        GtkBin *bin = GTK_BIN( child->data );
        GtkWidget *label = GTK_WIDGET( bin->child );
        gtk_widget_set_style( label, m_widgetStyle );

        child = child->next;
    }
}

void wxListBox::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (m_widget->window && cursor.Ok() && m_currentGdkCursor != cursor)
    {
        m_currentGdkCursor = cursor;
	
	gdk_window_set_cursor( GTK_WIDGET(m_list)->window, m_currentGdkCursor.GetCursor() );

        GList *child = m_list->children;
        while (child)
        {
            GtkBin *bin = GTK_BIN( child->data );
            GtkWidget *label = GTK_WIDGET( bin->child );
	    
	    gdk_window_set_cursor( label->window, m_currentGdkCursor.GetCursor() );

            child = child->next;
        }
    }

    UpdateWindowUI();
}

#endif
