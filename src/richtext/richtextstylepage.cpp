/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextstylepage.cpp
// Purpose:     Implements the rich text formatting dialog style name tab.
// Author:      Julian Smart
// Modified by:
// Created:     10/5/2006 11:34:55 AM
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextstylepage.h"

/*!
 * wxRichTextStylePage type definition
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextStylePage, wxRichTextDialogPage);

/*!
 * wxRichTextStylePage event table definition
 */

wxBEGIN_EVENT_TABLE(wxRichTextStylePage, wxRichTextDialogPage)

////@begin wxRichTextStylePage event table entries
    EVT_UPDATE_UI( ID_RICHTEXTSTYLEPAGE_NEXT_STYLE, wxRichTextStylePage::OnNextStyleUpdate )

////@end wxRichTextStylePage event table entries

wxEND_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextStylePage)

/*!
 * wxRichTextStylePage constructors
 */

wxRichTextStylePage::wxRichTextStylePage( )
{
    Init();
}

wxRichTextStylePage::wxRichTextStylePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * Initialise members
 */

void wxRichTextStylePage::Init()
{
    m_nameIsEditable = false;
    
////@begin wxRichTextStylePage member initialisation
    m_styleName = NULL;
    m_basedOn = NULL;
    m_nextStyle = NULL;
////@end wxRichTextStylePage member initialisation
}

/*!
 * wxRichTextStylePage creator
 */

bool wxRichTextStylePage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextStylePage creation
    wxRichTextDialogPage::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextStylePage creation
    return true;
}

/*!
 * Control creation for wxRichTextStylePage
 */

void wxRichTextStylePage::CreateControls()
{
////@begin wxRichTextStylePage content construction
    wxRichTextStylePage* itemRichTextDialogPage1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemRichTextDialogPage1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 1, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_styleName = new wxTextCtrl( itemRichTextDialogPage1, ID_RICHTEXTSTYLEPAGE_STYLE_NAME, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxTE_READONLY );
    m_styleName->SetHelpText(_("The style name."));
    if (wxRichTextStylePage::ShowToolTips())
        m_styleName->SetToolTip(_("The style name."));
    itemBoxSizer5->Add(m_styleName, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Based on:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText8, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_basedOnStrings;
    m_basedOn = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXTSTYLEPAGE_BASED_ON, wxEmptyString, wxDefaultPosition, wxSize(300, -1), m_basedOnStrings, wxCB_DROPDOWN );
    m_basedOn->SetHelpText(_("The style on which this style is based."));
    if (wxRichTextStylePage::ShowToolTips())
        m_basedOn->SetToolTip(_("The style on which this style is based."));
    itemBoxSizer5->Add(m_basedOn, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemRichTextDialogPage1, wxID_STATIC, _("&Next style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText10, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_nextStyleStrings;
    m_nextStyle = new wxComboBox( itemRichTextDialogPage1, ID_RICHTEXTSTYLEPAGE_NEXT_STYLE, wxEmptyString, wxDefaultPosition, wxSize(300, -1), m_nextStyleStrings, wxCB_DROPDOWN );
    m_nextStyle->SetHelpText(_("The default style for the next paragraph."));
    if (wxRichTextStylePage::ShowToolTips())
        m_nextStyle->SetToolTip(_("The default style for the next paragraph."));
    itemBoxSizer5->Add(m_nextStyle, 0, wxGROW|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxRichTextStylePage content construction
}

/// Transfer data from/to window
bool wxRichTextStylePage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    if (def)
    {
        wxRichTextParagraphStyleDefinition* paraDef = wxDynamicCast(def, wxRichTextParagraphStyleDefinition);
        if (paraDef)
            paraDef->SetNextStyle(m_nextStyle->GetValue());

        def->SetName(m_styleName->GetValue());
        def->SetBaseStyle(m_basedOn->GetValue());
    }

    return true;
}

bool wxRichTextStylePage::TransferDataToWindow()
{
    m_styleName->SetEditable(GetNameIsEditable());

    wxPanel::TransferDataToWindow();

    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    if (def)
    {
        m_basedOn->Freeze();
        m_nextStyle->Freeze();

        wxRichTextParagraphStyleDefinition* paraDef = wxDynamicCast(def, wxRichTextParagraphStyleDefinition);
        wxRichTextListStyleDefinition* listDef = wxDynamicCast(def, wxRichTextListStyleDefinition);
        wxRichTextCharacterStyleDefinition* charDef = wxDynamicCast(def, wxRichTextCharacterStyleDefinition);
        wxRichTextStyleSheet* sheet = wxRichTextFormattingDialog::GetDialog(this)->GetStyleSheet();
        wxRichTextBoxStyleDefinition* boxDef = wxDynamicCast(def, wxRichTextBoxStyleDefinition);

        m_styleName->SetValue(def->GetName());

        if (listDef)
        {
            if (m_nextStyle->GetCount() == 0)
            {
                if (sheet)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetListStyleCount(); i++)
                    {
                        wxRichTextListStyleDefinition* p = wxDynamicCast(sheet->GetListStyle(i), wxRichTextListStyleDefinition);
                        if (p)
                            m_nextStyle->Append(p->GetName());
                    }
                }
            }
            m_nextStyle->SetValue(listDef->GetNextStyle());
        }
        else if (paraDef)
        {
            if (m_nextStyle->GetCount() == 0)
            {
                if (sheet)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetParagraphStyleCount(); i++)
                    {
                        wxRichTextParagraphStyleDefinition* p = wxDynamicCast(sheet->GetParagraphStyle(i), wxRichTextParagraphStyleDefinition);
                        if (p)
                            m_nextStyle->Append(p->GetName());
                    }
                }
            }
            m_nextStyle->SetValue(paraDef->GetNextStyle());
        }

        if (m_basedOn->GetCount() == 0)
        {
            if (sheet)
            {
                if (listDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetListStyleCount(); i++)
                    {
                        wxRichTextListStyleDefinition* p = wxDynamicCast(sheet->GetListStyle(i), wxRichTextListStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
                else if (paraDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetParagraphStyleCount(); i++)
                    {
                        wxRichTextParagraphStyleDefinition* p = wxDynamicCast(sheet->GetParagraphStyle(i), wxRichTextParagraphStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
                else if (boxDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetBoxStyleCount(); i++)
                    {
                        wxRichTextBoxStyleDefinition* p = wxDynamicCast(sheet->GetBoxStyle(i), wxRichTextBoxStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
                else if (charDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetCharacterStyleCount(); i++)
                    {
                        wxRichTextCharacterStyleDefinition* p = wxDynamicCast(sheet->GetCharacterStyle(i), wxRichTextCharacterStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
            }
        }

        m_basedOn->SetValue(def->GetBaseStyle());

        m_nextStyle->Thaw();
        m_basedOn->Thaw();
    }

    return true;
}

wxRichTextAttr* wxRichTextStylePage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextStylePage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextStylePage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextStylePage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextStylePage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextStylePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextStylePage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextStylePage icon retrieval
}
/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTSTYLEPAGE_NEXT_STYLE
 */

void wxRichTextStylePage::OnNextStyleUpdate( wxUpdateUIEvent& event )
{
    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    event.Enable(wxDynamicCast(def, wxRichTextParagraphStyleDefinition) != NULL);
}

#endif // wxUSE_RICHTEXT
