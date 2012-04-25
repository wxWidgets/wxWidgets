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

    EVT_UPDATE_UI( ID_RICHTEXT_MIN_WIDTH, wxRichTextSizePage::OnRichtextMinWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MIN_W, wxRichTextSizePage::OnRichtextMinWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_MIN_HEIGHT, wxRichTextSizePage::OnRichtextMinHeightUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MIN_H, wxRichTextSizePage::OnRichtextMinHeightUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_MAX_WIDTH, wxRichTextSizePage::OnRichtextMaxWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MAX_W, wxRichTextSizePage::OnRichtextMaxWidthUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_MAX_HEIGHT, wxRichTextSizePage::OnRichtextMaxHeightUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MAX_H, wxRichTextSizePage::OnRichtextMaxHeightUpdate )

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
    m_minWidthCheckbox = NULL;
    m_minWidth = NULL;
    m_unitsMinW = NULL;
    m_minHeightCheckbox = NULL;
    m_minHeight = NULL;
    m_unitsMinH = NULL;
    m_maxWidthCheckbox = NULL;
    m_maxWidth = NULL;
    m_unitsMaxW = NULL;
    m_maxHeightCheckbox = NULL;
    m_maxHeight = NULL;
    m_unitsMaxH = NULL;
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

    wxFlexGridSizer* itemFlexGridSizer18 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer16->Add(itemFlexGridSizer18, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_widthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH_CHECKBOX, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_widthCheckbox->SetValue(false);
    m_widthCheckbox->SetHelpText(_("Enable the width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_widthCheckbox->SetToolTip(_("Enable the width value."));
    itemFlexGridSizer18->Add(m_widthCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_width = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
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
    itemFlexGridSizer18->Add(m_heightCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer24, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_height = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
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

    m_minWidthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MIN_WIDTH_CHECKBOX, _("Min width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_minWidthCheckbox->SetValue(false);
    m_minWidthCheckbox->SetHelpText(_("Enable the minimum width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minWidthCheckbox->SetToolTip(_("Enable the minimum width value."));
    itemFlexGridSizer18->Add(m_minWidthCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer28, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_minWidth = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MIN_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_minWidth->SetHelpText(_("The object minimum width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minWidth->SetToolTip(_("The object minimum width."));
    itemBoxSizer28->Add(m_minWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMinWStrings;
    m_unitsMinWStrings.Add(_("px"));
    m_unitsMinWStrings.Add(_("cm"));
    m_unitsMinWStrings.Add(_("percent"));
    m_unitsMinW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MIN_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMinWStrings, wxCB_READONLY );
    m_unitsMinW->SetStringSelection(_("px"));
    m_unitsMinW->SetHelpText(_("Units for the minimum object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMinW->SetToolTip(_("Units for the minimum object width."));
    itemBoxSizer28->Add(m_unitsMinW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_minHeightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MIN_HEIGHT_CHECKBOX, _("Min height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_minHeightCheckbox->SetValue(false);
    m_minHeightCheckbox->SetHelpText(_("Enable the minimum height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minHeightCheckbox->SetToolTip(_("Enable the minimum height value."));
    itemFlexGridSizer18->Add(m_minHeightCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer32 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer32, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_minHeight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MIN_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_minHeight->SetHelpText(_("The object minmum height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minHeight->SetToolTip(_("The object minmum height."));
    itemBoxSizer32->Add(m_minHeight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMinHStrings;
    m_unitsMinHStrings.Add(_("px"));
    m_unitsMinHStrings.Add(_("cm"));
    m_unitsMinHStrings.Add(_("percent"));
    m_unitsMinH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MIN_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMinHStrings, wxCB_READONLY );
    m_unitsMinH->SetStringSelection(_("px"));
    m_unitsMinH->SetHelpText(_("Units for the minimum object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMinH->SetToolTip(_("Units for the minimum object height."));
    itemBoxSizer32->Add(m_unitsMinH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_maxWidthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MAX_WIDTH_CHECKBOX, _("Max width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_maxWidthCheckbox->SetValue(false);
    m_maxWidthCheckbox->SetHelpText(_("Enable the maximum width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxWidthCheckbox->SetToolTip(_("Enable the maximum width value."));
    itemFlexGridSizer18->Add(m_maxWidthCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer36, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_maxWidth = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MAX_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_maxWidth->SetHelpText(_("The object maximum width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxWidth->SetToolTip(_("The object maximum width."));
    itemBoxSizer36->Add(m_maxWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMaxWStrings;
    m_unitsMaxWStrings.Add(_("px"));
    m_unitsMaxWStrings.Add(_("cm"));
    m_unitsMaxWStrings.Add(_("percent"));
    m_unitsMaxW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MAX_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMaxWStrings, wxCB_READONLY );
    m_unitsMaxW->SetStringSelection(_("px"));
    m_unitsMaxW->SetHelpText(_("Units for the maximum object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMaxW->SetToolTip(_("Units for the maximum object width."));
    itemBoxSizer36->Add(m_unitsMaxW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_maxHeightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MAX_HEIGHT_CHECKBOX, _("Max height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_maxHeightCheckbox->SetValue(false);
    m_maxHeightCheckbox->SetHelpText(_("Enable the height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxHeightCheckbox->SetToolTip(_("Enable the height value."));
    itemFlexGridSizer18->Add(m_maxHeightCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer40 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer18->Add(itemBoxSizer40, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_maxHeight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MAX_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_maxHeight->SetHelpText(_("The object maximum height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxHeight->SetToolTip(_("The object maximum height."));
    itemBoxSizer40->Add(m_maxHeight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMaxHStrings;
    m_unitsMaxHStrings.Add(_("px"));
    m_unitsMaxHStrings.Add(_("cm"));
    m_unitsMaxHStrings.Add(_("percent"));
    m_unitsMaxH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MAX_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMaxHStrings, wxCB_READONLY );
    m_unitsMaxH->SetStringSelection(_("px"));
    m_unitsMaxH->SetHelpText(_("Units for the maximum object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMaxH->SetToolTip(_("Units for the maximum object height."));
    itemBoxSizer40->Add(m_unitsMaxH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_alignmentControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_alignmentControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentControls->Add(itemBoxSizer44, 0, wxGROW, 5);

    wxStaticText* itemStaticText45 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Alignment"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText45->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer44->Add(itemStaticText45, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine46 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer44->Add(itemStaticLine46, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer47 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentControls->Add(itemBoxSizer47, 0, wxGROW, 5);

    itemBoxSizer47->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_verticalAlignmentCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_CHECKBOX, _("&Vertical alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_verticalAlignmentCheckbox->SetValue(false);
    m_verticalAlignmentCheckbox->SetHelpText(_("Enable vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentCheckbox->SetToolTip(_("Enable vertical alignment."));
    itemBoxSizer47->Add(m_verticalAlignmentCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_verticalAlignmentComboBoxStrings;
    m_verticalAlignmentComboBoxStrings.Add(_("Top"));
    m_verticalAlignmentComboBoxStrings.Add(_("Centred"));
    m_verticalAlignmentComboBoxStrings.Add(_("Bottom"));
    m_verticalAlignmentComboBox = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX, _("Top"), wxDefaultPosition, wxDefaultSize, m_verticalAlignmentComboBoxStrings, wxCB_READONLY );
    m_verticalAlignmentComboBox->SetStringSelection(_("Top"));
    m_verticalAlignmentComboBox->SetHelpText(_("Vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentComboBox->SetToolTip(_("Vertical alignment."));
    itemBoxSizer47->Add(m_verticalAlignmentComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_positionControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_positionControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer52 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer52, 0, wxGROW, 5);

    wxStaticText* itemStaticText53 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Position"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText53->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxBOLD, false, wxT("")));
    itemBoxSizer52->Add(itemStaticText53, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine54 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer52->Add(itemStaticLine54, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer55 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer55, 0, wxGROW, 5);

    itemBoxSizer55->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectParentSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer55->Add(m_moveObjectParentSizer, 0, wxALIGN_TOP, 5);

    wxBoxSizer* itemBoxSizer58 = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(itemBoxSizer58, 0, wxGROW, 5);

    m_offsetYCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET_CHECKBOX, _("Vertical &Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_offsetYCheckbox->SetValue(false);
    m_offsetYCheckbox->SetHelpText(_("Enable vertical offset."));
    if (wxRichTextSizePage::ShowToolTips())
        m_offsetYCheckbox->SetToolTip(_("Enable vertical offset."));
    itemBoxSizer58->Add(m_offsetYCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_offset = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_offset->SetMaxLength(10);
    m_offset->SetHelpText(_("The vertical offset relative to the paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        m_offset->SetToolTip(_("The vertical offset relative to the paragraph."));
    itemBoxSizer58->Add(m_offset, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsOffsetStrings;
    m_unitsOffsetStrings.Add(_("px"));
    m_unitsOffsetStrings.Add(_("cm"));
    m_unitsOffset = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsOffsetStrings, wxCB_READONLY );
    m_unitsOffset->SetStringSelection(_("px"));
    m_unitsOffset->SetHelpText(_("Units for the object offset."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsOffset->SetToolTip(_("Units for the object offset."));
    itemBoxSizer58->Add(m_unitsOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectSizer = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(m_moveObjectSizer, 0, wxGROW, 5);

    wxStaticText* itemStaticText63 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Move the object to:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_moveObjectSizer->Add(itemStaticText63, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton64 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_UP, _("&Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton64->SetHelpText(_("Moves the object to the previous paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton64->SetToolTip(_("Moves the object to the previous paragraph."));
    m_moveObjectSizer->Add(itemButton64, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton65 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_DOWN, _("&Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton65->SetHelpText(_("Moves the object to the next paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton65->SetToolTip(_("Moves the object to the next paragraph."));
    m_moveObjectSizer->Add(itemButton65, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

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

    wxRichTextImage* imageObj = NULL;
    if (dialog)
        imageObj = wxDynamicCast(dialog->GetObject(), wxRichTextImage);

    // For an image, show the original width and height if the size is not explicitly specified.
    if (imageObj && !GetAttributes()->GetTextBoxAttr().GetWidth().IsValid() && !GetAttributes()->GetTextBoxAttr().GetHeight().IsValid() &&
        imageObj->GetOriginalImageSize() != wxSize(-1, -1))
    {
        m_widthCheckbox->SetValue(false);
        m_heightCheckbox->SetValue(false);
        m_unitsW->SetSelection(0);
        m_unitsH->SetSelection(0);
        m_width->SetValue(wxString::Format(wxT("%d"), (int) imageObj->GetOriginalImageSize().GetWidth()));
        m_height->SetValue(wxString::Format(wxT("%d"), (int) imageObj->GetOriginalImageSize().GetHeight()));
    }
    else
    {
        wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetWidth(), m_width, m_unitsW, m_widthCheckbox);
        wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetHeight(), m_height, m_unitsH, m_heightCheckbox);
    }

    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMinSize().GetWidth(), m_minWidth, m_unitsMinW, m_minWidthCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMinSize().GetHeight(), m_minHeight, m_unitsMinH, m_minHeightCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMaxSize().GetWidth(), m_maxWidth, m_unitsMaxW, m_maxWidthCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMaxSize().GetHeight(), m_maxHeight, m_unitsMaxH, m_maxHeightCheckbox);
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
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMinSize().GetWidth(), m_minWidth, m_unitsMinW, m_minWidthCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMinSize().GetHeight(), m_minHeight, m_unitsMinH, m_minHeightCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMaxSize().GetWidth(), m_maxWidth, m_unitsMaxW, m_maxWidthCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMaxSize().GetHeight(), m_maxHeight, m_unitsMaxH, m_maxHeightCheckbox);
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

    wxRichTextObject* obj2 = container->InsertObjectWithUndo(buffer, rg.GetStart(), tempObj, buffer->GetRichTextCtrl(), 0);
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
    wxRichTextObject* obj2 = container->InsertObjectWithUndo(buffer, rg.GetStart()-1, tempObj, buffer->GetRichTextCtrl(), 0);
    dialog->SetObject(obj2);
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX
 */

void wxRichTextSizePage::OnRichtextVerticalAlignmentComboboxUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_verticalAlignmentCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_MIN_WIDTH
 */

void wxRichTextSizePage::OnRichtextMinWidthUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_minWidthCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_MIN_HEIGHT
 */

void wxRichTextSizePage::OnRichtextMinHeightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_minHeightCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_MAX_WIDTH
 */

void wxRichTextSizePage::OnRichtextMaxWidthUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_maxWidthCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_MAX_HEIGHT
 */

void wxRichTextSizePage::OnRichtextMaxHeightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_maxHeightCheckbox->GetValue());
}
