/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextimagedlg.cpp
// Purpose:
// Author:      Mingquan Yang
// Modified by:
// Created:     Wed 02 Jun 2010 11:27:23 CST
// RCS-ID:
// Copyright:   (c) Mingquan Yang
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/combobox.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/statline.h"

#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextctrl.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextImageDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextImageDialog, wxDialog )


/*!
 * wxRichTextImageDialog event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextImageDialog, wxDialog )

////@begin wxRichTextImageDialog event table entries
    EVT_BUTTON( ID_RICHTEXTIMAGEDIALOG_PARA_UP, wxRichTextImageDialog::OnRichtextimagedialogParaUpClick )

    EVT_BUTTON( ID_RICHTEXTIMAGEDIALOG_DOWN, wxRichTextImageDialog::OnRichtextimagedialogDownClick )

////@end wxRichTextImageDialog event table entries

END_EVENT_TABLE()


/*!
 * wxRichTextImageDialog constructors
 */

wxRichTextImageDialog::wxRichTextImageDialog()
{
    Init();
}

wxRichTextImageDialog::wxRichTextImageDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * wxRichTextImageDlg creator
 */

bool wxRichTextImageDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextImageDialog creation
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextImageDialog creation
    return true;
}


/*!
 * wxRichTextImageDialog destructor
 */

wxRichTextImageDialog::~wxRichTextImageDialog()
{
////@begin wxRichTextImageDialog destruction
////@end wxRichTextImageDialog destruction
}


/*!
 * Member initialisation
 */

void wxRichTextImageDialog::Init()
{
////@begin wxRichTextImageDialog member initialisation
    m_alignment = NULL;
    m_float = NULL;
    m_width = NULL;
    m_unitsW = NULL;
    m_height = NULL;
    m_unitsH = NULL;
    m_offset = NULL;
    m_unitsOffset = NULL;
    m_saveButton = NULL;
    m_cancelButton = NULL;
////@end wxRichTextImageDialog member initialisation
}


/*!
 * Control creation for wxRichTextImageDlg
 */

void wxRichTextImageDialog::CreateControls()
{
#ifdef __WXMAC__
    SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

////@begin wxRichTextImageDialog content construction
    wxRichTextImageDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer3->Add(itemFlexGridSizer4, 0, wxALIGN_TOP|wxRIGHT, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->Show(false);
    itemFlexGridSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_alignmentStrings;
    m_alignmentStrings.Add(_("Left"));
    m_alignmentStrings.Add(_("Centre"));
    m_alignmentStrings.Add(_("Right"));
    m_alignment = new wxComboBox( itemDialog1, ID_COMBOBOX_ALIGN, _("Left"), wxDefaultPosition, wxSize(80, -1), m_alignmentStrings, wxCB_READONLY );
    m_alignment->SetStringSelection(_("Left"));
    m_alignment->Show(false);
    itemFlexGridSizer4->Add(m_alignment, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Floating mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_floatStrings;
    m_floatStrings.Add(_("None"));
    m_floatStrings.Add(_("Left"));
    m_floatStrings.Add(_("Right"));
    m_float = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_FLOATING_MODE, _("None"), wxDefaultPosition, wxSize(80, -1), m_floatStrings, wxCB_READONLY );
    m_float->SetStringSelection(_("None"));
    m_float->SetHelpText(_("How the image will float relative to the text."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_float->SetToolTip(_("How the image will float relative to the text."));
    itemFlexGridSizer4->Add(m_float, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_width = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_width->SetHelpText(_("The image width to be shown - does not change the source image width."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_width->SetToolTip(_("The image width to be shown - does not change the source image width."));
    itemBoxSizer10->Add(m_width, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsWStrings;
    m_unitsWStrings.Add(_("px"));
    m_unitsWStrings.Add(_("cm"));
    m_unitsW = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_UNITS_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsWStrings, wxCB_READONLY );
    m_unitsW->SetStringSelection(_("px"));
    m_unitsW->SetHelpText(_("Units for the image width."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsW->SetToolTip(_("Units for the image width."));
    itemBoxSizer10->Add(m_unitsW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer14, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_height = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_height->SetHelpText(_("The image height to be shown - does not change the source image height."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_height->SetToolTip(_("The image height to be shown - does not change the source image height."));
    itemBoxSizer14->Add(m_height, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsHStrings;
    m_unitsHStrings.Add(_("px"));
    m_unitsHStrings.Add(_("cm"));
    m_unitsH = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_UNITS_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsHStrings, wxCB_READONLY );
    m_unitsH->SetStringSelection(_("px"));
    m_unitsH->SetHelpText(_("Units for the image height."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsH->SetToolTip(_("Units for the image height."));
    itemBoxSizer14->Add(m_unitsH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText17 = new wxStaticText( itemDialog1, wxID_STATIC, _("Image Vertical &Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer18, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_offset = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_offset->SetMaxLength(10);
    m_offset->SetHelpText(_("The vertical offset relative to the paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_offset->SetToolTip(_("The vertical offset relative to the paragraph."));
    itemBoxSizer18->Add(m_offset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsOffsetStrings;
    m_unitsOffsetStrings.Add(_("px"));
    m_unitsOffsetStrings.Add(_("cm"));
    m_unitsOffset = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsOffsetStrings, wxCB_READONLY );
    m_unitsOffset->SetStringSelection(_("px"));
    m_unitsOffset->SetHelpText(_("Units for the image offset."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsOffset->SetToolTip(_("Units for the image offset."));
    itemBoxSizer18->Add(m_unitsOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText21 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Move the image to:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText21, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer22, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    wxButton* itemButton23 = new wxButton( itemDialog1, ID_RICHTEXTIMAGEDIALOG_PARA_UP, _("&Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton23->SetHelpText(_("Moves the image to the previous paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        itemButton23->SetToolTip(_("Moves the image to the previous paragraph."));
    itemBoxSizer22->Add(itemButton23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton24 = new wxButton( itemDialog1, ID_RICHTEXTIMAGEDIALOG_DOWN, _("&Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton24->SetHelpText(_("Moves the image to the next paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        itemButton24->SetToolTip(_("Moves the image to the next paragraph."));
    itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine25 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(itemStaticLine25, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer26 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer26, 0, wxGROW|wxALL, 5);
    m_saveButton = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_saveButton->SetHelpText(_("Click to confirm your changes."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_saveButton->SetToolTip(_("Click to confirm your changes."));
    itemStdDialogButtonSizer26->AddButton(m_saveButton);

    m_cancelButton = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cancelButton->SetHelpText(_("Click to discard your changes."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_cancelButton->SetToolTip(_("Click to discard your changes."));
    itemStdDialogButtonSizer26->AddButton(m_cancelButton);

    itemStdDialogButtonSizer26->Realize();

////@end wxRichTextImageDialog content construction
}


/*!
 * Should we show tooltips?
 */

bool wxRichTextImageDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextImageDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextImageDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextImageDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextImageDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextImageDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextImageDialog icon retrieval
}

/*!
 * Set the image attribute
 */
void wxRichTextImageDialog::SetImageAttr(const wxRichTextAnchoredObjectAttr& attr)
{
   m_attr = attr;
   TransferDataToWindow();
}

/*!
 * Apply the new style
 */
wxRichTextImage* wxRichTextImageDialog::ApplyImageAttr()
{
    wxRichTextImage* image = wxDynamicCast(m_image, wxRichTextImage);

    TransferDataFromWindow();
    if (m_buffer->GetRichTextCtrl())
    {
        m_buffer->GetRichTextCtrl()->SetImageStyle(image, m_attr);
    }
    return image;
}

void wxRichTextImageDialog::SetImageObject(wxRichTextImage* image, wxRichTextBuffer* buffer)
{
    wxRichTextObject* parent = image->GetParent();

    m_buffer = buffer;
    m_image = image;
    m_parent = parent;
    SetImageAttr(image->GetAnchoredAttr());
    if (image->GetImageCache().IsOk())
    {
        if (m_attr.m_width == -1)
        {
            m_attr.m_width = image->GetImageCache().GetWidth();
            wxASSERT(m_attr.m_unitsW == wxRICHTEXT_PX);

            m_attr.m_unitsW = wxRICHTEXT_PX;
        }
        if (m_attr.m_height == -1)
        {
            m_attr.m_height = image->GetImageCache().GetHeight();
            wxASSERT(m_attr.m_unitsH == wxRICHTEXT_PX);

            m_attr.m_unitsH = wxRICHTEXT_PX;
        }
    }
}

bool wxRichTextImageDialog::TransferDataToWindow()
{
    int remain = 100;

    m_alignment->SetSelection(m_attr.m_align);
    m_float->SetSelection(m_attr.m_floating);

    // Update scale
    m_unitsW->SetSelection(m_attr.m_unitsW);
    m_unitsH->SetSelection(m_attr.m_unitsH);
    m_unitsOffset->SetSelection(m_attr.m_unitsOffset);

    // Update metric
    m_width->Clear();
    if (m_attr.m_unitsW == wxRICHTEXT_MM)
    {
        int remainder = m_attr.m_width % remain;
        *m_width << m_attr.m_width / remain;
        if (remainder)
        {
            *m_width << '.' << remainder;
        }
    }
    else
    {
        *m_width << m_attr.m_width;
    }

    m_height->Clear();
    if (m_attr.m_unitsH == wxRICHTEXT_MM)
    {
        int remainder = m_attr.m_height % remain;
        *m_height << m_attr.m_height / remain;
        if (remainder)
        {
            *m_height << '.' << remainder;
        }
    }
    else
    {
        *m_height << m_attr.m_height;
    }

    m_offset->Clear();
    if (m_attr.m_unitsOffset == wxRICHTEXT_MM)
    {
        int remainder = m_attr.m_offset % remain;
        *m_offset << m_attr.m_offset / remain;
        if (remainder)
        {
            *m_offset << '.' << remainder;
        }
    }
    else
    {
        *m_offset << m_attr.m_offset;
    }

    return true;
}

bool wxRichTextImageDialog::TransferDataFromWindow()
{
    wxString width = m_width->GetValue();
    wxString height = m_height->GetValue();
    wxString offset = m_offset->GetValue();
    int w, h, o;

    m_attr.m_align = m_alignment->GetSelection();
    m_attr.m_floating = m_float->GetSelection();

    m_attr.m_unitsW = m_unitsW->GetSelection();
    m_attr.m_unitsH = m_unitsH->GetSelection();
    m_attr.m_unitsOffset = m_unitsOffset->GetSelection();

    if (ConvertFromString(width, w, m_attr.m_unitsW))
        m_attr.m_width = w;
    if (ConvertFromString(height, h, m_attr.m_unitsH))
        m_attr.m_height = h;
    if (ConvertFromString(offset, o, m_attr.m_unitsOffset))
        m_attr.m_offset = o;

    return true;
}

bool wxRichTextImageDialog::ConvertFromString(const wxString& string, int& ret, int scale)
{
    const wxChar* chars = string.GetData();
    int remain = 2;
    bool dot = false;
    ret = 0;

    for (unsigned int i = 0; i < string.Len() && remain; i++)
    {
        if (!(chars[i] >= '0' && chars[i] <= '9') && !(scale == wxRICHTEXT_MM && chars[i] == '.'))
            return false;

        if (chars[i] == '.')
        {
            dot = true;
            continue;
        }

        if (dot)
            remain--;

        ret = ret * 10 + chars[i] - '0';
    }

    while (remain-- > 0 && scale == wxRICHTEXT_MM)
        ret *= 10;

    return true;
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_UP
 */
void wxRichTextImageDialog::OnRichtextimagedialogParaUpClick( wxCommandEvent& WXUNUSED(event))
{
    // Before editing this code, remove the block markers.
    wxRichTextRange range = m_image->GetRange();
    wxRichTextObjectList::compatibility_iterator iter = m_buffer->GetChildren().GetFirst();
    if (iter == NULL)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetPrevious();
    if (iter == NULL)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    m_image = m_image->Clone();

    m_buffer->DeleteRangeWithUndo(range, m_buffer->GetRichTextCtrl());
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_image, m_buffer->GetRichTextCtrl(), 0);
    m_parent = obj;
    m_image->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN
 */

void wxRichTextImageDialog::OnRichtextimagedialogDownClick( wxCommandEvent& WXUNUSED(event))
{
    // Before editing this code, remove the block markers.
    wxRichTextRange range = m_image->GetRange();
    wxRichTextObjectList::compatibility_iterator iter = m_buffer->GetChildren().GetFirst();
    if (iter == NULL)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetNext();
    if (iter == NULL)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    m_image = m_image->Clone();

    m_buffer->DeleteRangeWithUndo(range, m_buffer->GetRichTextCtrl());
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_image, m_buffer->GetRichTextCtrl(), 0);
    m_parent = obj;
    m_image->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));
}

