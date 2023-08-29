/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextmarginspage.cpp
// Purpose:     Implements the rich text formatting dialog margins page.
// Author:      Julian Smart
// Modified by: 
// Created:     20/10/2010 10:27:34
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/richtext/richtextmarginspage.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextMarginsPage type definition
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextMarginsPage, wxRichTextDialogPage);


/*!
 * wxRichTextMarginsPage event table definition
 */

wxBEGIN_EVENT_TABLE(wxRichTextMarginsPage, wxRichTextDialogPage)

////@begin wxRichTextMarginsPage event table entries
    EVT_UPDATE_UI( ID_RICHTEXT_LEFT_MARGIN, wxRichTextMarginsPage::OnRichtextLeftMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_LEFT_MARGIN_UNITS, wxRichTextMarginsPage::OnRichtextLeftMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT_MARGIN, wxRichTextMarginsPage::OnRichtextRightMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT_MARGIN_UNITS, wxRichTextMarginsPage::OnRichtextRightMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_TOP_MARGIN, wxRichTextMarginsPage::OnRichtextTopMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_TOP_MARGIN_UNITS, wxRichTextMarginsPage::OnRichtextTopMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM_MARGIN, wxRichTextMarginsPage::OnRichtextBottomMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM_MARGIN_UNITS, wxRichTextMarginsPage::OnRichtextBottomMarginUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_LEFT_PADDING, wxRichTextMarginsPage::OnRichtextLeftPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_LEFT_PADDING_UNITS, wxRichTextMarginsPage::OnRichtextLeftPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT_PADDING, wxRichTextMarginsPage::OnRichtextRightPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_RIGHT_PADDING_UNITS, wxRichTextMarginsPage::OnRichtextRightPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_TOP_PADDING, wxRichTextMarginsPage::OnRichtextTopPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_TOP_PADDING_UNITS, wxRichTextMarginsPage::OnRichtextTopPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM_PADDING, wxRichTextMarginsPage::OnRichtextBottomPaddingUpdate )

    EVT_UPDATE_UI( ID_RICHTEXT_BOTTOM_PADDING_UNITS, wxRichTextMarginsPage::OnRichtextBottomPaddingUpdate )

////@end wxRichTextMarginsPage event table entries

wxEND_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextMarginsPage)

/*!
 * wxRichTextMarginsPage constructors
 */

wxRichTextMarginsPage::wxRichTextMarginsPage()
{
    Init();
}

wxRichTextMarginsPage::wxRichTextMarginsPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * wxRichTextMarginsPage creator
 */

bool wxRichTextMarginsPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextMarginsPage creation
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextMarginsPage creation
    return true;
}


/*!
 * wxRichTextMarginsPage destructor
 */

wxRichTextMarginsPage::~wxRichTextMarginsPage()
{
////@begin wxRichTextMarginsPage destruction
////@end wxRichTextMarginsPage destruction
}


/*!
 * Member initialisation
 */

void wxRichTextMarginsPage::Init()
{
    m_ignoreUpdates = false;

////@begin wxRichTextMarginsPage member initialisation
    m_leftMarginCheckbox = nullptr;
    m_marginLeft = nullptr;
    m_unitsMarginLeft = nullptr;
    m_rightMarginCheckbox = nullptr;
    m_marginRight = nullptr;
    m_unitsMarginRight = nullptr;
    m_topMarginCheckbox = nullptr;
    m_marginTop = nullptr;
    m_unitsMarginTop = nullptr;
    m_bottomMarginCheckbox = nullptr;
    m_marginBottom = nullptr;
    m_unitsMarginBottom = nullptr;
    m_leftPaddingCheckbox = nullptr;
    m_paddingLeft = nullptr;
    m_unitsPaddingLeft = nullptr;
    m_rightPaddingCheckbox = nullptr;
    m_paddingRight = nullptr;
    m_unitsPaddingRight = nullptr;
    m_topPaddingCheckbox = nullptr;
    m_paddingTop = nullptr;
    m_unitsPaddingTop = nullptr;
    m_bottomPaddingCheckbox = nullptr;
    m_paddingBottom = nullptr;
    m_unitsPaddingBottom = nullptr;
////@end wxRichTextMarginsPage member initialisation
}


/*!
 * Control creation for wxRichTextMarginsPage
 */

void wxRichTextMarginsPage::CreateControls()
{    
////@begin wxRichTextMarginsPage content construction
    wxRichTextMarginsPage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Margins"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine6 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer4->Add(itemStaticLine6, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer7, 0, wxGROW, 5);

    itemBoxSizer7->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer9 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer7->Add(itemFlexGridSizer9, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_leftMarginCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_MARGIN_CHECKBOX, _("&Left:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_leftMarginCheckbox->SetValue(false);
    itemFlexGridSizer9->Add(m_leftMarginCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer9->Add(itemBoxSizer11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_marginLeft = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_MARGIN, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_marginLeft->SetHelpText(_("The left margin size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_marginLeft->SetToolTip(_("The left margin size."));
    itemBoxSizer11->Add(m_marginLeft, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMarginLeftStrings;
    m_unitsMarginLeftStrings.Add(_("px"));
    m_unitsMarginLeftStrings.Add(_("cm"));
    m_unitsMarginLeft = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_MARGIN_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMarginLeftStrings, wxCB_READONLY );
    m_unitsMarginLeft->SetStringSelection(_("px"));
    m_unitsMarginLeft->SetHelpText(_("Units for the left margin."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsMarginLeft->SetToolTip(_("Units for the left margin."));
    itemBoxSizer11->Add(m_unitsMarginLeft, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer11->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightMarginCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_MARGIN_CHECKBOX, _("&Right:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rightMarginCheckbox->SetValue(false);
    itemFlexGridSizer9->Add(m_rightMarginCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer9->Add(itemBoxSizer16, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_marginRight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_MARGIN, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_marginRight->SetHelpText(_("The right margin size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_marginRight->SetToolTip(_("The right margin size."));
    itemBoxSizer16->Add(m_marginRight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMarginRightStrings;
    m_unitsMarginRightStrings.Add(_("px"));
    m_unitsMarginRightStrings.Add(_("cm"));
    m_unitsMarginRight = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_MARGIN_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMarginRightStrings, wxCB_READONLY );
    m_unitsMarginRight->SetStringSelection(_("px"));
    m_unitsMarginRight->SetHelpText(_("Units for the right margin."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsMarginRight->SetToolTip(_("Units for the right margin."));
    itemBoxSizer16->Add(m_unitsMarginRight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_topMarginCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_MARGIN_CHECKBOX, _("&Top:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_topMarginCheckbox->SetValue(false);
    itemFlexGridSizer9->Add(m_topMarginCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer9->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_marginTop = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_TOP_MARGIN, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_marginTop->SetHelpText(_("The top margin size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_marginTop->SetToolTip(_("The top margin size."));
    itemBoxSizer20->Add(m_marginTop, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMarginTopStrings;
    m_unitsMarginTopStrings.Add(_("px"));
    m_unitsMarginTopStrings.Add(_("cm"));
    m_unitsMarginTop = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_MARGIN_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMarginTopStrings, wxCB_READONLY );
    m_unitsMarginTop->SetStringSelection(_("px"));
    m_unitsMarginTop->SetHelpText(_("Units for the top margin."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsMarginTop->SetToolTip(_("Units for the top margin."));
    itemBoxSizer20->Add(m_unitsMarginTop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer20->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomMarginCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_MARGIN_CHECKBOX, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_bottomMarginCheckbox->SetValue(false);
    itemFlexGridSizer9->Add(m_bottomMarginCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer9->Add(itemBoxSizer25, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_marginBottom = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_MARGIN, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_marginBottom->SetHelpText(_("The bottom margin size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_marginBottom->SetToolTip(_("The bottom margin size."));
    itemBoxSizer25->Add(m_marginBottom, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsMarginBottomStrings;
    m_unitsMarginBottomStrings.Add(_("px"));
    m_unitsMarginBottomStrings.Add(_("cm"));
    m_unitsMarginBottom = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_MARGIN_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsMarginBottomStrings, wxCB_READONLY );
    m_unitsMarginBottom->SetStringSelection(_("px"));
    m_unitsMarginBottom->SetHelpText(_("Units for the bottom margin."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsMarginBottom->SetToolTip(_("Units for the bottom margin."));
    itemBoxSizer25->Add(m_unitsMarginBottom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer28, 0, wxGROW, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Padding"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText29->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD));
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine30 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer28->Add(itemStaticLine30, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer31, 0, wxGROW, 5);

    itemBoxSizer31->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer33 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer31->Add(itemFlexGridSizer33, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_leftPaddingCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_PADDING_CHECKBOX, _("&Left:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_leftPaddingCheckbox->SetValue(false);
    itemFlexGridSizer33->Add(m_leftPaddingCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer33->Add(itemBoxSizer35, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_paddingLeft = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_PADDING, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_paddingLeft->SetHelpText(_("The left padding size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_paddingLeft->SetToolTip(_("The left padding size."));
    itemBoxSizer35->Add(m_paddingLeft, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsPaddingLeftStrings;
    m_unitsPaddingLeftStrings.Add(_("px"));
    m_unitsPaddingLeftStrings.Add(_("cm"));
    m_unitsPaddingLeft = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_LEFT_PADDING_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsPaddingLeftStrings, wxCB_READONLY );
    m_unitsPaddingLeft->SetStringSelection(_("px"));
    m_unitsPaddingLeft->SetHelpText(_("Units for the left padding."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsPaddingLeft->SetToolTip(_("Units for the left padding."));
    itemBoxSizer35->Add(m_unitsPaddingLeft, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer35->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rightPaddingCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_PADDING_CHECKBOX, _("&Right:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rightPaddingCheckbox->SetValue(false);
    itemFlexGridSizer33->Add(m_rightPaddingCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer40 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer33->Add(itemBoxSizer40, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_paddingRight = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_PADDING, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_paddingRight->SetHelpText(_("The right padding size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_paddingRight->SetToolTip(_("The right padding size."));
    itemBoxSizer40->Add(m_paddingRight, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsPaddingRightStrings;
    m_unitsPaddingRightStrings.Add(_("px"));
    m_unitsPaddingRightStrings.Add(_("cm"));
    m_unitsPaddingRight = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_RIGHT_PADDING_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsPaddingRightStrings, wxCB_READONLY );
    m_unitsPaddingRight->SetStringSelection(_("px"));
    m_unitsPaddingRight->SetHelpText(_("Units for the right padding."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsPaddingRight->SetToolTip(_("Units for the right padding."));
    itemBoxSizer40->Add(m_unitsPaddingRight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_topPaddingCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_PADDING_CHECKBOX, _("&Top:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_topPaddingCheckbox->SetValue(false);
    itemFlexGridSizer33->Add(m_topPaddingCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer33->Add(itemBoxSizer44, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_paddingTop = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_TOP_PADDING, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_paddingTop->SetHelpText(_("The top padding size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_paddingTop->SetToolTip(_("The top padding size."));
    itemBoxSizer44->Add(m_paddingTop, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsPaddingTopStrings;
    m_unitsPaddingTopStrings.Add(_("px"));
    m_unitsPaddingTopStrings.Add(_("cm"));
    m_unitsPaddingTop = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_TOP_PADDING_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsPaddingTopStrings, wxCB_READONLY );
    m_unitsPaddingTop->SetStringSelection(_("px"));
    m_unitsPaddingTop->SetHelpText(_("Units for the top padding."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsPaddingTop->SetToolTip(_("Units for the top padding."));
    itemBoxSizer44->Add(m_unitsPaddingTop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer44->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_bottomPaddingCheckbox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_PADDING_CHECKBOX, _("&Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_bottomPaddingCheckbox->SetValue(false);
    itemFlexGridSizer33->Add(m_bottomPaddingCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer49 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer33->Add(itemBoxSizer49, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_paddingBottom = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_PADDING, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_paddingBottom->SetHelpText(_("The bottom padding size."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_paddingBottom->SetToolTip(_("The bottom padding size."));
    itemBoxSizer49->Add(m_paddingBottom, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsPaddingBottomStrings;
    m_unitsPaddingBottomStrings.Add(_("px"));
    m_unitsPaddingBottomStrings.Add(_("cm"));
    m_unitsPaddingBottom = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_BOTTOM_PADDING_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsPaddingBottomStrings, wxCB_READONLY );
    m_unitsPaddingBottom->SetStringSelection(_("px"));
    m_unitsPaddingBottom->SetHelpText(_("Units for the bottom padding."));
    if (wxRichTextMarginsPage::ShowToolTips())
        m_unitsPaddingBottom->SetToolTip(_("Units for the bottom padding."));
    itemBoxSizer49->Add(m_unitsPaddingBottom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxRichTextMarginsPage content construction
}

wxRichTextAttr* wxRichTextMarginsPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextMarginsPage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

bool wxRichTextMarginsPage::TransferDataToWindow()
{
    // Margins
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetLeft(), m_marginLeft, m_unitsMarginLeft, m_leftMarginCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetTop(), m_marginTop, m_unitsMarginTop, m_topMarginCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetRight(), m_marginRight, m_unitsMarginRight, m_rightMarginCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetBottom(), m_marginBottom, m_unitsMarginBottom, m_bottomMarginCheckbox);

    // Padding
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetLeft(), m_paddingLeft, m_unitsPaddingLeft, m_leftPaddingCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetTop(), m_paddingTop, m_unitsPaddingTop, m_topPaddingCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetRight(), m_paddingRight, m_unitsPaddingRight, m_rightPaddingCheckbox);
    wxRichTextFormattingDialog::SetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetBottom(), m_paddingBottom, m_unitsPaddingBottom, m_bottomPaddingCheckbox);

    return true;
}

bool wxRichTextMarginsPage::TransferDataFromWindow()
{
    // Margins
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetLeft(), m_marginLeft, m_unitsMarginLeft, m_leftMarginCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetTop(), m_marginTop, m_unitsMarginTop, m_topMarginCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetRight(), m_marginRight, m_unitsMarginRight, m_rightMarginCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetMargins().GetBottom(), m_marginBottom, m_unitsMarginBottom, m_bottomMarginCheckbox);

    // Padding
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetLeft(), m_paddingLeft, m_unitsPaddingLeft, m_leftPaddingCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetTop(), m_paddingTop, m_unitsPaddingTop, m_topPaddingCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetRight(), m_paddingRight, m_unitsPaddingRight, m_rightPaddingCheckbox);
    wxRichTextFormattingDialog::GetDimensionValue(GetAttributes()->GetTextBoxAttr().GetPadding().GetBottom(), m_paddingBottom, m_unitsPaddingBottom, m_bottomPaddingCheckbox);

    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextMarginsPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextMarginsPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextMarginsPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextMarginsPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextMarginsPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextMarginsPage icon retrieval
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_LEFT_MARGIN
 */

void wxRichTextMarginsPage::OnRichtextLeftMarginUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_leftMarginCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_RIGHT_MARGIN
 */

void wxRichTextMarginsPage::OnRichtextRightMarginUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_rightMarginCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_TOP_MARGIN
 */

void wxRichTextMarginsPage::OnRichtextTopMarginUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_topMarginCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BOTTOM_MARGIN
 */

void wxRichTextMarginsPage::OnRichtextBottomMarginUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_bottomMarginCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_LEFT_PADDING
 */

void wxRichTextMarginsPage::OnRichtextLeftPaddingUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_leftPaddingCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_RIGHT_PADDING
 */

void wxRichTextMarginsPage::OnRichtextRightPaddingUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_rightPaddingCheckbox->GetValue());
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_TOP_PADDING
 */

void wxRichTextMarginsPage::OnRichtextTopPaddingUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_topPaddingCheckbox->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_BOTTOM_PADDING
 */

void wxRichTextMarginsPage::OnRichtextBottomPaddingUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_bottomPaddingCheckbox->GetValue());
}
