/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/msgdlg.cpp
// Purpose:     wxMessageDialog for GTK+2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2003/02/28
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MSGDLG && defined(__WXGTK20__) && !defined(__WXGPE__)

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/gtk/private.h"
#include <gtk/gtk.h>

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
{
    m_caption = caption;
    m_message = message;
    SetMessageDialogStyle(style);
    m_parent = wxGetTopLevelParent(parent);

    GtkMessageType type = GTK_MESSAGE_ERROR;
    GtkButtonsType buttons = GTK_BUTTONS_OK;

    if (style & wxYES_NO)
    {
		if (style & wxCANCEL)
			buttons = GTK_BUTTONS_NONE;
		else
	        buttons = GTK_BUTTONS_YES_NO;
    }

    if (style & wxOK)
    {
        if (style & wxCANCEL)
            buttons = GTK_BUTTONS_OK_CANCEL;
        else
            buttons = GTK_BUTTONS_OK;
    }

    if (style & wxICON_EXCLAMATION)
        type = GTK_MESSAGE_WARNING;
    else if (style & wxICON_ERROR)
        type = GTK_MESSAGE_ERROR;
    else if (style & wxICON_INFORMATION)
        type = GTK_MESSAGE_INFO;
    else if (style & wxICON_QUESTION)
        type = GTK_MESSAGE_QUESTION;
    else
    {
        // GTK+ doesn't have a "typeless" msg box, so try to auto detect...
        type = style & wxYES ? GTK_MESSAGE_QUESTION : GTK_MESSAGE_INFO;
    }

    m_widget = gtk_message_dialog_new(m_parent ?
                                          GTK_WINDOW(m_parent->m_widget) : NULL,
                                      GTK_DIALOG_MODAL,
                                      type, buttons,
                                      "%s", (const char*)wxGTK_CONV(m_message));
    if (m_caption != wxMessageBoxCaptionStr)
        gtk_window_set_title(GTK_WINDOW(m_widget), wxGTK_CONV(m_caption));

    if (style & wxYES_NO)
    {
        if (style & wxCANCEL)
		{
            gtk_dialog_add_button(GTK_DIALOG(m_widget), GTK_STOCK_NO,
                                  GTK_RESPONSE_NO);
            gtk_dialog_add_button(GTK_DIALOG(m_widget), GTK_STOCK_CANCEL,
                                  GTK_RESPONSE_CANCEL);
            gtk_dialog_add_button(GTK_DIALOG(m_widget), GTK_STOCK_YES,
                                  GTK_RESPONSE_YES);
		}
        if (style & wxNO_DEFAULT)
            gtk_dialog_set_default_response(GTK_DIALOG(m_widget), GTK_RESPONSE_NO);
        else
            gtk_dialog_set_default_response(GTK_DIALOG(m_widget), GTK_RESPONSE_YES);
    }

    if (m_parent)
        gtk_window_set_transient_for(GTK_WINDOW(m_widget),
                                     GTK_WINDOW(m_parent->m_widget));
}

wxMessageDialog::~wxMessageDialog()
{
}

int wxMessageDialog::ShowModal()
{
    // This should be necessary, but otherwise the
    // parent TLW will disappear..
    if (m_parent)
        gtk_window_present( GTK_WINDOW(m_parent->m_widget) );

    gint result = gtk_dialog_run(GTK_DIALOG(m_widget));
    gtk_widget_destroy(m_widget);
    m_widget = NULL;

    switch (result)
    {
        default:
            wxFAIL_MSG(_T("unexpected GtkMessageDialog return code"));
            // fall through

        case GTK_RESPONSE_CANCEL:
        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_CLOSE:
            return wxID_CANCEL;
        case GTK_RESPONSE_OK:
            return wxID_OK;
        case GTK_RESPONSE_YES:
            return wxID_YES;
        case GTK_RESPONSE_NO:
            return wxID_NO;
    }
}


#endif // wxUSE_MSGDLG && defined(__WXGTK20__) && !defined(__WXGPE__)
