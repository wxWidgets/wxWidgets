/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

#include "wx/fontdlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/debug.h"
#include "wx/msgdlg.h"

#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static
bool gtk_fontdialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxDialog *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

/*
    printf( "OnDelete from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    win->Close();

    return TRUE;
}

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

#ifdef __WXGTK12__
static
void gtk_fontdialog_ok_callback( GtkWidget *WXUNUSED(widget), wxFontDialog *dialog )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    GtkFontSelectionDialog *fontdlg = GTK_FONT_SELECTION_DIALOG(dialog->m_widget);
    GdkFont *gfont = gtk_font_selection_dialog_get_font(fontdlg);

    if (!gfont)
    {
        wxMessageBox(_("Please choose a valid font."), _("Error"), wxOK);
        return;
    }

    gchar *fontname = gtk_font_selection_dialog_get_font_name(fontdlg);
    wxFont font( gfont, fontname );
    g_free( fontname );
    dialog->m_fontData.SetChosenFont( font );

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}
#endif // GTK+ 1.2 andlater only

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

static
void gtk_fontdialog_cancel_callback( GtkWidget *WXUNUSED(w), wxFontDialog *dialog )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxFontDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFontDialog,wxDialog)

wxFontDialog::wxFontDialog( wxWindow *parent, wxFontData *WXUNUSED(data) )
{
    m_needParent = FALSE;

    if (!PreCreation( parent, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
                     wxDefaultValidator, T("fontdialog") ))
    {
        wxFAIL_MSG( T("wxXX creation failed") );
        return;
    }
#ifndef __WXGTK12__
    wxFAIL_MSG( T("TODO") );
#else // GTK+ 1.2
    wxString m_message( _("Choose font") );
    m_widget = gtk_font_selection_dialog_new( m_message.mbc_str() );

    int x = (gdk_screen_width () - 400) / 2;
    int y = (gdk_screen_height () - 400) / 2;
    gtk_widget_set_uposition( m_widget, x, y );

    GtkFontSelectionDialog *sel = GTK_FONT_SELECTION_DIALOG(m_widget);

    gtk_signal_connect( GTK_OBJECT(sel->ok_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_fontdialog_ok_callback), (gpointer*)this );

    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->ok_button)->child ), wxConvCurrent->cWX2MB(_("OK")) );

    gtk_signal_connect( GTK_OBJECT(sel->cancel_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_fontdialog_cancel_callback), (gpointer*)this );

    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->cancel_button)->child ), wxConvCurrent->cWX2MB(_("Cancel")) );

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_fontdialog_delete_callback), (gpointer)this );
#endif // GTK+ version
}

wxFontDialog::~wxFontDialog()
{
}

