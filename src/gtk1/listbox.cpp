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

#include "wx/dynarray.h"
#include "wx/listbox.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/checklst.h"

//-------------------------------------------------------------------------
// conditional compilation
//-------------------------------------------------------------------------

#if (GTK_MINOR_VERSION == 1)
#if (GTK_MICRO_VERSION >= 5)
#define NEW_GTK_SCROLL_CODE
#endif
#endif

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;
extern bool   g_blockEventsOnScroll;

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

static gint 
gtk_listbox_button_press_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxListBox *listbox )
{
    if (g_blockEventsOnDrag) return FALSE;
    if (g_blockEventsOnScroll) return FALSE;

    if (!listbox->HasVMT()) return FALSE;

    if (gdk_event->x > 15) return FALSE;
    
    int sel = listbox->GetIndex( widget );
    
    wxCheckListBox *clb = (wxCheckListBox *)listbox;
    
    clb->Check( sel, !clb->IsChecked(sel) );
    
    return FALSE;
}

//-----------------------------------------------------------------------------
// "select" and "deselect"
//-----------------------------------------------------------------------------

static void gtk_listitem_select_callback( GtkWidget *WXUNUSED(widget), wxListBox *listbox )
{
    if (!listbox->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, listbox->GetId() );

    wxArrayInt aSelections;
    int count = listbox->GetSelections(aSelections);
    if ( count > 0 )
    {
        event.m_commandInt = aSelections[0] ;
        event.m_clientData = listbox->GetClientData( event.m_commandInt );
        wxString str(listbox->GetString(event.m_commandInt));
        if (str != "") event.m_commandString = copystring((char *)(const char *)str);
    }
    else
    {
        event.m_commandInt = -1 ;
        event.m_commandString = copystring("") ;
    }

    event.SetEventObject( listbox );

    listbox->GetEventHandler()->ProcessEvent( event );
    if (event.m_commandString) delete[] event.m_commandString ;
}

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox()
{
    m_list = (GtkList *) NULL;
    m_hasCheckBoxes = FALSE;
}

bool wxListBox::Create( wxWindow *parent, wxWindowID id,
                        const wxPoint &pos, const wxSize &size,
                        int n, const wxString choices[],
                        long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;

    PreCreation( parent, id, pos, size, style, name );

    SetValidator( validator );

    m_widget = gtk_scrolled_window_new( (GtkAdjustment*) NULL, (GtkAdjustment*) NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

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
	
	if (m_hasCheckBoxes)
	{
            wxString str = "[-] ";
	    str += choices[i];
            list_item = gtk_list_item_new_with_label( str );
	}
	else
	{
            list_item = gtk_list_item_new_with_label( choices[i] );
	}

        gtk_container_add( GTK_CONTAINER(m_list), list_item );

        gtk_signal_connect( GTK_OBJECT(list_item), "select",
          GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

        if (style & wxLB_MULTIPLE)
            gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
              GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

	if (m_hasCheckBoxes)
	{
            gtk_signal_connect( GTK_OBJECT(list_item), 
	                        "button_press_event",
                                (GtkSignalFunc)gtk_listbox_button_press_callback, 
				(gpointer) this );
	}
	
        ConnectWidget( list_item );	
	
        gtk_widget_show( list_item );
    }

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();

    gtk_widget_realize( GTK_WIDGET(m_list) );

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );

    return TRUE;
}

wxListBox::~wxListBox()
{
    Clear();
}

void wxListBox::AppendCommon( const wxString &item )
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
    GtkWidget *list_item;
    
    if (m_hasCheckBoxes)
    {
        wxString str = "[-] ";
	str += item;
        list_item = gtk_list_item_new_with_label( str );
    }
    else
    {
        list_item = gtk_list_item_new_with_label( item );
    }

    gtk_signal_connect( GTK_OBJECT(list_item), "select",
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

    if (GetWindowStyleFlag() & wxLB_MULTIPLE)
        gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
          GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

    gtk_container_add( GTK_CONTAINER(m_list), list_item );

    if (m_widgetStyle) ApplyWidgetStyle();
  
    if (m_hasCheckBoxes)
    {
            gtk_signal_connect( GTK_OBJECT(list_item), 
	                        "button_press_event",
                                (GtkSignalFunc)gtk_listbox_button_press_callback, 
				(gpointer) this );
    }
	
    gtk_widget_show( list_item );

    ConnectWidget( list_item );

#ifndef NEW_GTK_DND_CODE
    if (m_dropTarget) m_dropTarget->RegisterWidget( list_item );
#endif
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
    wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return;
  
    node->SetData( (wxObject*) clientData );
}

void* wxListBox::GetClientData( int n )
{
    wxCHECK_MSG( m_widget != NULL, NULL, "invalid combobox" );
  
    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return NULL;
    
    return node->Data();
}

void wxListBox::SetClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, "invalid combobox" );
    
    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return;
  
    wxClientData *cd = (wxClientData*) node->Data();
    if (cd) delete cd;
  
    node->SetData( (wxObject*) clientData );
}

wxClientData* wxListBox::GetClientObject( int n )
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*)NULL, "invalid combobox" );
  
    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return (wxClientData*) NULL;
    
    return (wxClientData*) node->Data();
}

void wxListBox::Clear()
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );

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
    wxCHECK_RET( m_list != NULL, "invalid listbox" );

    GList *child = g_list_nth( m_list->children, n );

    wxCHECK_RET( child, "wrong listbox index" );

    GList *list = g_list_append( NULL, child->data );
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
    wxCHECK_RET( m_list != NULL, "invalid listbox" );

    gtk_list_unselect_item( m_list, n );
}

int wxListBox::FindString( const wxString &item ) const
{
    wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
    GList *child = m_list->children;
    int count = 0;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );
	
	wxString str = label->label;
	if (m_hasCheckBoxes) str.Remove( 0, 4 );
	
        if (str == item) return count;
	
        count++;
        child = child->next;
    }

  // it's not an error if the string is not found -> no wxCHECK

  return -1;
}

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
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
    wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
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
    wxCHECK_MSG( m_list != NULL, "", "invalid listbox" );
  
    GList *child = g_list_nth( m_list->children, n );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );
	
	wxString str = label->label;
	if (m_hasCheckBoxes) str.Remove( 0, 4 );
	
        return str;
    }
    wxFAIL_MSG("wrong listbox index");
    return "";
}

wxString wxListBox::GetStringSelection() const
{
    wxCHECK_MSG( m_list != NULL, "", "invalid listbox" );
  
    GList *selection = m_list->selection;
    if (selection)
    {
        GtkBin *bin = GTK_BIN( selection->data );
        GtkLabel *label = GTK_LABEL( bin->child );
	
	wxString str = label->label;
	if (m_hasCheckBoxes) str.Remove( 0, 4 );
	
        return str;
    }
    
    wxFAIL_MSG("no listbox selection available");
    return "";
}

int wxListBox::Number()
{
    wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
    GList *child = m_list->children;
    int count = 0;
    while (child) { count++; child = child->next; }
    return count;
}

bool wxListBox::Selected( int n )
{
    wxCHECK_MSG( m_list != NULL, FALSE, "invalid listbox" );
  
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
    wxFAIL_MSG("wrong listbox index");
    return FALSE;
}

void wxListBox::Set( int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
    wxFAIL_MSG("wxListBox::Set not implemented");
}

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
    wxFAIL_MSG("wxListBox::SetFirstItem not implemented");
}

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
    wxFAIL_MSG("wxListBox::SetFirstItem not implemented");
}

void wxListBox::SetSelection( int n, bool select )
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
    if (select)
        gtk_list_select_item( m_list, n );
    else
        gtk_list_unselect_item( m_list, n );
}

void wxListBox::SetString( int n, const wxString &string )
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
    GList *child = g_list_nth( m_list->children, n );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );
	
	wxString str;
	if (m_hasCheckBoxes) str += "[-] ";
	str += string;
	
        gtk_label_set( label, str );
    }
    else
    {
        wxFAIL_MSG("wrong listbox index");
    } 
}

void wxListBox::SetStringSelection( const wxString &string, bool select )
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
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

void wxListBox::SetDropTarget( wxDropTarget *dropTarget )
{
    wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
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
        m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
        gdk_window_set_background( window, m_backgroundColour.GetColor() );
        gdk_window_clear( window );
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
