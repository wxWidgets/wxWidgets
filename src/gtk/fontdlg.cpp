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

#include <gtk/gtk.h>

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

    // extract the relevant bits from it
    wxString xregistry, xencoding;
    char *dash = strrchr(fontname, '-');    // find the last dash
    if ( dash )
    {
        xencoding = dash + 1;
        *dash = '\0';
        dash = strrchr(fontname, '-');      // the last before one
        if ( dash )
        {
            xregistry = dash + 1;
        }
        else
        {
            wxFAIL_MSG( wxT("no registry in X font spec?") );
        }

        // restore the dash we changed to NUL above
        *(fontname + strlen(fontname)) = '-';
    }
    else
    {
        wxFAIL_MSG( wxT("no encoding in X font spec?") );
    }

    // transfer the X registry/encoding to wxFontData - they are used by
    // wxFontMapper after wxFontDialog returns
    wxFontData& fontdata = dialog->m_fontData;

    // we ignore the facename here - should be enough to choose an arbitrary
    // one if the registry/encoding are specified
    //  dialog->m_fontData.EncodingInfo().facename = xfamily;
    fontdata.EncodingInfo().xregistry = xregistry;
    fontdata.EncodingInfo().xencoding = xencoding;

    // pass fontdata to wxFont ctor so that it can get the encoding from there
    // if it is already known (otherwise it will try to deduce it itself)
    dialog->m_fontData.SetChosenFont( wxFont(fontname, fontdata) );

    g_free( fontname );

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}
#endif // GTK+ 1.2 and later only

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

wxFontDialog::wxFontDialog( wxWindow *parent, wxFontData *fontdata )
            : m_fontData(*fontdata)
{
    m_needParent = FALSE;

    if (!PreCreation( parent, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
                     wxDefaultValidator, wxT("fontdialog") ))
    {
        wxFAIL_MSG( wxT("wxXX creation failed") );
        return;
    }
    
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

    wxFont font = m_fontData.GetInitialFont();
    if( font.Ok() )
    {
        wxNativeFontInfo info = font.GetNativeFontInfo();

        if( info.xFontName.IsEmpty() )
            font.GetInternalFont();
        gtk_font_selection_dialog_set_font_name(sel, wxConvCurrent->cWX2MB(info.xFontName.GetData()));
    }
}

wxFontDialog::~wxFontDialog()
{
}

