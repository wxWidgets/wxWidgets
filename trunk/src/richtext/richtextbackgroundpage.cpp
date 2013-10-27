/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbackgroundpage.cpp
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     13/11/2010 11:17:25
// RCS-ID:      
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/richtext/richtextbackgroundpage.h"
#include "wx/richtext/richtextformatdlg.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextBackgroundPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextBackgroundPage, wxRichTextDialogPage )


/*!
 * wxRichTextBackgroundPage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextBackgroundPage, wxRichTextDialogPage )
EVT_BUTTON(ID_RICHTEXT_BACKGROUND_COLOUR_SWATCH, wxRichTextBackgroundPage::OnColourSwatch)

////@begin wxRichTextBackgroundPage event table entries
////@end wxRichTextBackgroundPage event table entries

END_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextBackgroundPage)

/*!
 * wxRichTextBackgroundPage constructors
 */

wxRichTextBackgroundPage::wxRichTextBackgroundPage()
{
    Init();
}

wxRichTextBackgroundPage::wxRichTextBackgroundPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * wxRichTextBackgroundPage creator
 */

bool wxRichTextBackgroundPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextBackgroundPage creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextBackgroundPage creation
    return true;
}


/*!
 * wxRichTextBackgroundPage destructor
 */

wxRichTextBackgroundPage::~wxRichTextBackgroundPage()
{
////@begin wxRichTextBackgroundPage destruction
////@end wxRichTextBackgroundPage destruction
}


/*!
 * Member initialisation
 */

void wxRichTextBackgroundPage::Init()
{
////@begin wxRichTextBackgroundPage member initialisation
    m_backgroundColourCheckBox = NULL;
    m_backgroundColourSwatch = NULL;
////@end wxRichTextBackgroundPage member initialisation
}


/*!
 * Control creation for wxRichTextBackgroundPage
 */

void wxRichTextBackgroundPage::CreateControls()
{    
////@begin wxRichTextBackgroundPage content construction
    wxRichTextBackgroundPage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Background"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine6 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer4->Add(itemStaticLine6, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer7, 0, wxGROW, 5);

    itemBoxSizer7->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_backgroundColourCheckBox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_BACKGROUND_COLOUR_CHECKBOX, _("Background &colour:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_backgroundColourCheckBox->SetValue(false);
    m_backgroundColourCheckBox->SetHelpText(_("Enables a background colour."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_backgroundColourCheckBox->SetToolTip(_("Enables a background colour."));
    itemBoxSizer7->Add(m_backgroundColourCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_backgroundColourSwatch = new wxRichTextColourSwatchCtrl( itemRichTextDialogPage1, ID_RICHTEXT_BACKGROUND_COLOUR_SWATCH, wxDefaultPosition, wxSize(80, 20), wxBORDER_THEME );
    m_backgroundColourSwatch->SetHelpText(_("The background colour."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_backgroundColourSwatch->SetToolTip(_("The background colour."));
    itemBoxSizer7->Add(m_backgroundColourSwatch, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxRichTextBackgroundPage content construction
}


/*!
 * Should we show tooltips?
 */

wxRichTextAttr* wxRichTextBackgroundPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

bool wxRichTextBackgroundPage::TransferDataToWindow()
{
    wxRichTextAttr* attr = GetAttributes();
    if (!attr->HasBackgroundColour())
    {
        m_backgroundColourCheckBox->SetValue(false);
        m_backgroundColourSwatch->SetColour(*wxWHITE);
    }
    else
    {
        m_backgroundColourCheckBox->SetValue(true);
        m_backgroundColourSwatch->SetColour(attr->GetBackgroundColour());
    }

    return true;
}

bool wxRichTextBackgroundPage::TransferDataFromWindow()
{
    wxRichTextAttr* attr = GetAttributes();
    if (m_backgroundColourCheckBox->GetValue())
    {
        attr->SetBackgroundColour(m_backgroundColourSwatch->GetColour());
    }
    else
    {
        attr->SetFlags(attr->GetFlags() & ~wxTEXT_ATTR_BACKGROUND_COLOUR);
    }
    
    return true;
}

// Respond to colour swatch click
void wxRichTextBackgroundPage::OnColourSwatch(wxCommandEvent& event)
{
    m_backgroundColourCheckBox->SetValue(true);
    event.Skip();
}

bool wxRichTextBackgroundPage::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextBackgroundPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextBackgroundPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextBackgroundPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextBackgroundPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextBackgroundPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextBackgroundPage icon retrieval
}
