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
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

#include "wx/gtk/textdlg.h"

/* Split message, using constraints to position controls */
static wxSize wxSplitMessage2( const wxString &message, wxWindow *parent )
{
    int y = 10;
    int w = 50;
    wxString line( _T("") );
    for (size_t pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( parent, -1, line, wxPoint(15,y) );
                wxSize size1( s1->GetSize() );
                if (size1.x > w) w = size1.x;
                line = _T("");
            }
            y += 18;
        }
        else
        {
            line += message[pos];
        }
    }
    
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( parent, -1, line, wxPoint(15,y) );
        wxSize size2( s2->GetSize() );
        if (size2.x > w) w = size2.x;
    }
        
    y += 18;
    
    return wxSize(w+30,y);
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

    wxSize message_size( wxSplitMessage2( message, this ) );

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxList m_buttons;
    
    int y = message_size.y + 15;
    
    wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_TEXT, value, wxPoint(-1, y), wxSize(350, -1));
    
    y += 65;

    if (style & wxOK) 
    {
        ok = new wxButton( this, wxID_OK, _("OK"), wxPoint(-1,y), wxSize(80,-1) );
	m_buttons.Append( ok );
    }

    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxPoint(-1,y), wxSize(80,-1) );
	m_buttons.Append( cancel );
    }

    if (ok)
    {
        ok->SetDefault();
        ok->SetFocus();
    }

    int w = wxMax( 350, m_buttons.GetCount() * 100 );
    w = wxMax( w, message_size.x );
    int space = w / (m_buttons.GetCount()*2);
    
    textCtrl->SetSize( 20, -1, w-10, -1 );
    
    int m = 0;
    wxNode *node = m_buttons.First();
    while (node)
    {
        wxWindow *win = (wxWindow*)node->Data();
        int x = (m*2+1)*space - 40 + 15;
        win->Move( x, -1 );
        node = node->Next();
        m++;
    }
    
#if wxUSE_STATLINE
    (void) new wxStaticLine( this, -1, wxPoint(0,y-20), wxSize(w+30, 5) );
#endif
    
    SetSize( w+30, y+40 );

    Centre( wxBOTH );


  wxEndBusyCursor();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
	wxTextCtrl *textCtrl = (wxTextCtrl *)FindWindow(wxID_TEXT);
	if ( textCtrl )
		m_value = textCtrl->GetValue();

	EndModal(wxID_OK);
}

