/////////////////////////////////////////////////////////////////////////////
// Name:        foldtestpanel.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     18/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "foldtestpanel.h"


////@begin XPM images
////@end XPM images

/*!
 * FoldTestPanel type definition
 */

IMPLEMENT_CLASS( FoldTestPanel, wxPanel )

/*!
 * FoldTestPanel event table definition
 */
BEGIN_EVENT_TABLE( FoldTestPanel, wxPanel )

////@begin FoldTestPanel event table entries
////@end FoldTestPanel event table entries

    //EVT_CAPTIONBAR(wxID_ANY, FoldTestPanel::OnCaptionPanel)
    EVT_CAPTIONBAR(wxID_ANY, FoldTestPanel::OnCaptionPanel)


END_EVENT_TABLE()

/*!
 * FoldTestPanel constructors
 */

FoldTestPanel::FoldTestPanel( )
{
    delete m_images;
}

FoldTestPanel::FoldTestPanel( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * FoldTestPanel creator
 */

bool FoldTestPanel::Create( wxWindow* parent, wxWindowID id, const wxString& WXUNUSED(caption), const wxPoint& pos, const wxSize& size, long style )
{
////@begin FoldTestPanel member initialisation
    blaat = NULL;
////@end FoldTestPanel member initialisation

////@begin FoldTestPanel creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end FoldTestPanel creation
    return true;
}

/*!
 * Control creation for FoldTestPanel
 */

void FoldTestPanel::CreateControls()
{

////@begin FoldTestPanel content construction

    FoldTestPanel* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    blaat = item2;
    item1->SetSizer(item2);
    item1->SetAutoLayout(true);
    /* wxPanel* item3 = new wxPanel( item1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL ); */
    wxPanel* item3 = new wxPanel( item1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    item2->Add(item3, 1, wxGROW|wxADJUST_MINSIZE, 5);
    wxBoxSizer* item4 = new wxBoxSizer(wxVERTICAL);
    item3->SetSizer(item4);
    item3->SetAutoLayout(true);
    wxString item5Strings[] = {
        _("One"),
        _("Two"),
        _("Three")
    };
    wxChoice* item5 = new wxChoice( item3, ID_CHOICE, wxDefaultPosition, wxDefaultSize, 3, item5Strings, 0 );
    item4->Add(item5, 0, wxGROW|wxALL, 5);
    wxTextCtrl* item6 = new wxTextCtrl( item3, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    item4->Add(item6, 1, wxGROW|wxALL, 5);
    wxRadioButton* item7 = new wxRadioButton( item3, ID_RADIOBUTTON, _("I like this"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->SetValue(true);
    item4->Add(item7, 0, wxALIGN_LEFT|wxALL, 5);
    wxRadioButton* item8 = new wxRadioButton( item3, ID_RADIOBUTTON1, _("I hate it"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->SetValue(false);
    item4->Add(item8, 0, wxALIGN_LEFT|wxALL, 5);
////@end FoldTestPanel content construction
}

void FoldTestPanel::OnCaptionPanel(wxCaptionBarEvent &WXUNUSED(event))
{
    // TODO: What else
}

/*!
 * Should we show tooltips?
 */

bool FoldTestPanel::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap FoldTestPanel::GetBitmapResource( const wxString& WXUNUSED(name) )
{
    // Bitmap retrieval
////@begin FoldTestPanel bitmap retrieval
    return wxNullBitmap;
////@end FoldTestPanel bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon FoldTestPanel::GetIconResource( const wxString& WXUNUSED(name) )
{
    // Icon retrieval
////@begin FoldTestPanel icon retrieval
    return wxNullIcon;
////@end FoldTestPanel icon retrieval
}

