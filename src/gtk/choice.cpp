/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/choice.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE || wxUSE_COMBOBOX

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/arrstr.h"
#endif

#include "wx/gtk/private.h"
#include "wx/gtk/private/eventsdisabler.h"
#include "wx/gtk/private/list.h"
#include "wx/gtk/private/value.h"

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C" {

static void
gtk_choice_changed_callback( GtkWidget *WXUNUSED(widget), wxChoice *choice )
{
    choice->SendSelectionChangedEvent(wxEVT_CHOICE);
}

}

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

void wxChoice::Init()
{
    m_strings = nullptr;
    m_stringCellIndex = 0;
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       const wxArrayString& choices,
                       long style, const wxValidator& validator,
                       const wxString &name )
{
    wxCArrayString chs(choices);

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                   style, validator, name );
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       int n, const wxString choices[],
                       long style, const wxValidator& validator,
                       const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxChoice creation failed") );
        return false;
    }

    if ( IsSorted() )
    {
        // if our m_strings != nullptr, Append() will check for it and insert
        // items in the correct order
        m_strings = new wxGtkCollatedArrayString;
    }

#ifdef __WXGTK3__
    m_widget = gtk_combo_box_text_new();

    // If any choices don't fit into the available space (in the always visible
    // part of the control, not the dropdown), GTK shows just the tail of the
    // string which does fit, which is bad for long strings and even worse for
    // the shorter ones, as they may end up being shown as completely blank.
    // Work around this brokenness by enabling ellipsization, especially as it
    // seems to be safe to do it unconditionally, i.e. there doesn't seem to be
    // any ill effects from having it on if everything does fit.
    const wxGtkList cells(gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(m_widget)));
    if (GTK_IS_CELL_RENDERER_TEXT(cells->data))
        g_object_set(G_OBJECT(cells->data), "ellipsize", PANGO_ELLIPSIZE_END, nullptr);
#else
    m_widget = gtk_combo_box_new_text();
#endif
    g_object_ref(m_widget);

    Append(n, choices);

    m_parent->DoAddChild( this );

    PostCreation(size);

    g_signal_connect_after (m_widget, "changed",
                            G_CALLBACK (gtk_choice_changed_callback), this);

    return true;
}

wxChoice::~wxChoice()
{
    Clear();
    delete m_strings;

 #ifdef __WXGTK3__
    // At least with GTK+ 3.22.9, destroying a shown combobox widget results in
    // a Gtk-CRITICAL debug message when the assertion fails inside a signal
    // handler called from gtk_widget_unrealize(), which is annoying, so avoid
    // it by hiding the widget before destroying it -- this doesn't look right,
    // but shouldn't do any harm either.
    Hide();
 #endif // __WXGTK3__
}

bool wxChoice::GTKHandleFocusOut()
{
    if ( wx_is_at_least_gtk2(10) )
    {
        gboolean isShown;
        g_object_get( m_widget, "popup-shown", &isShown, nullptr );

        // Don't send "focus lost" events if the focus is grabbed by our own
        // popup, it counts as part of this window, even though wx doesn't know
        // about it (and can't, because GtkComboBox doesn't expose it).
        if ( isShown )
            return true;
    }

    return wxChoiceBase::GTKHandleFocusOut();
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                            unsigned int pos,
                            void **clientData, wxClientDataType type)
{
    wxCHECK_MSG( m_widget != nullptr, -1, wxT("invalid control") );

    wxASSERT_MSG( !IsSorted() || (pos == GetCount()),
                 wxT("In a sorted choice data could only be appended"));

    const int count = items.GetCount();

    int n = wxNOT_FOUND;

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_combo_box_get_model( GTK_COMBO_BOX( m_widget ) );
    GtkListStore *store = GTK_LIST_STORE( model );

    gtk_widget_freeze_child_notify(m_widget);

    for ( int i = 0; i < count; ++i )
    {
        n = pos + i;
        // If sorted, use this wxSortedArrayStrings to determine
        // the right insertion point
        if (m_strings)
            n = m_strings->Add(items[i]);

        gtk_list_store_insert_with_values(store, &iter, n, m_stringCellIndex,
                                          items[i].utf8_str().data(), -1);

        m_clientData.Insert( nullptr, n );
        AssignNewItemClientData(n, clientData, i, type);
    }

    gtk_widget_thaw_child_notify(m_widget);


    InvalidateBestSize();

    return n;
}

void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_clientData[n] = clientData;
}

void* wxChoice::DoGetItemClientData(unsigned int n) const
{
    return m_clientData[n];
}

void wxChoice::DoClear()
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid control") );

    wxGtkEventsDisabler<wxChoice> noEvents(this);

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel* model = gtk_combo_box_get_model( combobox );
    if (model)
        gtk_list_store_clear(GTK_LIST_STORE(model));

    m_clientData.Clear();

    if (m_strings)
        m_strings->Clear();

    InvalidateBestSize();
}

void wxChoice::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid control") );
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxChoice::Delete") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel* model = gtk_combo_box_get_model( combobox );
    GtkListStore* store = GTK_LIST_STORE(model);
    GtkTreeIter iter;
    if ( !gtk_tree_model_iter_nth_child(model, &iter, nullptr, n) )
    {
        // This is really not supposed to happen for a valid index.
        wxFAIL_MSG(wxS("Item unexpectedly not found."));
        return;
    }
    gtk_list_store_remove( store, &iter );

    m_clientData.RemoveAt( n );
    if ( m_strings )
        m_strings->RemoveAt( n );

    InvalidateBestSize();
}

int wxChoice::FindString( const wxString &item, bool bCase ) const
{
    wxCHECK_MSG( m_widget != nullptr, wxNOT_FOUND, wxT("invalid control") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel* model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first( model, &iter );
    if (!gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter ))
        return -1;
    int count = 0;
    do
    {
        wxGtkValue value;
        gtk_tree_model_get_value( model, &iter, m_stringCellIndex, value );
        wxString str = wxString::FromUTF8Unchecked( g_value_get_string( value ) );

        if (item.IsSameAs( str, bCase ) )
            return count;

        count++;
    }
    while ( gtk_tree_model_iter_next(model, &iter) );

    return wxNOT_FOUND;
}

int wxChoice::GetSelection() const
{
    return gtk_combo_box_get_active( GTK_COMBO_BOX( m_widget ) );
}

void wxChoice::SetString(unsigned int n, const wxString &text)
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid control") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    wxCHECK_RET( IsValid(n), wxT("invalid index") );

    GtkTreeModel *model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    if (gtk_tree_model_iter_nth_child (model, &iter, nullptr, n))
    {
        wxGtkValue value(G_TYPE_STRING);
        g_value_set_string( value, text.utf8_str() );
        gtk_list_store_set_value( GTK_LIST_STORE(model), &iter, m_stringCellIndex, value );
    }

    InvalidateBestSize();
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG( m_widget != nullptr, wxEmptyString, wxT("invalid control") );

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel *model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;
    if (!gtk_tree_model_iter_nth_child (model, &iter, nullptr, n))
    {
        wxFAIL_MSG( "invalid index" );
        return wxString();
    }

    wxGtkValue value;
    gtk_tree_model_get_value( model, &iter, m_stringCellIndex, value );
    return wxString::FromUTF8Unchecked( g_value_get_string( value ) );
}

unsigned int wxChoice::GetCount() const
{
    wxCHECK_MSG( m_widget != nullptr, 0, wxT("invalid control") );

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

void wxChoice::SetSelection( int n )
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid control") );

    wxGtkEventsDisabler<wxChoice> noEvents(this);

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    gtk_combo_box_set_active( combobox, n );
}

void wxChoice::SetColumns(int n)
{
    gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(m_widget), n);
}

int wxChoice::GetColumns() const
{
    return gtk_combo_box_get_wrap_width(GTK_COMBO_BOX(m_widget));
}

void wxChoice::GTKDisableEvents()
{
    g_signal_handlers_block_by_func(m_widget,
                                (gpointer) gtk_choice_changed_callback, this);
}

void wxChoice::GTKEnableEvents()
{
    g_signal_handlers_unblock_by_func(m_widget,
                                (gpointer) gtk_choice_changed_callback, this);
}

GdkWindow *wxChoice::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return gtk_widget_get_window(m_widget);
}

wxSize wxChoice::DoGetBestSize() const
{
    // Get the height of the control from GTK+ itself, but use our own version
    // to compute the width large enough to show all our strings as GTK+
    // doesn't seem to take the control contents into account.
    return GetSizeFromTextSize(wxChoiceBase::DoGetBestSize().x);
}

wxSize wxChoice::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    wxASSERT_MSG( m_widget, wxS("GetSizeFromTextSize called before creation") );

    // a GtkEntry for wxComboBox and a GtkCellView for wxChoice
    GtkWidget* childPart = gtk_bin_get_child(GTK_BIN(m_widget));

#ifdef __WXGTK3__
    // Preferred size for wxChoice can be incorrect when control is empty,
    // work around this by temporarily adding an item.
    GtkTreeModel* model = nullptr;
    if (GTK_IS_CELL_VIEW(childPart))
    {
        model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_widget));
        GtkTreeIter iter;
        if (gtk_tree_model_get_iter_first(model, &iter))
            model = nullptr;
        else
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(m_widget), "Gg");
    }
#endif

    // We are interested in the difference of sizes between the whole contol
    // and its child part. I.e. arrow, separators, etc.
    GtkRequisition req;
    gtk_widget_get_preferred_size(childPart, nullptr, &req);
    wxSize tsize(GTKGetPreferredSize(m_widget));

#ifdef __WXGTK3__
    if (model)
        gtk_list_store_clear(GTK_LIST_STORE(model));
#endif

    tsize.x -= req.width;
    if (tsize.x < 0)
        tsize.x = 0;
    tsize.x += xlen;

    // For a wxChoice, not for wxComboBox, add some margins
    if ( !GTK_IS_ENTRY(childPart) )
        tsize.IncBy(5, 0);

    // Perhaps the user wants something different from CharHeight
    if ( ylen > 0 )
        tsize.IncBy(0, ylen - GetCharHeight());

    return tsize;
}

void wxChoice::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GTKApplyStyle(m_widget, style);
    GTKApplyStyle(gtk_bin_get_child(GTK_BIN(m_widget)), style);
}

// static
wxVisualAttributes
wxChoice::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_combo_box_new());
}

#endif // wxUSE_CHOICE || wxUSE_COMBOBOX
