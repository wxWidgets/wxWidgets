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

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Alignment:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_alignmentStrings;
    m_alignmentStrings.Add(_("Left"));
    m_alignmentStrings.Add(_("Centre"));
    m_alignmentStrings.Add(_("Right"));
    m_alignment = new wxComboBox( itemDialog1, ID_COMBOBOX_ALIGN, _("Left"), wxDefaultPosition, wxSize(80, -1), m_alignmentStrings, wxCB_READONLY );
    m_alignment->SetStringSelection(_("Left"));
    itemBoxSizer6->Add(m_alignment, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Floating:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_floatStrings;
    m_floatStrings.Add(_("None"));
    m_floatStrings.Add(_("Left"));
    m_floatStrings.Add(_("Right"));
    m_float = new wxComboBox( itemDialog1, ID_COMBOBOX, _("None"), wxDefaultPosition, wxSize(80, -1), m_floatStrings, wxCB_READONLY );
    m_float->SetStringSelection(_("None"));
    itemBoxSizer9->Add(m_float, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer12->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer14->Add(itemBoxSizer15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText16 = new wxStaticText( itemDialog1, wxID_STATIC, _("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_width = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(m_width, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_scaleWStrings;
    m_scaleWStrings.Add(_("px"));
    m_scaleWStrings.Add(_("cm"));
    m_scaleW = new wxComboBox( itemDialog1, ID_COMBOBOX_SCALE_W, _("px"), wxDefaultPosition, wxSize(60, -1), m_scaleWStrings, wxCB_DROPDOWN );
    m_scaleW->SetStringSelection(_("px"));
    itemBoxSizer15->Add(m_scaleW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer14->Add(itemBoxSizer19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemDialog1, wxID_STATIC, _("Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_height = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(m_height, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_scaleHStrings;
    m_scaleHStrings.Add(_("px"));
    m_scaleHStrings.Add(_("cm"));
    m_scaleH = new wxComboBox( itemDialog1, ID_COMBOBOX_SCALE_H, _("px"), wxDefaultPosition, wxSize(60, -1), m_scaleHStrings, wxCB_DROPDOWN );
    m_scaleH->SetStringSelection(_("px"));
    itemBoxSizer19->Add(m_scaleH, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer23, 0, wxGROW|wxALL, 5);

    itemBoxSizer23->Add(5, 5, 10, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_saveButton = new wxButton( itemDialog1, ID_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
    m_saveButton->SetDefault();
    itemBoxSizer23->Add(m_saveButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_cancelButton = new wxButton( itemDialog1, ID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer23->Add(m_cancelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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
