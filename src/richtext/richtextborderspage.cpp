/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextborderspage.cpp
// Purpose:     A border editing page for the wxRTC formatting dialog.
// Author:      Julian Smart
// Created:     21/10/2010 11:34:24
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "wx/richtext/richtextborderspage.h"

////@begin XPM images
////@end XPM images

/*!
 * wxRichTextBordersPage type definition
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextBordersPage, wxRichTextDialogPage);

/*!
 * wxRichTextBordersPage event table definition
 */

wxBEGIN_EVENT_TABLE(wxRichTextBordersPage, wxRichTextDialogPage)

    EVT_CHECKBOX(wxID_ANY, wxRichTextBordersPage::OnCommand)
    EVT_TEXT(wxID_ANY, wxRichTextBordersPage::OnCommand)
    EVT_TEXT_ENTER(wxID_ANY, wxRichTextBordersPage::OnCommand)
    EVT_COMBOBOX(wxID_ANY, wxRichTextBordersPage::OnCommand)
    EVT_BUTTON(wxID_ANY, wxRichTextBordersPage::OnCommand)

////@begin wxRichTextBordersPage event table entries
    EVT_CHECKBOX( ID_RICHTEXT_BORDER_LEFT_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_TEXT( ID_RICHTEXT_BORDER_LEFT, wxRichTextBordersPage::OnRichtextBorderLeftValueTextUpdated )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_LEFT, wxRichTextBordersPage::OnRichtextBorderLeftUpdate )
    EVT_COMBOBOX( ID_RICHTEXT_BORDER_LEFT_UNITS, wxRichTextBordersPage::OnRichtextBorderLeftUnitsSelected )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_LEFT_UNITS, wxRichTextBordersPage::OnRichtextBorderLeftUpdate )
    EVT_COMBOBOX( ID_RICHTEXT_BORDER_LEFT_STYLE, wxRichTextBordersPage::OnRichtextBorderLeftStyleSelected )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_LEFT_STYLE, wxRichTextBordersPage::OnRichtextBorderLeftUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_LEFT_COLOUR, wxRichTextBordersPage::OnRichtextBorderLeftUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_BORDER_RIGHT_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_RIGHT_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_RIGHT, wxRichTextBordersPage::OnRichtextBorderRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_RIGHT_UNITS, wxRichTextBordersPage::OnRichtextBorderRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_RIGHT_STYLE, wxRichTextBordersPage::OnRichtextBorderRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_RIGHT_COLOUR, wxRichTextBordersPage::OnRichtextBorderRightUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_BORDER_TOP_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_TOP_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_TOP, wxRichTextBordersPage::OnRichtextBorderTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_TOP_UNITS, wxRichTextBordersPage::OnRichtextBorderTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_TOP_STYLE, wxRichTextBordersPage::OnRichtextBorderTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_TOP_COLOUR, wxRichTextBordersPage::OnRichtextBorderTopUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_BORDER_BOTTOM_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_BOTTOM_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_BOTTOM, wxRichTextBordersPage::OnRichtextBorderBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_BOTTOM_UNITS, wxRichTextBordersPage::OnRichtextBorderBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_BOTTOM_STYLE, wxRichTextBordersPage::OnRichtextBorderBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_BOTTOM_COLOUR, wxRichTextBordersPage::OnRichtextBorderBottomUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_BORDER_SYNCHRONIZE, wxRichTextBordersPage::OnRichtextBorderSynchronizeClick )
    EVT_UPDATE_UI( ID_RICHTEXT_BORDER_SYNCHRONIZE, wxRichTextBordersPage::OnRichtextBorderSynchronizeUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_OUTLINE_LEFT_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_TEXT( ID_RICHTEXT_OUTLINE_LEFT, wxRichTextBordersPage::OnRichtextOutlineLeftTextUpdated )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_LEFT, wxRichTextBordersPage::OnRichtextOutlineLeftUpdate )
    EVT_COMBOBOX( ID_RICHTEXT_OUTLINE_LEFT_UNITS, wxRichTextBordersPage::OnRichtextOutlineLeftUnitsSelected )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_LEFT_UNITS, wxRichTextBordersPage::OnRichtextOutlineLeftUpdate )
    EVT_COMBOBOX( ID_RICHTEXT_OUTLINE_LEFT_STYLE, wxRichTextBordersPage::OnRichtextOutlineLeftStyleSelected )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_LEFT_STYLE, wxRichTextBordersPage::OnRichtextOutlineLeftUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_LEFT_COLOUR, wxRichTextBordersPage::OnRichtextOutlineLeftUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_OUTLINE_RIGHT_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_RIGHT_CHECKBOX, wxRichTextBordersPage::OnRichtextOutlineOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_RIGHT, wxRichTextBordersPage::OnRichtextOutlineRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_RIGHT_UNITS, wxRichTextBordersPage::OnRichtextOutlineRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_RIGHT_STYLE, wxRichTextBordersPage::OnRichtextOutlineRightUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_RIGHT_COLOUR, wxRichTextBordersPage::OnRichtextOutlineRightUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_OUTLINE_TOP_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_TOP_CHECKBOX, wxRichTextBordersPage::OnRichtextOutlineOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_TOP, wxRichTextBordersPage::OnRichtextOutlineTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_TOP_UNITS, wxRichTextBordersPage::OnRichtextOutlineTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_TOP_STYLE, wxRichTextBordersPage::OnRichtextOutlineTopUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_TOP_COLOUR, wxRichTextBordersPage::OnRichtextOutlineTopUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_OUTLINE_BOTTOM_CHECKBOX, wxRichTextBordersPage::OnRichtextBorderCheckboxClick )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_BOTTOM_CHECKBOX, wxRichTextBordersPage::OnRichtextOutlineOtherCheckboxUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_BOTTOM, wxRichTextBordersPage::OnRichtextOutlineBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_BOTTOM_UNITS, wxRichTextBordersPage::OnRichtextOutlineBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_BOTTOM_STYLE, wxRichTextBordersPage::OnRichtextOutlineBottomUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_BOTTOM_COLOUR, wxRichTextBordersPage::OnRichtextOutlineBottomUpdate )
    EVT_CHECKBOX( ID_RICHTEXT_OUTLINE_SYNCHRONIZE, wxRichTextBordersPage::OnRichtextOutlineSynchronizeClick )
    EVT_UPDATE_UI( ID_RICHTEXT_OUTLINE_SYNCHRONIZE, wxRichTextBordersPage::OnRichtextOutlineSynchronizeUpdate )
    EVT_UPDATE_UI( ID_RICHTEXTBORDERSPAGE_CORNER_TEXT, wxRichTextBordersPage::OnRichtextborderspageCornerUpdate )
    EVT_UPDATE_UI( ID_RICHTEXTBORDERSPAGE_CORNER_UNITS, wxRichTextBordersPage::OnRichtextborderspageCornerUpdate )
////@end wxRichTextBordersPage event table entries

wxEND_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextBordersPage)

/*!
 * wxRichTextBordersPage constructors
 */

wxRichTextBordersPage::wxRichTextBordersPage()
{
    Init();
}

wxRichTextBordersPage::wxRichTextBordersPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * wxRichTextBordersPage creator
 */

bool wxRichTextBordersPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextBordersPage creation
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextBordersPage creation
    return true;
}

/*!
 * wxRichTextBordersPage destructor
 */

wxRichTextBordersPage::~wxRichTextBordersPage()
{
////@begin wxRichTextBordersPage destruction
////@end wxRichTextBordersPage destruction
}

/*!
 * Member initialisation
 */

void wxRichTextBordersPage::Init()
{
    m_ignoreUpdates = false;

////@begin wxRichTextBordersPage member initialisation
    m_leftBorderCheckbox = nullptr;
    m_leftBorderWidth = nullptr;
    m_leftBorderWidthUnits = nullptr;
    m_leftBorderStyle = nullptr;
    m_leftBorderColour = nullptr;
    m_rightBorderCheckbox = nullptr;
    m_rightBorderWidth = nullptr;
    m_rightBorderWidthUnits = nullptr;
    m_rightBorderStyle = nullptr;
    m_rightBorderColour = nullptr;
    m_topBorderCheckbox = nullptr;
    m_topBorderWidth = nullptr;
    m_topBorderWidthUnits = nullptr;
    m_topBorderStyle = nullptr;
    m_topBorderColour = nullptr;
    m_bottomBorderCheckbox = nullptr;
    m_bottomBorderWidth = nullptr;
    m_bottomBorderWidthUnits = nullptr;
    m_bottomBorderStyle = nullptr;
    m_bottomBorderColour = nullptr;
    m_borderSyncCtrl = nullptr;
    m_leftOutlineCheckbox = nullptr;
    m_leftOutlineWidth = nullptr;
    m_leftOutlineWidthUnits = nullptr;
    m_leftOutlineStyle = nullptr;
    m_leftOutlineColour = nullptr;
    m_rightOutlineCheckbox = nullptr;
    m_rightOutlineWidth = nullptr;
    m_rightOutlineWidthUnits = nullptr;
    m_rightOutlineStyle = nullptr;
    m_rightOutlineColour = nullptr;
    m_topOutlineCheckbox = nullptr;
    m_topOutlineWidth = nullptr;
    m_topOutlineWidthUnits = nullptr;
    m_topOutlineStyle = nullptr;
    m_topOutlineColour = nullptr;
    m_bottomOutlineCheckbox = nullptr;
    m_bottomOutlineWidth = nullptr;
    m_bottomOutlineWidthUnits = nullptr;
    m_bottomOutlineStyle = nullptr;
    m_bottomOutlineColour = nullptr;
    m_outlineSyncCtrl = nullptr;
    m_cornerRadiusCheckBox = nullptr;
    m_cornerRadiusText = nullptr;
    m_cornerRadiusUnits = nullptr;
    m_borderPreviewCtrl = nullptr;
////@end wxRichTextBordersPage member initialisation
}

/*!
 * Control creation for wxRichTextBordersPage
 */

void wxRichTextBordersPage::CreateControls()
{    
////@begin wxRichTextBordersPage content construction
    wxRichTextBordersPage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxNotebook* itemNotebook4 = new wxNotebook( itemRichTextDialogPage1, ID_RICHTEXTBORDERSPAGE_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    wxPanel* itemPanel5 = new wxPanel( itemNotebook4, ID_RICHTEXTBORDERSPAGE_BORDERS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 0, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer7->Add(itemBoxSizer8, 0, wxGROW, 5);
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel5, wxID_STATIC, _("Border"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine10 = new wxStaticLine( itemPanel5, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer8->Add(itemStaticLine10, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer7->Add(itemBoxSizer11, 0, wxGROW, 5);
    itemBoxSizer11->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer11->Add(itemFlexGridSizer13, 0, wxGROW, 5);
    m_leftBorderCheckbox = new wxCheckBox( itemPanel5, ID_RICHTEXT_BORDER_LEFT_CHECKBOX, _("&Left:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_leftBorderCheckbox->SetValue(false);
    itemFlexGridSizer13->Add(m_leftBorderCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer13->Add(itemBoxSizer15, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_leftBorderWidth = new wxTextCtrl( itemPanel5, ID_RICHTEXT_BORDER_LEFT, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer15->Add(m_leftBorderWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_leftBorderWidthUnitsStrings;
    m_leftBorderWidthUnitsStrings.Add(_("px"));
    m_leftBorderWidthUnitsStrings.Add(_("cm"));
    m_leftBorderWidthUnitsStrings.Add(_("pt"));
    m_leftBorderWidthUnits = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_LEFT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_leftBorderWidthUnitsStrings, wxCB_READONLY );
    m_leftBorderWidthUnits->SetStringSelection(_("px"));
    m_leftBorderWidthUnits->SetHelpText(_("Units for the left border width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_leftBorderWidthUnits->SetToolTip(_("Units for the left border width."));
    itemBoxSizer15->Add(m_leftBorderWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer15->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_leftBorderStyleStrings;
    m_leftBorderStyle = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_LEFT_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_leftBorderStyleStrings, wxCB_READONLY );
    m_leftBorderStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_leftBorderStyle->SetToolTip(_("The border line style."));
    itemBoxSizer15->Add(m_leftBorderStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer15->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_leftBorderColour = new wxRichTextColourSwatchCtrl( itemPanel5, ID_RICHTEXT_BORDER_LEFT_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer15->Add(m_leftBorderColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightBorderCheckbox = new wxCheckBox( itemPanel5, ID_RICHTEXT_BORDER_RIGHT_CHECKBOX, _("&Right:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_rightBorderCheckbox->SetValue(false);
    itemFlexGridSizer13->Add(m_rightBorderCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer13->Add(itemBoxSizer23, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_rightBorderWidth = new wxTextCtrl( itemPanel5, ID_RICHTEXT_BORDER_RIGHT, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer23->Add(m_rightBorderWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_rightBorderWidthUnitsStrings;
    m_rightBorderWidthUnitsStrings.Add(_("px"));
    m_rightBorderWidthUnitsStrings.Add(_("cm"));
    m_rightBorderWidthUnitsStrings.Add(_("pt"));
    m_rightBorderWidthUnits = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_RIGHT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_rightBorderWidthUnitsStrings, wxCB_READONLY );
    m_rightBorderWidthUnits->SetStringSelection(_("px"));
    m_rightBorderWidthUnits->SetHelpText(_("Units for the right border width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_rightBorderWidthUnits->SetToolTip(_("Units for the right border width."));
    itemBoxSizer23->Add(m_rightBorderWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer23->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_rightBorderStyleStrings;
    m_rightBorderStyle = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_RIGHT_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_rightBorderStyleStrings, wxCB_READONLY );
    m_rightBorderStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_rightBorderStyle->SetToolTip(_("The border line style."));
    itemBoxSizer23->Add(m_rightBorderStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer23->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_rightBorderColour = new wxRichTextColourSwatchCtrl( itemPanel5, ID_RICHTEXT_BORDER_RIGHT_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer23->Add(m_rightBorderColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_topBorderCheckbox = new wxCheckBox( itemPanel5, ID_RICHTEXT_BORDER_TOP_CHECKBOX, _("&Top:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_topBorderCheckbox->SetValue(false);
    itemFlexGridSizer13->Add(m_topBorderCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer13->Add(itemBoxSizer31, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_topBorderWidth = new wxTextCtrl( itemPanel5, ID_RICHTEXT_BORDER_TOP, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer31->Add(m_topBorderWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_topBorderWidthUnitsStrings;
    m_topBorderWidthUnitsStrings.Add(_("px"));
    m_topBorderWidthUnitsStrings.Add(_("cm"));
    m_topBorderWidthUnitsStrings.Add(_("pt"));
    m_topBorderWidthUnits = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_TOP_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_topBorderWidthUnitsStrings, wxCB_READONLY );
    m_topBorderWidthUnits->SetStringSelection(_("px"));
    m_topBorderWidthUnits->SetHelpText(_("Units for the top border width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_topBorderWidthUnits->SetToolTip(_("Units for the top border width."));
    itemBoxSizer31->Add(m_topBorderWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer31->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_topBorderStyleStrings;
    m_topBorderStyle = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_TOP_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_topBorderStyleStrings, wxCB_READONLY );
    m_topBorderStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_topBorderStyle->SetToolTip(_("The border line style."));
    itemBoxSizer31->Add(m_topBorderStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer31->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_topBorderColour = new wxRichTextColourSwatchCtrl( itemPanel5, ID_RICHTEXT_BORDER_TOP_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer31->Add(m_topBorderColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomBorderCheckbox = new wxCheckBox( itemPanel5, ID_RICHTEXT_BORDER_BOTTOM_CHECKBOX, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_bottomBorderCheckbox->SetValue(false);
    itemFlexGridSizer13->Add(m_bottomBorderCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer13->Add(itemBoxSizer39, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_bottomBorderWidth = new wxTextCtrl( itemPanel5, ID_RICHTEXT_BORDER_BOTTOM, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer39->Add(m_bottomBorderWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_bottomBorderWidthUnitsStrings;
    m_bottomBorderWidthUnitsStrings.Add(_("px"));
    m_bottomBorderWidthUnitsStrings.Add(_("cm"));
    m_bottomBorderWidthUnitsStrings.Add(_("pt"));
    m_bottomBorderWidthUnits = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_BOTTOM_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_bottomBorderWidthUnitsStrings, wxCB_READONLY );
    m_bottomBorderWidthUnits->SetStringSelection(_("px"));
    m_bottomBorderWidthUnits->SetHelpText(_("Units for the bottom border width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_bottomBorderWidthUnits->SetToolTip(_("Units for the bottom border width."));
    itemBoxSizer39->Add(m_bottomBorderWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer39->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_bottomBorderStyleStrings;
    m_bottomBorderStyle = new wxComboBox( itemPanel5, ID_RICHTEXT_BORDER_BOTTOM_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_bottomBorderStyleStrings, wxCB_READONLY );
    m_bottomBorderStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_bottomBorderStyle->SetToolTip(_("The border line style."));
    itemBoxSizer39->Add(m_bottomBorderStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer39->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_bottomBorderColour = new wxRichTextColourSwatchCtrl( itemPanel5, ID_RICHTEXT_BORDER_BOTTOM_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer39->Add(m_bottomBorderColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer13->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_borderSyncCtrl = new wxCheckBox( itemPanel5, ID_RICHTEXT_BORDER_SYNCHRONIZE, _("&Synchronize values"), wxDefaultPosition, wxDefaultSize, 0 );
    m_borderSyncCtrl->SetValue(false);
    m_borderSyncCtrl->SetHelpText(_("Check to edit all borders simultaneously."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_borderSyncCtrl->SetToolTip(_("Check to edit all borders simultaneously."));
    itemFlexGridSizer13->Add(m_borderSyncCtrl, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook4->AddPage(itemPanel5, _("Border"));

    wxPanel* itemPanel48 = new wxPanel( itemNotebook4, ID_RICHTEXTBORDERSPAGE_OUTLINE, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer49 = new wxBoxSizer(wxVERTICAL);
    itemPanel48->SetSizer(itemBoxSizer49);

    wxBoxSizer* itemBoxSizer50 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer49->Add(itemBoxSizer50, 0, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer51 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer50->Add(itemBoxSizer51, 0, wxGROW, 5);
    wxStaticText* itemStaticText52 = new wxStaticText( itemPanel48, wxID_STATIC, _("Outline"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText52->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer51->Add(itemStaticText52, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine53 = new wxStaticLine( itemPanel48, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer51->Add(itemStaticLine53, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer54 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer50->Add(itemBoxSizer54, 0, wxGROW, 5);
    itemBoxSizer54->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer56 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer54->Add(itemFlexGridSizer56, 0, wxGROW, 5);
    m_leftOutlineCheckbox = new wxCheckBox( itemPanel48, ID_RICHTEXT_OUTLINE_LEFT_CHECKBOX, _("&Left:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_leftOutlineCheckbox->SetValue(false);
    itemFlexGridSizer56->Add(m_leftOutlineCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer58 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer56->Add(itemBoxSizer58, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_leftOutlineWidth = new wxTextCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_LEFT, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer58->Add(m_leftOutlineWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_leftOutlineWidthUnitsStrings;
    m_leftOutlineWidthUnitsStrings.Add(_("px"));
    m_leftOutlineWidthUnitsStrings.Add(_("cm"));
    m_leftOutlineWidthUnitsStrings.Add(_("pt"));
    m_leftOutlineWidthUnits = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_LEFT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_leftOutlineWidthUnitsStrings, wxCB_READONLY );
    m_leftOutlineWidthUnits->SetStringSelection(_("px"));
    m_leftOutlineWidthUnits->SetHelpText(_("Units for the left outline width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_leftOutlineWidthUnits->SetToolTip(_("Units for the left outline width."));
    itemBoxSizer58->Add(m_leftOutlineWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer58->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_leftOutlineStyleStrings;
    m_leftOutlineStyle = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_LEFT_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_leftOutlineStyleStrings, wxCB_READONLY );
    m_leftOutlineStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_leftOutlineStyle->SetToolTip(_("The border line style."));
    itemBoxSizer58->Add(m_leftOutlineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer58->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_leftOutlineColour = new wxRichTextColourSwatchCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_LEFT_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer58->Add(m_leftOutlineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightOutlineCheckbox = new wxCheckBox( itemPanel48, ID_RICHTEXT_OUTLINE_RIGHT_CHECKBOX, _("&Right:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_rightOutlineCheckbox->SetValue(false);
    itemFlexGridSizer56->Add(m_rightOutlineCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer66 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer56->Add(itemBoxSizer66, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_rightOutlineWidth = new wxTextCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_RIGHT, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer66->Add(m_rightOutlineWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_rightOutlineWidthUnitsStrings;
    m_rightOutlineWidthUnitsStrings.Add(_("px"));
    m_rightOutlineWidthUnitsStrings.Add(_("cm"));
    m_rightOutlineWidthUnitsStrings.Add(_("pt"));
    m_rightOutlineWidthUnits = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_RIGHT_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_rightOutlineWidthUnitsStrings, wxCB_READONLY );
    m_rightOutlineWidthUnits->SetStringSelection(_("px"));
    m_rightOutlineWidthUnits->SetHelpText(_("Units for the right outline width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_rightOutlineWidthUnits->SetToolTip(_("Units for the right outline width."));
    itemBoxSizer66->Add(m_rightOutlineWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer66->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_rightOutlineStyleStrings;
    m_rightOutlineStyle = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_RIGHT_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_rightOutlineStyleStrings, wxCB_READONLY );
    m_rightOutlineStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_rightOutlineStyle->SetToolTip(_("The border line style."));
    itemBoxSizer66->Add(m_rightOutlineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer66->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_rightOutlineColour = new wxRichTextColourSwatchCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_RIGHT_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer66->Add(m_rightOutlineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_topOutlineCheckbox = new wxCheckBox( itemPanel48, ID_RICHTEXT_OUTLINE_TOP_CHECKBOX, _("&Top:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_topOutlineCheckbox->SetValue(false);
    itemFlexGridSizer56->Add(m_topOutlineCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer74 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer56->Add(itemBoxSizer74, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_topOutlineWidth = new wxTextCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_TOP, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer74->Add(m_topOutlineWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_topOutlineWidthUnitsStrings;
    m_topOutlineWidthUnitsStrings.Add(_("px"));
    m_topOutlineWidthUnitsStrings.Add(_("cm"));
    m_topOutlineWidthUnitsStrings.Add(_("pt"));
    m_topOutlineWidthUnits = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_TOP_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_topOutlineWidthUnitsStrings, wxCB_READONLY );
    m_topOutlineWidthUnits->SetStringSelection(_("px"));
    m_topOutlineWidthUnits->SetHelpText(_("Units for the top outline width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_topOutlineWidthUnits->SetToolTip(_("Units for the top outline width."));
    itemBoxSizer74->Add(m_topOutlineWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer74->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_topOutlineStyleStrings;
    m_topOutlineStyle = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_TOP_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_topOutlineStyleStrings, wxCB_READONLY );
    m_topOutlineStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_topOutlineStyle->SetToolTip(_("The border line style."));
    itemBoxSizer74->Add(m_topOutlineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer74->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_topOutlineColour = new wxRichTextColourSwatchCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_TOP_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer74->Add(m_topOutlineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomOutlineCheckbox = new wxCheckBox( itemPanel48, ID_RICHTEXT_OUTLINE_BOTTOM_CHECKBOX, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_bottomOutlineCheckbox->SetValue(false);
    itemFlexGridSizer56->Add(m_bottomOutlineCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer82 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer56->Add(itemBoxSizer82, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_bottomOutlineWidth = new wxTextCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_BOTTOM, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer82->Add(m_bottomOutlineWidth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_bottomOutlineWidthUnitsStrings;
    m_bottomOutlineWidthUnitsStrings.Add(_("px"));
    m_bottomOutlineWidthUnitsStrings.Add(_("cm"));
    m_bottomOutlineWidthUnitsStrings.Add(_("pt"));
    m_bottomOutlineWidthUnits = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_BOTTOM_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_bottomOutlineWidthUnitsStrings, wxCB_READONLY );
    m_bottomOutlineWidthUnits->SetStringSelection(_("px"));
    m_bottomOutlineWidthUnits->SetHelpText(_("Units for the bottom outline width."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_bottomOutlineWidthUnits->SetToolTip(_("Units for the bottom outline width."));
    itemBoxSizer82->Add(m_bottomOutlineWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer82->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_bottomOutlineStyleStrings;
    m_bottomOutlineStyle = new wxComboBox( itemPanel48, ID_RICHTEXT_OUTLINE_BOTTOM_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_bottomOutlineStyleStrings, wxCB_READONLY );
    m_bottomOutlineStyle->SetHelpText(_("The border line style."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_bottomOutlineStyle->SetToolTip(_("The border line style."));
    itemBoxSizer82->Add(m_bottomOutlineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer82->Add(2, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_bottomOutlineColour = new wxRichTextColourSwatchCtrl( itemPanel48, ID_RICHTEXT_OUTLINE_BOTTOM_COLOUR, wxDefaultPosition, wxSize(40, 20), wxBORDER_THEME );
    itemBoxSizer82->Add(m_bottomOutlineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer56->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_outlineSyncCtrl = new wxCheckBox( itemPanel48, ID_RICHTEXT_OUTLINE_SYNCHRONIZE, _("&Synchronize values"), wxDefaultPosition, wxDefaultSize, 0 );
    m_outlineSyncCtrl->SetValue(false);
    m_outlineSyncCtrl->SetHelpText(_("Check to edit all borders simultaneously."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_outlineSyncCtrl->SetToolTip(_("Check to edit all borders simultaneously."));
    itemFlexGridSizer56->Add(m_outlineSyncCtrl, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook4->AddPage(itemPanel48, _("Outline"));

    wxPanel* itemPanel91 = new wxPanel( itemNotebook4, ID_RICHTEXTBORDERSPAGE_CORNER, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer92 = new wxBoxSizer(wxVERTICAL);
    itemPanel91->SetSizer(itemBoxSizer92);

    wxBoxSizer* itemBoxSizer93 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer92->Add(itemBoxSizer93, 1, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer94 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer93->Add(itemBoxSizer94, 0, wxGROW, 5);
    wxStaticText* itemStaticText95 = new wxStaticText( itemPanel91, wxID_STATIC, _("Corner"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText95->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer94->Add(itemStaticText95, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine96 = new wxStaticLine( itemPanel91, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer94->Add(itemStaticLine96, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer97 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer93->Add(itemFlexGridSizer97, 0, wxALIGN_LEFT, 5);
    m_cornerRadiusCheckBox = new wxCheckBox( itemPanel91, ID_RICHTEXTBORDERSPAGE_CORNER_CHECKBOX, _("Corner &radius:"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_cornerRadiusCheckBox->SetValue(false);
    m_cornerRadiusCheckBox->SetHelpText(_("An optional corner radius for adding rounded corners."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_cornerRadiusCheckBox->SetToolTip(_("An optional corner radius for adding rounded corners."));
    itemFlexGridSizer97->Add(m_cornerRadiusCheckBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer99 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer97->Add(itemBoxSizer99, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_cornerRadiusText = new wxTextCtrl( itemPanel91, ID_RICHTEXTBORDERSPAGE_CORNER_TEXT, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    m_cornerRadiusText->SetHelpText(_("The value of the corner radius."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_cornerRadiusText->SetToolTip(_("The value of the corner radius."));
    itemBoxSizer99->Add(m_cornerRadiusText, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_cornerRadiusUnitsStrings;
    m_cornerRadiusUnitsStrings.Add(_("px"));
    m_cornerRadiusUnitsStrings.Add(_("cm"));
    m_cornerRadiusUnitsStrings.Add(_("pt"));
    m_cornerRadiusUnits = new wxComboBox( itemPanel91, ID_RICHTEXTBORDERSPAGE_CORNER_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_cornerRadiusUnitsStrings, wxCB_READONLY );
    m_cornerRadiusUnits->SetStringSelection(_("px"));
    m_cornerRadiusUnits->SetHelpText(_("Units for the corner radius."));
    if (wxRichTextBordersPage::ShowToolTips())
        m_cornerRadiusUnits->SetToolTip(_("Units for the corner radius."));
    itemBoxSizer99->Add(m_cornerRadiusUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook4->AddPage(itemPanel91, _("Corner"));

    itemBoxSizer3->Add(itemNotebook4, 0, wxGROW|wxALL, 5);

    m_borderPreviewCtrl = new wxRichTextBorderPreviewCtrl( itemRichTextDialogPage1, ID_RICHTEXT_BORDER_PREVIEW, wxDefaultPosition, wxSize(60, 60), wxBORDER_THEME|wxFULL_REPAINT_ON_RESIZE );
    itemBoxSizer3->Add(m_borderPreviewCtrl, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

////@end wxRichTextBordersPage content construction

    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_NONE); m_borderStyleNames.Add(_("None"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_SOLID); m_borderStyleNames.Add(_("Solid"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_DOTTED); m_borderStyleNames.Add(_("Dotted"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_DASHED); m_borderStyleNames.Add(_("Dashed"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_DOUBLE); m_borderStyleNames.Add(_("Double"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_GROOVE); m_borderStyleNames.Add(_("Groove"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_RIDGE); m_borderStyleNames.Add(_("Ridge"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_INSET); m_borderStyleNames.Add(_("Inset"));
    m_borderStyles.Add(wxTEXT_BOX_ATTR_BORDER_OUTSET); m_borderStyleNames.Add(_("Outset"));

    m_ignoreUpdates = true;

    FillStyleComboBox(m_leftBorderStyle);
    FillStyleComboBox(m_rightBorderStyle);
    FillStyleComboBox(m_topBorderStyle);
    FillStyleComboBox(m_bottomBorderStyle);
    
    FillStyleComboBox(m_leftOutlineStyle);
    FillStyleComboBox(m_rightOutlineStyle);
    FillStyleComboBox(m_topOutlineStyle);
    FillStyleComboBox(m_bottomOutlineStyle);
    
    m_borderPreviewCtrl->SetAttributes(GetAttributes());

    m_ignoreUpdates = false;
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextBordersPage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

// Updates the preview
void wxRichTextBordersPage::OnCommand(wxCommandEvent& event)
{
    event.Skip();
    if (m_ignoreUpdates)
        return;

    if (event.GetId() == ID_RICHTEXT_BORDER_LEFT_COLOUR && m_borderSyncCtrl->GetValue())
    {
        m_rightBorderColour->SetColour(m_leftBorderColour->GetColour());
        m_rightBorderColour->Refresh();

        m_topBorderColour->SetColour(m_leftBorderColour->GetColour());
        m_topBorderColour->Refresh();

        m_bottomBorderColour->SetColour(m_leftBorderColour->GetColour());
        m_bottomBorderColour->Refresh();
    }

    if (event.GetId() == ID_RICHTEXT_OUTLINE_LEFT_COLOUR && m_outlineSyncCtrl->GetValue())
    {
        m_rightOutlineColour->SetColour(m_leftOutlineColour->GetColour());
        m_rightOutlineColour->Refresh();

        m_topOutlineColour->SetColour(m_leftOutlineColour->GetColour());
        m_topOutlineColour->Refresh();

        m_bottomOutlineColour->SetColour(m_leftOutlineColour->GetColour());
        m_bottomOutlineColour->Refresh();
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

wxRichTextAttr* wxRichTextBordersPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

// Fill style combo
void wxRichTextBordersPage::FillStyleComboBox(wxComboBox* styleComboBox)
{
    styleComboBox->Freeze();
    styleComboBox->Append(m_borderStyleNames);
    styleComboBox->Thaw();    
}

// Updates the synchronization checkboxes to reflect the state of the attributes
void wxRichTextBordersPage::UpdateSyncControls()
{
    bool ignoreUpdates = m_ignoreUpdates;
    
    m_ignoreUpdates = true;

    if (GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft() == GetAttributes()->GetTextBoxAttr().GetBorder().GetRight() &&
        GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft() == GetAttributes()->GetTextBoxAttr().GetBorder().GetTop() &&
        GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft() == GetAttributes()->GetTextBoxAttr().GetBorder().GetBottom())
    {
        m_borderSyncCtrl->SetValue(true);
    }
    else
    {
        m_borderSyncCtrl->SetValue(false);
    }
    
    if (GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft() == GetAttributes()->GetTextBoxAttr().GetOutline().GetRight() &&
        GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft() == GetAttributes()->GetTextBoxAttr().GetOutline().GetTop() &&
        GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft() == GetAttributes()->GetTextBoxAttr().GetOutline().GetBottom())
    {
        m_outlineSyncCtrl->SetValue(true);
    }
    else
    {
        m_outlineSyncCtrl->SetValue(false);
    }
    
    m_ignoreUpdates = ignoreUpdates;
}

bool wxRichTextBordersPage::TransferDataToWindow()
{
    m_ignoreUpdates = true;

    // Border
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft(), m_leftBorderWidth, m_leftBorderWidthUnits, m_leftBorderCheckbox, m_leftBorderStyle, m_leftBorderColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetRight(), m_rightBorderWidth, m_rightBorderWidthUnits, m_rightBorderCheckbox, m_rightBorderStyle, m_rightBorderColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetTop(), m_topBorderWidth, m_topBorderWidthUnits, m_topBorderCheckbox, m_topBorderStyle, m_topBorderColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetBottom(), m_bottomBorderWidth, m_bottomBorderWidthUnits, m_bottomBorderCheckbox, m_bottomBorderStyle, m_bottomBorderColour, m_borderStyles);

    // Outline
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft(), m_leftOutlineWidth, m_leftOutlineWidthUnits, m_leftOutlineCheckbox, m_leftOutlineStyle, m_leftOutlineColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetRight(), m_rightOutlineWidth, m_rightOutlineWidthUnits, m_rightOutlineCheckbox, m_rightOutlineStyle, m_rightOutlineColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetTop(), m_topOutlineWidth, m_topOutlineWidthUnits, m_topOutlineCheckbox, m_topOutlineStyle, m_topOutlineColour, m_borderStyles);
    SetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetBottom(), m_bottomOutlineWidth, m_bottomOutlineWidthUnits, m_bottomOutlineCheckbox, m_bottomOutlineStyle, m_bottomOutlineColour, m_borderStyles);

    // Corner radius
    if (!GetAttributes()->GetTextBoxAttr().HasCornerRadius())
    {
        m_cornerRadiusCheckBox->Set3StateValue(wxCHK_UNDETERMINED);
        m_cornerRadiusText->SetValue(wxT("0"));
        m_cornerRadiusUnits->SetSelection(0);
    }
    else
    {
        wxArrayInt units;
        units.Add(wxTEXT_ATTR_UNITS_PIXELS);
        units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
        units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);
        
        wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetCornerRadius(), m_cornerRadiusText, m_cornerRadiusUnits, nullptr,
            & units);

        if (GetAttributes()->GetTextBoxAttr().GetCornerRadius().GetValue() == 0)
            m_cornerRadiusCheckBox->Set3StateValue(wxCHK_UNCHECKED);
        else
            m_cornerRadiusCheckBox->Set3StateValue(wxCHK_CHECKED);
    }

    m_ignoreUpdates = false;

    UpdateSyncControls();

    return true;
}

bool wxRichTextBordersPage::TransferDataFromWindow()
{
    // Border
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft(), m_leftBorderWidth, m_leftBorderWidthUnits, m_leftBorderCheckbox, m_leftBorderStyle, m_leftBorderColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetRight(), m_rightBorderWidth, m_rightBorderWidthUnits, m_rightBorderCheckbox, m_rightBorderStyle, m_rightBorderColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetTop(), m_topBorderWidth, m_topBorderWidthUnits, m_topBorderCheckbox, m_topBorderStyle, m_topBorderColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetBorder().GetBottom(), m_bottomBorderWidth, m_bottomBorderWidthUnits, m_bottomBorderCheckbox, m_bottomBorderStyle, m_bottomBorderColour, m_borderStyles);

    // Outline
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft(), m_leftOutlineWidth, m_leftOutlineWidthUnits, m_leftOutlineCheckbox, m_leftOutlineStyle, m_leftOutlineColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetRight(), m_rightOutlineWidth, m_rightOutlineWidthUnits, m_rightOutlineCheckbox, m_rightOutlineStyle, m_rightOutlineColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetTop(), m_topOutlineWidth, m_topOutlineWidthUnits, m_topOutlineCheckbox, m_topOutlineStyle, m_topOutlineColour, m_borderStyles);
    GetBorderValue(GetAttributes()->GetTextBoxAttr().GetOutline().GetBottom(), m_bottomOutlineWidth, m_bottomOutlineWidthUnits, m_bottomOutlineCheckbox, m_bottomOutlineStyle, m_bottomOutlineColour, m_borderStyles);

    // Corner radius
    {
        wxArrayInt units;
        units.Add(wxTEXT_ATTR_UNITS_PIXELS);
        units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
        units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);
            
        wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetCornerRadius(), m_cornerRadiusText, m_cornerRadiusUnits, nullptr,
            & units);

        if (m_cornerRadiusCheckBox->Get3StateValue() == wxCHK_UNDETERMINED)
        {
            // When we apply the attributes, we won't apply this one, to leave the original unchanged.
            GetAttributes()->GetTextBoxAttr().GetCornerRadius().Reset();
            GetAttributes()->GetTextBoxAttr().RemoveFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
        }
        else if (m_cornerRadiusCheckBox->Get3StateValue() == wxCHK_UNCHECKED)
        {
            GetAttributes()->GetTextBoxAttr().GetCornerRadius().SetValue(0);
            GetAttributes()->GetTextBoxAttr().AddFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
        }
        else
        {
            GetAttributes()->GetTextBoxAttr().AddFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
        }
    }

    return true;
}

// Set the border controls
void wxRichTextBordersPage::SetBorderValue(wxTextAttrBorder& border, wxTextCtrl* widthValueCtrl, wxComboBox* widthUnitsCtrl, wxCheckBox* checkBox,
        wxComboBox* styleCtrl, wxRichTextColourSwatchCtrl* colourCtrl, const wxArrayInt& borderStyles)
{
    if (!border.IsValid())
    {
        checkBox->Set3StateValue(wxCHK_UNDETERMINED);
        widthValueCtrl->SetValue(wxT("0"));
        widthUnitsCtrl->SetSelection(0);
        colourCtrl->SetColour(*wxBLACK);
        styleCtrl->SetSelection(0);
    }
    else
    {
        wxArrayInt units;
        units.Add(wxTEXT_ATTR_UNITS_PIXELS);
        units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
        units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);
        
        wxRichTextFormattingDialog::SetDimensionValue(border.GetWidth(), widthValueCtrl, widthUnitsCtrl, nullptr,
            & units);

        int sel = borderStyles.Index(border.GetStyle());
        if (sel == -1)
            sel = 1;
        styleCtrl->SetSelection(sel);        
        colourCtrl->SetColour(border.GetColour());
        
        if (sel == 0)
            checkBox->Set3StateValue(wxCHK_UNCHECKED);
        else
            checkBox->Set3StateValue(wxCHK_CHECKED);
    }
}

// Get data from the border controls
void wxRichTextBordersPage::GetBorderValue(wxTextAttrBorder& border, wxTextCtrl* widthValueCtrl, wxComboBox* widthUnitsCtrl, wxCheckBox* checkBox,
        wxComboBox* styleCtrl, wxRichTextColourSwatchCtrl* colourCtrl, const wxArrayInt& borderStyles)
{
    wxArrayInt units;
    units.Add(wxTEXT_ATTR_UNITS_PIXELS);
    units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
    units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);
        
    wxRichTextFormattingDialog::GetDimensionValue(border.GetWidth(), widthValueCtrl, widthUnitsCtrl, nullptr,
        & units);

    int sel = styleCtrl->GetSelection();
    border.SetColour(colourCtrl->GetColour());

    if (checkBox->Get3StateValue() == wxCHK_UNDETERMINED)
    {
        // When we apply the attributes, we won't apply this one, to leave the original unchanged.
        border.Reset();
    }
    else if (checkBox->Get3StateValue() == wxCHK_UNCHECKED)
    {
        border.SetStyle(wxTEXT_BOX_ATTR_BORDER_NONE);
        border.GetWidth().SetValue(0);
    }
    else
    {
        if (sel != -1)
            border.SetStyle(borderStyles[sel]);
    }
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextBordersPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextBordersPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextBordersPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextBordersPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextBordersPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextBordersPage icon retrieval
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_LEFT
 */

void wxRichTextBordersPage::OnRichtextBorderLeftUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_leftBorderCheckbox->Get3StateValue() == wxCHK_CHECKED);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_RIGHT
 */

void wxRichTextBordersPage::OnRichtextBorderRightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_rightBorderCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_borderSyncCtrl->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_TOP
 */

void wxRichTextBordersPage::OnRichtextBorderTopUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_topBorderCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_borderSyncCtrl->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_BOTTOM
 */

void wxRichTextBordersPage::OnRichtextBorderBottomUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_bottomBorderCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_borderSyncCtrl->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_LEFT
 */

void wxRichTextBordersPage::OnRichtextOutlineLeftUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_leftOutlineCheckbox->Get3StateValue() == wxCHK_CHECKED);
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_RIGHT
 */

void wxRichTextBordersPage::OnRichtextOutlineRightUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_rightOutlineCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_outlineSyncCtrl->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_TOP
 */

void wxRichTextBordersPage::OnRichtextOutlineTopUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_topOutlineCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_outlineSyncCtrl->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_BOTTOM
 */

void wxRichTextBordersPage::OnRichtextOutlineBottomUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_bottomOutlineCheckbox->Get3StateValue() == wxCHK_CHECKED && !m_outlineSyncCtrl->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBORDERSPAGE_CORNER_TEXT
 */

void wxRichTextBordersPage::OnRichtextborderspageCornerUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_cornerRadiusCheckBox->Get3StateValue() == wxCHK_CHECKED);
}

/*!
 * wxEVT_CHECKBOX event handler for ID_RICHTEXT_BORDER_LEFT_CHECKBOX
 */

void wxRichTextBordersPage::OnRichtextBorderCheckboxClick( wxCommandEvent& event )
{
    if (m_ignoreUpdates)
        return;

    m_ignoreUpdates = true;
    wxCheckBox* checkBox = nullptr;
    wxComboBox* comboBox = nullptr;
    if (event.GetId() == ID_RICHTEXT_OUTLINE_LEFT_CHECKBOX)
    {
        checkBox = m_leftOutlineCheckbox;
        comboBox = m_leftOutlineStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_OUTLINE_TOP_CHECKBOX)
    {
        checkBox = m_topOutlineCheckbox;
        comboBox = m_topOutlineStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_OUTLINE_RIGHT_CHECKBOX)
    {
        checkBox = m_rightOutlineCheckbox;
        comboBox = m_rightOutlineStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_OUTLINE_BOTTOM_CHECKBOX)
    {
        checkBox = m_bottomOutlineCheckbox;
        comboBox = m_bottomOutlineStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_BORDER_LEFT_CHECKBOX)
    {
        checkBox = m_leftBorderCheckbox;
        comboBox = m_leftBorderStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_BORDER_TOP_CHECKBOX)
    {
        checkBox = m_topBorderCheckbox;
        comboBox = m_topBorderStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_BORDER_RIGHT_CHECKBOX)
    {
        checkBox = m_rightBorderCheckbox;
        comboBox = m_rightBorderStyle;
    }
    else if (event.GetId() == ID_RICHTEXT_BORDER_BOTTOM_CHECKBOX)
    {
        checkBox = m_bottomBorderCheckbox;
        comboBox = m_bottomBorderStyle;
    }
    
    if (checkBox && comboBox)
    {
        if (checkBox->Get3StateValue() == wxCHK_UNCHECKED || checkBox->Get3StateValue() == wxCHK_UNDETERMINED)
            comboBox->SetSelection(0);
        else
            comboBox->SetSelection(1);
        
        if (event.GetId() == ID_RICHTEXT_BORDER_LEFT_CHECKBOX && m_borderSyncCtrl->GetValue())
        {
            m_topBorderCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_topBorderStyle->SetSelection(comboBox->GetSelection());

            m_rightBorderCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_rightBorderStyle->SetSelection(comboBox->GetSelection());

            m_bottomBorderCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_bottomBorderStyle->SetSelection(comboBox->GetSelection());
        }
        
        if (event.GetId() == ID_RICHTEXT_OUTLINE_LEFT_CHECKBOX && m_outlineSyncCtrl->GetValue())
        {
            m_topOutlineCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_topOutlineStyle->SetSelection(comboBox->GetSelection());

            m_rightOutlineCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_rightOutlineStyle->SetSelection(comboBox->GetSelection());

            m_bottomOutlineCheckbox->Set3StateValue(checkBox->Get3StateValue());
            m_bottomOutlineStyle->SetSelection(comboBox->GetSelection());
        }
        
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }

    m_ignoreUpdates = false;
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RICHTEXT_BORDER_SYNCHRONIZE
 */

void wxRichTextBordersPage::OnRichtextBorderSynchronizeClick( wxCommandEvent& event )
{
    if (m_ignoreUpdates)
        return;
        
    if (event.IsChecked())
    {
        TransferDataFromWindow();
        GetAttributes()->GetTextBoxAttr().GetBorder().GetTop() = GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft();
        GetAttributes()->GetTextBoxAttr().GetBorder().GetRight() = GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft();
        GetAttributes()->GetTextBoxAttr().GetBorder().GetBottom() = GetAttributes()->GetTextBoxAttr().GetBorder().GetLeft();        
        m_ignoreUpdates = true;
        TransferDataToWindow();
        m_ignoreUpdates = false;

        if (m_borderPreviewCtrl)
        {
            TransferDataFromWindow();
            m_borderPreviewCtrl->Refresh();
        }
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_SYNCHRONIZE
 */

void wxRichTextBordersPage::OnRichtextBorderSynchronizeUpdate( wxUpdateUIEvent& WXUNUSED(event) )
{
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BORDER_RIGHT_CHECKBOX
 */

void wxRichTextBordersPage::OnRichtextBorderOtherCheckboxUpdate( wxUpdateUIEvent& event )
{
    event.Enable(!m_borderSyncCtrl->GetValue());
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXT_BORDER_LEFT
 */

void wxRichTextBordersPage::OnRichtextBorderLeftValueTextUpdated( wxCommandEvent& event )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_borderSyncCtrl->GetValue())
    {
        wxString value = event.GetString();
        m_ignoreUpdates = true;
        m_rightBorderWidth->SetValue(value);
        m_topBorderWidth->SetValue(value);
        m_bottomBorderWidth->SetValue(value);
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXT_BORDER_LEFT_UNITS
 */

void wxRichTextBordersPage::OnRichtextBorderLeftUnitsSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_borderSyncCtrl->GetValue())
    {
        m_ignoreUpdates = true;
        m_rightBorderWidthUnits->SetSelection(m_leftBorderWidthUnits->GetSelection());
        m_topBorderWidthUnits->SetSelection(m_leftBorderWidthUnits->GetSelection());
        m_bottomBorderWidthUnits->SetSelection(m_leftBorderWidthUnits->GetSelection());
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXT_BORDER_LEFT_STYLE
 */

void wxRichTextBordersPage::OnRichtextBorderLeftStyleSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_borderSyncCtrl->GetValue())
    {
        m_ignoreUpdates = true;
        m_rightBorderStyle->SetSelection(m_leftBorderStyle->GetSelection());
        m_topBorderStyle->SetSelection(m_leftBorderStyle->GetSelection());
        m_bottomBorderStyle->SetSelection(m_leftBorderStyle->GetSelection());
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RICHTEXT_OUTLINE_SYNCHRONIZE
 */

void wxRichTextBordersPage::OnRichtextOutlineSynchronizeClick( wxCommandEvent& event )
{
    if (m_ignoreUpdates)
        return;
        
    if (event.IsChecked())
    {
        TransferDataFromWindow();
        GetAttributes()->GetTextBoxAttr().GetOutline().GetTop() = GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft();
        GetAttributes()->GetTextBoxAttr().GetOutline().GetRight() = GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft();
        GetAttributes()->GetTextBoxAttr().GetOutline().GetBottom() = GetAttributes()->GetTextBoxAttr().GetOutline().GetLeft();        
        m_ignoreUpdates = true;
        TransferDataToWindow();
        m_ignoreUpdates = false;

        if (m_borderPreviewCtrl)
        {
            TransferDataFromWindow();
            m_borderPreviewCtrl->Refresh();
        }
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_SYNCHRONIZE
 */

void wxRichTextBordersPage::OnRichtextOutlineSynchronizeUpdate( wxUpdateUIEvent& WXUNUSED(event) )
{
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OUTLINE_RIGHT_CHECKBOX
 */

void wxRichTextBordersPage::OnRichtextOutlineOtherCheckboxUpdate( wxUpdateUIEvent& event )
{
    event.Enable(!m_outlineSyncCtrl->GetValue());
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXT_OUTLINE_LEFT
 */

void wxRichTextBordersPage::OnRichtextOutlineLeftTextUpdated( wxCommandEvent& event )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_outlineSyncCtrl->GetValue())
    {
        wxString value = event.GetString();
        m_ignoreUpdates = true;
        m_rightOutlineWidth->SetValue(value);
        m_topOutlineWidth->SetValue(value);
        m_bottomOutlineWidth->SetValue(value);
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXT_OUTLINE_LEFT_UNITS
 */

void wxRichTextBordersPage::OnRichtextOutlineLeftUnitsSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_outlineSyncCtrl->GetValue())
    {
        m_ignoreUpdates = true;
        m_rightOutlineWidthUnits->SetSelection(m_leftOutlineWidthUnits->GetSelection());
        m_topOutlineWidthUnits->SetSelection(m_leftOutlineWidthUnits->GetSelection());
        m_bottomOutlineWidthUnits->SetSelection(m_leftOutlineWidthUnits->GetSelection());
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXT_OUTLINE_LEFT_STYLE
 */

void wxRichTextBordersPage::OnRichtextOutlineLeftStyleSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_ignoreUpdates)
        return;
        
    if (m_outlineSyncCtrl->GetValue())
    {
        m_ignoreUpdates = true;
        m_rightOutlineStyle->SetSelection(m_leftOutlineStyle->GetSelection());
        m_topOutlineStyle->SetSelection(m_leftOutlineStyle->GetSelection());
        m_bottomOutlineStyle->SetSelection(m_leftOutlineStyle->GetSelection());
        m_ignoreUpdates = false;
    }

    if (m_borderPreviewCtrl)
    {
        TransferDataFromWindow();
        m_borderPreviewCtrl->Refresh();
    }
}

wxBEGIN_EVENT_TABLE(wxRichTextBorderPreviewCtrl, wxWindow)
    EVT_PAINT(wxRichTextBorderPreviewCtrl::OnPaint)
wxEND_EVENT_TABLE()

wxRichTextBorderPreviewCtrl::wxRichTextBorderPreviewCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& sz, long style)
{
    if ((style & wxBORDER_MASK) == wxBORDER_DEFAULT)
        style |= wxBORDER_THEME;

    m_attributes = nullptr;

    wxWindow::Create(parent, id, pos, sz, style);
    SetBackgroundColour(*wxWHITE);
}

void wxRichTextBorderPreviewCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    if (m_attributes)
    {
        wxRect rect = GetClientRect();
        
        wxRichTextAttr attr;
        attr.GetTextBoxAttr().GetBorder() = m_attributes->GetTextBoxAttr().GetBorder();
        attr.GetTextBoxAttr().GetOutline() = m_attributes->GetTextBoxAttr().GetOutline();
        if (m_attributes->GetTextBoxAttr().HasCornerRadius())
            attr.GetTextBoxAttr().SetCornerRadius(m_attributes->GetTextBoxAttr().GetCornerRadius());
        if (m_attributes->HasBackgroundColour())
            attr.SetBackgroundColour(m_attributes->GetBackgroundColour());
        
        wxTextAttrDimension marginDim(10, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTextBoxAttr().GetMargins().GetLeft() = marginDim;
        attr.GetTextBoxAttr().GetMargins().GetTop() = marginDim;
        attr.GetTextBoxAttr().GetMargins().GetRight() = marginDim;
        attr.GetTextBoxAttr().GetMargins().GetBottom() = marginDim;

        int margin = 10;
        rect.x += margin;
        rect.y += margin;
        rect.width -= 2*margin;
        rect.height -= 2*margin;
        
        wxRichTextObject::DrawBoxAttributes(dc, nullptr, attr, rect);
    }
}
