/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include <stdio.h>
  #include "wx/utils.h"
  #include "wx/dialog.h"
  #include "wx/button.h"
  #include "wx/stattext.h"
  #include "wx/textctrl.h"
  #include "wx/intl.h"
  #include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

#include "wx/gtk/textdlg.h"

static void wxSplitMessage2( const wxString &message, wxWindow *parent, wxSizer* sizer )
{
    wxString line;
    for (size_t pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( parent, -1, line );
		sizer->Add( s1 );
                line = _T("");
            }
        }
        else
        {
            line += message[pos];
        }
    }
    
    // remaining text behind last '\n'
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( parent, -1, line );
	sizer->Add( s2 );
    }
}

// wxTextEntryDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
	EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)
#endif

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxString& value, long style, const wxPoint& pos):
	wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxTAB_TRAVERSAL)
{
    m_dialogStyle = style;
    m_value = value;

    wxBeginBusyCursor();

    wxBox *topsizer = new wxBox( wxVERTICAL );

    // 1) text message
    wxBox *textsizer = new wxBox( wxVERTICAL );
    wxSplitMessage2( message, this, textsizer );
    topsizer->Add( textsizer, 0, wxALL, 10 );

    // 2) text ctrl
    wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_TEXT, value, wxDefaultPosition, wxSize(300, -1));
    topsizer->Add( textCtrl, 1, wxEXPAND | wxLEFT|wxRIGHT, 15 );

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 3 );
#endif


    // 4) buttons
    wxBox *buttonsizer = new wxBox( wxHORIZONTAL );

    wxButton *ok = (wxButton *) NULL;
    if (style & wxOK)
    {
        ok = new wxButton( this, wxID_OK, _("OK") );
	buttonsizer->Add( ok, 0, wxLEFT|wxRIGHT, 10 );
    }

    wxButton *cancel = (wxButton *) NULL;
    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	buttonsizer->Add( cancel, 0, wxLEFT|wxRIGHT, 10 );
    }
    
    topsizer->Add( buttonsizer, 0, wxCENTRE | wxALL, 10 );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    SetSizer( topsizer );
    SetAutoLayout( TRUE );

    Centre( wxBOTH );

    if (ok)
        ok->SetDefault();

    textCtrl->SetFocus();

    wxEndBusyCursor();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
	wxTextCtrl *textCtrl = (wxTextCtrl *)FindWindow(wxID_TEXT);
	if ( textCtrl )
		m_value = textCtrl->GetValue();

	EndModal(wxID_OK);
}

