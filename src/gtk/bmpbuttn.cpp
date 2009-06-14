/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bmpbuttn.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBitmapButton;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_bmpbutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
    event.SetEventObject(button);
    button->HandleWindowEvent(event);
}
}

//-----------------------------------------------------------------------------
// "enter"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_bmpbutton_enter_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    button->GTKMouseEnters();
}
}

//-----------------------------------------------------------------------------
// "leave"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_bmpbutton_leave_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    button->GTKMouseLeaves();
}
}

//-----------------------------------------------------------------------------
// "pressed"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_bmpbutton_press_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    button->GTKPressed();
}
}

//-----------------------------------------------------------------------------
// "released"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_bmpbutton_release_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    button->GTKReleased();
}
}

//-----------------------------------------------------------------------------
// wxBitmapButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton,wxButton)

BEGIN_EVENT_TABLE(wxBitmapButton, wxButton)
    EVT_SET_FOCUS(wxBitmapButton::OnFocusChange)
    EVT_KILL_FOCUS(wxBitmapButton::OnFocusChange)
END_EVENT_TABLE()


void wxBitmapButton::Init()
{
    m_mouseHovers =
    m_isPressed = false;
}

bool wxBitmapButton::Create( wxWindow *parent,
                             wxWindowID id,
                             const wxBitmap& bitmap,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxBitmapButton creation failed") );
        return false;
    }

    m_bitmaps[State_Normal] = bitmap;

    m_widget = gtk_button_new();
    g_object_ref(m_widget);

    if (style & wxNO_BORDER)
       gtk_button_set_relief( GTK_BUTTON(m_widget), GTK_RELIEF_NONE );

    if (bitmap.IsOk())
    {
        OnSetBitmap();
    }

    g_signal_connect_after (m_widget, "clicked",
                            G_CALLBACK (gtk_bmpbutton_clicked_callback),
                            this);

    g_signal_connect (m_widget, "enter",
                      G_CALLBACK (gtk_bmpbutton_enter_callback), this);
    g_signal_connect (m_widget, "leave",
                      G_CALLBACK (gtk_bmpbutton_leave_callback), this);
    g_signal_connect (m_widget, "pressed",
                      G_CALLBACK (gtk_bmpbutton_press_callback), this);
    g_signal_connect (m_widget, "released",
                      G_CALLBACK (gtk_bmpbutton_release_callback), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

void wxBitmapButton::SetLabel( const wxString &label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid button") );

    wxControl::SetLabel( label );
}

void wxBitmapButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    if (!GTK_BIN(m_widget)->child)
        return;

    wxButton::DoApplyWidgetStyle(style);
}

void wxBitmapButton::OnSetBitmap()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid bitmap button") );

    InvalidateBestSize();

    wxBitmap the_one;
    if (!IsThisEnabled())
        the_one = GetBitmapDisabled();
    else if (m_isPressed)
        the_one = GetBitmapPressed();
    else if (m_mouseHovers)
        the_one = GetBitmapHover();
    else if (HasFocus())
        the_one = GetBitmapFocus();

    if (!the_one.IsOk())
    {
        the_one = GetBitmapLabel();
        if (!the_one.IsOk())
            return;
    }

    GtkWidget* image = GTK_BIN(m_widget)->child;
    if (image == NULL)
    {
        image = gtk_image_new();
        gtk_widget_show(image);
        gtk_container_add(GTK_CONTAINER(m_widget), image);
    }
    // always use pixbuf, because pixmap mask does not
    // work with disabled images in some themes
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), the_one.GetPixbuf());
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    return wxControl::DoGetBestSize();
}

bool wxBitmapButton::Enable( bool enable )
{
    if ( !wxWindow::Enable(enable) )
        return false;

    OnSetBitmap();

    return true;
}

void wxBitmapButton::GTKMouseEnters()
{
    m_mouseHovers = true;
    OnSetBitmap();
}

void wxBitmapButton::GTKMouseLeaves()
{
    m_mouseHovers = false;
    OnSetBitmap();
}

void wxBitmapButton::GTKPressed()
{
    m_isPressed = true;
    OnSetBitmap();
}

void wxBitmapButton::GTKReleased()
{
    m_isPressed = false;
    OnSetBitmap();
}

void wxBitmapButton::OnFocusChange(wxFocusEvent& event)
{
    event.Skip();
    OnSetBitmap();
}

#endif // wxUSE_BMPBUTTON
