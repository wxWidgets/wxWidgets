/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbulletspage.cpp
// Purpose:
// Author:      Julian Smart
// Modified by:
// Created:     10/4/2006 10:32:31 AM
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextbulletspage.h"

#include "wx/richtext/richtextsymboldlg.h"

/*!
 * wxRichTextBulletsPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextBulletsPage, wxPanel )

/*!
 * wxRichTextBulletsPage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextBulletsPage, wxPanel )

////@begin wxRichTextBulletsPage event table entries
    EVT_LISTBOX( ID_RICHTEXTBULLETSPAGE_STYLELISTBOX, wxRichTextBulletsPage::OnStylelistboxSelected )

    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_SYMBOLSTATIC, wxRichTextBulletsPage::OnSymbolstaticUpdate )

    EVT_COMBOBOX( ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL, wxRichTextBulletsPage::OnSymbolctrlSelected )
    EVT_TEXT( ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL, wxRichTextBulletsPage::OnSymbolctrlUpdated )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL, wxRichTextBulletsPage::OnSymbolctrlUpdate )

    EVT_BUTTON( ID_RICHTEXTBULLETSPAGE_CHOOSE_SYMBOL, wxRichTextBulletsPage::OnChooseSymbolClick )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_CHOOSE_SYMBOL, wxRichTextBulletsPage::OnChooseSymbolUpdate )

    EVT_COMBOBOX( ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL, wxRichTextBulletsPage::OnSymbolfontctrlSelected )
    EVT_TEXT( ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL, wxRichTextBulletsPage::OnSymbolfontctrlUpdated )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL, wxRichTextBulletsPage::OnSymbolfontctrlUIUpdate )

    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_NUMBERSTATIC, wxRichTextBulletsPage::OnNumberstaticUpdate )

    EVT_SPINCTRL( ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, wxRichTextBulletsPage::OnNumberctrlUpdated )
    EVT_SPIN_UP( ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, wxRichTextBulletsPage::OnNumberctrlUp )
    EVT_SPIN_DOWN( ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, wxRichTextBulletsPage::OnNumberctrlDown )
    EVT_TEXT( ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, wxRichTextBulletsPage::OnNumberctrlTextUpdated )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, wxRichTextBulletsPage::OnNumberctrlUpdate )

    EVT_CHECKBOX( ID_RICHTEXTBULLETSPAGE_PARENTHESESCTRL, wxRichTextBulletsPage::OnParenthesesctrlClick )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_PARENTHESESCTRL, wxRichTextBulletsPage::OnParenthesesctrlUpdate )

    EVT_CHECKBOX( ID_RICHTEXTBULLETSPAGE_PERIODCTRL, wxRichTextBulletsPage::OnPeriodctrlClick )
    EVT_UPDATE_UI( ID_RICHTEXTBULLETSPAGE_PERIODCTRL, wxRichTextBulletsPage::OnPeriodctrlUpdate )

////@end wxRichTextBulletsPage event table entries

END_EVENT_TABLE()

/*!
 * wxRichTextBulletsPage constructors
 */

wxRichTextBulletsPage::wxRichTextBulletsPage( )
{
    Init();
}

wxRichTextBulletsPage::wxRichTextBulletsPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * Initialise members
 */

void wxRichTextBulletsPage::Init()
{
    m_hasBulletStyle = false;
    m_hasBulletNumber = false;
    m_hasBulletSymbol = false;
    m_dontUpdate = false;

////@begin wxRichTextBulletsPage member initialisation
    m_styleListBox = NULL;
    m_symbolCtrl = NULL;
    m_symbolFontCtrl = NULL;
    m_numberCtrl = NULL;
    m_parenthesesCtrl = NULL;
    m_periodCtrl = NULL;
    m_previewCtrl = NULL;
////@end wxRichTextBulletsPage member initialisation
}

/*!
 * wxRichTextBulletsPage creator
 */

bool wxRichTextBulletsPage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextBulletsPage creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextBulletsPage creation
    return true;
}

/*!
 * Control creation for wxRichTextBulletsPage
 */

void wxRichTextBulletsPage::CreateControls()
{
////@begin wxRichTextBulletsPage content construction
    wxRichTextBulletsPage* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Bullet style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_styleListBoxStrings = NULL;
    m_styleListBox = new wxListBox( itemPanel1, ID_RICHTEXTBULLETSPAGE_STYLELISTBOX, wxDefaultPosition, wxDefaultSize, 0, m_styleListBoxStrings, wxLB_SINGLE );
    m_styleListBox->SetHelpText(_("The available bullet styles."));
    if (ShowToolTips())
        m_styleListBox->SetToolTip(_("The available bullet styles."));
    itemBoxSizer5->Add(m_styleListBox, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine9 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer4->Add(itemStaticLine9, 0, wxGROW|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer11, 0, wxGROW, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, ID_RICHTEXTBULLETSPAGE_SYMBOLSTATIC, _("&Symbol:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer11->Add(itemBoxSizer13, 0, wxGROW, 5);

    wxString* m_symbolCtrlStrings = NULL;
    m_symbolCtrl = new wxComboBox( itemPanel1, ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL, _T(""), wxDefaultPosition, wxSize(60, -1), 0, m_symbolCtrlStrings, wxCB_DROPDOWN );
    m_symbolCtrl->SetHelpText(_("The bullet character."));
    if (ShowToolTips())
        m_symbolCtrl->SetToolTip(_("The bullet character."));
    itemBoxSizer13->Add(m_symbolCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 5);

    wxButton* itemButton15 = new wxButton( itemPanel1, ID_RICHTEXTBULLETSPAGE_CHOOSE_SYMBOL, _("Ch&oose..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton15->SetHelpText(_("Click to browse for a symbol."));
    if (ShowToolTips())
        itemButton15->SetToolTip(_("Click to browse for a symbol."));
    itemBoxSizer13->Add(itemButton15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer11->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel1, ID_RICHTEXTBULLETSPAGE_SYMBOLSTATIC, _("Symbol &font:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText17, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_symbolFontCtrlStrings = NULL;
    m_symbolFontCtrl = new wxComboBox( itemPanel1, ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0, m_symbolFontCtrlStrings, wxCB_DROPDOWN );
    m_symbolFontCtrl->SetHelpText(_("Available fonts."));
    if (ShowToolTips())
        m_symbolFontCtrl->SetToolTip(_("Available fonts."));
    itemBoxSizer11->Add(m_symbolFontCtrl, 0, wxGROW|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine20 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer4->Add(itemStaticLine20, 0, wxGROW|wxALL, 5);

    itemBoxSizer4->Add(2, 1, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer22, 0, wxGROW, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel1, ID_RICHTEXTBULLETSPAGE_NUMBERSTATIC, _("&Number:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemStaticText23, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_numberCtrl = new wxSpinCtrl( itemPanel1, ID_RICHTEXTBULLETSPAGE_NUMBERCTRL, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100000, 0 );
    m_numberCtrl->SetHelpText(_("The list item number."));
    if (ShowToolTips())
        m_numberCtrl->SetToolTip(_("The list item number."));
    itemBoxSizer22->Add(m_numberCtrl, 0, wxGROW|wxALL, 5);

    itemBoxSizer22->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_parenthesesCtrl = new wxCheckBox( itemPanel1, ID_RICHTEXTBULLETSPAGE_PARENTHESESCTRL, _("&Parentheses"), wxDefaultPosition, wxDefaultSize, 0 );
    m_parenthesesCtrl->SetValue(false);
    m_parenthesesCtrl->SetHelpText(_("Check to enclose the bullet in parentheses."));
    if (ShowToolTips())
        m_parenthesesCtrl->SetToolTip(_("Check to enclose the bullet in parentheses."));
    itemBoxSizer22->Add(m_parenthesesCtrl, 0, wxALIGN_LEFT|wxALL, 5);

    m_periodCtrl = new wxCheckBox( itemPanel1, ID_RICHTEXTBULLETSPAGE_PERIODCTRL, _("Peri&od"), wxDefaultPosition, wxDefaultSize, 0 );
    m_periodCtrl->SetValue(false);
    m_periodCtrl->SetHelpText(_("Check to add a period after the bullet."));
    if (ShowToolTips())
        m_periodCtrl->SetToolTip(_("Check to add a period after the bullet."));
    itemBoxSizer22->Add(m_periodCtrl, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_previewCtrl = new wxRichTextCtrl( itemPanel1, ID_RICHTEXTBULLETSPAGE_PREVIEW_CTRL, wxEmptyString, wxDefaultPosition, wxSize(350, 180), wxSUNKEN_BORDER );
    m_previewCtrl->SetHelpText(_("Shows a preview of the bullet settings."));
    if (ShowToolTips())
        m_previewCtrl->SetToolTip(_("Shows a preview of the bullet settings."));
    itemBoxSizer3->Add(m_previewCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxRichTextBulletsPage content construction

    m_styleListBox->Append(_("(None)"));
    m_styleListBox->Append(_("Arabic"));
    m_styleListBox->Append(_("Upper case letters"));
    m_styleListBox->Append(_("Lower case letters"));
    m_styleListBox->Append(_("Upper case roman numerals"));
    m_styleListBox->Append(_("Lower case roman numerals"));
    m_styleListBox->Append(_("Symbol"));
    m_styleListBox->Append(_("Bitmap"));

    m_symbolCtrl->Append(_("*"));
    m_symbolCtrl->Append(_("-"));
    m_symbolCtrl->Append(_(">"));
    m_symbolCtrl->Append(_("+"));
    m_symbolCtrl->Append(_("~"));

    wxArrayString facenames = wxRichTextCtrl::GetAvailableFontNames();
    facenames.Sort();

    m_symbolFontCtrl->Append(facenames);
}

/// Transfer data from/to window
bool wxRichTextBulletsPage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

    wxTextAttrEx* attr = GetAttributes();

    if (m_hasBulletStyle)
    {
        long bulletStyle = 0;

        int index = m_styleListBox->GetSelection();
        if (index == 1)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_ARABIC;
        else if (index == 2)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER;
        else if (index == 3)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER;
        else if (index == 4)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER;
        else if (index == 5)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER;
        else if (index == 6)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_SYMBOL;
        else if (index == 7)
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_BITMAP;

        if (m_parenthesesCtrl->GetValue())
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        if (m_periodCtrl->GetValue())
            bulletStyle |= wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        attr->SetBulletStyle(bulletStyle);
    }

    if (m_hasBulletNumber)
    {
        attr->SetBulletNumber(m_numberCtrl->GetValue());
    }

    if (m_hasBulletSymbol)
    {
        wxChar c(wxT('*'));
        if (m_symbolCtrl->GetValue().length() > 0)
            c = m_symbolCtrl->GetValue()[0];
        attr->SetBulletSymbol(c);
        attr->SetBulletFont(m_symbolFontCtrl->GetValue());
    }
    return true;
}

bool wxRichTextBulletsPage::TransferDataToWindow()
{
    m_dontUpdate = true;

    wxPanel::TransferDataToWindow();

    wxTextAttrEx* attr = GetAttributes();

    if (attr->HasBulletStyle())
    {
        int index = -1;
        if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC)
            index = 1;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER)
            index = 2;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER)
            index = 3;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER)
            index = 4;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)
            index = 5;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL)
            index = 6;
        else if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP)
            index = 7;
        m_styleListBox->SetSelection(index);

        if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PARENTHESES)
            m_parenthesesCtrl->SetValue(true);
        else
            m_parenthesesCtrl->SetValue(false);

        if (attr->GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PERIOD)
            m_periodCtrl->SetValue(true);
        else
            m_periodCtrl->SetValue(false);
    }
    else
        m_styleListBox->SetSelection(-1);

    if (attr->HasBulletSymbol())
    {
        wxChar c = attr->GetBulletSymbol();
        wxString s;
        s << c;
        m_symbolCtrl->SetValue(s);
        m_symbolFontCtrl->SetValue(attr->GetBulletFont());
    }
    else
        m_symbolCtrl->SetValue(wxEmptyString);

    if (attr->HasBulletNumber())
        m_numberCtrl->SetValue(attr->GetBulletNumber());
    else
        m_numberCtrl->SetValue(0);

    UpdatePreview();

    m_dontUpdate = false;

    return true;
}

/// Updates the bullet preview
void wxRichTextBulletsPage::UpdatePreview()
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
      (wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_SYMBOL|
       wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|
       wxTEXT_ATTR_LINE_SPACING));

    wxFont font(m_previewCtrl->GetFont());
    font.SetPointSize(9);
    m_previewCtrl->SetFont(font);

    wxTextAttrEx normalParaAttr;
    normalParaAttr.SetFont(font);
    normalParaAttr.SetTextColour(wxColour(wxT("LIGHT GREY")));

    m_previewCtrl->Freeze();
    m_previewCtrl->Clear();

    m_previewCtrl->BeginStyle(normalParaAttr);
    m_previewCtrl->WriteText(s_para1);
    m_previewCtrl->EndStyle();
    
    m_previewCtrl->BeginStyle(attr);
    m_previewCtrl->WriteText(s_para2);
    m_previewCtrl->EndStyle();

    m_previewCtrl->BeginStyle(normalParaAttr);
    m_previewCtrl->WriteText(s_para3);
    m_previewCtrl->EndStyle();

    m_previewCtrl->Thaw();
}

wxTextAttrEx* wxRichTextBulletsPage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextBulletsPage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextBulletsPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextBulletsPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextBulletsPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextBulletsPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextBulletsPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextBulletsPage icon retrieval
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_RICHTEXTBULLETSPAGE_STYLELISTBOX
 */

void wxRichTextBulletsPage::OnStylelistboxSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletStyle = true;

        if (m_styleListBox->GetSelection() == 6)
            m_hasBulletSymbol = true;

        UpdatePreview();
    }
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL
 */

void wxRichTextBulletsPage::OnSymbolctrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletSymbol = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL
 */

void wxRichTextBulletsPage::OnSymbolctrlUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletSymbol = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLCTRL
 */

void wxRichTextBulletsPage::OnSymbolctrlUpdate( wxUpdateUIEvent& event )
{
    OnSymbolUpdate(event);
}

/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_RICHTEXTBULLETSPAGE_NUMBERCTRL
 */

void wxRichTextBulletsPage::OnNumberctrlUpdated( wxSpinEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletNumber = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_SCROLL_LINEUP event handler for ID_RICHTEXTBULLETSPAGE_NUMBERCTRL
 */

void wxRichTextBulletsPage::OnNumberctrlUp( wxSpinEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletNumber = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_SCROLL_LINEDOWN event handler for ID_RICHTEXTBULLETSPAGE_NUMBERCTRL
 */

void wxRichTextBulletsPage::OnNumberctrlDown( wxSpinEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletNumber = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTBULLETSPAGE_NUMBERCTRL
 */

void wxRichTextBulletsPage::OnNumberctrlTextUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletNumber = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_NUMBERCTRL
 */

void wxRichTextBulletsPage::OnNumberctrlUpdate( wxUpdateUIEvent& event )
{
    OnNumberUpdate(event);
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RICHTEXTBULLETSPAGE_PARENTHESESCTRL
 */

void wxRichTextBulletsPage::OnParenthesesctrlClick( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletStyle = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_PARENTHESESCTRL
 */

void wxRichTextBulletsPage::OnParenthesesctrlUpdate( wxUpdateUIEvent& event )
{
    int sel = m_styleListBox->GetSelection();
    event.Enable(m_hasBulletStyle && (sel != 6 && sel != 7 && sel != 0));
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RICHTEXTBULLETSPAGE_PERIODCTRL
 */

void wxRichTextBulletsPage::OnPeriodctrlClick( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_dontUpdate)
    {
        m_hasBulletStyle = true;
        UpdatePreview();
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_PERIODCTRL
 */

void wxRichTextBulletsPage::OnPeriodctrlUpdate( wxUpdateUIEvent& event )
{
    int sel = m_styleListBox->GetSelection();
    event.Enable(m_hasBulletStyle && (sel != 6 && sel != 7 && sel != 0));
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXTBULLETSPAGE_CHOOSE_SYMBOL
 */

void wxRichTextBulletsPage::OnChooseSymbolClick( wxCommandEvent& WXUNUSED(event) )
{
    int sel = m_styleListBox->GetSelection();
    if (m_hasBulletStyle && sel == 6)
    {
        wxString symbol = m_symbolCtrl->GetValue();
        wxString fontName = m_symbolFontCtrl->GetValue();
        wxSymbolPickerDialog dlg(symbol, fontName, fontName, this);

        if (dlg.ShowModal() == wxID_OK)
        {
            m_dontUpdate = true;

            m_symbolCtrl->SetValue(dlg.GetSymbol());
            m_symbolFontCtrl->SetValue(dlg.GetFontName());

            UpdatePreview();

            m_dontUpdate = false;
        }
    }
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_CHOOSE_SYMBOL
 */

void wxRichTextBulletsPage::OnChooseSymbolUpdate( wxUpdateUIEvent& event )
{
    OnSymbolUpdate(event);
}
/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL
 */

void wxRichTextBulletsPage::OnSymbolfontctrlSelected( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;
    UpdatePreview();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL
 */

void wxRichTextBulletsPage::OnSymbolfontctrlUpdated( wxCommandEvent& WXUNUSED(event) )
{
    if (m_dontUpdate)
        return;
    UpdatePreview();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLFONTCTRL
 */

void wxRichTextBulletsPage::OnSymbolfontctrlUIUpdate( wxUpdateUIEvent& event )
{
    OnSymbolUpdate(event);
}

/// Update for symbol-related controls
void wxRichTextBulletsPage::OnSymbolUpdate( wxUpdateUIEvent& event )
{
    int sel = m_styleListBox->GetSelection();
    event.Enable(m_hasBulletStyle && (sel == 6));
}

/// Update for number-related controls
void wxRichTextBulletsPage::OnNumberUpdate( wxUpdateUIEvent& event )
{
    int sel = m_styleListBox->GetSelection();
    event.Enable( m_hasBulletStyle && (sel != 6 && sel != 7 && sel != 0));
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_SYMBOLSTATIC
 */

void wxRichTextBulletsPage::OnSymbolstaticUpdate( wxUpdateUIEvent& event )
{
    OnSymbolUpdate(event);
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTBULLETSPAGE_NUMBERSTATIC
 */

void wxRichTextBulletsPage::OnNumberstaticUpdate( wxUpdateUIEvent& event )
{
    OnNumberUpdate(event);
}

#endif // wxUSE_RICHTEXT
