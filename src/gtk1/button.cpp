/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "button.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_BUTTON

#include "wx/button.h"
#include "wx/stockitem.h"

#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxButton *button )
{
    if (g_isIdle)
       wxapp_install_idle_handler();

    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
    event.SetEventObject(button);
    button->GetEventHandler()->ProcessEvent(event);
}
}

//-----------------------------------------------------------------------------
// "style_set" from m_widget
//-----------------------------------------------------------------------------

static gint
gtk_button_style_set_callback( GtkWidget *m_widget, GtkStyle *WXUNUSED(style), wxButton *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();
    
    int left_border = 0;
    int right_border = 0;
    int top_border = 0;
    int bottom_border = 0;
        
    /* the default button has a border around it */
    if (GTK_WIDGET_CAN_DEFAULT(m_widget))
    {
#ifdef __WXGTK20__
        GtkBorder *default_border = NULL;
        gtk_widget_style_get( m_widget, "default_border", &default_border, NULL );
        if (default_border)
        {
            left_border += default_border->left;
            right_border += default_border->right;
            top_border += default_border->top;
            bottom_border += default_border->bottom;
            g_free( default_border );
        }
#else
        left_border = 6;
        right_border = 6;
        top_border = 6;
        bottom_border = 5;
#endif
        win->DoMoveWindow( win->m_x-top_border,
                           win->m_y-left_border,
                           win->m_width+left_border+right_border,
                           win->m_height+top_border+bottom_border );
    }      

    return FALSE;
}

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxButton,wxControl)

wxButton::wxButton()
{
}

wxButton::~wxButton()
{
}

bool wxButton::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxButton creation failed") );
        return FALSE;
    }

/*
    wxString label2( label );
    for (size_t i = 0; i < label2.Len(); i++)
    {
        if (label2.GetChar(i) == wxT('&'))
        label2.SetChar(i,wxT('_'));
    }

    GtkWidget *accel_label = gtk_accel_label_new( label2.mb_str() );
    gtk_widget_show( accel_label );

    m_widget = gtk_button_new();
    gtk_container_add( GTK_CONTAINER(m_widget), accel_label );

    gtk_accel_label_set_accel_widget( GTK_ACCEL_LABEL(accel_label), m_widget );

    guint accel_key = gtk_label_parse_uline (GTK_LABEL(accel_label), label2.mb_str() );
    gtk_accel_label_refetch( GTK_ACCEL_LABEL(accel_label) );

    wxControl::SetLabel( label );
*/

#ifdef __WXGTK20__
    m_widget = gtk_button_new_with_mnemonic("");
#else
    m_widget = gtk_button_new_with_label("");
#endif

    float x_alignment = 0.5;
    if (HasFlag(wxBU_LEFT))
        x_alignment = 0.0;
    else if (HasFlag(wxBU_RIGHT))
        x_alignment = 1.0;

    float y_alignment = 0.5;
    if (HasFlag(wxBU_TOP))
        y_alignment = 0.0;
    else if (HasFlag(wxBU_BOTTOM))
        y_alignment = 1.0;

#if __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        gtk_button_set_alignment(GTK_BUTTON(m_widget), x_alignment, y_alignment);
    }
    else
#endif
    {
        if (GTK_IS_MISC(BUTTON_CHILD(m_widget)))
            gtk_misc_set_alignment (GTK_MISC (BUTTON_CHILD (m_widget)),
                                x_alignment, y_alignment);
    }

    SetLabel(label);

    if (style & wxNO_BORDER)
       gtk_button_set_relief( GTK_BUTTON(m_widget), GTK_RELIEF_NONE );

    gtk_signal_connect_after( GTK_OBJECT(m_widget), "clicked",
      GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );

    gtk_signal_connect_after( GTK_OBJECT(m_widget), "style_set",
      GTK_SIGNAL_FUNC(gtk_button_style_set_callback), (gpointer*) this );
      
    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}
    

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxCHECK_RET( parent, _T("button without parent?") );

    parent->SetDefaultItem(this);
    
    GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    gtk_widget_grab_default( m_widget );
    
    // resize for default border
    gtk_button_style_set_callback( m_widget, NULL, this );
}

/* static */
wxSize wxButtonBase::GetDefaultSize()
{
#ifdef __WXGTK20__
    static wxSize size = wxDefaultSize;
    if (size == wxDefaultSize)
    {
        // NB: Default size of buttons should be same as size of stock
        //     buttons as used in most GTK+ apps. Unfortunately it's a little
        //     tricky to obtain this size: stock button's size may be smaller
        //     than size of button in GtkButtonBox and vice versa,
        //     GtkButtonBox's minimal button size may be smaller than stock
        //     button's size. We have to retrieve both values and combine them.

        GtkWidget *wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *box = gtk_hbutton_box_new();
        GtkWidget *btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
        gtk_container_add(GTK_CONTAINER(box), btn);
        gtk_container_add(GTK_CONTAINER(wnd), box);
        GtkRequisition req;
        gtk_widget_size_request(btn, &req);

        gint minwidth, minheight;
        gtk_widget_style_get(box,
                             "child-min-width", &minwidth,
                             "child-min-height", &minheight,
                             NULL);

        size.x = wxMax(minwidth, req.width);
        size.y = wxMax(minheight, req.height);
        
        gtk_widget_destroy(wnd);
    }
    return size;
#else
    return wxSize(80,26);
#endif
}

void wxButton::SetLabel( const wxString &lbl )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid button") );

    wxString label(lbl);

    if (label.empty() && wxIsStockID(m_windowId))
        label = wxGetStockLabel(m_windowId);

    wxControl::SetLabel(label);

#ifdef __WXGTK20__
    if (wxIsStockID(m_windowId) && wxIsStockLabel(m_windowId, label))
    {
        const char *stock = wxGetStockGtkID(m_windowId);
        if (stock)
        {
            gtk_button_set_label(GTK_BUTTON(m_widget), stock);
            gtk_button_set_use_stock(GTK_BUTTON(m_widget), TRUE);
            return;
        }
    }

    wxString label2 = PrepareLabelMnemonics(label);
    gtk_button_set_label(GTK_BUTTON(m_widget), wxGTK_CONV(label2));
    gtk_button_set_use_stock(GTK_BUTTON(m_widget), FALSE);
    
    ApplyWidgetStyle( false );
    
#else
    gtk_label_set(GTK_LABEL(BUTTON_CHILD(m_widget)), wxGTK_CONV(GetLabel()));
#endif
}

bool wxButton::Enable( bool enable )
{
    if ( !wxControl::Enable( enable ) )
        return FALSE;

    gtk_widget_set_sensitive( BUTTON_CHILD(m_widget), enable );

    return TRUE;
}

bool wxButton::IsOwnGtkWindow( GdkWindow *window )
{
#ifdef __WXGTK20__
    return GTK_BUTTON(m_widget)->event_window;
#else
    return (window == m_widget->window);
#endif
}

void wxButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_widget, style);
    gtk_widget_modify_style(BUTTON_CHILD(m_widget), style);
}

wxSize wxButton::DoGetBestSize() const
{
    // the default button in wxGTK is bigger than the other ones because of an
    // extra border around it, but we don't want to take it into account in
    // our size calculations (otherwsie the result is visually ugly), so
    // always return the size of non default button from here
    const bool isDefault = GTK_WIDGET_HAS_DEFAULT(m_widget);
    if ( isDefault )
    {
        // temporarily unset default flag
        GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    }

    wxSize ret( wxControl::DoGetBestSize() );

    if ( isDefault )
    {
        // set it back again
        GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    }

#ifndef __WXGTK20__
    ret.x += 10;  // add a few pixels for sloppy (but common) themes
#endif
    
    if (!HasFlag(wxBU_EXACTFIT))
    {
        wxSize defaultSize = GetDefaultSize();
        if (ret.x < defaultSize.x) ret.x = defaultSize.x;
        if (ret.y < defaultSize.y) ret.y = defaultSize.y;
    }

    CacheBestSize(ret);
    return ret;
}

// static
wxVisualAttributes
wxButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_button_new);
}

#endif // wxUSE_BUTTON

