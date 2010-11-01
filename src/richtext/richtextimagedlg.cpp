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

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Floating mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_width = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_WIDTH, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_width->SetHelpText(_("The image width to be shown - does not change the source image width."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_width->SetToolTip(_("The image width to be shown - does not change the source image width."));
    itemBoxSizer8->Add(m_width, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsWStrings;
    m_unitsWStrings.Add(_("px"));
    m_unitsWStrings.Add(_("cm"));
    m_unitsW = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_UNITS_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsWStrings, wxCB_READONLY );
    m_unitsW->SetStringSelection(_("px"));
    m_unitsW->SetHelpText(_("Units for the image width."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsW->SetToolTip(_("Units for the image width."));
    itemBoxSizer8->Add(m_unitsW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_height = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_HEIGHT, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_height->SetHelpText(_("The image height to be shown - does not change the source image height."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_height->SetToolTip(_("The image height to be shown - does not change the source image height."));
    itemBoxSizer12->Add(m_height, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsHStrings;
    m_unitsHStrings.Add(_("px"));
    m_unitsHStrings.Add(_("cm"));
    m_unitsH = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_UNITS_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsHStrings, wxCB_READONLY );
    m_unitsH->SetStringSelection(_("px"));
    m_unitsH->SetHelpText(_("Units for the image height."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsH->SetToolTip(_("Units for the image height."));
    itemBoxSizer12->Add(m_unitsH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText15 = new wxStaticText( itemDialog1, wxID_STATIC, _("Image Vertical &Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer16, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_offset = new wxTextCtrl( itemDialog1, ID_RICHTEXTIMAGEDIALOG_OFFSET, wxEmptyString, wxDefaultPosition, wxSize(65, -1), 0 );
    m_offset->SetMaxLength(10);
    m_offset->SetHelpText(_("The vertical offset relative to the paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_offset->SetToolTip(_("The vertical offset relative to the paragraph."));
    itemBoxSizer16->Add(m_offset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_unitsOffsetStrings;
    m_unitsOffsetStrings.Add(_("px"));
    m_unitsOffsetStrings.Add(_("cm"));
    m_unitsOffset = new wxComboBox( itemDialog1, ID_RICHTEXTIMAGEDIALOG_OFFSET_UNITS, _("px"), wxDefaultPosition, wxSize(60, -1), m_unitsOffsetStrings, wxCB_READONLY );
    m_unitsOffset->SetStringSelection(_("px"));
    m_unitsOffset->SetHelpText(_("Units for the image offset."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_unitsOffset->SetToolTip(_("Units for the image offset."));
    itemBoxSizer16->Add(m_unitsOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Move the image to:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText19, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer4->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    wxButton* itemButton21 = new wxButton( itemDialog1, ID_RICHTEXTIMAGEDIALOG_PARA_UP, _("&Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton21->SetHelpText(_("Moves the image to the previous paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        itemButton21->SetToolTip(_("Moves the image to the previous paragraph."));
    itemBoxSizer20->Add(itemButton21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton22 = new wxButton( itemDialog1, ID_RICHTEXTIMAGEDIALOG_DOWN, _("&Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton22->SetHelpText(_("Moves the image to the next paragraph."));
    if (wxRichTextImageDialog::ShowToolTips())
        itemButton22->SetToolTip(_("Moves the image to the next paragraph."));
    itemBoxSizer20->Add(itemButton22, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine23 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(itemStaticLine23, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer24 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer24, 0, wxGROW|wxALL, 5);
    m_saveButton = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_saveButton->SetHelpText(_("Click to confirm your changes."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_saveButton->SetToolTip(_("Click to confirm your changes."));
    itemStdDialogButtonSizer24->AddButton(m_saveButton);

    m_cancelButton = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cancelButton->SetHelpText(_("Click to discard your changes."));
    if (wxRichTextImageDialog::ShowToolTips())
        m_cancelButton->SetToolTip(_("Click to discard your changes."));
    itemStdDialogButtonSizer24->AddButton(m_cancelButton);

    itemStdDialogButtonSizer24->Realize();

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
void wxRichTextImageDialog::SetImageAttr(const wxRichTextAttr& textAttr)
{
   m_textAttr = textAttr;

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
        m_buffer->GetRichTextCtrl()->SetImageStyle(image, m_textAttr);
    }
    return image;
}

void wxRichTextImageDialog::SetImageObject(wxRichTextImage* image, wxRichTextBuffer* buffer)
{
    wxRichTextObject* parent = image->GetParent();

    m_buffer = buffer;
    m_image = image;
    m_parent = parent;
    SetImageAttr(image->GetAttributes());
    if (image->GetImageCache().IsOk())
    {
        if (!m_textAttr.GetTextBoxAttr().GetWidth().IsPresent() || m_textAttr.GetTextBoxAttr().GetWidth().GetValue() <= 0)
        {
            m_textAttr.GetTextBoxAttr().GetWidth().SetValue(image->GetImageCache().GetWidth());
            m_textAttr.GetTextBoxAttr().GetWidth().SetUnits(wxTEXT_ATTR_UNITS_PIXELS);
        }
        if (!m_textAttr.GetTextBoxAttr().GetHeight().IsPresent() || m_textAttr.GetTextBoxAttr().GetHeight().GetValue() <= 0)
        {
            m_textAttr.GetTextBoxAttr().GetHeight().SetValue(image->GetImageCache().GetHeight());
            m_textAttr.GetTextBoxAttr().GetHeight().SetUnits(wxTEXT_ATTR_UNITS_PIXELS);
        }
    }
}

void wxRichTextImageDialog::SetDimensionValue(wxTextAttrDimension& dim, wxTextCtrl* valueCtrl, wxComboBox* unitsCtrl)
{
    int unitsIdx = 0;
    
    if (!dim.IsPresent())
    {
        dim.SetValue(0);
        dim.SetUnits(wxTEXT_ATTR_UNITS_PIXELS);
    }

    if (dim.GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
    {
        unitsIdx = 1;
        float value = float(dim.GetValue()) / 10.0;
        valueCtrl->SetValue(wxString::Format(wxT("%.2f"), value));
    }
    else
    {
        unitsIdx = 0;
        valueCtrl->SetValue(wxString::Format(wxT("%d"), (int) dim.GetValue()));
    }

    unitsCtrl->SetSelection(unitsIdx);
}

void wxRichTextImageDialog::GetDimensionValue(wxTextAttrDimension& dim, wxTextCtrl* valueCtrl, wxComboBox* unitsCtrl)
{
    if (unitsCtrl->GetSelection() == 1)
        dim.SetUnits(wxTEXT_ATTR_UNITS_TENTHS_MM);
    else
        dim.SetUnits(wxTEXT_ATTR_UNITS_PIXELS);

    int value = 0;
    if (ConvertFromString(valueCtrl->GetValue(), value, dim.GetUnits()))
        dim.SetValue(value);
}

bool wxRichTextImageDialog::TransferDataToWindow()
{
    m_float->SetSelection(m_textAttr.GetTextBoxAttr().GetFloatMode());

    SetDimensionValue(m_textAttr.GetTextBoxAttr().GetWidth(), m_width, m_unitsW);
    SetDimensionValue(m_textAttr.GetTextBoxAttr().GetHeight(), m_height, m_unitsH);    
    SetDimensionValue(m_textAttr.GetTextBoxAttr().GetTop(), m_offset, m_unitsOffset);    

    return true;
}

bool wxRichTextImageDialog::TransferDataFromWindow()
{
    m_textAttr.GetTextBoxAttr().SetFloatMode(m_float->GetSelection());

    GetDimensionValue(m_textAttr.GetTextBoxAttr().GetWidth(), m_width, m_unitsW);
    GetDimensionValue(m_textAttr.GetTextBoxAttr().GetHeight(), m_height, m_unitsH);    
    GetDimensionValue(m_textAttr.GetTextBoxAttr().GetTop(), m_offset, m_unitsOffset);    

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
        if (!(chars[i] >= '0' && chars[i] <= '9') && !(scale == wxTEXT_ATTR_UNITS_TENTHS_MM && chars[i] == '.'))
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

    while (remain-- > 0 && scale == wxTEXT_ATTR_UNITS_TENTHS_MM)
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
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetPrevious();
    if (!iter)
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
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetNext();
    if (!iter)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    m_image = m_image->Clone();

    m_buffer->DeleteRangeWithUndo(range, m_buffer->GetRichTextCtrl());
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_image, m_buffer->GetRichTextCtrl(), 0);
    m_parent = obj;
    m_image->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));
}

