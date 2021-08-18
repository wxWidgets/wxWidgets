/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/richmsgdlgg.cpp
// Purpose:     wxGenericRichMessageDialog
// Author:      Rickard Westerlund
// Created:     2010-07-04
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_RICHMSGDLG

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
#endif

#include "wx/collpane.h"
#include "wx/richmsgdlg.h"
#include "wx/statline.h"
#include "wx/artprov.h"

wxIMPLEMENT_CLASS(wxRichMessageDialog, wxDialog)

// ----------------------------------------------------------------------------
// Events and handlers
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxGenericRichMessageDialog, wxRichMessageDialogBase)
    EVT_COLLAPSIBLEPANE_CHANGED(wxID_ANY,
                                wxGenericRichMessageDialog::OnPaneChanged)
wxEND_EVENT_TABLE()

void wxGenericRichMessageDialog::OnPaneChanged(wxCollapsiblePaneEvent& event)
{
    if ( event.GetCollapsed() )
        m_detailsPane->SetLabel( m_detailsExpanderCollapsedLabel );
    else
        m_detailsPane->SetLabel( m_detailsExpanderExpandedLabel );
}

// ----------------------------------------------------------------------------
// wxGenericRichMessageDialog
// ----------------------------------------------------------------------------

void wxGenericRichMessageDialog::AddMessageDialogCheckBox(wxSizer *sizer)
{
    if ( !m_checkBoxText.empty() )
    {
        m_checkBox = new wxCheckBox(this, wxID_ANY, m_checkBoxText);
        m_checkBox->SetValue(m_checkBoxValue);

        sizer->Add(m_checkBox, wxSizerFlags().Left().Border(wxLEFT|wxTOP, 10));
    }
}

void wxGenericRichMessageDialog::AddMessageDialogDetails(wxSizer *sizer)
{
    if ( !m_detailedText.empty() )
    {
        wxSizer *sizerDetails = new wxBoxSizer( wxHORIZONTAL );

        m_detailsPane =
            new wxCollapsiblePane( this, -1, m_detailsExpanderCollapsedLabel );

        // add the detailed text
        wxWindow *windowPane = m_detailsPane->GetPane();
        wxSizer *sizerPane = new wxBoxSizer( wxHORIZONTAL );
        sizerPane->Add( new wxStaticText( windowPane, -1, m_detailedText ) );
        windowPane->SetSizer( sizerPane );

        sizerDetails->Add( m_detailsPane, wxSizerFlags().Expand() );
        sizer->Add( sizerDetails, 0, wxTOP|wxLEFT|wxRIGHT | wxALIGN_LEFT, 10 );
    }

    if ( !m_footerText.empty() )
    {
        // add footer
        sizer->Add( new wxStaticLine(this), wxSizerFlags().Expand().Border() );
        wxSizer *footerSizer = new wxBoxSizer(wxHORIZONTAL);
        if (m_footerIcon)
        {
            wxSize iconSize = wxArtProvider::GetNativeSizeHint(wxART_MENU);

            wxStaticBitmap* footerIcon = new wxStaticBitmap(this, wxID_ANY,
                wxArtProvider::GetIcon(wxArtProvider::GetMessageBoxIconId(m_footerIcon),
                wxART_MESSAGE_BOX, iconSize));
            footerSizer->Add( footerIcon,
                wxSizerFlags().Border(wxLEFT|wxRIGHT).CenterVertical() );
        }
        footerSizer->Add( new wxStaticText(this, wxID_ANY, m_footerText),
            wxSizerFlags().CenterVertical() );
        sizer->Add( footerSizer, wxSizerFlags().Border().Expand() );
    }
}

bool wxGenericRichMessageDialog::IsCheckBoxChecked() const
{
    // This function can be called before the dialog is shown and hence before
    // the check box is created.
    return m_checkBox ? m_checkBox->IsChecked() : m_checkBoxValue;
}

#endif // wxUSE_RICHMSGDLG
