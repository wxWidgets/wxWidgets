/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextsizepage.cpp
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     20/10/2010 10:23:24
// RCS-ID:      
// Copyright:   (c) Julian Smart
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/richtext/richtextsizepage.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextSizePage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextSizePage, wxRichTextDialogPage )


/*!
 * wxRichTextSizePage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextSizePage, wxRichTextDialogPage )

////@begin wxRichTextSizePage event table entries
    EVT_UPDATE_UI( ID_RICHTEXT_WIDTH, wxRichTextSizePage::OnRichtextWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_W, wxRichTextSizePage::OnRichtextWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_HEIGHT, wxRichTextSizePage::OnRichtextHeightUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_H, wxRichTextSizePage::OnRichtextHeightUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX, wxRichTextSizePage::OnRichtextVerticalAlignmentComboboxUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_OFFSET, wxRichTextSizePage::OnRichtextOffsetUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_OFFSET_UNITS, wxRichTextSizePage::OnRichtextOffsetUpdate )

    EVT_BUTTON( ID_RICHTEXT_PARA_UP, wxRichTextSizePage::OnRichtextParaUpClick )

    EVT_BUTTON( ID_RICHTEXT_PARA_DOWN, wxRichTextSizePage::OnRichtextParaDownClick )

////@end wxRichTextSizePage event table entries

END_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextSizePage)

/*!
 * wxRichTextSizePage constructors
 */

wxRichTextSizePage::wxRichTextSizePage()
{
    Init();
}

wxRichTextSizePage::wxRichTextSizePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * wxRichTextSizePage creator
 */

bool wxRichTextSizePage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextSizePage creation
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextSizePage creation
    return true;
}


/*!
 * wxRichTextSizePage destructor
 */

wxRichTextSizePage::~wxRichTextSizePage()
{
////@begin wxRichTextSizePage destruction
////@end wxRichTextSizePage destruction
}


/*!
 * Member initialisation
 */

void wxRichTextSizePage::Init()
{
////@begin wxRichTextSizePage member initialisation
    m_parentSizer = NULL;
    m_floatingControls = NULL;
    m_float = NULL;
    m_widthCheckbox = NULL;
    m_width = NULL;
    m_unitsW = NULL;
    m_heightCheckbox = NULL;
    m_height = NULL;
    m_unitsH = NULL;
    m_alignmentControls = NULL;
    m_verticalAlignmentCheckbox = NULL;
    m_verticalAlignmentComboBox = NULL;
    m_positionControls = NULL;
    m_moveObjectParentSizer = NULL;
    m_offsetYCheckbox = NULL;
    m_offset = NULL;
    m_unitsOffset = NULL;
    m_moveObjectSizer = NULL;
////@end wxRichTextSizePage member initialisation
}


/*!
 * Control creation for wxRichTextSizePage
 */

void wxRichTextSizePage::CreateControls()
{    
////@begin wxRichTextSizePage content construction
    wxRichTextSizePage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    m_parentSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(m_parentSizer, 0, wxGROW|wxALL, 5);

    m_floatingControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_floatingControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    m_floatingControls->Add(itemBoxSizer5, 0, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Floating"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText6->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine7 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer5->Add(itemStaticLine7, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    m_floatingControls->Add(itemBoxSizer8, 0, wxGROW, 5);

    itemBoxSizer8->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer10 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer8->Add(itemFlexGridSizer10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Floating mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer10->Add(itemStaticText11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_floatStrings;
    m_floatStrings.Add(_("None"));
    m_floatStrings.Add(_("Left"));
    m_floatStrings.Add(_("Right"));
    m_float = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_FLOATING_MODE, _("None"), wxDefaultPosition, wxSize(80, -1), m_floatStrings, wxCB_READONLY );
    m_float->SetStringSelection(_("None"));
    m_float->SetHelpText(_("How the object will float relative to the text."));
    if (wxRichTextSizePage::ShowToolTips())
        m_float->SetToolTip(_("How the object will float relative to the text."));
    itemFlexGridSizer10->Add(m_float, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    m_parentSizer->Add(itemBoxSizer13, 0, wxGROW, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Size"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText14->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine15 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer13->Add(itemStaticLine15, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    m_parentSizer->Add(itemBoxSizer16, 0, wxGROW, 5);

    itemBoxSizer16->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer18 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer16->Add(itemFlexGridSizer18, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_widthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH_CHECKBOX, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_widthCheckbox->SetValue(false);
    m_widthCheckbox->SetHelpText(_("Enable the width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_widthCheckbox->SetToolTip(_("Enable the width value."));
    itemFlexGridSizer18->Add(m_widthCheckbox, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_width = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_width->SetHelpText(_("The object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_width->SetToolTip(_("The object width."));
    itemBoxSizer20->Add(m_width, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsWStrings;
    m_unitsWStrings.Add(_("px"));
    m_unitsWStrings.Add(_("cm"));
    m_unitsWStrings.Add(_("percent"));
    m_unitsW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsWStrings, wxCB_READONLY );
    m_unitsW->SetStringSelection(_("px"));
    m_unitsW->SetHelpText(_("Units for the object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsW->SetToolTip(_("Units for the object width."));
    itemBoxSizer20->Add(m_unitsW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_heightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_HEIGHT_CHECKBOX, _("&Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_heightCheckbox->SetValue(false);
    m_heightCheckbox->SetHelpText(_("Enable the height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_heightCheckbox->SetToolTip(_("Enable the height value."));
    itemFlexGridSizer18->Add(m_heightCheckbox, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer24, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_height = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_height->SetHelpText(_("The object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_height->SetToolTip(_("The object height."));
    itemBoxSizer24->Add(m_height, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsHStrings;
    m_unitsHStrings.Add(_("px"));
    m_unitsHStrings.Add(_("cm"));
    m_unitsHStrings.Add(_("percent"));
    m_unitsH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsHStrings, wxCB_READONLY );
    m_unitsH->SetStringSelection(_("px"));
    m_unitsH->SetHelpText(_("Units for the object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsH->SetToolTip(_("Units for the object height."));
    itemBoxSizer24->Add(m_unitsH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_alignmentControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_alignmentControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentControls->Add(itemBoxSizer28, 0, wxGROW, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Alignment"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText29->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine30 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer28->Add(itemStaticLine30, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentControls->Add(itemBoxSizer31, 0, wxGROW, 5);

    itemBoxSizer31->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_verticalAlignmentCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_CHECKBOX, _("&Vertical alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_verticalAlignmentCheckbox->SetValue(false);
    m_verticalAlignmentCheckbox->SetHelpText(_("Enable vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentCheckbox->SetToolTip(_("Enable vertical alignment."));
    itemBoxSizer31->Add(m_verticalAlignmentCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_verticalAlignmentComboBoxStrings;
    m_verticalAlignmentComboBoxStrings.Add(_("Top"));
    m_verticalAlignmentComboBoxStrings.Add(_("Centred"));
    m_verticalAlignmentComboBoxStrings.Add(_("Bottom"));
    m_verticalAlignmentComboBox = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX, _("Top"), wxDefaultPosition, wxDefaultSize, m_verticalAlignmentComboBoxStrings, wxCB_READONLY );
    m_verticalAlignmentComboBox->SetStringSelection(_("Top"));
    m_verticalAlignmentComboBox->SetHelpText(_("Vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentComboBox->SetToolTip(_("Vertical alignment."));
    itemBoxSizer31->Add(m_verticalAlignmentComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_positionControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_positionControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer36, 0, wxGROW, 5);

    wxStaticText* itemStaticText37 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Position"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText37->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine38 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer36->Add(itemStaticLine38, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer39, 0, wxGROW, 5);

    itemBoxSizer39->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectParentSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer39->Add(m_moveObjectParentSizer, 0, wxALIGN_TOP, 5);

    wxBoxSizer* itemBoxSizer42 = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(itemBoxSizer42, 0, wxGROW, 5);

    m_offsetYCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET_CHECKBOX, _("Vertical &Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_offsetYCheckbox->SetValue(false);
    m_offsetYCheckbox->SetHelpText(_("Enable vertical offset."));
    if (wxRichTextSizePage::ShowToolTips())
        m_offsetYCheckbox->SetToolTip(_("Enable vertical offset."));
    itemBoxSizer42->Add(m_offsetYCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_offset = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_offset->SetMaxLength(10);
    m_offset->SetHelpText(_("The vertical offset relative to the paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        m_offset->SetToolTip(_("The vertical offset relative to the paragraph."));
    itemBoxSizer42->Add(m_offset, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsOffsetStrings;
    m_unitsOffsetStrings.Add(_("px"));
    m_unitsOffsetStrings.Add(_("cm"));
    m_unitsOffset = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsOffsetStrings, wxCB_READONLY );
    m_unitsOffset->SetStringSelection(_("px"));
    m_unitsOffset->SetHelpText(_("Units for the object offset."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsOffset->SetToolTip(_("Units for the object offset."));
    itemBoxSizer42->Add(m_unitsOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectSizer = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(m_moveObjectSizer, 0, wxGROW, 5);

    wxStaticText* itemStaticText47 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Move the object to:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_moveObjectSizer->Add(itemStaticText47, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton48 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_UP, _("&Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton48->SetHelpText(_("Moves the object to the previous paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton48->SetToolTip(_("Moves the object to the previous paragraph."));
    m_moveObjectSizer->Add(itemButton48, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton49 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_DOWN, _("&Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton49->SetHelpText(_("Moves the object to the next paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton49->SetToolTip(_("Moves the object to the next paragraph."));
    m_moveObjectSizer->Add(itemButton49, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

////@end wxRichTextSizePage content construction
}

wxRichTextAttr* wxRichTextSizePage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextSizePage::ShowToolTips()
{
    return true;
}

bool wxRichTextSizePage::TransferDataToWindow()
{
    m_float->SetSelection(GetAttributes()->GetTextBoxAttr().GetFloatMode());
    m_verticalAlignmentCheckbox->SetValue(GetAttributes()->GetTextBoxAttr().HasVerticalAlignment());
    if (GetAttributes()->GetTextBoxAttr().HasVerticalAlignment())
    {
        if ((int) GetAttributes()->GetTextBoxAttr().GetVerticalAlignment() > 0 &&
            (int) GetAttributes()->GetTextBoxAttr().GetVerticalAlignment() < 4)
        {
            m_verticalAlignmentComboBox->SetSelection(((int) GetAttributes()->GetTextBoxAttr().GetVerticalAlignment() - 1));
        }
        else
            m_verticalAlignmentComboBox->SetSelection(0);
    }

    wxRichTextFormattingDialog* dialog = wxRichTextFormattingDialog::GetDialog(this);

    if (dialog)
    {
        // Only show the Move Object controls if there's an object.
        if (!dialog->GetObject())
        {
            m_moveObjectParentSizer->Show(m_moveObjectSizer, false);
            GetSizer()->Layout();
        }
    }

    if (dialog && dialog->GetObject())
    {
        wxTextAttrSize size = dialog->GetObject()->GetNaturalSize();
        if (size.GetWidth().IsValid() && size.GetHeight().IsValid())
        {
            if (!GetAttributes()->GetTextBoxAttr().GetWidth().IsValid() || GetAttributes()->GetTextBoxAttr().GetWidth().GetValue() <= 0)
            {
                GetAttributes()->GetTextBoxAttr().GetWidth() = size.GetWidth();
            }

            if (!GetAttributes()->GetTextBoxAttr().GetHeight().IsValid() || GetAttributes()->GetTextBoxAttr().GetHeight().GetValue() <= 0)
            {
                GetAttributes()->GetTextBoxAttr().GetHeight() = size.GetHeight();
            }
        }
    }

    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetWidth(), m_width, m_unitsW, m_widthCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetHeight(), m_height, m_unitsH, m_heightCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetTop(), m_offset, m_unitsOffset, m_offsetYCheckbox);

    return true;
}

bool wxRichTextSizePage::TransferDataFromWindow()
{
    GetAttributes()->GetTextBoxAttr().SetFloatMode((wxTextBoxAttrFloatStyle) m_float->GetSelection());
    if (m_float->GetSelection() == 0)
        GetAttributes()->GetTextBoxAttr().RemoveFlag(wxTEXT_BOX_ATTR_FLOAT);

    if (m_verticalAlignmentCheckbox->GetValue())
        GetAttributes()->GetTextBoxAttr().SetVerticalAlignment((wxTextBoxAttrVerticalAlignment) (m_verticalAlignmentComboBox->GetSelection() + 1));
    else
    {
        GetAttributes()->GetTextBoxAttr().SetVerticalAlignment(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_NONE);
        GetAttributes()->GetTextBoxAttr().RemoveFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);
    }

    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetWidth(), m_width, m_unitsW, m_widthCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetHeight(), m_height, m_unitsH, m_heightCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetTop(), m_offset, m_unitsOffset, m_offsetYCheckbox);

    return true;
}

// Show/hide position controls
void wxRichTextSizePage::ShowPositionControls(bool show)
{
    if (m_parentSizer)
    {
        m_parentSizer->Show(m_positionControls, show);
        Layout();
    }
}

// Show/hide floating controls
void wxRichTextSizePage::ShowFloatingControls(bool show)
{
    if (m_parentSizer)
    {
        m_parentSizer->Show(m_floatingControls, show);
        Layout();
    }
}


/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextSizePage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextSizePage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextSizePage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextSizePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextSizePage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextSizePage icon retrieval
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_WIDTH
 */

void wxRichTextSizePage::OnRichtextWidthUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_widthCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_HEIGHT
 */

void wxRichTextSizePage::OnRichtextHeightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_heightCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OFFSET
 */

void wxRichTextSizePage::OnRichtextOffsetUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_offsetYCheckbox->GetValue());
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXT_PARA_UP
 */

void wxRichTextSizePage::OnRichtextParaUpClick( wxCommandEvent& WXUNUSED(event) )
{
    wxRichTextFormattingDialog* dialog = wxRichTextFormattingDialog::GetDialog(this);
    if (!dialog || !dialog->GetObject())
        return;
        
    // Make sure object attributes are up-to-date
    dialog->TransferDataFromWindow();
        
    wxRichTextBuffer* buffer = dialog->GetObject()->GetBuffer();
    wxRichTextParagraphLayoutBox* container = dialog->GetObject()->GetParentContainer();
    wxRichTextObject* parent = dialog->GetObject()->GetParent();
    if (!container || !parent || !buffer)
        return;

    wxRichTextRange range = dialog->GetObject()->GetRange();
    wxRichTextObjectList::compatibility_iterator iter = container->GetChildren().GetFirst();
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == parent)
            break;
        iter = iter->GetNext();
    }
    if (!iter)
        return;
    iter = iter->GetPrevious();
    if (!iter)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    // tempObj will be deleted along with the undo object, and a clone of it will be
    // returned by InsertObjectWithUndo
    wxRichTextObject* tempObj = dialog->GetObject()->Clone();

    container->DeleteRangeWithUndo(range, buffer->GetRichTextCtrl(), buffer);

    wxRichTextObject* obj2 = container->InsertObjectWithUndo(rg.GetStart(), tempObj, buffer->GetRichTextCtrl(), buffer, 0);
    dialog->SetObject(obj2);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXT_DOWN
 */

void wxRichTextSizePage::OnRichtextParaDownClick( wxCommandEvent& WXUNUSED(event) )
{
    wxRichTextFormattingDialog* dialog = wxRichTextFormattingDialog::GetDialog(this);
    if (!dialog || !dialog->GetObject())
        return;
        
    // Make sure object attributes are up-to-date
    dialog->TransferDataFromWindow();
        
    wxRichTextBuffer* buffer = dialog->GetObject()->GetBuffer();
    wxRichTextParagraphLayoutBox* container = dialog->GetObject()->GetParentContainer();
    wxRichTextObject* parent = dialog->GetObject()->GetParent();
    if (!container || !parent || !buffer)
        return;

    wxRichTextRange range = dialog->GetObject()->GetRange();

    wxRichTextObjectList::compatibility_iterator iter = buffer->GetChildren().GetFirst();
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetNext();
    if (!iter)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    // tempObj will be deleted along with the undo object, and a clone of it will be
    // returned by InsertObjectWithUndo
    wxRichTextObject* tempObj = dialog->GetObject()->Clone();

    container->DeleteRangeWithUndo(range, buffer->GetRichTextCtrl(), buffer);

    // Minus one because we deleted an object
    wxRichTextObject* obj2 = container->InsertObjectWithUndo(rg.GetStart()-1, tempObj, buffer->GetRichTextCtrl(), buffer, 0);
    dialog->SetObject(obj2);
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX
 */

void wxRichTextSizePage::OnRichtextVerticalAlignmentComboboxUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_verticalAlignmentCheckbox->GetValue());
}

