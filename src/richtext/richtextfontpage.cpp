/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richeditfontpage.cpp
// Purpose:     Font page for wxRichTextFormattingDialog
// Author:      Julian Smart
// Modified by:
// Created:     2006-10-02
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "../../include/wx/richtext/richtextfontpage.h"

/*!
 * wxRichTextFontPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextFontPage, wxPanel )

/*!
 * wxRichTextFontPage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextFontPage, wxPanel )
    EVT_LISTBOX( ID_RICHTEXTFONTPAGE_FACELISTBOX, wxRichTextFontPage::OnFaceListBoxSelected )
    EVT_BUTTON( ID_RICHTEXTFONTPAGE_COLOURCTRL, wxRichTextFontPage::OnColourClicked )

////@begin wxRichTextFontPage event table entries
    EVT_TEXT( ID_RICHTEXTFONTPAGE_FACETEXTCTRL, wxRichTextFontPage::OnFaceTextCtrlUpdated )

    EVT_TEXT( ID_RICHTEXTFONTPAGE_SIZETEXTCTRL, wxRichTextFontPage::OnSizeTextCtrlUpdated )

    EVT_LISTBOX( ID_RICHTEXTFONTPAGE_SIZELISTBOX, wxRichTextFontPage::OnSizeListBoxSelected )

    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_STYLECTRL, wxRichTextFontPage::OnStyleCtrlSelected )

    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_WEIGHTCTRL, wxRichTextFontPage::OnWeightCtrlSelected )

    EVT_COMBOBOX( ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL, wxRichTextFontPage::OnUnderliningCtrlSelected )

////@end wxRichTextFontPage event table entries

END_EVENT_TABLE()

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

////@begin wxRichTextFontPage member initialisation
    m_faceTextCtrl = NULL;
    m_faceListBox = NULL;
    m_sizeTextCtrl = NULL;
    m_sizeListBox = NULL;
    m_styleCtrl = NULL;
    m_weightCtrl = NULL;
    m_underliningCtrl = NULL;
    m_colourCtrl = NULL;
    m_previewCtrl = NULL;
////@end wxRichTextFontPage member initialisation
}

/*!
 * wxRichTextFontPage creator
 */

bool wxRichTextFontPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextFontPage creation
    wxPanel::Create( parent, id, pos, size, style );

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
    wxRichTextFontPage* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 1, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 1, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Font:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_faceTextCtrl = new wxTextCtrl( itemPanel1, ID_RICHTEXTFONTPAGE_FACETEXTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_faceTextCtrl->SetHelpText(_("Type a font name."));
    if (ShowToolTips())
        m_faceTextCtrl->SetToolTip(_("Type a font name."));
    itemBoxSizer5->Add(m_faceTextCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_faceListBox = new wxRichTextFontListBox( itemPanel1, ID_RICHTEXTFONTPAGE_FACELISTBOX, wxDefaultPosition, wxSize(200, 140), wxSIMPLE_BORDER );
    m_faceListBox->SetHelpText(_("Lists the available fonts."));
    if (ShowToolTips())
        m_faceListBox->SetToolTip(_("Lists the available fonts."));
    itemBoxSizer5->Add(m_faceListBox, 1, wxGROW|wxALL|wxFIXED_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer9, 0, wxGROW, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_sizeTextCtrl = new wxTextCtrl( itemPanel1, ID_RICHTEXTFONTPAGE_SIZETEXTCTRL, _T(""), wxDefaultPosition, wxSize(50, -1), 0 );
    m_sizeTextCtrl->SetHelpText(_("Type a size in points."));
    if (ShowToolTips())
        m_sizeTextCtrl->SetToolTip(_("Type a size in points."));
    itemBoxSizer9->Add(m_sizeTextCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_sizeListBoxStrings = NULL;
    m_sizeListBox = new wxListBox( itemPanel1, ID_RICHTEXTFONTPAGE_SIZELISTBOX, wxDefaultPosition, wxSize(50, -1), 0, m_sizeListBoxStrings, wxLB_SINGLE );
    m_sizeListBox->SetHelpText(_("Lists font sizes in points."));
    if (ShowToolTips())
        m_sizeListBox->SetToolTip(_("Lists font sizes in points."));
    itemBoxSizer9->Add(m_sizeListBox, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxFIXED_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer13, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer14, 1, wxGROW, 5);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, wxID_STATIC, _("Font st&yle:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer14->Add(itemStaticText15, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_styleCtrlStrings = NULL;
    m_styleCtrl = new wxComboBox( itemPanel1, ID_RICHTEXTFONTPAGE_STYLECTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0, m_styleCtrlStrings, wxCB_READONLY );
    m_styleCtrl->SetHelpText(_("Select regular or italic style."));
    if (ShowToolTips())
        m_styleCtrl->SetToolTip(_("Select regular or italic style."));
    itemBoxSizer14->Add(m_styleCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer17, 1, wxGROW, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemPanel1, wxID_STATIC, _("Font &weight:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(itemStaticText18, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_weightCtrlStrings = NULL;
    m_weightCtrl = new wxComboBox( itemPanel1, ID_RICHTEXTFONTPAGE_WEIGHTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0, m_weightCtrlStrings, wxCB_READONLY );
    m_weightCtrl->SetHelpText(_("Select regular or bold."));
    if (ShowToolTips())
        m_weightCtrl->SetToolTip(_("Select regular or bold."));
    itemBoxSizer17->Add(m_weightCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer20, 1, wxGROW, 5);

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Underlining:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer20->Add(itemStaticText21, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_underliningCtrlStrings = NULL;
    m_underliningCtrl = new wxComboBox( itemPanel1, ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0, m_underliningCtrlStrings, wxCB_READONLY );
    m_underliningCtrl->SetHelpText(_("Select underlining or no underlining."));
    if (ShowToolTips())
        m_underliningCtrl->SetToolTip(_("Select underlining or no underlining."));
    itemBoxSizer20->Add(m_underliningCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer23, 0, wxGROW, 5);

    wxStaticText* itemStaticText24 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer23->Add(itemStaticText24, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_colourCtrl = new wxRichTextColourSwatchCtrl( itemPanel1, ID_RICHTEXTFONTPAGE_COLOURCTRL, wxDefaultPosition, wxSize(40, 20), wxSIMPLE_BORDER );
    m_colourCtrl->SetHelpText(_("Click to change the text colour."));
    if (ShowToolTips())
        m_colourCtrl->SetToolTip(_("Click to change the text colour."));
    itemBoxSizer23->Add(m_colourCtrl, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_previewCtrl = new wxRichTextFontPreviewCtrl( itemPanel1, ID_RICHTEXTFONTPAGE_PREVIEWCTRL, wxDefaultPosition, wxSize(100, 80), wxSIMPLE_BORDER );
    m_previewCtrl->SetHelpText(_("Shows a preview of the font settings."));
    if (ShowToolTips())
        m_previewCtrl->SetToolTip(_("Shows a preview of the font settings."));
    itemBoxSizer3->Add(m_previewCtrl, 0, wxGROW|wxALL, 5);

////@end wxRichTextFontPage content construction

    m_faceListBox->UpdateFonts();

    m_styleCtrl->Append(_("Regular"));
    m_styleCtrl->Append(_("Italic"));

    m_weightCtrl->Append(_("Regular"));
    m_weightCtrl->Append(_("Bold"));

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

    wxTextAttrEx* attr = GetAttributes();

    if (m_faceListBox->GetSelection() != wxNOT_FOUND)
    {
        wxString faceName = m_faceListBox->GetFaceName(m_faceListBox->GetSelection());
        if (!faceName.IsEmpty())
        {
            wxFont font(attr->GetFont().Ok() ? attr->GetFont() : *wxNORMAL_FONT);
            font.SetFaceName(faceName);
            wxSetFontPreservingStyles(*attr, font);
            attr->SetFlags(attr->GetFlags() | wxTEXT_ATTR_FONT_FACE);
        }
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_FACE));

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty())
    {
        int sz = wxAtoi(strSize);
        if (sz > 0)
        {
            wxFont font(attr->GetFont().Ok() ? attr->GetFont() : *wxNORMAL_FONT);
            font.SetPointSize(sz);
            wxSetFontPreservingStyles(*attr, font);
            attr->SetFlags(attr->GetFlags() | wxTEXT_ATTR_FONT_SIZE);
        }
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_SIZE));

    if (m_styleCtrl->GetSelection() != wxNOT_FOUND)
    {
        int style;
        if (m_styleCtrl->GetStringSelection() == _("Italic"))
            style = wxITALIC;
        else
            style = wxNORMAL;

        wxFont font(attr->GetFont().Ok() ? attr->GetFont() : *wxNORMAL_FONT);
        font.SetStyle(style);
        wxSetFontPreservingStyles(*attr, font);
        attr->SetFlags(attr->GetFlags() | wxTEXT_ATTR_FONT_ITALIC);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_ITALIC));

    if (m_weightCtrl->GetSelection() != wxNOT_FOUND)
    {
        int weight;
        if (m_weightCtrl->GetStringSelection() == _("Bold"))
            weight = wxBOLD;
        else
            weight = wxNORMAL;

        wxFont font(attr->GetFont().Ok() ? attr->GetFont() : *wxNORMAL_FONT);
        font.SetWeight(weight);
        wxSetFontPreservingStyles(*attr, font);
        attr->SetFlags(attr->GetFlags() | wxTEXT_ATTR_FONT_WEIGHT);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_WEIGHT));

    if (m_underliningCtrl->GetSelection() != wxNOT_FOUND)
    {
        bool underlined;
        if (m_underliningCtrl->GetStringSelection() == _("Underlined"))
            underlined = true;
        else
            underlined = false;

        wxFont font(attr->GetFont().Ok() ? attr->GetFont() : *wxNORMAL_FONT);
        font.SetUnderlined(underlined);
        wxSetFontPreservingStyles(*attr, font);
        attr->SetFlags(attr->GetFlags() | wxTEXT_ATTR_FONT_UNDERLINE);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_FONT_UNDERLINE));

    if (m_colourPresent)
    {
        attr->SetTextColour(m_colourCtrl->GetBackgroundColour());
    }
    else
        attr->SetFlags(attr->GetFlags() & (~ wxTEXT_ATTR_TEXT_COLOUR));

    return true;
}

bool wxRichTextFontPage::TransferDataToWindow()
{
    wxPanel::TransferDataToWindow();

    m_dontUpdate = true;
    wxTextAttrEx* attr = GetAttributes();

    if (attr->HasFont() && attr->HasFaceName())
    {
        m_faceTextCtrl->SetValue(attr->GetFont().GetFaceName());
        m_faceListBox->SetFaceNameSelection(attr->GetFont().GetFaceName());
    }
    else
    {
        m_faceTextCtrl->SetValue(wxEmptyString);
        m_faceListBox->SetFaceNameSelection(wxEmptyString);
    }

    if (attr->HasFont() && attr->HasSize())
    {
        wxString strSize = wxString::Format(wxT("%d"), attr->GetFont().GetPointSize());
        m_sizeTextCtrl->SetValue(strSize);
        if (m_sizeListBox->FindString(strSize) != wxNOT_FOUND)
            m_sizeListBox->SetStringSelection(strSize);
    }
    else
    {
        m_sizeTextCtrl->SetValue(wxEmptyString);
        m_sizeListBox->SetSelection(wxNOT_FOUND);
    }

    if (attr->HasFont() && attr->HasWeight())
    {
        if (attr->GetFont().GetWeight() == wxBOLD)
            m_weightCtrl->SetSelection(1);
        else
            m_weightCtrl->SetSelection(0);
    }
    else
    {
        m_weightCtrl->SetSelection(wxNOT_FOUND);
    }

    if (attr->HasFont() && attr->HasItalic())
    {
        if (attr->GetFont().GetStyle() == wxITALIC)
            m_styleCtrl->SetSelection(1);
        else
            m_styleCtrl->SetSelection(0);
    }
    else
    {
        m_styleCtrl->SetSelection(wxNOT_FOUND);
    }

    if (attr->HasFont() && attr->HasUnderlined())
    {
        if (attr->GetFont().GetUnderlined())
            m_underliningCtrl->SetSelection(1);
        else
            m_underliningCtrl->SetSelection(0);
    }
    else
    {
        m_underliningCtrl->SetSelection(wxNOT_FOUND);
    }

    if (attr->HasTextColour())
    {
        m_colourCtrl->SetBackgroundColour(attr->GetTextColour());
        m_colourPresent = true;
    }

    UpdatePreview();

    m_dontUpdate = false;

    return true;
}

wxTextAttrEx* wxRichTextFontPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/// Updates the font preview
void wxRichTextFontPage::UpdatePreview()
{
    wxFont font(*wxNORMAL_FONT);

    if (m_colourPresent)
        m_previewCtrl->SetForegroundColour(m_colourCtrl->GetBackgroundColour());

    if (m_faceListBox->GetSelection() != wxNOT_FOUND)
    {
        wxString faceName = m_faceListBox->GetFaceName(m_faceListBox->GetSelection());
        font.SetFaceName(faceName);
    }

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty())
    {
        int sz = wxAtoi(strSize);
        if (sz > 0)
            font.SetPointSize(sz);
    }

    if (m_styleCtrl->GetSelection() != wxNOT_FOUND)
    {
        int style;
        if (m_styleCtrl->GetStringSelection() == _("Italic"))
            style = wxITALIC;
        else
            style = wxNORMAL;

        font.SetStyle(style);
    }

    if (m_weightCtrl->GetSelection() != wxNOT_FOUND)
    {
        int weight;
        if (m_weightCtrl->GetStringSelection() == _("Bold"))
            weight = wxBOLD;
        else
            weight = wxNORMAL;

        font.SetWeight(weight);
    }

    if (m_underliningCtrl->GetSelection() != wxNOT_FOUND)
    {
        bool underlined;
        if (m_underliningCtrl->GetStringSelection() == _("Underlined"))
            underlined = true;
        else
            underlined = false;

        font.SetUnderlined(underlined);
    }

    m_previewCtrl->SetFont(font);
    m_previewCtrl->Refresh();
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextFontPage::ShowToolTips()
{
    return true;
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
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTFONTPAGE_FACETEXTCTRL
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
                if (arr[i].Mid(0, facename.Length()).Lower() == facename.Lower())
                {
                    m_faceListBox->ScrollToLine(i);
                    break;
                }
            }
        }
    }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTFONTPAGE_SIZETEXTCTRL
 */

void wxRichTextFontPage::OnSizeTextCtrlUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;

    wxString strSize = m_sizeTextCtrl->GetValue();
    if (!strSize.IsEmpty() && m_sizeListBox->FindString(strSize) != wxNOT_FOUND)
        m_sizeListBox->SetStringSelection(strSize);
    UpdatePreview();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_RICHTEXTFONTPAGE_SIZELISTBOX
 */

void wxRichTextFontPage::OnSizeListBoxSelected( wxCommandEvent& event )
{
    m_dontUpdate = true;

    m_sizeTextCtrl->SetValue(event.GetString());

    m_dontUpdate = false;

    UpdatePreview();
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_RICHTEXTFONTPAGE_FACELISTBOX
 */

void wxRichTextFontPage::OnFaceListBoxSelected( wxCommandEvent& WXUNUSED(event) )
{
    m_dontUpdate = true;

    m_faceTextCtrl->SetValue(m_faceListBox->GetFaceName(m_faceListBox->GetSelection()));

    m_dontUpdate = false;

    UpdatePreview();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTFONTPAGE_STYLECTRL
 */

void wxRichTextFontPage::OnStyleCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    UpdatePreview();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTFONTPAGE_UNDERLINING_CTRL
 */

void wxRichTextFontPage::OnUnderliningCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    UpdatePreview();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTFONTPAGE_WEIGHTCTRL
 */

void wxRichTextFontPage::OnWeightCtrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    UpdatePreview();
}

void wxRichTextFontPage::OnColourClicked( wxCommandEvent& WXUNUSED(event) )
{
    m_colourPresent = true;

    UpdatePreview();
}
