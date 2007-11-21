/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
// Created:     27/06/2004
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

#include "test.h"

////@begin XPM images
////@end XPM images

/*!
 * TestTest type definition
 */

IMPLEMENT_CLASS( TestTest, wxDialog )

/*!
 * TestTest event table definition
 */

BEGIN_EVENT_TABLE( TestTest, wxDialog )

////@begin TestTest event table entries
////@end TestTest event table entries

END_EVENT_TABLE()

/*!
 * TestTest constructors
 */

TestTest::TestTest( )
{
}

TestTest::TestTest( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * TestTest creator
 */

bool TestTest::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TestTest member initialisation
    blaat = NULL;
    blaat = NULL;
////@end TestTest member initialisation

////@begin TestTest creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end TestTest creation
    return true;
}

/*!
 * Control creation for TestTest
 */

void TestTest::CreateControls()
{
////@begin TestTest content construction

    TestTest* item1 = this;

    wxFlexGridSizer* item2 = new wxFlexGridSizer(2, 1, 0, 0);
    item2->AddGrowableRow(0);
    item2->AddGrowableCol(0);
    item1->SetSizer(item2);
    item1->SetAutoLayout(true);
    wxPanel* item3 = new wxPanel( item1, ID_PANEL7, wxDefaultPosition, wxSize(100, 50), wxNO_BORDER|wxTAB_TRAVERSAL );
    item2->Add(item3, 1, wxGROW|wxGROW|wxADJUST_MINSIZE, 5);
    wxBoxSizer* item4 = new wxBoxSizer(wxVERTICAL);
    item3->SetSizer(item4);
    item3->SetAutoLayout(true);
    wxPanel* item5 = new wxPanel( item3, ID_PANEL6, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
    item4->Add(item5, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    wxFlexGridSizer* item6 = new wxFlexGridSizer(2, 1, 0, 0);
    item6->AddGrowableRow(1);
    item6->AddGrowableCol(0);
    item5->SetSizer(item6);
    item5->SetAutoLayout(true);
    wxStaticText* item7 = new wxStaticText( item5, wxID_STATIC, _("Static text"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    wxPanel* item8 = new wxPanel( item5, ID_PANEL3, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item6->Add(item8, 1, wxGROW|wxGROW|wxALL, 5);
    wxBoxSizer* item9 = new wxBoxSizer(wxVERTICAL);
    blaat = item9;
    item8->SetSizer(item9);
    item8->SetAutoLayout(true);
    wxPanel* item10 = new wxPanel( item8, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
    item9->Add(item10, 1, wxGROW, 5);
    wxBoxSizer* item11 = new wxBoxSizer(wxVERTICAL);
    item10->SetSizer(item11);
    item10->SetAutoLayout(true);
    wxString item12Strings[] = {
        _("One"),
        _("Two"),
        _("Three")
    };
    wxChoice* item12 = new wxChoice( item10, ID_CHOICE, wxDefaultPosition, wxDefaultSize, 3, item12Strings, 0 );
    item11->Add(item12, 0, wxGROW|wxALL, 5);
    wxRadioButton* item13 = new wxRadioButton( item10, ID_RADIOBUTTON, _("I like this"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->SetValue(true);
    item11->Add(item13, 0, wxALIGN_LEFT|wxALL, 5);
    wxRadioButton* item14 = new wxRadioButton( item10, ID_RADIOBUTTON1, _("I hate it"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->SetValue(false);
    item11->Add(item14, 0, wxALIGN_LEFT|wxALL, 5);
    wxPanel* item15 = new wxPanel( item3, ID_PANEL2, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
    item4->Add(item15, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    wxFlexGridSizer* item16 = new wxFlexGridSizer(2, 1, 0, 0);
    item16->AddGrowableRow(1);
    item16->AddGrowableCol(0);
    item15->SetSizer(item16);
    item15->SetAutoLayout(true);
    wxStaticText* item17 = new wxStaticText( item15, wxID_STATIC, _("Static text"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item17, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL|wxADJUST_MINSIZE|wxFIXED_MINSIZE, 5);
    wxPanel* item18 = new wxPanel( item15, ID_PANEL4, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item16->Add(item18, 0, wxGROW|wxGROW|wxALL, 5);
    wxBoxSizer* item19 = new wxBoxSizer(wxVERTICAL);
    blaat = item19;
    item18->SetSizer(item19);
    item18->SetAutoLayout(true);
    wxPanel* item20 = new wxPanel( item18, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
    item19->Add(item20, 1, wxGROW, 5);
    wxBoxSizer* item21 = new wxBoxSizer(wxVERTICAL);
    item20->SetSizer(item21);
    item20->SetAutoLayout(true);
    wxString item22Strings[] = {
        _("One"),
        _("Two"),
        _("Three")
    };
    wxChoice* item22 = new wxChoice( item20, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 3, item22Strings, 0 );
    item21->Add(item22, 0, wxGROW|wxALL, 5);
    wxRadioButton* item23 = new wxRadioButton( item20, ID_RADIOBUTTON2, _("I like this"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->SetValue(true);
    item21->Add(item23, 0, wxALIGN_LEFT|wxALL, 5);
    wxRadioButton* item24 = new wxRadioButton( item20, ID_RADIOBUTTON3, _("I hate it"), wxDefaultPosition, wxDefaultSize, 0 );
    item24->SetValue(false);
    item21->Add(item24, 0, wxALIGN_LEFT|wxALL, 5);
    wxPanel* item25 = new wxPanel( item1, ID_PANEL1, wxDefaultPosition, wxSize(100, 20), wxNO_BORDER|wxTAB_TRAVERSAL );
    item25->SetBackgroundColour(wxColour(98, 98, 98));
    item2->Add(item25, 0, wxGROW|wxGROW|wxFIXED_MINSIZE, 5);
////@end TestTest content construction
}

/*!
 * Should we show tooltips?
 */

bool TestTest::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TestTest::GetBitmapResource( const wxString& WXUNUSED(name) )
{
    // Bitmap retrieval
////@begin TestTest bitmap retrieval
    return wxNullBitmap;
////@end TestTest bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TestTest::GetIconResource( const wxString& WXUNUSED(name) )
{
    // Icon retrieval
////@begin TestTest icon retrieval
    return wxNullIcon;
////@end TestTest icon retrieval
}
