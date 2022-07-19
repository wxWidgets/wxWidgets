/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextfontpage.cpp
// Purpose:     Font page for wxRichTextFormattingDialog
// Author:      Julian Smart
// Modified by:
// Created:     2006-10-02
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/richtext/richtextfontpage.h"

/*!
 * wxRichTextFontPage type definition
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextFontPage, wxRichTextDialogPage);

/*!
 * wxRichTextFontPage event table definition
 */

wxBEGIN_EVENT_TABLE( wxRichTextFontPage, wxRichTextDialogPage )
    EVT_LISTBOX( ID_RICHTEXTFONTPAGE_FACELISTBOX, wxRichTextFontPage::OnFaceListBoxSelected )
    EVT_BUTTON( ID_RICHTEXTFONTPAGE_COLOURCTRL, wxRichTextFontPage::OnColourClicked )
    EVT_BUTTON( ID_RICHTEXTFONTPAGE_BGCOLOURCTRL, wxRichTextFontPage::OnColourClicked )

////@begin wxRichTextFontPage event table entries
    EVT_IDLE( wxRichTextFontPage::OnIdle )
    EVT_TEXT( ID_RICHTEXTFONTPAGE_FACETEXTCTRL, wxRichTextFontPage::OnFaceTextCtrlUpdated )
    EVT_TEXT( ID_RICHTEXTFONTPAGE_SIZETEXTCTRL, wxRichTextFontPage::OnSizeTextCtrlUpdated )
    EVT_SPIN_UP( ID_RICHTEXTFONTPAGE_SPINBUTTONS, wxRichTextFontPage::OnRichtextfontpageSpinbuttonsUp )
    EVT_SPIN_DOWN( ID_RICHTEXTFONTPAGE_SPINBUTTONS, wxRichTextFontPage::OnRichtextfontpageSpinbuttonsDown )
    EVT_CHOICE( ID_RICHTEXTFONTPAGE_SIZE_UNITS, wxRichTextFontPage::OnRichtextfontpageSizeUnitsSelected )
    EVT_LISTBOX( ID_RICHTEXTFONTPAGE_SIZELISTBOX, wxRichTextFontPage::OnSizeListBoxSelected )
    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_STYLECTRL, wxRichTextFontPage::OnStyleCtrlSelected )
    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_WEIGHTCTRL, wxRichTextFontPage::OnWeightCtrlSelected )
    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL, wxRichTextFontPage::OnUnderliningCtrlSelected )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_COLOURCTRL_LABEL, wxRichTextFontPage::OnUnderliningCtrlSelected )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_BGCOLOURCTRL_LABEL, wxRichTextFontPage::OnUnderliningCtrlSelected )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_STRIKETHROUGHCTRL, wxRichTextFontPage::OnStrikethroughctrlClick )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_CAPSCTRL, wxRichTextFontPage::OnCapsctrlClick )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_SMALLCAPSCTRL, wxRichTextFontPage::OnCapsctrlClick )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_SUPERSCRIPT, wxRichTextFontPage::OnRichtextfontpageSuperscriptClick )
    EVT_CHECKBOX( ID_RICHTEXTFONTPAGE_SUBSCRIPT, wxRichTextFontPage::OnRichtextfontpageSubscriptClick )
////@end wxRichTextFontPage event table entries

wxEND_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextFontPage)

int wxRichTextFontPage::sm_allowedTextEffects = 0xFFFF & ~(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION|wxTEXT_ATTR_EFFECT_RTL);

/*!
 * wxRichTextFontPage constructors
 */

wxRichTextFontPage::wxRichTextFontPage( )
{
    Init();
}

wxRichTextFontPage::wxRichTextFontPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * Initialise members
 */

void wxRichTextFontPage::Init()
{
    m_dontUpdate = false;
    m_colourPresent = false;
    m_bgColourPresent = false;

////@begin wxRichTextFontPage member initialisation
    m_innerSizer = NULL;
    m_faceTextCtrl = NULL;
    m_sizeTextCtrl = NULL;
    m_fontSizeSpinButtons = NULL;
    m_sizeUnitsCtrl = NULL;
    m_fontListBoxParent = NULL;
    m_faceListBox = NULL;
    m_sizeListBox = NULL;
    m_styleCtrl = NULL;
    m_weightCtrl = NULL;
    m_underliningCtrl = NULL;
    m_textColourLabel = NULL;
    m_colourCtrl = NULL;
    m_bgColourLabel = NULL;
    m_bgColourCtrl = NULL;
    m_strikethroughCtrl = NULL;
    m_capitalsCtrl = NULL;
    m_smallCapitalsCtrl = NULL;
    m_superscriptCtrl = NULL;
    m_subscriptCtrl = NULL;
    m_rtlParentSizer = NULL;
    m_rtlCtrl = NULL;
    m_suppressHyphenationCtrl = NULL;
    m_previewCtrl = NULL;
////@end wxRichTextFontPage member initialisation
}

/*!
 * wxRichTextFontPage creator
 */

bool wxRichTextFontPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextFontPage creation
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextFontPage creation
    return true;
}

/*!
 * Control creation for wxRichTextFontPage
 */

void wxRichTextFontPage::CreateControls()
{
////@begin wxRichTextFontPage content construction
    wxRichTextFontPage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    m_innerSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(m_innerSizer, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    m_innerSizer->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 1, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Font:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_faceTextCtrl = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_FACETEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_faceTextCtrl->SetHelpText(_("Type a font name."));
    if (wxRichTextFontPage::ShowToolTips())
        m_faceTextCtrl->SetToolTip(_("Type a font name."));
    itemBoxSizer5->Add(m_faceTextCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer8, 0, wxGROW, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer8->Add(itemBoxSizer10, 0, wxGROW, 5);

    m_sizeTextCtrl = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SIZETEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0 );
    m_sizeTextCtrl->SetHelpText(_("Type a size in points."));
    if (wxRichTextFontPage::ShowToolTips())
        m_sizeTextCtrl->SetToolTip(_("Type a size in points."));
    itemBoxSizer10->Add(m_sizeTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5);

    m_fontSizeSpinButtons = new wxSpinButton( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SPINBUTTONS, wxDefaultPosition, wxSize(-1, 20), wxSP_VERTICAL );
    m_fontSizeSpinButtons->SetRange(0, 100);
    m_fontSizeSpinButtons->SetValue(0);
    itemBoxSizer10->Add(m_fontSizeSpinButtons, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5);

    wxArrayString m_sizeUnitsCtrlStrings;
    m_sizeUnitsCtrlStrings.Add(_("pt"));
    m_sizeUnitsCtrlStrings.Add(_("px"));
    m_sizeUnitsCtrl = new wxChoice( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SIZE_UNITS, wxDefaultPosition, wxDefaultSize, m_sizeUnitsCtrlStrings, 0 );
    m_sizeUnitsCtrl->SetStringSelection(_("pt"));
    m_sizeUnitsCtrl->SetHelpText(_("The font size units, points or pixels."));
    if (wxRichTextFontPage::ShowToolTips())
        m_sizeUnitsCtrl->SetToolTip(_("The font size units, points or pixels."));
    itemBoxSizer10->Add(m_sizeUnitsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5);

    m_fontListBoxParent = new wxBoxSizer(wxHORIZONTAL);
    m_innerSizer->Add(m_fontListBoxParent, 1, wxGROW, 5);

    m_faceListBox = new wxRichTextFontListBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_FACELISTBOX, wxDefaultPosition, wxSize(200, 100), 0 );
    m_faceListBox->SetHelpText(_("Lists the available fonts."));
    if (wxRichTextFontPage::ShowToolTips())
        m_faceListBox->SetToolTip(_("Lists the available fonts."));
    m_fontListBoxParent->Add(m_faceListBox, 1, wxGROW|wxALL|wxFIXED_MINSIZE, 5);

    wxArrayString m_sizeListBoxStrings;
    m_sizeListBox = new wxListBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SIZELISTBOX, wxDefaultPosition, wxSize(50, -1), m_sizeListBoxStrings, wxLB_SINGLE );
    m_sizeListBox->SetHelpText(_("Lists font sizes in points."));
    if (wxRichTextFontPage::ShowToolTips())
        m_sizeListBox->SetToolTip(_("Lists font sizes in points."));
    m_fontListBoxParent->Add(m_sizeListBox, 0, wxGROW|wxALL|wxFIXED_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    m_innerSizer->Add(itemBoxSizer17, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer18, 0, wxGROW, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Font st&yle:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_styleCtrlStrings;
    m_styleCtrl = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_STYLECTRL, wxEmptyString, wxDefaultPosition, wxSize(110, -1), m_styleCtrlStrings, wxCB_READONLY );
    m_styleCtrl->SetHelpText(_("Select regular or italic style."));
    if (wxRichTextFontPage::ShowToolTips())
        m_styleCtrl->SetToolTip(_("Select regular or italic style."));
    itemBoxSizer18->Add(m_styleCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer21, 0, wxGROW, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("Font &weight:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemStaticText22, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_weightCtrlStrings;
    m_weightCtrl = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_WEIGHTCTRL, wxEmptyString, wxDefaultPosition, wxSize(110, -1), m_weightCtrlStrings, wxCB_READONLY );
    m_weightCtrl->SetHelpText(_("Select regular or bold."));
    if (wxRichTextFontPage::ShowToolTips())
        m_weightCtrl->SetToolTip(_("Select regular or bold."));
    itemBoxSizer21->Add(m_weightCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer24, 0, wxGROW, 5);

    wxStaticText* itemStaticText25 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Underlining:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer24->Add(itemStaticText25, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_underliningCtrlStrings;
    m_underliningCtrl = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL, wxEmptyString, wxDefaultPosition, wxSize(110, -1), m_underliningCtrlStrings, wxCB_READONLY );
    m_underliningCtrl->SetHelpText(_("Select underlining or no underlining."));
    if (wxRichTextFontPage::ShowToolTips())
        m_underliningCtrl->SetToolTip(_("Select underlining or no underlining."));
    itemBoxSizer24->Add(m_underliningCtrl, 0, wxGROW|wxALL, 5);

    itemBoxSizer17->Add(0, 0, 1, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer28, 0, wxGROW, 5);

    m_textColourLabel = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_COLOURCTRL_LABEL, _("&Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_textColourLabel->SetValue(false);
    itemBoxSizer28->Add(m_textColourLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_colourCtrl = new wxRichTextColourSwatchCtrl( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_COLOURCTRL, wxDefaultPosition, wxSize(40, 20), 0 );
    m_colourCtrl->SetHelpText(_("Click to change the text colour."));
    if (wxRichTextFontPage::ShowToolTips())
        m_colourCtrl->SetToolTip(_("Click to change the text colour."));
    itemBoxSizer28->Add(m_colourCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer31, 0, wxGROW, 5);

    m_bgColourLabel = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_BGCOLOURCTRL_LABEL, _("&Bg colour:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_bgColourLabel->SetValue(false);
    itemBoxSizer31->Add(m_bgColourLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_bgColourCtrl = new wxRichTextColourSwatchCtrl( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_BGCOLOURCTRL, wxDefaultPosition, wxSize(40, 20), 0 );
    m_bgColourCtrl->SetHelpText(_("Click to change the text background colour."));
    if (wxRichTextFontPage::ShowToolTips())
        m_bgColourCtrl->SetToolTip(_("Click to change the text background colour."));
    itemBoxSizer31->Add(m_bgColourCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    m_innerSizer->Add(itemBoxSizer34, 0, wxGROW, 5);

    m_strikethroughCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_STRIKETHROUGHCTRL, _("&Strikethrough"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_strikethroughCtrl->SetValue(false);
    m_strikethroughCtrl->SetHelpText(_("Check to show a line through the text."));
    if (wxRichTextFontPage::ShowToolTips())
        m_strikethroughCtrl->SetToolTip(_("Check to show a line through the text."));
    itemBoxSizer34->Add(m_strikethroughCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_capitalsCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_CAPSCTRL, _("Ca&pitals"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_capitalsCtrl->SetValue(false);
    m_capitalsCtrl->SetHelpText(_("Check to show the text in capitals."));
    if (wxRichTextFontPage::ShowToolTips())
        m_capitalsCtrl->SetToolTip(_("Check to show the text in capitals."));
    itemBoxSizer34->Add(m_capitalsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_smallCapitalsCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SMALLCAPSCTRL, _("Small C&apitals"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_smallCapitalsCtrl->SetValue(false);
    m_smallCapitalsCtrl->SetHelpText(_("Check to show the text in small capitals."));
    if (wxRichTextFontPage::ShowToolTips())
        m_smallCapitalsCtrl->SetToolTip(_("Check to show the text in small capitals."));
    itemBoxSizer34->Add(m_smallCapitalsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_superscriptCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SUPERSCRIPT, _("Supe&rscript"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_superscriptCtrl->SetValue(false);
    m_superscriptCtrl->SetHelpText(_("Check to show the text in superscript."));
    if (wxRichTextFontPage::ShowToolTips())
        m_superscriptCtrl->SetToolTip(_("Check to show the text in superscript."));
    itemBoxSizer34->Add(m_superscriptCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_subscriptCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SUBSCRIPT, _("Subscrip&t"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_subscriptCtrl->SetValue(false);
    m_subscriptCtrl->SetHelpText(_("Check to show the text in subscript."));
    if (wxRichTextFontPage::ShowToolTips())
        m_subscriptCtrl->SetToolTip(_("Check to show the text in subscript."));
    itemBoxSizer34->Add(m_subscriptCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_rtlParentSizer = new wxBoxSizer(wxHORIZONTAL);
    m_innerSizer->Add(m_rtlParentSizer, 0, wxALIGN_LEFT, 5);

    m_rtlCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_RIGHT_TO_LEFT, _("Rig&ht-to-left"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_rtlCtrl->SetValue(false);
    m_rtlCtrl->SetHelpText(_("Check to indicate right-to-left text layout."));
    if (wxRichTextFontPage::ShowToolTips())
        m_rtlCtrl->SetToolTip(_("Check to indicate right-to-left text layout."));
    m_rtlParentSizer->Add(m_rtlCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_suppressHyphenationCtrl = new wxCheckBox( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_SUBSCRIPT_SUPPRESS_HYPHENATION, _("Suppress hyphe&nation"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
    m_suppressHyphenationCtrl->SetValue(false);
    m_suppressHyphenationCtrl->SetHelpText(_("Check to suppress hyphenation."));
    if (wxRichTextFontPage::ShowToolTips())
        m_suppressHyphenationCtrl->SetToolTip(_("Check to suppress hyphenation."));
    m_rtlParentSizer->Add(m_suppressHyphenationCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_innerSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    m_previewCtrl = new wxRichTextFontPreviewCtrl( itemRichTextDialogPage1, ID_RICHTEXTFONTPAGE_PREVIEWCTRL, wxDefaultPosition, wxSize(100, 60), 0 );
    m_previewCtrl->SetHelpText(_("Shows a preview of the font settings."));
    if (wxRichTextFontPage::ShowToolTips())
        m_previewCtrl->SetToolTip(_("Shows a preview of the font settings."));
    m_innerSizer->Add(m_previewCtrl, 0, wxGROW|wxALL, 5);

////@end wxRichTextFontPage content construction

    m_fontSizeSpinButtons->SetRange(0, 999);

    if ((GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_RTL) == 0)
        m_rtlParentSizer->Show(m_rtlCtrl, false);
    if ((GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION) == 0)
        m_rtlParentSizer->Show(m_suppressHyphenationCtrl, false);

    if ((GetAllowedTextEffects() & (wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION|wxTEXT_ATTR_EFFECT_RTL)) == 0)        
        m_innerSizer->Show(m_rtlParentSizer, false);

    m_faceListBox->UpdateFonts();

    m_styleCtrl->Append(_("(none)"));
    m_styleCtrl->Append(_("Regular"));
    m_styleCtrl->Append(_("Italic"));

    m_weightCtrl->Append(_("(none)"));
    m_weightCtrl->Append(_("Regular"));
    m_weightCtrl->Append(_("Bold"));

    m_underliningCtrl->Append(_("(none)"));
    m_underliningCtrl->Append(_("Not underlined"));
    m_underliningCtrl->Append(_("Underlined"));

    wxString nStr;
    int i;
    for (i = 8; i < 40; i++)
    {
        nStr.Printf(wxT("%d"), i);
        m_sizeListBox->Append(nStr);
    }
    m_sizeListBox->Append(wxT("48"));
    m_sizeListBox->Append(wxT("72"));
}

/// Transfer data from/to window
bool wxRichTextFontPage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

    wxRichTextAttr* attr = GetAttributes();

    if (!m_faceTextCtrl->GetValue().IsEmpty())
    {
        wxString faceName = m_faceTextCtrl->GetValue();
        attr->SetFontFaceName(faceName);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_FACE));

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty())
    {
        int sz = wxAtoi(strSize);
        if (sz > 0)
        {
            if (m_sizeUnitsCtrl->GetSelection() == 0)
                attr->SetFontPointSize(sz);
            else
                attr->SetFontPixelSize(sz);
        }
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_SIZE));

    if (m_styleCtrl->GetSelection() != wxNOT_FOUND && m_styleCtrl->GetSelection() != 0)
    {
        wxFontStyle style;
        if (m_styleCtrl->GetSelection() == 2)
            style = wxFONTSTYLE_ITALIC;
        else
            style = wxFONTSTYLE_NORMAL;

        attr->SetFontStyle(style);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_ITALIC));

    if (m_weightCtrl->GetSelection() != wxNOT_FOUND && m_weightCtrl->GetSelection() != 0)
    {
        wxFontWeight weight;
        if (m_weightCtrl->GetSelection() == 2)
            weight = wxFONTWEIGHT_BOLD;
        else
            weight = wxFONTWEIGHT_NORMAL;

        attr->SetFontWeight(weight);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_WEIGHT));

    if (m_underliningCtrl->GetSelection() != wxNOT_FOUND && m_underliningCtrl->GetSelection() != 0)
    {
        bool underlined;
        if (m_underliningCtrl->GetSelection() == 2)
            underlined = true;
        else
            underlined = false;

        attr->SetFontUnderlined(underlined);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_UNDERLINE));

    if (m_textColourLabel->GetValue())
    {
        attr->SetTextColour(m_colourCtrl->GetColour());
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_TEXT_COLOUR));

    if (m_bgColourLabel->GetValue())
    {
        attr->SetBackgroundColour(m_bgColourCtrl->GetColour());
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_BACKGROUND_COLOUR));

    if (m_strikethroughCtrl->Get3StateValue() != wxCHK_UNDETERMINED)
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_STRIKETHROUGH);

        if (m_strikethroughCtrl->Get3StateValue() == wxCHK_CHECKED)
            attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
        else
            attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
    }
    else
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
        attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
    }

    if (m_capitalsCtrl->Get3StateValue() != wxCHK_UNDETERMINED)
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_CAPITALS);

        if (m_capitalsCtrl->Get3StateValue() == wxCHK_CHECKED)
            attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_CAPITALS);
        else
            attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_CAPITALS);
    }
    else
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~wxTEXT_ATTR_EFFECT_CAPITALS);
        attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_CAPITALS);
    }

    if (m_smallCapitalsCtrl->Get3StateValue() != wxCHK_UNDETERMINED)
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_SMALL_CAPITALS);

        if (m_smallCapitalsCtrl->Get3StateValue() == wxCHK_CHECKED)
            attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_SMALL_CAPITALS);
        else
            attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SMALL_CAPITALS);
    }
    else
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~wxTEXT_ATTR_EFFECT_SMALL_CAPITALS);
        attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SMALL_CAPITALS);
    }

    if (m_superscriptCtrl->Get3StateValue() == wxCHK_CHECKED)
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
        attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
        attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SUBSCRIPT);
    }
    else if (m_subscriptCtrl->Get3StateValue() == wxCHK_CHECKED)
    {
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_SUBSCRIPT);
        attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_SUBSCRIPT);
        attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
    }
    else
    {
        // If they are undetermined, we don't want to include these flags in the text effects - the objects
        // should retain their original style.
        attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~(wxTEXT_ATTR_EFFECT_SUBSCRIPT|wxTEXT_ATTR_EFFECT_SUPERSCRIPT) );
        attr->SetTextEffects(attr->GetTextEffects() & ~(wxTEXT_ATTR_EFFECT_SUBSCRIPT|wxTEXT_ATTR_EFFECT_SUPERSCRIPT) );
    }

    if (GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_RTL)
    {
        if (m_rtlCtrl->Get3StateValue() != wxCHK_UNDETERMINED)
        {
            attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_RTL);

            if (m_rtlCtrl->Get3StateValue() == wxCHK_CHECKED)
                attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_RTL);
            else
                attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_RTL);
        }
        else
        {
            attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~wxTEXT_ATTR_EFFECT_RTL);
            attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_RTL);
        }
    }

    if (GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION)
    {
        if (m_suppressHyphenationCtrl->Get3StateValue() != wxCHK_UNDETERMINED)
        {
            attr->SetTextEffectFlags(attr->GetTextEffectFlags() | wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);

            if (m_suppressHyphenationCtrl->Get3StateValue() == wxCHK_CHECKED)
                attr->SetTextEffects(attr->GetTextEffects() | wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
            else
                attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
        }
        else
        {
            attr->SetTextEffectFlags(attr->GetTextEffectFlags() & ~wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
            attr->SetTextEffects(attr->GetTextEffects() & ~wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
        }
    }

    if (attr->GetTextEffectFlags() == 0)
        attr->SetFlags(attr->GetFlags() & ~wxTEXT_ATTR_EFFECTS);

    return true;
}

bool wxRichTextFontPage::TransferDataToWindow()
{
    wxPanel::TransferDataToWindow();

    m_dontUpdate = true;
    wxRichTextAttr* attr = GetAttributes();

    if (attr->HasFontFaceName())
    {
        m_faceTextCtrl->SetValue(attr->GetFontFaceName());
        m_faceListBox->SetFaceNameSelection(attr->GetFont().GetFaceName());
    }
    else
    {
        m_faceTextCtrl->SetValue(wxEmptyString);
        m_faceListBox->SetFaceNameSelection(wxEmptyString);
    }

    if (attr->HasFontPointSize())
    {
        wxString strSize = wxString::Format(wxT("%d"), attr->GetFontSize());
        m_sizeTextCtrl->SetValue(strSize);
        m_fontSizeSpinButtons->SetValue(attr->GetFontSize());
        m_sizeUnitsCtrl->SetSelection(0);
        if (m_sizeListBox->FindString(strSize) != wxNOT_FOUND)
            m_sizeListBox->SetStringSelection(strSize);
    }
    else if (attr->HasFontPixelSize())
    {
        wxString strSize = wxString::Format(wxT("%d"), attr->GetFontSize());
        m_sizeTextCtrl->SetValue(strSize);
        m_fontSizeSpinButtons->SetValue(attr->GetFontSize());
        m_sizeUnitsCtrl->SetSelection(1);
        m_sizeListBox->SetSelection(wxNOT_FOUND);
    }
    else
    {
        m_sizeTextCtrl->SetValue(wxEmptyString);
        m_sizeListBox->SetSelection(wxNOT_FOUND);
    }

    if (attr->HasFontWeight())
    {
        if (attr->GetFontWeight() == wxFONTWEIGHT_BOLD)
            m_weightCtrl->SetSelection(2);
        else
            m_weightCtrl->SetSelection(1);
    }
    else
    {
        m_weightCtrl->SetSelection(0);
    }

    if (attr->HasFontItalic())
    {
        if (attr->GetFontStyle() == wxFONTSTYLE_ITALIC)
            m_styleCtrl->SetSelection(2);
        else
            m_styleCtrl->SetSelection(1);
    }
    else
    {
        m_styleCtrl->SetSelection(0);
    }

    if (attr->HasFontUnderlined())
    {
        if (attr->GetFontUnderlined())
            m_underliningCtrl->SetSelection(2);
        else
            m_underliningCtrl->SetSelection(1);
    }
    else
    {
        m_underliningCtrl->SetSelection(0);
    }

    if (attr->HasTextColour())
    {
        m_colourCtrl->SetColour(attr->GetTextColour());
        m_textColourLabel->SetValue(true);
        m_colourPresent = true;
    }
    else
    {
        m_colourCtrl->SetColour(*wxBLACK);
        m_textColourLabel->SetValue(false);
    }

    if (attr->HasBackgroundColour())
    {
        m_bgColourCtrl->SetColour(attr->GetBackgroundColour());
        m_bgColourLabel->SetValue(true);
        m_bgColourPresent = true;
    }
    else
    {
        m_bgColourCtrl->SetColour(*wxWHITE);
        m_bgColourLabel->SetValue(false);
    }

    if (attr->HasTextEffects())
    {
        if (attr->GetTextEffectFlags() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH)
        {
            if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH)
                m_strikethroughCtrl->Set3StateValue(wxCHK_CHECKED);
            else
                m_strikethroughCtrl->Set3StateValue(wxCHK_UNCHECKED);
        }
        else
            m_strikethroughCtrl->Set3StateValue(wxCHK_UNDETERMINED);

        if (attr->GetTextEffectFlags() & wxTEXT_ATTR_EFFECT_CAPITALS)
        {
            if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS)
                m_capitalsCtrl->Set3StateValue(wxCHK_CHECKED);
            else
                m_capitalsCtrl->Set3StateValue(wxCHK_UNCHECKED);
        }
        else
            m_capitalsCtrl->Set3StateValue(wxCHK_UNDETERMINED);

        if (attr->GetTextEffectFlags() & wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)
        {
            if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)
                m_smallCapitalsCtrl->Set3StateValue(wxCHK_CHECKED);
            else
                m_smallCapitalsCtrl->Set3StateValue(wxCHK_UNCHECKED);
        }
        else
            m_smallCapitalsCtrl->Set3StateValue(wxCHK_UNDETERMINED);

        if ( attr->GetTextEffectFlags() & (wxTEXT_ATTR_EFFECT_SUPERSCRIPT | wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
        {
            if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT)
            {
                m_superscriptCtrl->Set3StateValue(wxCHK_CHECKED);
                m_subscriptCtrl->Set3StateValue(wxCHK_UNCHECKED);
            }
            else if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT)
            {
                m_superscriptCtrl->Set3StateValue(wxCHK_UNCHECKED);
                m_subscriptCtrl->Set3StateValue(wxCHK_CHECKED);
            }
            else
            {
                m_superscriptCtrl->Set3StateValue(wxCHK_UNCHECKED);
                m_subscriptCtrl->Set3StateValue(wxCHK_UNCHECKED);
            }
        }
        else
        {
            m_superscriptCtrl->Set3StateValue(wxCHK_UNDETERMINED);
            m_subscriptCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        }

        if (GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_RTL)
        {
            if (attr->GetTextEffectFlags() & wxTEXT_ATTR_EFFECT_RTL)
            {
                if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_RTL)
                    m_rtlCtrl->Set3StateValue(wxCHK_CHECKED);
                else
                    m_rtlCtrl->Set3StateValue(wxCHK_UNCHECKED);
            }
            else
                m_rtlCtrl->Set3StateValue(wxCHK_UNDETERMINED);        
        }

        if (GetAllowedTextEffects() & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION)
        {
            if (attr->GetTextEffectFlags() & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION)
            {
                if (attr->GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION)
                    m_suppressHyphenationCtrl->Set3StateValue(wxCHK_CHECKED);
                else
                    m_suppressHyphenationCtrl->Set3StateValue(wxCHK_UNCHECKED);
            }
            else
                m_suppressHyphenationCtrl->Set3StateValue(wxCHK_UNDETERMINED);        
        }
    }
    else
    {
        m_strikethroughCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_capitalsCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_smallCapitalsCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_superscriptCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_subscriptCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_rtlCtrl->Set3StateValue(wxCHK_UNDETERMINED);
        m_suppressHyphenationCtrl->Set3StateValue(wxCHK_UNDETERMINED);
    }

    UpdatePreview();

    m_dontUpdate = false;

    return true;
}

wxRichTextAttr* wxRichTextFontPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/// Updates the font preview
void wxRichTextFontPage::UpdatePreview()
{
    wxRichTextAttr attr;

    if (m_textColourLabel->GetValue())
        m_previewCtrl->SetForegroundColour(m_colourCtrl->GetColour());
    else
    {
        m_previewCtrl->SetForegroundColour(*wxBLACK);
        if (!(m_colourCtrl->GetColour() == *wxBLACK))
        {
            m_colourCtrl->SetColour(*wxBLACK);
            m_colourCtrl->Refresh();
        }
    }

    if (m_bgColourLabel->GetValue())
        m_previewCtrl->SetBackgroundColour(m_bgColourCtrl->GetColour());
    else
    {
        m_previewCtrl->SetBackgroundColour(*wxWHITE);

        if (!(m_bgColourCtrl->GetColour() == *wxWHITE))
        {
            m_bgColourCtrl->SetColour(*wxWHITE);
            m_bgColourCtrl->Refresh();
        }
    }

    if (m_faceListBox->GetSelection() != wxNOT_FOUND)
    {
        wxString faceName = m_faceListBox->GetFaceName(m_faceListBox->GetSelection());
        attr.SetFontFaceName(faceName);
    }

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty())
    {
        int sz = wxAtoi(strSize);
        if (sz > 0)
        {
            if (m_sizeUnitsCtrl->GetSelection() == 1)
                attr.SetFontPixelSize(sz);
            else
                attr.SetFontPointSize(sz);
        }
    }

    if (m_styleCtrl->GetSelection() != wxNOT_FOUND && m_styleCtrl->GetSelection() != 0)
    {
        wxFontStyle style;
        if (m_styleCtrl->GetSelection() == 2)
            style = wxFONTSTYLE_ITALIC;
        else
            style = wxFONTSTYLE_NORMAL;

        attr.SetFontStyle(style);
    }

    if (m_weightCtrl->GetSelection() != wxNOT_FOUND && m_weightCtrl->GetSelection() != 0)
    {
        wxFontWeight weight;
        if (m_weightCtrl->GetSelection() == 2)
            weight = wxFONTWEIGHT_BOLD;
        else
            weight = wxFONTWEIGHT_NORMAL;

        attr.SetFontWeight(weight);
    }

    if (m_underliningCtrl->GetSelection() != wxNOT_FOUND && m_underliningCtrl->GetSelection() != 0)
    {
        bool underlined;
        if (m_underliningCtrl->GetSelection() == 2)
            underlined = true;
        else
            underlined = false;

        attr.SetFontUnderlined(underlined);
    }

    int textEffects = 0;

    if (m_strikethroughCtrl->Get3StateValue() == wxCHK_CHECKED)
    {
        textEffects |= wxTEXT_ATTR_EFFECT_STRIKETHROUGH;
    }

    if (m_capitalsCtrl->Get3StateValue() == wxCHK_CHECKED)
    {
        textEffects |= wxTEXT_ATTR_EFFECT_CAPITALS;
    }

    if (m_smallCapitalsCtrl->Get3StateValue() == wxCHK_CHECKED)
    {
        textEffects |= wxTEXT_ATTR_EFFECT_SMALL_CAPITALS;
    }

    if ( m_superscriptCtrl->Get3StateValue() == wxCHK_CHECKED )
        textEffects |= wxTEXT_ATTR_EFFECT_SUPERSCRIPT;
    else if ( m_subscriptCtrl->Get3StateValue() == wxCHK_CHECKED )
        textEffects |= wxTEXT_ATTR_EFFECT_SUBSCRIPT;

    wxFont font = attr.GetFont();
    m_previewCtrl->SetFont(font);
    m_previewCtrl->SetTextEffects(textEffects);
    m_previewCtrl->Refresh();
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextFontPage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextFontPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextFontPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextFontPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextFontPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextFontPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextFontPage icon retrieval
}

/*!
 * wxEVT_TEXT event handler for ID_RICHTEXTFONTPAGE_FACETEXTCTRL
 */

void wxRichTextFontPage::OnFaceTextCtrlUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    wxString facename = m_faceTextCtrl->GetValue();
    if (!facename.IsEmpty())
    {
        if (m_faceListBox->HasFaceName(facename))
        {
            m_faceListBox->SetFaceNameSelection(facename);
            UpdatePreview();
        }
        else
        {
            // Try to find a partial match
            const wxArrayString& arr = m_faceListBox->GetFaceNames();
            size_t i;
            for (i = 0; i < arr.GetCount(); i++)
            {
                if (arr[i].Mid(0, facename.length()).Lower() == facename.Lower())
                {
                    m_faceListBox->ScrollToRow(i);
                    break;
                }
            }
        }
    }
}


/*!
 * wxEVT_TEXT event handler for ID_RICHTEXTFONTPAGE_SIZETEXTCTRL
 */

void wxRichTextFontPage::OnSizeTextCtrlUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    m_dontUpdate = true;

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty() && m_sizeListBox->FindString(strSize) != wxNOT_FOUND)
        m_sizeListBox->SetStringSelection(strSize);
    if (!strSize.IsEmpty())
        m_fontSizeSpinButtons->SetValue(wxAtoi(strSize));

    m_dontUpdate = false;

    UpdatePreview();
}


/*!
 * wxEVT_LISTBOX event handler for ID_RICHTEXTFONTPAGE_SIZELISTBOX
 */

void wxRichTextFontPage::OnSizeListBoxSelected( wxCommandEvent& event )
{
    bool oldDontUpdate = m_dontUpdate;
    m_dontUpdate = true;

    m_sizeTextCtrl->SetValue(event.GetString());
    if (!event.GetString().IsEmpty())
        m_fontSizeSpinButtons->SetValue(wxAtoi(event.GetString()));

    m_dontUpdate = oldDontUpdate;

    if (m_dontUpdate)
        return;

    UpdatePreview();
}

/*!
 * wxEVT_LISTBOX event handler for ID_RICHTEXTFONTPAGE_FACELISTBOX
 */

void wxRichTextFontPage::OnFaceListBoxSelected( wxCommandEvent& WXUNUSED(event) )
{
    bool oldDontUpdate = m_dontUpdate;
    m_dontUpdate = true;

    m_faceTextCtrl->SetValue(m_faceListBox->GetFaceName(m_faceListBox->GetSelection()));

    m_dontUpdate = oldDontUpdate;

    if (m_dontUpdate)
        return;

    UpdatePreview();
}

/*!
 * wxEVT_COMBOBOX event handler for ID_RICHTEXTFONTPAGE_STYLECTRL
 */

void wxRichTextFontPage::OnStyleCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}


/*!
 * wxEVT_COMBOBOX event handler for ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL
 */

void wxRichTextFontPage::OnUnderliningCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}


/*!
 * wxEVT_COMBOBOX event handler for ID_RICHTEXTFONTPAGE_WEIGHTCTRL
 */

void wxRichTextFontPage::OnWeightCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}

void wxRichTextFontPage::OnColourClicked( wxCommandEvent& event )
{
    if (event.GetId() == m_colourCtrl->GetId())
        m_colourPresent = true;
    else if (event.GetId() == m_bgColourCtrl->GetId())
        m_bgColourPresent = true;

    m_dontUpdate = true;

    if (event.GetId() == m_colourCtrl->GetId())
    {
        m_textColourLabel->SetValue(true);
    }
    else if (event.GetId() == m_bgColourCtrl->GetId())
    {
        m_bgColourLabel->SetValue(true);
    }

    m_dontUpdate = false;

    UpdatePreview();
}
/*!
 * wxEVT_CHECKBOX event handler for ID_RICHTEXTFONTPAGE_STRIKETHROUGHCTRL
 */

void wxRichTextFontPage::OnStrikethroughctrlClick( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}

/*!
 * wxEVT_CHECKBOX event handler for ID_RICHTEXTFONTPAGE_CAPSCTRL
 */

void wxRichTextFontPage::OnCapsctrlClick( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}

/*!
 * wxEVT_CHECKBOX event handler for ID_RICHTEXTFONTPAGE_SUPERSCRIPT
 */

void wxRichTextFontPage::OnRichtextfontpageSuperscriptClick( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    if ( m_superscriptCtrl->Get3StateValue() == wxCHK_CHECKED)
        m_subscriptCtrl->Set3StateValue( wxCHK_UNCHECKED );

    UpdatePreview();
}

/*!
 * wxEVT_CHECKBOX event handler for ID_RICHTEXTFONTPAGE_SUBSCRIPT
 */

void wxRichTextFontPage::OnRichtextfontpageSubscriptClick( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    if ( m_subscriptCtrl->Get3StateValue() == wxCHK_CHECKED)
        m_superscriptCtrl->Set3StateValue( wxCHK_UNCHECKED );

    UpdatePreview();
}

/*!
 * wxEVT_CHOICE event handler for ID_RICHTEXTFONTPAGE_SIZE_UNITS
 */

void wxRichTextFontPage::OnRichtextfontpageSizeUnitsSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    UpdatePreview();
}

/*!
 * wxEVT_SPINCTRL event handler for ID_RICHTEXTFONTPAGE_SPINBUTTONS
 */

void wxRichTextFontPage::OnRichtextfontpageSpinbuttonsUp( wxSpinEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    m_dontUpdate = true;

    wxString text = m_sizeTextCtrl->GetValue();
    int size = 12;
    if (!text.IsEmpty())
    {
        size = wxAtoi(text);
        size ++;
    }

    if (size < 1 || size > 999)
        size = 12;

    if (m_fontSizeSpinButtons->GetValue() != size)
        m_fontSizeSpinButtons->SetValue(size);

    wxString newText(wxString::Format(wxT("%d"), size));

    m_sizeTextCtrl->SetValue(newText);
    if (!newText.IsEmpty() && m_sizeListBox->FindString(newText) != wxNOT_FOUND)
        m_sizeListBox->SetStringSelection(newText);
    UpdatePreview();

    m_dontUpdate = false;
}

/*!
 * wxEVT_SCROLL_LINEDOWN event handler for ID_RICHTEXTFONTPAGE_SPINBUTTONS
 */

void wxRichTextFontPage::OnRichtextfontpageSpinbuttonsDown( wxSpinEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    m_dontUpdate = true;

    wxString text = m_sizeTextCtrl->GetValue();
    int size = 12;
    if (!text.IsEmpty())
    {
        size = wxAtoi(text);
        if (size > 1)
            size --;
    }

    if (size < 1 || size > 999)
        size = 12;

    if (m_fontSizeSpinButtons->GetValue() != size)
        m_fontSizeSpinButtons->SetValue(size);

    wxString newText(wxString::Format(wxT("%d"), size));

    m_sizeTextCtrl->SetValue(newText);
    if (!newText.IsEmpty() && m_sizeListBox->FindString(newText) != wxNOT_FOUND)
        m_sizeListBox->SetStringSelection(newText);
    UpdatePreview();

    m_dontUpdate = false;
}

/*!
 * wxEVT_IDLE event handler for ID_RICHTEXTFONTPAGE
 */

void wxRichTextFontPage::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    if (!m_sizeUnitsCtrl)
        return;

    if (m_sizeUnitsCtrl->GetSelection() == 1 && m_sizeListBox->IsShown())
    {
        m_fontListBoxParent->Show(m_sizeListBox, false);
        Layout();
    }
    else if (m_sizeUnitsCtrl->GetSelection() == 0 && !m_sizeListBox->IsShown())
    {
        m_fontListBoxParent->Show(m_sizeListBox, true);
        Layout();
    }
    if (!wxRichTextFormattingDialog::GetDialog(this)->HasOption(wxRichTextFormattingDialog::Option_AllowPixelFontSize) &&
        m_sizeUnitsCtrl->IsEnabled())
    {
        m_sizeUnitsCtrl->Disable();
    }
}
