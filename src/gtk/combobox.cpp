/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"    // for wxEVT_COMMAND_TEXT_UPDATED
    #include "wx/arrstr.h"
#endif

#include "wx/gtk/private.h"

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C" {
static void
gtkcombobox_text_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (!combo->m_hasVMT) return;

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
    event.SetString( combo->GetValue() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}

static void
gtkcombobox_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (!combo->m_hasVMT) return;

    if (combo->GetSelection() == -1)
        return;

    wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, combo->GetId() );
    event.SetInt( combo->GetSelection() );
    event.SetString( combo->GetStringSelection() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}
}

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

BEGIN_EVENT_TABLE(wxComboBox, wxControl)
    EVT_CHAR(wxComboBox::OnChar)

    EVT_MENU(wxID_CUT, wxComboBox::OnCut)
    EVT_MENU(wxID_COPY, wxComboBox::OnCopy)
    EVT_MENU(wxID_PASTE, wxComboBox::OnPaste)
    EVT_MENU(wxID_UNDO, wxComboBox::OnUndo)
    EVT_MENU(wxID_REDO, wxComboBox::OnRedo)
    EVT_MENU(wxID_CLEAR, wxComboBox::OnDelete)
    EVT_MENU(wxID_SELECTALL, wxComboBox::OnSelectAll)

    EVT_UPDATE_UI(wxID_CUT, wxComboBox::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxComboBox::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxComboBox::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxComboBox::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxComboBox::OnUpdateRedo)
    EVT_UPDATE_UI(wxID_CLEAR, wxComboBox::OnUpdateDelete)
    EVT_UPDATE_UI(wxID_SELECTALL, wxComboBox::OnUpdateSelectAll)
END_EVENT_TABLE()

bool wxComboBox::Create( wxWindow *parent, wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos, const wxSize& size,
                         const wxArrayString& choices,
                         long style, const wxValidator& validator,
                         const wxString& name )
{
    wxCArrayString chs(choices);

    return Create( parent, id, value, pos, size, chs.GetCount(),
                   chs.GetStrings(), style, validator, name );
}

bool wxComboBox::Create( wxWindow *parent, wxWindowID id, const wxString& value,
                         const wxPoint& pos, const wxSize& size,
                         int n, const wxString choices[],
                         long style, const wxValidator& validator,
                         const wxString& name )
{
    m_strings = NULL;
    m_ignoreNextUpdate = false;
    m_prevSelection = 0;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxComboBox creation failed") );
        return false;
    }

    if(HasFlag(wxCB_SORT))
        m_strings = new wxSortedArrayString();

    m_widget = gtk_combo_box_entry_new_text();

    GtkEntry * const entry = GetEntry();

    gtk_entry_set_editable( entry, TRUE );

    Append(n, choices);

    m_parent->DoAddChild( this );

    m_focusWidget = GTK_WIDGET( entry );

    PostCreation(size);

    ConnectWidget( m_widget );

    gtk_entry_set_text( entry, wxGTK_CONV(value) );

    if (style & wxCB_READONLY)
        gtk_entry_set_editable( entry, FALSE );

    g_signal_connect_after (entry, "changed",
                        G_CALLBACK (gtkcombobox_text_changed_callback), this);

    g_signal_connect_after (m_widget, "changed",
                        G_CALLBACK (gtkcombobox_changed_callback), this);


    SetInitialSize(size); // need this too because this is a wxControlWithItems

    return true;
}

GtkEntry *wxComboBox::GetEntry() const
{
    return GTK_ENTRY(GTK_BIN(m_widget)->child);
}

GtkEditable *wxComboBox::GetEditable() const
{
    return GTK_EDITABLE( GTK_BIN(m_widget)->child );
}

wxComboBox::~wxComboBox()
{
    Clear();

    delete m_strings;
}

void wxComboBox::SetFocus()
{
    if ( m_hasFocus )
    {
        // don't do anything if we already have focus
        return;
    }

    gtk_widget_grab_focus( m_focusWidget );
}

int wxComboBox::DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type)
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

    wxASSERT_MSG( !IsSorted() || (pos == GetCount()),
                 _T("In a sorted combobox data could only be appended"));

    const int count = items.GetCount();

    int n = wxNOT_FOUND;

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    for( int i = 0; i < count; ++i )
    {
        n = pos + i;
        // If sorted, use this wxSortedArrayStrings to determine
        // the right insertion point
        if(m_strings)
            n = m_strings->Add(items[i]);

        gtk_combo_box_insert_text( combobox, n, wxGTK_CONV( items[i] ) );

        m_clientData.Insert( NULL, n );
        AssignNewItemClientData(n, clientData, i, type);
    }

    InvalidateBestSize();

    return n;
}

void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_clientData[n] = clientData;
}

void* wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return m_clientData[n];
}

void wxComboBox::DoClear()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    const unsigned int count = GetCount();
    for (unsigned int i = 0; i < count; i++)
        gtk_combo_box_remove_text( combobox, 0 );

    m_clientData.Clear();

    if(m_strings)
        m_strings->Clear();

    EnableEvents();

    InvalidateBestSize();
}

void wxComboBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    wxCHECK_RET( IsValid(n), wxT("invalid index") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    gtk_combo_box_remove_text( combobox, n );

    m_clientData.RemoveAt( n );
    if(m_strings)
        m_strings->RemoveAt( n );

    InvalidateBestSize();
}

void wxComboBox::SetString(unsigned int n, const wxString &text)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    wxCHECK_RET( IsValid(n), wxT("invalid index") );

    GtkTreeModel *model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    if (gtk_tree_model_iter_nth_child (model, &iter, NULL, n))
    {
        GValue value = { 0, };
        g_value_init( &value, G_TYPE_STRING );
        g_value_set_string( &value, wxGTK_CONV( text ) );
        gtk_list_store_set_value( GTK_LIST_STORE(model), &iter, 0, &value );
        g_value_unset( &value );
    }

    InvalidateBestSize();
}

int wxComboBox::FindString( const wxString &item, bool bCase ) const
{
    wxCHECK_MSG( m_widget != NULL, wxNOT_FOUND, wxT("invalid combobox") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel* model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first( model, &iter );
    if (!gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter ))
        return -1;
    int count = 0;
    do
    {
        GValue value = { 0, };
        gtk_tree_model_get_value( model, &iter, 0, &value );
        wxString str = wxGTK_CONV_BACK( g_value_get_string( &value ) );
        g_value_unset( &value );

        if (item.IsSameAs( str, bCase ) )
            return count;

        count++;
    }
    while ( gtk_tree_model_iter_next(model, &iter) );

    return wxNOT_FOUND;
}

int wxComboBox::GetSelection() const
{
    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    return gtk_combo_box_get_active( combobox );
}

int wxComboBox::GetCurrentSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    return gtk_combo_box_get_active( combobox );
}

wxString wxComboBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( m_widget != NULL, wxEmptyString, wxT("invalid combobox") );

    wxString str;

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel *model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    if (gtk_tree_model_iter_nth_child (model, &iter, NULL, n))
    {
        GValue value = { 0, };
        gtk_tree_model_get_value( model, &iter, 0, &value );
        wxString tmp = wxGTK_CONV_BACK( g_value_get_string( &value ) );
        g_value_unset( &value );
        return tmp;
    }

    return str;
}

unsigned int wxComboBox::GetCount() const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid combobox") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel* model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first( model, &iter );
    if (!gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter ))
        return 0;
    unsigned int ret = 1;
    while (gtk_tree_model_iter_next( model, &iter ))
        ret++;
    return ret;
}

void wxComboBox::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    gtk_combo_box_set_active( combobox, n );

    EnableEvents();
}

void wxComboBox::OnChar( wxKeyEvent &event )
{
    switch ( event.GetKeyCode() )
    {
        case WXK_RETURN:
            if ( HasFlag(wxTE_PROCESS_ENTER) )
            {
                // GTK automatically selects an item if its in the list
                wxCommandEvent eventEnter(wxEVT_COMMAND_TEXT_ENTER, GetId());
                eventEnter.SetString( GetValue() );
                eventEnter.SetInt( GetSelection() );
                eventEnter.SetEventObject( this );

                if ( GetEventHandler()->ProcessEvent(eventEnter) )
                {
                    // Catch GTK event so that GTK doesn't open the drop
                    // down list upon RETURN.
                    return;
                }
            }
            break;
    }

    event.Skip();
}

void wxComboBox::DisableEvents()
{
    g_signal_handlers_block_by_func(GTK_BIN(m_widget)->child,
        (gpointer)gtkcombobox_text_changed_callback, this);

    g_signal_handlers_block_by_func(m_widget,
        (gpointer)gtkcombobox_changed_callback, this);
}

void wxComboBox::EnableEvents()
{
    g_signal_handlers_unblock_by_func(GTK_BIN(m_widget)->child,
        (gpointer)gtkcombobox_text_changed_callback, this);

    g_signal_handlers_unblock_by_func(m_widget,
        (gpointer)gtkcombobox_changed_callback, this);
}

GtkWidget* wxComboBox::GetConnectWidget()
{
    return GTK_WIDGET( GetEntry() );
}

GdkWindow *wxComboBox::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    wxUnusedVar(windows);

    return GetEntry()->text_area;
}

wxSize wxComboBox::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );

    // we know better our horizontal extent: it depends on the longest string
    // in the combobox
    if ( m_widget )
    {
        int width;
        unsigned int count = GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            GetTextExtent(GetString(n), &width, NULL, NULL, NULL );
            if ( width > ret.x )
                ret.x = width;
        }
    }

    // empty combobox should have some reasonable default size too
    if ( ret.x < 100 )
        ret.x = 100;

    CacheBestSize(ret);
    return ret;
}

// static
wxVisualAttributes
wxComboBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_combo_box_entry_new, true);
}

// ----------------------------------------------------------------------------
// standard event handling
// ----------------------------------------------------------------------------

void wxComboBox::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxComboBox::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxComboBox::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxComboBox::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxComboBox::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxComboBox::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    long from, to;
    GetSelection(& from, & to);
    if (from != -1 && to != -1)
        Remove(from, to);
}

void wxComboBox::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    SetSelection(-1, -1);
}

void wxComboBox::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxComboBox::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxComboBox::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxComboBox::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxComboBox::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

void wxComboBox::OnUpdateDelete(wxUpdateUIEvent& event)
{
    event.Enable(HasSelection() && IsEditable()) ;
}

void wxComboBox::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

#endif // wxUSE_COMBOBOX
