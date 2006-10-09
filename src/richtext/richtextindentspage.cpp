/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextindentspage.cpp
// Purpose:
// Author:      Julian Smart
// Modified by:
// Created:     10/3/2006 2:28:21 PM
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextindentspage.h"

#ifndef WX_PRECOMP
#endif

/*!
 * wxRichTextIndentsSpacingPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextIndentsSpacingPage, wxPanel )

/*!
 * wxRichTextIndentsSpacingPage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextIndentsSpacingPage, wxPanel )

////@begin wxRichTextIndentsSpacingPage event table entries
    EVT_RADIOBUTTON( ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_LEFT, wxRichTextIndentsSpacingPage::OnAlignmentLeftSelected )

    EVT_RADIOBUTTON( ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_RIGHT, wxRichTextIndentsSpacingPage::OnAlignmentRightSelected )

    EVT_RADIOBUTTON( ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_JUSTIFIED, wxRichTextIndentsSpacingPage::OnAlignmentJustifiedSelected )

    EVT_RADIOBUTTON( ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_CENTRED, wxRichTextIndentsSpacingPage::OnAlignmentCentredSelected )

    EVT_RADIOBUTTON( ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_INDETERMINATE, wxRichTextIndentsSpacingPage::OnAlignmentIndeterminateSelected )

    EVT_TEXT( ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT, wxRichTextIndentsSpacingPage::OnIndentLeftUpdated )

    EVT_TEXT( ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT_FIRST, wxRichTextIndentsSpacingPage::OnIndentLeftFirstUpdated )

    EVT_TEXT( ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_RIGHT, wxRichTextIndentsSpacingPage::OnIndentRightUpdated )

    EVT_TEXT( ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_BEFORE, wxRichTextIndentsSpacingPage::OnSpacingBeforeUpdated )

    EVT_TEXT( ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_AFTER, wxRichTextIndentsSpacingPage::OnSpacingAfterUpdated )

    EVT_COMBOBOX( ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_LINE, wxRichTextIndentsSpacingPage::OnSpacingLineSelected )

////@end wxRichTextIndentsSpacingPage event table entries

END_EVENT_TABLE()

/*!
 * wxRichTextIndentsSpacingPage constructors
 */

wxRichTextIndentsSpacingPage::wxRichTextIndentsSpacingPage( )
{
    Init();
}

wxRichTextIndentsSpacingPage::wxRichTextIndentsSpacingPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * Initialise members
 */
 
void wxRichTextIndentsSpacingPage::Init()
{
    m_dontUpdate = false;

////@begin wxRichTextIndentsSpacingPage member initialisation
    m_alignmentLeft = NULL;
    m_alignmentRight = NULL;
    m_alignmentJustified = NULL;
    m_alignmentCentred = NULL;
    m_alignmentIndeterminate = NULL;
    m_indentLeft = NULL;
    m_indentLeftFirst = NULL;
    m_indentRight = NULL;
    m_spacingBefore = NULL;
    m_spacingAfter = NULL;
    m_spacingLine = NULL;
    m_previewCtrl = NULL;
////@end wxRichTextIndentsSpacingPage member initialisation

}

/*!
 * wxRichTextIndentsSpacingPage creator
 */

bool wxRichTextIndentsSpacingPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextIndentsSpacingPage creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextIndentsSpacingPage creation
    return true;
}

/*!
 * Control creation for wxRichTextIndentsSpacingPage
 */

void wxRichTextIndentsSpacingPage::CreateControls()
{
////@begin wxRichTextIndentsSpacingPage content construction
    wxRichTextIndentsSpacingPage* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Alignment"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemBoxSizer7->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer7->Add(itemBoxSizer9, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 5);

    m_alignmentLeft = new wxRadioButton( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_LEFT, _("&Left"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_alignmentLeft->SetValue(false);
    itemBoxSizer9->Add(m_alignmentLeft, 0, wxALIGN_LEFT|wxALL, 5);

    m_alignmentRight = new wxRadioButton( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_RIGHT, _("&Right"), wxDefaultPosition, wxDefaultSize, 0 );
    m_alignmentRight->SetValue(false);
    itemBoxSizer9->Add(m_alignmentRight, 0, wxALIGN_LEFT|wxALL, 5);

    m_alignmentJustified = new wxRadioButton( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_JUSTIFIED, _("&Justified"), wxDefaultPosition, wxDefaultSize, 0 );
    m_alignmentJustified->SetValue(false);
    itemBoxSizer9->Add(m_alignmentJustified, 0, wxALIGN_LEFT|wxALL, 5);

    m_alignmentCentred = new wxRadioButton( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_CENTRED, _("Cen&tred"), wxDefaultPosition, wxDefaultSize, 0 );
    m_alignmentCentred->SetValue(false);
    itemBoxSizer9->Add(m_alignmentCentred, 0, wxALIGN_LEFT|wxALL, 5);

    m_alignmentIndeterminate = new wxRadioButton( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_INDETERMINATE, _("&Indeterminate"), wxDefaultPosition, wxDefaultSize, 0 );
    m_alignmentIndeterminate->SetValue(false);
    itemBoxSizer9->Add(m_alignmentIndeterminate, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine16 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer4->Add(itemStaticLine16, 0, wxGROW|wxLEFT|wxBOTTOM, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer18, 0, wxGROW, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Indentation (tenths of a mm)"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer18->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer20->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxFlexGridSizer* itemFlexGridSizer22 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer20->Add(itemFlexGridSizer22, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Left:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(itemStaticText23, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer24, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_indentLeft = new wxTextCtrl( itemPanel1,
                                   ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT,
                                   wxEmptyString,
                                   wxDefaultPosition,
                                   wxSize(50, wxDefaultCoord),
                                   0 );
    itemBoxSizer24->Add(m_indentLeft, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel1, wxID_STATIC, _("Left (&first line):"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(itemStaticText26, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer27, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_indentLeftFirst = new wxTextCtrl( itemPanel1,
                                        ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT_FIRST,
                                        wxEmptyString,
                                        wxDefaultPosition,
                                        wxSize(50, wxDefaultCoord),
                                        0 );
    itemBoxSizer27->Add(m_indentLeftFirst, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Right:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(itemStaticText29, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer30 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer22->Add(itemBoxSizer30, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_indentRight = new wxTextCtrl( itemPanel1,
                                    ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_RIGHT,
                                    wxEmptyString,
                                    wxDefaultPosition,
                                    wxSize(50, wxDefaultCoord),
                                    0 );
    itemBoxSizer30->Add(m_indentRight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine33 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer4->Add(itemStaticLine33, 0, wxGROW|wxTOP|wxBOTTOM, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer35, 0, wxGROW, 5);

    wxStaticText* itemStaticText36 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Spacing (tenths of a mm)"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer35->Add(itemStaticText36, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer37, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer37->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxFlexGridSizer* itemFlexGridSizer39 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer39, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText40 = new wxStaticText( itemPanel1, wxID_STATIC, _("Before a paragraph:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(itemStaticText40, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer41 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer39->Add(itemBoxSizer41, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_spacingBefore = new wxTextCtrl( itemPanel1,
                                      ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_BEFORE,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(50, wxDefaultCoord),
                                      0 );
    itemBoxSizer41->Add(m_spacingBefore, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText43 = new wxStaticText( itemPanel1, wxID_STATIC, _("After a paragraph:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(itemStaticText43, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer39->Add(itemBoxSizer44, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_spacingAfter = new wxTextCtrl( itemPanel1,
                                     ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_AFTER,
                                     wxEmptyString,
                                     wxDefaultPosition,
                                     wxSize(50, wxDefaultCoord),
                                     0 );
    itemBoxSizer44->Add(m_spacingAfter, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText46 = new wxStaticText( itemPanel1, wxID_STATIC, _("Line spacing:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(itemStaticText46, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer47 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer39->Add(itemBoxSizer47, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxString m_spacingLineStrings[] = {
        _("Single"),
        _("1.5"),
        _("2")
    };
    m_spacingLine = new wxComboBox( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_LINE, _("Single"), wxDefaultPosition, wxDefaultSize, 3, m_spacingLineStrings, wxCB_READONLY );
    m_spacingLine->SetStringSelection(_("Single"));
    itemBoxSizer47->Add(m_spacingLine, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_previewCtrl = new wxRichTextCtrl( itemPanel1, ID_RICHTEXTINDENTSSPACINGPAGE_PREVIEW_CTRL, wxEmptyString, wxDefaultPosition, wxSize(350, 180), wxSUNKEN_BORDER );
    m_previewCtrl->SetHelpText(_("Shows a preview of the paragraph settings."));
    if (ShowToolTips())
        m_previewCtrl->SetToolTip(_("Shows a preview of the paragraph settings."));
    itemBoxSizer3->Add(m_previewCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxRichTextIndentsSpacingPage content construction
}

wxTextAttrEx* wxRichTextIndentsSpacingPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/// Updates the font preview
void wxRichTextIndentsSpacingPage::UpdatePreview()
{
    static const wxChar* s_para1 = wxT("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. \
Nullam ante sapien, vestibulum nonummy, pulvinar sed, luctus ut, lacus.");

    static const wxChar* s_para2 = wxT("\nDuis pharetra consequat dui. Cum sociis natoque penatibus \
et magnis dis parturient montes, nascetur ridiculus mus. Nullam vitae justo id mauris lobortis interdum.");

    static const wxChar* s_para3 = wxT("\nInteger convallis dolor at augue \
iaculis malesuada. Donec bibendum ipsum ut ante porta fringilla.\n");

    TransferDataFromWindow();
    wxTextAttrEx attr(*GetAttributes());
    attr.SetFlags(attr.GetFlags() &
      (wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|
       wxTEXT_ATTR_LINE_SPACING|
       wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_SYMBOL));

    wxFont font(m_previewCtrl->GetFont());
    font.SetPointSize(9);
    m_previewCtrl->SetFont(font);

    font.SetWeight(wxBOLD);
    attr.SetFont(font);

    m_previewCtrl->Clear();

    m_previewCtrl->WriteText(s_para1);
    m_previewCtrl->BeginStyle(attr);
    m_previewCtrl->WriteText(s_para2);
    m_previewCtrl->EndStyle();
    m_previewCtrl->WriteText(s_para3);
}

/// Transfer data from/to window
bool wxRichTextIndentsSpacingPage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

    wxTextAttrEx* attr = GetAttributes();

    if (m_alignmentLeft->GetValue())
        attr->SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    else if (m_alignmentCentred->GetValue())
        attr->SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    else if (m_alignmentRight->GetValue())
        attr->SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
    else if (m_alignmentJustified->GetValue())
        attr->SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
    else
    {
        attr->SetAlignment(wxTEXT_ALIGNMENT_DEFAULT);
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_ALIGNMENT));
    }

    wxString leftIndent(m_indentLeft->GetValue());
    wxString leftFirstIndent(m_indentLeftFirst->GetValue());
    if (!leftIndent.empty())
    {
        int visualLeftIndent = wxAtoi(leftIndent);
        int visualLeftFirstIndent = wxAtoi(leftFirstIndent);
        int actualLeftIndent = visualLeftFirstIndent;
        int actualLeftSubIndent = visualLeftIndent - visualLeftFirstIndent;

        attr->SetLeftIndent(actualLeftIndent, actualLeftSubIndent);
    }
    else
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_LEFT_INDENT));

    wxString rightIndent(m_indentRight->GetValue());
    if (!rightIndent.empty())
        attr->SetRightIndent(wxAtoi(rightIndent));
    else
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_RIGHT_INDENT));

    wxString spacingAfter(m_spacingAfter->GetValue());
    if (!spacingAfter.empty())
        attr->SetParagraphSpacingAfter(wxAtoi(spacingAfter));
    else
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_PARA_SPACING_AFTER));

    wxString spacingBefore(m_spacingBefore->GetValue());
    if (!spacingBefore.empty())
        attr->SetParagraphSpacingBefore(wxAtoi(spacingBefore));
    else
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_PARA_SPACING_BEFORE));

    int spacingIndex = m_spacingLine->GetSelection();
    int lineSpacing = 0;
    if (spacingIndex == 0)
        lineSpacing = 10;
    else if (spacingIndex == 1)
        lineSpacing = 15;
    else if (spacingIndex == 2)
        lineSpacing = 20;

    if (lineSpacing == 0)
        attr->SetFlags(attr->GetFlags() & (~wxTEXT_ATTR_LINE_SPACING));
    else
        attr->SetLineSpacing(lineSpacing);

    return true;
}

bool wxRichTextIndentsSpacingPage::TransferDataToWindow()
{
    m_dontUpdate = true;

    wxPanel::TransferDataToWindow();

    wxTextAttrEx* attr = GetAttributes();

    if (attr->HasAlignment())
    {
        if (attr->GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
            m_alignmentLeft->SetValue(true);
        else if (attr->GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
            m_alignmentRight->SetValue(true);
        else if (attr->GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
            m_alignmentCentred->SetValue(true);
        else if (attr->GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
            m_alignmentJustified->SetValue(true);
        else
            m_alignmentIndeterminate->SetValue(true);
    }
    else
        m_alignmentIndeterminate->SetValue(true);

    if (attr->HasLeftIndent())
    {
        wxString leftIndent(wxString::Format(wxT("%ld"), attr->GetLeftIndent() + attr->GetLeftSubIndent()));
        wxString leftFirstIndent(wxString::Format(wxT("%ld"), attr->GetLeftIndent()));

        m_indentLeft->SetValue(leftIndent);
        m_indentLeftFirst->SetValue(leftFirstIndent);
    }
    else
    {
        m_indentLeft->SetValue(wxEmptyString);
        m_indentLeftFirst->SetValue(wxEmptyString);
    }

    if (attr->HasRightIndent())
    {
        wxString rightIndent(wxString::Format(wxT("%ld"), attr->GetRightIndent()));

        m_indentRight->SetValue(rightIndent);
    }
    else
        m_indentRight->SetValue(wxEmptyString);

    if (attr->HasParagraphSpacingAfter())
    {
        wxString spacingAfter(wxString::Format(wxT("%d"), attr->GetParagraphSpacingAfter()));

        m_spacingAfter->SetValue(spacingAfter);
    }
    else
        m_spacingAfter->SetValue(wxEmptyString);

    if (attr->HasParagraphSpacingBefore())
    {
        wxString spacingBefore(wxString::Format(wxT("%d"), attr->GetParagraphSpacingBefore()));

        m_spacingBefore->SetValue(spacingBefore);
    }
    else
        m_spacingBefore->SetValue(wxEmptyString);

    if (attr->HasLineSpacing())
    {
        int index = 0;

        int lineSpacing = attr->GetLineSpacing();
        if (lineSpacing == 10)
            index = 0;
        else if (lineSpacing == 15)
            index = 1;
        else if (lineSpacing == 20)
            index = 2;
        else
            index = -1;

        m_spacingLine->SetSelection(index);
    }
    else
        m_spacingLine->SetSelection(-1);

    UpdatePreview();

    m_dontUpdate = false;

    return true;
}


/*!
 * Should we show tooltips?
 */

bool wxRichTextIndentsSpacingPage::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextIndentsSpacingPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextIndentsSpacingPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextIndentsSpacingPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextIndentsSpacingPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextIndentsSpacingPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextIndentsSpacingPage icon retrieval
}
/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_LEFT
 */

void wxRichTextIndentsSpacingPage::OnAlignmentLeftSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_RIGHT
 */

void wxRichTextIndentsSpacingPage::OnAlignmentRightSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_JUSTIFIED
 */

void wxRichTextIndentsSpacingPage::OnAlignmentJustifiedSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_CENTRED
 */

void wxRichTextIndentsSpacingPage::OnAlignmentCentredSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_ALIGNMENT_INDETERMINATE
 */

void wxRichTextIndentsSpacingPage::OnAlignmentIndeterminateSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT
 */

void wxRichTextIndentsSpacingPage::OnIndentLeftUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_LEFT_FIRST
 */

void wxRichTextIndentsSpacingPage::OnIndentLeftFirstUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_INDENT_RIGHT
 */

void wxRichTextIndentsSpacingPage::OnIndentRightUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_BEFORE
 */

void wxRichTextIndentsSpacingPage::OnSpacingBeforeUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_AFTER
 */

void wxRichTextIndentsSpacingPage::OnSpacingAfterUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTINDENTSSPACINGPAGE_SPACING_LINE
 */

void wxRichTextIndentsSpacingPage::OnSpacingLineSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
        UpdatePreview();
}

#endif // wxUSE_RICHTEXT
