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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx/richtext/richtextimagedlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextctrl.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextImageDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextImageDlg, wxDialog )


/*!
 * wxRichTextImageDlg event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextImageDlg, wxDialog )

////@begin wxRichTextImageDlg event table entries
    EVT_BUTTON( ID_BUTTON_PARA_UP, wxRichTextImageDlg::OnButtonParaUpClick )

    EVT_BUTTON( ID_BUTTON_PARA_DOWN, wxRichTextImageDlg::OnButtonParaDownClick )

////@end wxRichTextImageDlg event table entries

END_EVENT_TABLE()


/*!
 * wxRichTextImageDlg constructors
 */

wxRichTextImageDlg::wxRichTextImageDlg()
{
    Init();
}

wxRichTextImageDlg::wxRichTextImageDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * wxRichTextImageDlg creator
 */

bool wxRichTextImageDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextImageDlg creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextImageDlg creation
    return true;
}


/*!
 * wxRichTextImageDlg destructor
 */

wxRichTextImageDlg::~wxRichTextImageDlg()
{
////@begin wxRichTextImageDlg destruction
////@end wxRichTextImageDlg destruction
}


/*!
 * Member initialisation
 */

void wxRichTextImageDlg::Init()
{
////@begin wxRichTextImageDlg member initialisation
    m_alignment = NULL;
    m_float = NULL;
    m_width = NULL;
    m_scaleW = NULL;
    m_height = NULL;
    m_scaleH = NULL;
    m_offset = NULL;
    m_scaleOffset = NULL;
    m_saveButton = NULL;
    m_cancelButton = NULL;
////@end wxRichTextImageDlg member initialisation
}


/*!
 * Control creation for wxRichTextImageDlg
 */

void wxRichTextImageDlg::CreateControls()
{    
////@begin wxRichTextImageDlg content construction
    wxRichTextImageDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer3->Add(itemFlexGridSizer4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_alignmentStrings;
    m_alignmentStrings.Add(_("Left"));
    m_alignmentStrings.Add(_("Centre"));
    m_alignmentStrings.Add(_("Right"));
    m_alignment = new wxComboBox( itemDialog1, ID_COMBOBOX_ALIGN, _("Left"), wxDefaultPosition, wxSize(80, -1), m_alignmentStrings, wxCB_READONLY );
    m_alignment->SetStringSelection(_("Left"));
    itemFlexGridSizer4->Add(m_alignment, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Floating:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_floatStrings;
    m_floatStrings.Add(_("None"));
    m_floatStrings.Add(_("Left"));
    m_floatStrings.Add(_("Right"));
    m_float = new wxComboBox( itemDialog1, ID_COMBOBOX, _("None"), wxDefaultPosition, wxSize(80, -1), m_floatStrings, wxCB_READONLY );
    m_float->SetStringSelection(_("None"));
    itemFlexGridSizer4->Add(m_float, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer9 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer3->Add(itemFlexGridSizer9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_width = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_width, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_scaleWStrings;
    m_scaleWStrings.Add(_("px"));
    m_scaleWStrings.Add(_("cm"));
    m_scaleW = new wxComboBox( itemDialog1, ID_COMBOBOX_SCALE_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_scaleWStrings, wxCB_READONLY );
    m_scaleW->SetStringSelection(_("px"));
    itemFlexGridSizer9->Add(m_scaleW, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemDialog1, wxID_STATIC, _("Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(itemStaticText13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_height = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_height, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_scaleHStrings;
    m_scaleHStrings.Add(_("px"));
    m_scaleHStrings.Add(_("cm"));
    m_scaleH = new wxComboBox( itemDialog1, ID_COMBOBOX_SCALE_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_scaleHStrings, wxCB_READONLY );
    m_scaleH->SetStringSelection(_("px"));
    itemFlexGridSizer9->Add(m_scaleH, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer17 = new wxFlexGridSizer(1, 3, 0, 0);
    itemBoxSizer16->Add(itemFlexGridSizer17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemDialog1, wxID_STATIC, _("Move the image to"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(itemStaticText18, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton19 = new wxButton( itemDialog1, ID_BUTTON_PARA_UP, _("Previous Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(itemButton19, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton20 = new wxButton( itemDialog1, ID_BUTTON_PARA_DOWN, _("Next Paragraph"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(itemButton20, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( itemDialog1, wxID_STATIC, _("Image Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemStaticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_offset = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_OFFSET, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_offset->SetMaxLength(10);
    itemBoxSizer21->Add(m_offset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_scaleOffsetStrings;
    m_scaleOffsetStrings.Add(_("px"));
    m_scaleOffsetStrings.Add(_("cm"));
    m_scaleOffset = new wxComboBox( itemDialog1, ID_COMBOBOX_OFFSET, _("px"), wxDefaultPosition, wxSize(60, -1), m_scaleOffsetStrings, wxCB_READONLY );
    m_scaleOffset->SetStringSelection(_("px"));
    itemBoxSizer21->Add(m_scaleOffset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer21->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer26, 0, wxGROW|wxALL, 5);

    itemBoxSizer26->Add(5, 5, 10, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_saveButton = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_saveButton->SetDefault();
    itemBoxSizer26->Add(m_saveButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_cancelButton = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(m_cancelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxRichTextImageDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool wxRichTextImageDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextImageDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextImageDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextImageDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextImageDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextImageDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextImageDlg icon retrieval
}

/*!
 * Set the image attribute
 */
void wxRichTextImageDlg::SetImageAttr(const wxRichTextImageAttr& attr)
{
   m_attr = attr;
   TransferDataToWindow();
}

/*!
 * Apply the new style
 */
wxRichTextImage* wxRichTextImageDlg::ApplyImageAttr()
{
    wxRichTextImage* image = wxDynamicCast(m_image, wxRichTextImage);

    TransferDataFromWindow();
    if (m_ctrl)
    {
        m_ctrl->SetImageStyle(image, m_attr);
    }
    return image;
}

void wxRichTextImageDlg::SetImageObject(wxRichTextImage* image, wxRichTextBuffer* buffer, wxRichTextCtrl* ctrl)
{
    wxRichTextObject* parent = image->GetParent();

    m_ctrl = ctrl;
    m_buffer = buffer;
    m_image = image;
    m_parent = parent;
    SetImageAttr(image->GetImageAttr());
}

bool wxRichTextImageDlg::TransferDataToWindow()
{
    int remain = 100;

    m_alignment->SetSelection(m_attr.m_align);
    m_float->SetSelection(m_attr.m_floating);

    // Update scale
    m_scaleW->SetSelection(m_attr.m_scaleW);
    m_scaleH->SetSelection(m_attr.m_scaleH);
    m_scaleOffset->SetSelection(m_attr.m_scaleO);

    // Update metric
    m_width->Clear();
    if (m_attr.m_scaleW == wxRICHTEXT_MM)
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
    if (m_attr.m_scaleH == wxRICHTEXT_MM)
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
    if (m_attr.m_scaleO == wxRICHTEXT_MM)
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

bool wxRichTextImageDlg::TransferDataFromWindow()
{
    wxString width = m_width->GetValue();
    wxString height = m_height->GetValue();
    wxString offset = m_offset->GetValue();
    int w, h, o;

    m_attr.m_align = m_alignment->GetSelection();
    m_attr.m_floating = m_float->GetSelection();

    m_attr.m_scaleW = m_scaleW->GetSelection();
    m_attr.m_scaleH = m_scaleH->GetSelection();
    m_attr.m_scaleO = m_scaleOffset->GetSelection();

    if (ConvertFromString(width, w, m_attr.m_scaleW))
        m_attr.m_width = w;
    if (ConvertFromString(height, h, m_attr.m_scaleH))
        m_attr.m_height = h;
    if (ConvertFromString(offset, o, m_attr.m_scaleO))
        m_attr.m_offset = o;

    return true;
}

bool wxRichTextImageDlg::ConvertFromString(const wxString& string, int& ret, int scale)
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
void wxRichTextImageDlg::OnButtonParaUpClick( wxCommandEvent& WXUNUSED(event))
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_UP in wxRichTextImageDlg.
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
    m_image->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));

    m_buffer->DeleteRangeWithUndo(range, m_ctrl);
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_image, m_ctrl, 0);
    m_parent = obj;
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_UP in wxRichTextImageDlg. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN
 */

void wxRichTextImageDlg::OnButtonParaDownClick( wxCommandEvent& WXUNUSED(event))
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN in wxRichTextImageDlg.
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
    m_image->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));

    m_buffer->DeleteRangeWithUndo(range, m_ctrl);
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_image, m_ctrl, 0);
    m_parent = obj;
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN in wxRichTextImageDlg. 
}

