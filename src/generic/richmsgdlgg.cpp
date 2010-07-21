/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/richmsgdlgg.cpp
// Purpose:     wxGenericRichMessageDialog
// Author:      Rickard Westerlund
// Created:     2010-07-04
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_RICHMSGDLG

#ifndef WX_PRECOMP
    #include "wx/stattext.h"
    #include "wx/sizer.h"
#endif

#include "wx/collpane.h"
#include "wx/richmsgdlg.h"

// ----------------------------------------------------------------------------
// Events and handlers
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGenericRichMessageDialog, wxRichMessageDialogBase)
    EVT_COLLAPSIBLEPANE_CHANGED(wxID_ANY,
                                wxGenericRichMessageDialog::OnPaneChanged)
END_EVENT_TABLE()

void wxGenericRichMessageDialog::OnPaneChanged(wxCollapsiblePaneEvent& event)
{
    if ( event.GetCollapsed() )
        m_detailsExpander->SetLabel( m_detailsExpanderCollapsedLabel );
    else
        m_detailsExpander->SetLabel( m_detailsExpanderExpandedLabel );
}

// ----------------------------------------------------------------------------
// wxGenericRichMessageDialog
// ----------------------------------------------------------------------------

void wxGenericRichMessageDialog::AddMessageDialogCheckBox(wxSizer *sizer)
{
    if ( !m_checkBoxText.empty() )
    {
        wxSizer *sizerCheckBox = new wxBoxSizer( wxHORIZONTAL );

        m_checkBox = new wxCheckBox( this,
                                     wxID_ANY,
                                     m_checkBoxText );
        m_checkBox->SetValue( m_checkBoxValue );
        sizerCheckBox->Add( m_checkBox, 0, wxBOTTOM | wxALIGN_LEFT );

        sizer->Add( sizerCheckBox, 0, wxTOP|wxLEFT|wxRIGHT | wxALIGN_LEFT, 10 );
    }
}

void wxGenericRichMessageDialog::AddMessageDialogDetails(wxSizer *sizer)
{
    if ( !m_detailedText.empty() )
    {
        wxSizer *sizerDetails = new wxBoxSizer( wxHORIZONTAL );

        m_detailsExpander =
            new wxCollapsiblePane( this, -1, m_detailsExpanderCollapsedLabel );

        // add the detailed text
        wxWindow *windowPane = m_detailsExpander->GetPane();
        wxSizer *sizerPane = new wxBoxSizer( wxHORIZONTAL );
        sizerPane->Add( new wxStaticText( windowPane, -1, m_detailedText ) );
        windowPane->SetSizer( sizerPane );

        sizerDetails->Add( m_detailsExpander, wxSizerFlags().Right().Expand() );
        sizer->Add( sizerDetails, 0, wxTOP|wxLEFT|wxRIGHT | wxALIGN_LEFT, 10 );
    }
}

#endif // wxUSE_RICHMSGDLG
