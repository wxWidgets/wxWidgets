/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlgg.cpp
// Purpose:     wxGenericMessageDialog
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Markus Holzem, Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/statbmp.h"
    #include "wx/layout.h"
    #include "wx/intl.h"
    #include "wx/icon.h"
    #include "wx/sizer.h"
    #include "wx/app.h"
#endif

#include <stdio.h>
#include <string.h>

#include "wx/generic/msgdlgg.h"

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGenericMessageDialog, wxDialog)
        EVT_BUTTON(wxID_YES, wxGenericMessageDialog::OnYes)
        EVT_BUTTON(wxID_NO, wxGenericMessageDialog::OnNo)
        EVT_BUTTON(wxID_CANCEL, wxGenericMessageDialog::OnCancel)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxGenericMessageDialog, wxDialog)

wxGenericMessageDialog::wxGenericMessageDialog( wxWindow *parent,
                                                const wxString& message,
                                                const wxString& caption,
                                                long style,
                                                const wxPoint& pos)
                      : wxDialog( parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
    m_dialogStyle = style;

    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );
    
    // 1) icon
    if (style & wxICON_MASK)
    {
         wxStaticBitmap *icon = new wxStaticBitmap(
            this, -1, wxTheApp->GetStdIcon(style & wxICON_MASK));
         icon_text->Add( icon, 0, wxCENTER );
    }
    
    // 2) text
    icon_text->Add( CreateTextSizer( message ), 0, wxCENTER | wxLEFT, 10 );
    
    topsizer->Add( icon_text, 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
    
#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    topsizer->Add( CreateButtonSizer( style ), 0, wxCENTRE | wxALL, 10 );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
    
    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    wxSize size( GetSize() );
    if (size.x < size.y*3/2)
    {
        size.x = size.y*3/2;
	SetSize( size );
    }

    Centre( wxBOTH | wxCENTER_FRAME);

    wxEndBusyCursor();
}

void wxGenericMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_YES );
}

void wxGenericMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_NO );
}

void wxGenericMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    /* Allow cancellation via ESC/Close button except if
       only YES and NO are specified. */
    if ( (m_dialogStyle & wxYES_NO) != wxYES_NO || (m_dialogStyle & wxCANCEL) )
    {
        EndModal( wxID_CANCEL );
    }
}


