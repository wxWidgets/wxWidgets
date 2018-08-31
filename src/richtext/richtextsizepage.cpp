/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextsizepage.cpp
// Purpose:     Implements the rich text formatting dialog size page.
// Author:      Julian Smart
// Modified by:
// Created:     20/10/2010 10:23:24
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/richtext/richtextsizepage.h"

////@begin XPM images
////@end XPM images

/*!
 * wxRichTextSizePage type definition
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextSizePage, wxRichTextDialogPage);


/*!
 * wxRichTextSizePage event table definition
 */

wxBEGIN_EVENT_TABLE(wxRichTextSizePage, wxRichTextDialogPage)

////@begin wxRichTextSizePage event table entries
    EVT_UPDATE_UI( ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX, wxRichTextSizePage::OnRichtextVerticalAlignmentComboboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_WIDTH, wxRichTextSizePage::OnRichtextWidthUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_W, wxRichTextSizePage::OnRichtextWidthUnitsUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_HEIGHT, wxRichTextSizePage::OnRichtextHeightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_H, wxRichTextSizePage::OnRichtextHeightUnitsUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_MIN_WIDTH, wxRichTextSizePage::OnRichtextMinWidthUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MIN_W, wxRichTextSizePage::OnRichtextMinWidthUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_MIN_HEIGHT, wxRichTextSizePage::OnRichtextMinHeightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MIN_H, wxRichTextSizePage::OnRichtextMinHeightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_MAX_WIDTH, wxRichTextSizePage::OnRichtextMaxWidthUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MAX_W, wxRichTextSizePage::OnRichtextMaxWidthUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_MAX_HEIGHT, wxRichTextSizePage::OnRichtextMaxHeightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_UNITS_MAX_H, wxRichTextSizePage::OnRichtextMaxHeightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_LEFT, wxRichTextSizePage::OnRichtextLeftUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_LEFT_UNITS, wxRichTextSizePage::OnRichtextLeftUnitsUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_TOP, wxRichTextSizePage::OnRichtextTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_TOP_UNITS, wxRichTextSizePage::OnRichtextTopUnitsUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT, wxRichTextSizePage::OnRichtextRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT_UNITS, wxRichTextSizePage::OnRichtextRightUnitsUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM, wxRichTextSizePage::OnRichtextBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM_UNITS, wxRichTextSizePage::OnRichtextBottomUnitsUpdate )
    EVT_BUTTON( ID_RICHTEXT_PARA_UP, wxRichTextSizePage::OnRichtextParaUpClick )
    EVT_BUTTON( ID_RICHTEXT_PARA_DOWN, wxRichTextSizePage::OnRichtextParaDownClick )
////@end wxRichTextSizePage event table entries

wxEND_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextSizePage)

bool wxRichTextSizePage::sm_showFloatingControls = true;
bool wxRichTextSizePage::sm_showMinMaxSizeControls = true;
bool wxRichTextSizePage::sm_showPositionControls = true;
bool wxRichTextSizePage::sm_showPositionModeControls = true;
bool wxRichTextSizePage::sm_showRightBottomPositionControls = true;
bool wxRichTextSizePage::sm_showAlignmentControls = true;
bool wxRichTextSizePage::sm_showFloatingAndAlignmentControls = true;
bool wxRichTextSizePage::sm_enablePositionAndSizeUnits = true;
bool wxRichTextSizePage::sm_enablePositionAndSizeCheckboxes = true;
bool wxRichTextSizePage::sm_showMoveObjectControls = true;

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
    sm_showFloatingControls = true;
    sm_showPositionControls = true;
    sm_showPositionModeControls = true;
    sm_showRightBottomPositionControls = true;
    sm_showAlignmentControls = true;
    sm_showFloatingAndAlignmentControls = true;
    sm_showMinMaxSizeControls = true;
    sm_enablePositionAndSizeUnits = true;
    sm_enablePositionAndSizeCheckboxes = true;
    sm_showMoveObjectControls = true;
    
////@begin wxRichTextSizePage destruction
////@end wxRichTextSizePage destruction
}


/*!
 * Member initialisation
 */

void wxRichTextSizePage::Init()
{
////@begin wxRichTextSizePage member initialisation
    m_positionMode = 0;
    m_parentSizer = NULL;
    m_floatingAlignmentSizer = NULL;
    m_floatingSizer = NULL;
    m_float = NULL;
    m_alignmentSizer = NULL;
    m_verticalAlignmentCheckbox = NULL;
    m_verticalAlignmentComboBox = NULL;
    m_sizeSizer = NULL;
    m_widthSizer = NULL;
    m_widthCheckbox = NULL;
    m_widthLabel = NULL;
    m_width = NULL;
    m_unitsW = NULL;
    m_heightSizer = NULL;
    m_heightCheckbox = NULL;
    m_heightLabel = NULL;
    m_height = NULL;
    m_unitsH = NULL;
    m_minWidthCheckbox = NULL;
    m_minWidthSizer = NULL;
    m_minWidth = NULL;
    m_unitsMinW = NULL;
    m_minHeightCheckbox = NULL;
    m_minHeightSizer = NULL;
    m_minHeight = NULL;
    m_unitsMinH = NULL;
    m_maxWidthCheckbox = NULL;
    m_maxWidthSizer = NULL;
    m_maxWidth = NULL;
    m_unitsMaxW = NULL;
    m_maxHeightCheckbox = NULL;
    m_maxHeightSizer = NULL;
    m_maxHeight = NULL;
    m_unitsMaxH = NULL;
    m_positionControls = NULL;
    m_moveObjectParentSizer = NULL;
    m_positionModeSizer = NULL;
    m_positionModeCtrl = NULL;
    m_positionGridSizer = NULL;
    m_leftSizer = NULL;
    m_positionLeftCheckbox = NULL;
    m_leftLabel = NULL;
    m_left = NULL;
    m_unitsLeft = NULL;
    m_topSizer = NULL;
    m_positionTopCheckbox = NULL;
    m_topLabel = NULL;
    m_top = NULL;
    m_unitsTop = NULL;
    m_rightSizer = NULL;
    m_positionRightCheckbox = NULL;
    m_rightLabel = NULL;
    m_rightPositionSizer = NULL;
    m_right = NULL;
    m_unitsRight = NULL;
    m_bottomSizer = NULL;
    m_positionBottomCheckbox = NULL;
    m_bottomLabel = NULL;
    m_bottomPositionSizer = NULL;
    m_bottom = NULL;
    m_unitsBottom = NULL;
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

    m_floatingAlignmentSizer = new wxBoxSizer(wxHORIZONTAL);
    m_parentSizer->Add(m_floatingAlignmentSizer, 0, wxGROW, 5);

    m_floatingSizer = new wxBoxSizer(wxHORIZONTAL);
    m_floatingAlignmentSizer->Add(m_floatingSizer, 1, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    m_floatingSizer->Add(itemBoxSizer6, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(itemBoxSizer7, 0, wxGROW, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Floating"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText8->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer7->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine9 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer7->Add(itemStaticLine9, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(itemBoxSizer10, 0, wxGROW, 5);

    itemBoxSizer10->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer10->Add(itemFlexGridSizer12, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Floating mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(itemStaticText13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_floatStrings;
    m_floatStrings.Add(_("None"));
    m_floatStrings.Add(_("Left"));
    m_floatStrings.Add(_("Right"));
    m_float = new wxChoice( itemRichTextDialogPage1, ID_RICHTEXT_FLOATING_MODE, wxDefaultPosition, wxSize(80, -1), m_floatStrings, 0 );
    m_float->SetStringSelection(_("None"));
    m_float->SetHelpText(_("How the object will float relative to the text."));
    if (wxRichTextSizePage::ShowToolTips())
        m_float->SetToolTip(_("How the object will float relative to the text."));
    itemFlexGridSizer12->Add(m_float, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_floatingSizer->Add(0, 0, 1, wxALIGN_CENTER_VERTICAL, 5);

    m_alignmentSizer = new wxBoxSizer(wxVERTICAL);
    m_floatingAlignmentSizer->Add(m_alignmentSizer, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentSizer->Add(itemBoxSizer17, 0, wxGROW, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Alignment"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText18->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer17->Add(itemStaticText18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine19 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer17->Add(itemStaticLine19, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    m_alignmentSizer->Add(itemBoxSizer20, 0, wxGROW, 5);

    itemBoxSizer20->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_verticalAlignmentCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_CHECKBOX, _("&Vertical alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_verticalAlignmentCheckbox->SetValue(false);
    m_verticalAlignmentCheckbox->SetHelpText(_("Enable vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentCheckbox->SetToolTip(_("Enable vertical alignment."));
    itemBoxSizer20->Add(m_verticalAlignmentCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_verticalAlignmentComboBoxStrings;
    m_verticalAlignmentComboBoxStrings.Add(_("Top"));
    m_verticalAlignmentComboBoxStrings.Add(_("Centred"));
    m_verticalAlignmentComboBoxStrings.Add(_("Bottom"));
    m_verticalAlignmentComboBox = new wxChoice( itemRichTextDialogPage1, ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX, wxDefaultPosition, wxDefaultSize, m_verticalAlignmentComboBoxStrings, 0 );
    m_verticalAlignmentComboBox->SetStringSelection(_("Top"));
    m_verticalAlignmentComboBox->SetHelpText(_("Vertical alignment."));
    if (wxRichTextSizePage::ShowToolTips())
        m_verticalAlignmentComboBox->SetToolTip(_("Vertical alignment."));
    itemBoxSizer20->Add(m_verticalAlignmentComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    m_parentSizer->Add(itemBoxSizer24, 0, wxGROW, 5);

    wxStaticText* itemStaticText25 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Size"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText25->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer24->Add(itemStaticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine26 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer24->Add(itemStaticLine26, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    m_parentSizer->Add(itemBoxSizer27, 0, wxGROW, 5);

    itemBoxSizer27->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_sizeSizer = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer27->Add(m_sizeSizer, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_widthSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_widthSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_widthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH_CHECKBOX, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_widthCheckbox->SetValue(false);
    m_widthCheckbox->SetHelpText(_("Enable the width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_widthCheckbox->SetToolTip(_("Enable the width value."));
    m_widthSizer->Add(m_widthCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_widthLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_widthLabel->Show(false);
    m_widthSizer->Add(m_widthLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(itemBoxSizer33, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_width = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_width->SetHelpText(_("The object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_width->SetToolTip(_("The object width."));
    itemBoxSizer33->Add(m_width, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsWStrings;
    m_unitsWStrings.Add(_("px"));
    m_unitsWStrings.Add(_("cm"));
    m_unitsWStrings.Add(_("%"));
    m_unitsW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsWStrings, wxCB_READONLY );
    m_unitsW->SetStringSelection(_("px"));
    m_unitsW->SetHelpText(_("Units for the object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsW->SetToolTip(_("Units for the object width."));
    itemBoxSizer33->Add(m_unitsW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_heightSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_heightSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_heightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_HEIGHT_CHECKBOX, _("&Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_heightCheckbox->SetValue(false);
    m_heightCheckbox->SetHelpText(_("Enable the height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_heightCheckbox->SetToolTip(_("Enable the height value."));
    m_heightSizer->Add(m_heightCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_heightLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_heightLabel->Show(false);
    m_heightSizer->Add(m_heightLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(itemBoxSizer39, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_height = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_height->SetHelpText(_("The object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_height->SetToolTip(_("The object height."));
    itemBoxSizer39->Add(m_height, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsHStrings;
    m_unitsHStrings.Add(_("px"));
    m_unitsHStrings.Add(_("cm"));
    m_unitsHStrings.Add(_("%"));
    m_unitsH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsHStrings, wxCB_READONLY );
    m_unitsH->SetStringSelection(_("px"));
    m_unitsH->SetHelpText(_("Units for the object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsH->SetToolTip(_("Units for the object height."));
    itemBoxSizer39->Add(m_unitsH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_minWidthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MIN_WIDTH_CHECKBOX, _("Min width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_minWidthCheckbox->SetValue(false);
    m_minWidthCheckbox->SetHelpText(_("Enable the minimum width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minWidthCheckbox->SetToolTip(_("Enable the minimum width value."));
    m_sizeSizer->Add(m_minWidthCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_minWidthSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_minWidthSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_minWidth = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MIN_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_minWidth->SetHelpText(_("The object minimum width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minWidth->SetToolTip(_("The object minimum width."));
    m_minWidthSizer->Add(m_minWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMinWStrings;
    m_unitsMinWStrings.Add(_("px"));
    m_unitsMinWStrings.Add(_("cm"));
    m_unitsMinWStrings.Add(_("%"));
    m_unitsMinW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MIN_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMinWStrings, wxCB_READONLY );
    m_unitsMinW->SetStringSelection(_("px"));
    m_unitsMinW->SetHelpText(_("Units for the minimum object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMinW->SetToolTip(_("Units for the minimum object width."));
    m_minWidthSizer->Add(m_unitsMinW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_minHeightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MIN_HEIGHT_CHECKBOX, _("Min height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_minHeightCheckbox->SetValue(false);
    m_minHeightCheckbox->SetHelpText(_("Enable the minimum height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minHeightCheckbox->SetToolTip(_("Enable the minimum height value."));
    m_sizeSizer->Add(m_minHeightCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_minHeightSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_minHeightSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_minHeight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MIN_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_minHeight->SetHelpText(_("The object minimum height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_minHeight->SetToolTip(_("The object minimum height."));
    m_minHeightSizer->Add(m_minHeight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMinHStrings;
    m_unitsMinHStrings.Add(_("px"));
    m_unitsMinHStrings.Add(_("cm"));
    m_unitsMinHStrings.Add(_("%"));
    m_unitsMinH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MIN_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMinHStrings, wxCB_READONLY );
    m_unitsMinH->SetStringSelection(_("px"));
    m_unitsMinH->SetHelpText(_("Units for the minimum object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMinH->SetToolTip(_("Units for the minimum object height."));
    m_minHeightSizer->Add(m_unitsMinH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_maxWidthCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MAX_WIDTH_CHECKBOX, _("Max width:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_maxWidthCheckbox->SetValue(false);
    m_maxWidthCheckbox->SetHelpText(_("Enable the maximum width value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxWidthCheckbox->SetToolTip(_("Enable the maximum width value."));
    m_sizeSizer->Add(m_maxWidthCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_maxWidthSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_maxWidthSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_maxWidth = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MAX_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_maxWidth->SetHelpText(_("The object maximum width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxWidth->SetToolTip(_("The object maximum width."));
    m_maxWidthSizer->Add(m_maxWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMaxWStrings;
    m_unitsMaxWStrings.Add(_("px"));
    m_unitsMaxWStrings.Add(_("cm"));
    m_unitsMaxWStrings.Add(_("%"));
    m_unitsMaxW = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MAX_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMaxWStrings, wxCB_READONLY );
    m_unitsMaxW->SetStringSelection(_("px"));
    m_unitsMaxW->SetHelpText(_("Units for the maximum object width."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMaxW->SetToolTip(_("Units for the maximum object width."));
    m_maxWidthSizer->Add(m_unitsMaxW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_maxHeightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_MAX_HEIGHT_CHECKBOX, _("Max height:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_maxHeightCheckbox->SetValue(false);
    m_maxHeightCheckbox->SetHelpText(_("Enable the height value."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxHeightCheckbox->SetToolTip(_("Enable the height value."));
    m_sizeSizer->Add(m_maxHeightCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_maxHeightSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizeSizer->Add(m_maxHeightSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_maxHeight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_MAX_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_maxHeight->SetHelpText(_("The object maximum height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_maxHeight->SetToolTip(_("The object maximum height."));
    m_maxHeightSizer->Add(m_maxHeight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMaxHStrings;
    m_unitsMaxHStrings.Add(_("px"));
    m_unitsMaxHStrings.Add(_("cm"));
    m_unitsMaxHStrings.Add(_("%"));
    m_unitsMaxH = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_UNITS_MAX_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMaxHStrings, wxCB_READONLY );
    m_unitsMaxH->SetStringSelection(_("px"));
    m_unitsMaxH->SetHelpText(_("Units for the maximum object height."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsMaxH->SetToolTip(_("Units for the maximum object height."));
    m_maxHeightSizer->Add(m_unitsMaxH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_positionControls = new wxBoxSizer(wxVERTICAL);
    m_parentSizer->Add(m_positionControls, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer59 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer59, 0, wxGROW, 5);

    wxStaticText* itemStaticText60 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Position"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText60->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer59->Add(itemStaticText60, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine61 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer59->Add(itemStaticLine61, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer62 = new wxBoxSizer(wxHORIZONTAL);
    m_positionControls->Add(itemBoxSizer62, 0, wxGROW, 5);

    itemBoxSizer62->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectParentSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer62->Add(m_moveObjectParentSizer, 0, wxALIGN_TOP, 5);

    m_positionModeSizer = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(m_positionModeSizer, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText66 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Position mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_positionModeSizer->Add(itemStaticText66, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_positionModeCtrlStrings;
    m_positionModeCtrlStrings.Add(_("Static"));
    m_positionModeCtrlStrings.Add(_("Relative"));
    m_positionModeCtrlStrings.Add(_("Absolute"));
    m_positionModeCtrlStrings.Add(_("Fixed"));
    m_positionModeCtrl = new wxChoice( itemRichTextDialogPage1, ID_RICHTEXT_POSITION_MODE, wxDefaultPosition, wxDefaultSize, m_positionModeCtrlStrings, 0 );
    m_positionModeCtrl->SetStringSelection(_("Static"));
    m_positionModeSizer->Add(m_positionModeCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_positionGridSizer = new wxFlexGridSizer(0, 4, 0, 0);
    m_moveObjectParentSizer->Add(m_positionGridSizer, 0, wxGROW, 5);

    m_leftSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_leftSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_positionLeftCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_CHECKBOX, _("&Left:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_positionLeftCheckbox->SetValue(false);
    m_positionLeftCheckbox->SetHelpText(_("The left position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_positionLeftCheckbox->SetToolTip(_("The left position."));
    m_leftSizer->Add(m_positionLeftCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_leftLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Left:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_leftLabel->Show(false);
    m_leftSizer->Add(m_leftLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer72 = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(itemBoxSizer72, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

    m_left = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_LEFT, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_left->SetMaxLength(10);
    m_left->SetHelpText(_("The left position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_left->SetToolTip(_("The left position."));
    itemBoxSizer72->Add(m_left, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsLeftStrings;
    m_unitsLeftStrings.Add(_("px"));
    m_unitsLeftStrings.Add(_("cm"));
    m_unitsLeftStrings.Add(_("%"));
    m_unitsLeft = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsLeftStrings, wxCB_READONLY );
    m_unitsLeft->SetStringSelection(_("px"));
    m_unitsLeft->SetHelpText(_("Units for the left position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsLeft->SetToolTip(_("Units for the left position."));
    itemBoxSizer72->Add(m_unitsLeft, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_topSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_topSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_positionTopCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_CHECKBOX, _("&Top:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_positionTopCheckbox->SetValue(false);
    m_positionTopCheckbox->SetHelpText(_("The top position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_positionTopCheckbox->SetToolTip(_("The top position."));
    m_topSizer->Add(m_positionTopCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_topLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Top:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_topLabel->Show(false);
    m_topSizer->Add(m_topLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer78 = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(itemBoxSizer78, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

    m_top = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_TOP, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_top->SetMaxLength(10);
    m_top->SetHelpText(_("The top position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_top->SetToolTip(_("The top position."));
    itemBoxSizer78->Add(m_top, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsTopStrings;
    m_unitsTopStrings.Add(_("px"));
    m_unitsTopStrings.Add(_("cm"));
    m_unitsTopStrings.Add(_("%"));
    m_unitsTop = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsTopStrings, wxCB_READONLY );
    m_unitsTop->SetStringSelection(_("px"));
    m_unitsTop->SetHelpText(_("Units for the top position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsTop->SetToolTip(_("Units for the top position."));
    itemBoxSizer78->Add(m_unitsTop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_rightSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_positionRightCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_CHECKBOX, _("&Right:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_positionRightCheckbox->SetValue(false);
    m_positionRightCheckbox->SetHelpText(_("The right position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_positionRightCheckbox->SetToolTip(_("The right position."));
    m_rightSizer->Add(m_positionRightCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_rightLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Right:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rightLabel->Show(false);
    m_rightSizer->Add(m_rightLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightPositionSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_rightPositionSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

    m_right = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_right->SetMaxLength(10);
    m_right->SetHelpText(_("The right position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_right->SetToolTip(_("The right position."));
    m_rightPositionSizer->Add(m_right, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsRightStrings;
    m_unitsRightStrings.Add(_("px"));
    m_unitsRightStrings.Add(_("cm"));
    m_unitsRightStrings.Add(_("%"));
    m_unitsRight = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsRightStrings, wxCB_READONLY );
    m_unitsRight->SetStringSelection(_("px"));
    m_unitsRight->SetHelpText(_("Units for the right position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsRight->SetToolTip(_("Units for the right position."));
    m_rightPositionSizer->Add(m_unitsRight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_bottomSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_positionBottomCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_CHECKBOX, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_positionBottomCheckbox->SetValue(false);
    m_positionBottomCheckbox->SetHelpText(_("The bottom position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_positionBottomCheckbox->SetToolTip(_("The bottom position."));
    m_bottomSizer->Add(m_positionBottomCheckbox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_bottomLabel = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_bottomLabel->Show(false);
    m_bottomSizer->Add(m_bottomLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomPositionSizer = new wxBoxSizer(wxHORIZONTAL);
    m_positionGridSizer->Add(m_bottomPositionSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

    m_bottom = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_bottom->SetMaxLength(10);
    m_bottom->SetHelpText(_("The bottom position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_bottom->SetToolTip(_("The bottom position."));
    m_bottomPositionSizer->Add(m_bottom, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsBottomStrings;
    m_unitsBottomStrings.Add(_("px"));
    m_unitsBottomStrings.Add(_("cm"));
    m_unitsBottomStrings.Add(_("%"));
    m_unitsBottom = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsBottomStrings, wxCB_READONLY );
    m_unitsBottom->SetStringSelection(_("px"));
    m_unitsBottom->SetHelpText(_("Units for the bottom position."));
    if (wxRichTextSizePage::ShowToolTips())
        m_unitsBottom->SetToolTip(_("Units for the bottom position."));
    m_bottomPositionSizer->Add(m_unitsBottom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_moveObjectSizer = new wxBoxSizer(wxHORIZONTAL);
    m_moveObjectParentSizer->Add(m_moveObjectSizer, 0, wxGROW, 5);

    wxStaticText* itemStaticText94 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Move the object to:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_moveObjectSizer->Add(itemStaticText94, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton95 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_UP, _("&Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton95->SetHelpText(_("Moves the object to the previous paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton95->SetToolTip(_("Moves the object to the previous paragraph."));
    m_moveObjectSizer->Add(itemButton95, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton96 = new wxButton( itemRichTextDialogPage1, ID_RICHTEXT_PARA_DOWN, _("&Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton96->SetHelpText(_("Moves the object to the next paragraph."));
    if (wxRichTextSizePage::ShowToolTips())
        itemButton96->SetToolTip(_("Moves the object to the next paragraph."));
    m_moveObjectSizer->Add(itemButton96, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

#if wxUSE_VALIDATORS
    // Set validators
    m_positionModeCtrl->SetValidator( wxGenericValidator(& m_positionMode) );
#endif
////@end wxRichTextSizePage content construction

    if (!sm_enablePositionAndSizeCheckboxes)
    {
        m_widthSizer->Show(m_widthCheckbox, false);
        m_widthSizer->Show(m_widthLabel, true);
        m_heightSizer->Show(m_heightCheckbox, false);
        m_heightSizer->Show(m_heightLabel, true);

        m_leftSizer->Show(m_positionLeftCheckbox, false);
        m_leftSizer->Show(m_leftLabel, true);
        m_topSizer->Show(m_positionTopCheckbox, false);
        m_topSizer->Show(m_topLabel, true);

        if (sm_showRightBottomPositionControls)
        {
            m_rightSizer->Show(m_positionRightCheckbox, false);
            m_rightSizer->Show(m_rightLabel, true);
            m_bottomSizer->Show(m_positionBottomCheckbox, false);
            m_bottomSizer->Show(m_bottomLabel, true);
        }
    }

    if (!sm_showFloatingAndAlignmentControls)
        m_parentSizer->Show(m_floatingAlignmentSizer, false);

    if (!sm_showFloatingControls)
        m_floatingAlignmentSizer->Show(m_floatingSizer, false);

    if (!sm_showAlignmentControls)
        m_floatingAlignmentSizer->Show(m_alignmentSizer, false);

    if (!sm_showPositionControls)
        m_parentSizer->Show(m_positionControls, false);
        
    if (!sm_showMinMaxSizeControls)
    {
        m_sizeSizer->Show(m_minWidthCheckbox, false);
        m_sizeSizer->Show(m_minWidthSizer, false);
        m_sizeSizer->Show(m_minHeightCheckbox, false);
        m_sizeSizer->Show(m_minHeightSizer, false);
        m_sizeSizer->Show(m_maxWidthCheckbox, false);
        m_sizeSizer->Show(m_maxWidthSizer, false);
        m_sizeSizer->Show(m_maxHeightCheckbox, false);
        m_sizeSizer->Show(m_maxHeightSizer, false);
    }

    if (!sm_showRightBottomPositionControls)
    {
        m_positionGridSizer->Show(m_rightSizer, false);
        m_positionGridSizer->Show(m_rightPositionSizer, false);
        m_positionGridSizer->Show(m_bottomSizer, false);
        m_positionGridSizer->Show(m_bottomPositionSizer, false);
    }

    if (!sm_showPositionModeControls)
        m_moveObjectParentSizer->Show(m_positionModeSizer, false);

    if (!sm_showMoveObjectControls)
        m_moveObjectParentSizer->Show(m_moveObjectSizer, false);
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
    return wxRichTextFormattingDialog::ShowToolTips();
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

    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetLeft(), m_left, m_unitsLeft, m_positionLeftCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetTop(), m_top, m_unitsTop, m_positionTopCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetRight(), m_right, m_unitsRight, m_positionRightCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetBottom(), m_bottom, m_unitsBottom, m_positionBottomCheckbox);

    if ((GetAttributes()->GetTextBoxAttr().GetLeft().GetPosition() & wxTEXT_BOX_ATTR_POSITION_FIXED) ||
        (GetAttributes()->GetTextBoxAttr().GetRight().GetPosition() & wxTEXT_BOX_ATTR_POSITION_FIXED) ||
        (GetAttributes()->GetTextBoxAttr().GetTop().GetPosition() & wxTEXT_BOX_ATTR_POSITION_FIXED) ||
        (GetAttributes()->GetTextBoxAttr().GetBottom().GetPosition() & wxTEXT_BOX_ATTR_POSITION_FIXED))
    {
        m_positionMode = 3;
    }
    else
    if ((GetAttributes()->GetTextBoxAttr().GetLeft().GetPosition() & wxTEXT_BOX_ATTR_POSITION_ABSOLUTE) ||
        (GetAttributes()->GetTextBoxAttr().GetRight().GetPosition() & wxTEXT_BOX_ATTR_POSITION_ABSOLUTE) ||
        (GetAttributes()->GetTextBoxAttr().GetTop().GetPosition() & wxTEXT_BOX_ATTR_POSITION_ABSOLUTE) ||
        (GetAttributes()->GetTextBoxAttr().GetBottom().GetPosition() & wxTEXT_BOX_ATTR_POSITION_ABSOLUTE))
    {
        m_positionMode = 2;
    }
    else
    if ((GetAttributes()->GetTextBoxAttr().GetLeft().GetPosition() & wxTEXT_BOX_ATTR_POSITION_RELATIVE) ||
        (GetAttributes()->GetTextBoxAttr().GetRight().GetPosition() & wxTEXT_BOX_ATTR_POSITION_RELATIVE) ||
        (GetAttributes()->GetTextBoxAttr().GetTop().GetPosition() & wxTEXT_BOX_ATTR_POSITION_RELATIVE) ||
        (GetAttributes()->GetTextBoxAttr().GetBottom().GetPosition() & wxTEXT_BOX_ATTR_POSITION_RELATIVE))
    {
        m_positionMode = 1;
    }
    else
        m_positionMode = 0;

    return wxPanel::TransferDataToWindow();
}

bool wxRichTextSizePage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

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

    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetLeft(), m_left, m_unitsLeft, m_positionLeftCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetTop(), m_top, m_unitsTop, m_positionTopCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetRight(), m_right, m_unitsRight, m_positionRightCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetBottom(), m_bottom, m_unitsBottom, m_positionBottomCheckbox);

    wxTextBoxAttrPosition flags;
    if (m_positionMode == 3)
        flags = wxTEXT_BOX_ATTR_POSITION_FIXED;
    else if (m_positionMode == 2)
        flags = wxTEXT_BOX_ATTR_POSITION_ABSOLUTE;
    else if (m_positionMode == 1)
        flags = wxTEXT_BOX_ATTR_POSITION_RELATIVE;
    else
        flags = wxTEXT_BOX_ATTR_POSITION_STATIC;

    if (GetAttributes()->GetTextBoxAttr().GetLeft().IsValid())
        GetAttributes()->GetTextBoxAttr().GetLeft().SetPosition(flags);
    if (GetAttributes()->GetTextBoxAttr().GetTop().IsValid())
        GetAttributes()->GetTextBoxAttr().GetTop().SetPosition(flags);
    if (GetAttributes()->GetTextBoxAttr().GetRight().IsValid())
        GetAttributes()->GetTextBoxAttr().GetRight().SetPosition(flags);
    if (GetAttributes()->GetTextBoxAttr().GetBottom().IsValid())
        GetAttributes()->GetTextBoxAttr().GetBottom().SetPosition(flags);

    return true;
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
 * wxEVT_BUTTON event handler for ID_RICHTEXT_PARA_UP
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
 * wxEVT_BUTTON event handler for ID_RICHTEXT_DOWN
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

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_LEFT
 */

void wxRichTextSizePage::OnRichtextLeftUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionLeftCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_RIGHT
 */

void wxRichTextSizePage::OnRichtextRightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionRightCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_TOP
 */

void wxRichTextSizePage::OnRichtextTopUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionTopCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BOTTOM
 */

void wxRichTextSizePage::OnRichtextBottomUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionBottomCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_WIDTH_UNITS
 */

void wxRichTextSizePage::OnRichtextWidthUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_widthCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_HEIGHT_UNITS
 */

void wxRichTextSizePage::OnRichtextHeightUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_heightCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_LEFT_UNITS
 */

void wxRichTextSizePage::OnRichtextLeftUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionLeftCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_RIGHT_UNITS
 */

void wxRichTextSizePage::OnRichtextRightUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionRightCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_TOP_UNITS
 */

void wxRichTextSizePage::OnRichtextTopUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionTopCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BOTTOM_UNITS
 */

void wxRichTextSizePage::OnRichtextBottomUnitsUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_positionBottomCheckbox->GetValue() && sm_enablePositionAndSizeUnits);
}
