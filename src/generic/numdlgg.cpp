/////////////////////////////////////////////////////////////////////////////
// Name:        numdlgg.cpp
// Purpose:     wxGetNumberFromUser implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "numdlgg.h"
    #pragma implementation "numdlgg.h"
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

// this is where wxGetNumberFromUser() is declared
#include "wx/generic/textdlgg.h"

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


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNumberEntryDialog : public wxDialog
{
public:
    wxNumberEntryDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& prompt,
                        const wxString& caption,
                        long value, long min, long max,
                        const wxPoint& pos);

    long GetValue() const { return m_value; }

    // implementation only
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

protected:
    wxTextCtrl *m_spinctrl; // TODO replace it with wxSpinCtrl once it's done

    long m_value, m_min, m_max;

private:
    DECLARE_EVENT_TABLE()
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNumberEntryDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNumberEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxNumberEntryDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, wxNumberEntryDialog::OnCancel)
END_EVENT_TABLE()

wxNumberEntryDialog::wxNumberEntryDialog(wxWindow *parent,
                                         const wxString& message,
                                         const wxString& prompt,
                                         const wxString& caption,
                                         long value,
                                         long min,
                                         long max,
                                         const wxPoint& pos)
                   : wxDialog(parent, -1, caption,
                              pos, wxDefaultSize,
                              wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
    m_value = value;
    m_max = max;
    m_min = min;

    wxBeginBusyCursor();
    
    wxBox *topsizer = new wxBox( wxVERTICAL );

    // 1) text message
    wxBox *textsizer = new wxBox( wxVERTICAL );
    wxSplitMessage2( message, this, textsizer );
    topsizer->Add( textsizer, 0, wxALL, 10 );

    // 2) prompt and text ctrl
    wxBox *inputsizer = new wxBox( wxHORIZONTAL );
    // prompt if any
    if (!prompt.IsEmpty())
        inputsizer->Add( new wxStaticText( this, -1, prompt ), 0, wxCENTER | wxLEFT, 10 );
    // spin ctrl
    wxString valStr;
    valStr.Printf(_T("%lu"), m_value);
    m_spinctrl = new wxTextCtrl(this, -1, valStr, wxDefaultPosition, wxSize( 140, -1 ) );
    inputsizer->Add( m_spinctrl, 1, wxCENTER | wxLEFT | wxRIGHT, 10 );
    // add both    
    topsizer->Add( inputsizer, 1, wxEXPAND | wxLEFT|wxRIGHT, 5 );

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif


    // 4) buttons
    wxBox *buttonsizer = new wxBox( wxHORIZONTAL );

    wxButton *ok = (wxButton *) NULL;
//    if (style & wxOK)
    {
        ok = new wxButton( this, wxID_OK, _("OK") );
	buttonsizer->Add( ok, 0, wxLEFT|wxRIGHT, 10 );
    }

    wxButton *cancel = (wxButton *) NULL;
//    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	buttonsizer->Add( cancel, 0, wxLEFT|wxRIGHT, 10 );
    }
    
    topsizer->Add( buttonsizer, 0, wxCENTRE | wxALL, 10 );

    SetSizer( topsizer );
    SetAutoLayout( TRUE );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    if (ok)
        ok->SetDefault();

    m_spinctrl->SetFocus();

    wxEndBusyCursor();
}

void wxNumberEntryDialog::OnOK(wxCommandEvent& event)
{
    if ( (wxSscanf(m_spinctrl->GetValue(), _T("%lu"), &m_value) != 1) ||
         (m_value < m_min) || (m_value > m_max) )
    {
        // not a number or out of range
        m_value = -1;
    }

    EndModal(wxID_OK);
}

void wxNumberEntryDialog::OnCancel(wxCommandEvent& event)
{
    m_value = -1;

    EndModal(wxID_CANCEL);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// wxGetTextFromUser is in utilscmn.cpp

long wxGetNumberFromUser(const wxString& msg,
                         const wxString& prompt,
                         const wxString& title,
                         long value,
                         long min,
                         long max,
                         wxWindow *parent,
                         const wxPoint& pos)
{
    wxNumberEntryDialog dialog(parent, msg, prompt, title,
                               value, min, max, pos);
    (void)dialog.ShowModal();

    return dialog.GetValue();
}
