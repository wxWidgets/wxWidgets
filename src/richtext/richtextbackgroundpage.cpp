/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbackgroundpage.cpp
// Purpose:     Implements the rich text formatting dialog background
//              properties page.
// Author:      Julian Smart
// Created:     13/11/2010 11:17:25
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextBackgroundPage, wxRichTextDialogPage);


/*!
 * wxRichTextBackgroundPage event table definition
 */

wxBEGIN_EVENT_TABLE( wxRichTextBackgroundPage, wxRichTextDialogPage )
    EVT_BUTTON(ID_RICHTEXT_BACKGROUND_COLOUR_SWATCH, wxRichTextBackgroundPage::OnColourSwatch)

////@begin wxRichTextBackgroundPage event table entries
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_HORIZONTAL_OFFSET, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_HORIZONTAL_OFFSET_UNITS, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_VERTICAL_OFFSET, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_VERTICAL_OFFSET_UNITS, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_USE_SHADOW_COLOUR, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXTSHADOWCOLOURSWATCHCTRL, wxRichTextBackgroundPage::OnRichtextshadowcolourswatchctrlUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_USE_SHADOW_SPREAD, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_SPREAD, wxRichTextBackgroundPage::OnRichtextShadowSpreadUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_SPREAD_UNITS, wxRichTextBackgroundPage::OnRichtextShadowSpreadUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_USE_BLUR_DISTANCE, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_BLUR_DISTANCE, wxRichTextBackgroundPage::OnRichtextShadowBlurUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_BLUR_DISTANCE_UNITS, wxRichTextBackgroundPage::OnRichtextShadowBlurUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_USE_SHADOW_OPACITY, wxRichTextBackgroundPage::OnRichtextShadowUpdate )
    EVT_UPDATE_UI( ID_RICHTEXT_SHADOW_OPACITY, wxRichTextBackgroundPage::OnRichtextShadowOpacityUpdate )
////@end wxRichTextBackgroundPage event table entries

wxEND_EVENT_TABLE()

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
    m_backgroundColourCheckBox = nullptr;
    m_backgroundColourSwatch = nullptr;
    m_shadowBox = nullptr;
    m_useShadow = nullptr;
    m_offsetX = nullptr;
    m_unitsHorizontalOffset = nullptr;
    m_offsetY = nullptr;
    m_unitsVerticalOffset = nullptr;
    m_shadowColourCheckBox = nullptr;
    m_shadowColourSwatch = nullptr;
    m_useShadowSpread = nullptr;
    m_spread = nullptr;
    m_unitsShadowSpread = nullptr;
    m_useBlurDistance = nullptr;
    m_blurDistance = nullptr;
    m_unitsBlurDistance = nullptr;
    m_useShadowOpacity = nullptr;
    m_opacity = nullptr;
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
    itemStaticText5->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD, false, wxT("")));
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

    m_shadowBox = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(m_shadowBox, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    m_shadowBox->Add(itemBoxSizer12, 0, wxGROW, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Shadow"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText13->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFamily(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetStyle(), wxFONTWEIGHT_BOLD, false, wxT("")));
    itemBoxSizer12->Add(itemStaticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine14 = new wxStaticLine( itemRichTextDialogPage1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer12->Add(itemStaticLine14, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    m_shadowBox->Add(itemBoxSizer15, 0, wxGROW, 5);

    itemBoxSizer15->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer15->Add(itemBoxSizer17, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_useShadow = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_USE_SHADOW, _("Use &shadow"), wxDefaultPosition, wxDefaultSize, 0 );
    m_useShadow->SetValue(false);
    m_useShadow->SetHelpText(_("Enables a shadow."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_useShadow->SetToolTip(_("Enables a shadow."));
    itemBoxSizer17->Add(m_useShadow, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer19, 0, wxGROW, 5);

    itemBoxSizer19->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer19->Add(itemBoxSizer21, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxFlexGridSizer* itemFlexGridSizer22 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer21->Add(itemFlexGridSizer22, 0, wxGROW, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Horizontal offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(itemStaticText23, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer24, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_offsetX = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_HORIZONTAL_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_offsetX->SetHelpText(_("The horizontal offset."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_offsetX->SetToolTip(_("The horizontal offset."));
    itemBoxSizer24->Add(m_offsetX, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsHorizontalOffsetStrings;
    m_unitsHorizontalOffsetStrings.Add(_("px"));
    m_unitsHorizontalOffsetStrings.Add(_("cm"));
    m_unitsHorizontalOffsetStrings.Add(_("pt"));
    m_unitsHorizontalOffset = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_HORIZONTAL_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsHorizontalOffsetStrings, wxCB_READONLY );
    m_unitsHorizontalOffset->SetStringSelection(_("px"));
    m_unitsHorizontalOffset->SetHelpText(_("Units for this value."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_unitsHorizontalOffset->SetToolTip(_("Units for this value."));
    itemBoxSizer24->Add(m_unitsHorizontalOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText27 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Vertical offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(itemStaticText27, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer28, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_offsetY = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_VERTICAL_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_offsetY->SetHelpText(_("The vertical offset."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_offsetY->SetToolTip(_("The vertical offset."));
    itemBoxSizer28->Add(m_offsetY, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsVerticalOffsetStrings;
    m_unitsVerticalOffsetStrings.Add(_("px"));
    m_unitsVerticalOffsetStrings.Add(_("cm"));
    m_unitsVerticalOffsetStrings.Add(_("pt"));
    m_unitsVerticalOffset = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_VERTICAL_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsVerticalOffsetStrings, wxCB_READONLY );
    m_unitsVerticalOffset->SetStringSelection(_("px"));
    m_unitsVerticalOffset->SetHelpText(_("Units for this value."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_unitsVerticalOffset->SetToolTip(_("Units for this value."));
    itemBoxSizer28->Add(m_unitsVerticalOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_shadowColourCheckBox = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_USE_SHADOW_COLOUR, _("Shadow c&olour:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_shadowColourCheckBox->SetValue(false);
    m_shadowColourCheckBox->SetHelpText(_("Enables the shadow colour."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_shadowColourCheckBox->SetToolTip(_("Enables the shadow colour."));
    itemFlexGridSizer22->Add(m_shadowColourCheckBox, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_shadowColourSwatch = new wxRichTextColourSwatchCtrl( itemRichTextDialogPage1, ID_RICHTEXTSHADOWCOLOURSWATCHCTRL, wxDefaultPosition, wxSize(60, 20), wxBORDER_THEME );
    m_shadowColourSwatch->SetHelpText(_("The shadow colour."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_shadowColourSwatch->SetToolTip(_("The shadow colour."));
    itemFlexGridSizer22->Add(m_shadowColourSwatch, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_useShadowSpread = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_USE_SHADOW_SPREAD, _("Sh&adow spread:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_useShadowSpread->SetValue(false);
    m_useShadowSpread->SetHelpText(_("Enables the shadow spread."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_useShadowSpread->SetToolTip(_("Enables the shadow spread."));
    itemFlexGridSizer22->Add(m_useShadowSpread, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer34, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_spread = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_SPREAD, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_spread->SetHelpText(_("The shadow spread."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_spread->SetToolTip(_("The shadow spread."));
    itemBoxSizer34->Add(m_spread, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsShadowSpreadStrings;
    m_unitsShadowSpreadStrings.Add(_("px"));
    m_unitsShadowSpreadStrings.Add(_("cm"));
    m_unitsShadowSpreadStrings.Add(_("pt"));
    m_unitsShadowSpread = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_SPREAD_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsShadowSpreadStrings, wxCB_READONLY );
    m_unitsShadowSpread->SetStringSelection(_("px"));
    m_unitsShadowSpread->SetHelpText(_("Units for this value."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_unitsShadowSpread->SetToolTip(_("Units for this value."));
    itemBoxSizer34->Add(m_unitsShadowSpread, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_useBlurDistance = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_USE_BLUR_DISTANCE, _("&Blur distance:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_useBlurDistance->SetValue(false);
    m_useBlurDistance->SetHelpText(_("Enables the blur distance."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_useBlurDistance->SetToolTip(_("Enables the blur distance."));
    itemFlexGridSizer22->Add(m_useBlurDistance, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer38, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_blurDistance = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_BLUR_DISTANCE, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_blurDistance->SetHelpText(_("The shadow blur distance."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_blurDistance->SetToolTip(_("The shadow blur distance."));
    itemBoxSizer38->Add(m_blurDistance, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_unitsBlurDistanceStrings;
    m_unitsBlurDistanceStrings.Add(_("px"));
    m_unitsBlurDistanceStrings.Add(_("cm"));
    m_unitsBlurDistanceStrings.Add(_("pt"));
    m_unitsBlurDistance = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_BLUR_DISTANCE_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsBlurDistanceStrings, wxCB_READONLY );
    m_unitsBlurDistance->SetStringSelection(_("px"));
    m_unitsBlurDistance->SetHelpText(_("Units for this value."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_unitsBlurDistance->SetToolTip(_("Units for this value."));
    itemBoxSizer38->Add(m_unitsBlurDistance, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_useShadowOpacity = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXT_USE_SHADOW_OPACITY, _("Opaci&ty:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_useShadowOpacity->SetValue(false);
    m_useShadowOpacity->SetHelpText(_("Enables the shadow opacity."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_useShadowOpacity->SetToolTip(_("Enables the shadow opacity."));
    itemFlexGridSizer22->Add(m_useShadowOpacity, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer42 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer42, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_opacity = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXT_SHADOW_OPACITY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), 0 );
    m_opacity->SetHelpText(_("The shadow opacity."));
    if (wxRichTextBackgroundPage::ShowToolTips())
        m_opacity->SetToolTip(_("The shadow opacity."));
    itemBoxSizer42->Add(m_opacity, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxStaticText* itemStaticText44 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("%"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer42->Add(itemStaticText44, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

    m_useShadow->SetValue(attr->GetTextBoxAttr().GetShadow().IsValid());
    
    wxArrayInt units;
    units.Add(wxTEXT_ATTR_UNITS_PIXELS);
    units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
    units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);

    wxArrayInt percentUnits;
    percentUnits.Add(wxTEXT_ATTR_UNITS_PERCENTAGE);

    if (!attr->GetTextBoxAttr().GetShadow().GetOffsetX().IsValid())
        attr->GetTextBoxAttr().GetShadow().GetOffsetX().SetValue(0, wxTEXT_ATTR_UNITS_PIXELS);
    if (!attr->GetTextBoxAttr().GetShadow().GetOffsetY().IsValid())
        attr->GetTextBoxAttr().GetShadow().GetOffsetY().SetValue(0, wxTEXT_ATTR_UNITS_PIXELS);

    wxRichTextFormattingDialog::SetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOffsetX(), m_offsetX, m_unitsHorizontalOffset, nullptr,
        & units);
    wxRichTextFormattingDialog::SetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOffsetY(), m_offsetY, m_unitsVerticalOffset, nullptr,
        & units);
    wxRichTextFormattingDialog::SetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetSpread(), m_spread, m_unitsShadowSpread, m_useShadowSpread,
        & units);
    wxRichTextFormattingDialog::SetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetBlurDistance(), m_blurDistance, m_unitsBlurDistance, m_useBlurDistance,
        & units);
    wxRichTextFormattingDialog::SetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOpacity(), m_opacity, nullptr, m_useShadowOpacity,
        & percentUnits);

    if (!attr->GetTextBoxAttr().GetShadow().HasColour() || !attr->GetTextBoxAttr().GetShadow().IsValid())
    {
        m_shadowColourCheckBox->SetValue(false);
        m_shadowColourSwatch->SetColour(*wxWHITE);
    }
    else
    {
        m_shadowColourCheckBox->SetValue(true);
        m_shadowColourSwatch->SetColour(attr->GetTextBoxAttr().GetShadow().GetColour());
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
    
    if (!m_useShadow->GetValue())
        attr->GetTextBoxAttr().GetShadow().Reset();
    else
    {
        attr->GetTextBoxAttr().GetShadow().SetValid(true);

        wxArrayInt units;
        units.Add(wxTEXT_ATTR_UNITS_PIXELS);
        units.Add(wxTEXT_ATTR_UNITS_TENTHS_MM);
        units.Add(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);

        wxArrayInt percentUnits;
        percentUnits.Add(wxTEXT_ATTR_UNITS_PERCENTAGE);

        wxRichTextFormattingDialog::GetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOffsetX(), m_offsetX, m_unitsHorizontalOffset, nullptr,
            & units);
        wxRichTextFormattingDialog::GetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOffsetY(), m_offsetY, m_unitsVerticalOffset, nullptr,
            & units);
        wxRichTextFormattingDialog::GetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetSpread(), m_spread, m_unitsShadowSpread, m_useShadowSpread,
            & units);
        wxRichTextFormattingDialog::GetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetBlurDistance(), m_blurDistance, m_unitsBlurDistance, m_useBlurDistance,
            & units);
        wxRichTextFormattingDialog::GetDimensionValue(attr->GetTextBoxAttr().GetShadow().GetOpacity(), m_opacity, nullptr, m_useShadowOpacity,
            & percentUnits);

        if (m_shadowColourCheckBox->GetValue())
        {
            attr->GetTextBoxAttr().GetShadow().SetColour(m_shadowColourSwatch->GetColour());
        }
        else
        {
            attr->GetTextBoxAttr().GetShadow().SetFlags(attr->GetTextBoxAttr().GetShadow().GetFlags() & ~wxTEXT_BOX_ATTR_BORDER_COLOUR);
        }
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
    return wxRichTextFormattingDialog::ShowToolTips();
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

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_SHADOW_HORIZONTAL_OFFSET
 */

void wxRichTextBackgroundPage::OnRichtextShadowUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_useShadow->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTSHADOWCOLOURSWATCHCTRL
 */

void wxRichTextBackgroundPage::OnRichtextshadowcolourswatchctrlUpdate( wxUpdateUIEvent& WXUNUSED(event) )
{
    // For now don't disable because of ugly disabled border on Windows
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_SHADOW_SPREAD
 */

void wxRichTextBackgroundPage::OnRichtextShadowSpreadUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_useShadow->GetValue() && m_useShadowSpread->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_SHADOW_BLUR_DISTANCE
 */

void wxRichTextBackgroundPage::OnRichtextShadowBlurUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_useShadow->GetValue() && m_useBlurDistance->GetValue());
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXT_SHADOW_OPACITY
 */

void wxRichTextBackgroundPage::OnRichtextShadowOpacityUpdate( wxUpdateUIEvent& event )
{
    event.Enable(m_useShadow->GetValue() && m_useShadowOpacity->GetValue());
}
