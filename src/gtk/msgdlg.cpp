/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.cpp
// Purpose:     wxMessageDialog for GTK+2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2003/02/28
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_MSGDLG && defined(__WXGTK20__)

#include "wx/gtk/private.h"
#include <gtk/gtk.h>

#include "wx/msgdlg.h"
#include "wx/intl.h"

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
{
    m_caption = caption;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
}

int wxMessageDialog::ShowModal()
{
    GtkWidget *dlg;
    GtkMessageType type;
    GtkButtonsType buttons = GTK_BUTTONS_OK;
    
    if (m_dialogStyle & wxYES_NO)
    {
        buttons = GTK_BUTTONS_YES_NO;
    }

    if (m_dialogStyle & wxOK)
    {
        if (m_dialogStyle & wxCANCEL)
            buttons = GTK_BUTTONS_OK_CANCEL;
        else
            buttons = GTK_BUTTONS_OK;
    }
    
    if (m_dialogStyle & wxICON_EXCLAMATION)
        type = GTK_MESSAGE_WARNING;
    else if (m_dialogStyle & wxICON_ERROR)
        type = GTK_MESSAGE_ERROR;
    else if (m_dialogStyle & wxICON_INFORMATION)
        type = GTK_MESSAGE_INFO;
    else if (m_dialogStyle & wxICON_QUESTION)
        type = GTK_MESSAGE_QUESTION;

    dlg = gtk_message_dialog_new(m_parent ?
                                    GTK_WINDOW(m_parent->m_widget) : NULL,
                                 GTK_DIALOG_MODAL,
                                 type, buttons,
                                 "%s", (const char*)wxGTK_CONV(m_message));
    if (m_caption != wxMessageBoxCaptionStr)
        gtk_window_set_title(GTK_WINDOW(dlg), wxGTK_CONV(m_caption));

    if (m_dialogStyle & wxYES_NO)
    {
        if (m_dialogStyle & wxCANCEL)
            gtk_dialog_add_button(GTK_DIALOG(dlg), GTK_STOCK_CANCEL,
                                  GTK_RESPONSE_CANCEL);
        if (m_dialogStyle & wxNO_DEFAULT)
            gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_NO);
        else
            gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_YES);
    }
    
    gint result = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    switch (result)
    {
        default:
            wxFAIL_MSG(_T("unexpected GtkMessageDialog return code"));
            // fall through

        case GTK_RESPONSE_CANCEL: 
            return wxID_CANCEL;
        case GTK_RESPONSE_OK:
            return wxID_OK;
        case GTK_RESPONSE_YES:
            return wxID_YES;
        case GTK_RESPONSE_NO:
            return wxID_NO;
    }
}

#endif // wxUSE_MSGDLG && defined(__WXGTK20__)

