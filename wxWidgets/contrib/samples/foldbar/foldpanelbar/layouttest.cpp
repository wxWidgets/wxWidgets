/////////////////////////////////////////////////////////////////////////////
// Name:        layouttest.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
// Created:     25/06/2004
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

#include "layouttest.h"

////@begin XPM images
////@end XPM images

/*!
 * LayoutTest type definition
 */

IMPLEMENT_CLASS( LayoutTest, wxPanel )

/*!
 * LayoutTest event table definition
 */

BEGIN_EVENT_TABLE( LayoutTest, wxPanel )

////@begin LayoutTest event table entries
////@end LayoutTest event table entries

END_EVENT_TABLE()

/*!
 * LayoutTest constructors
 */

LayoutTest::LayoutTest( )
{
}

LayoutTest::LayoutTest( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * LayoutTest creator
 */

bool LayoutTest::Create( wxWindow* parent, wxWindowID id, const wxString& WXUNUSED(caption), const wxPoint& pos, const wxSize& size, long style )
{
////@begin LayoutTest member initialisation
////@end LayoutTest member initialisation

////@begin LayoutTest creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end LayoutTest creation
    return true;
}

/*!
 * Control creation for LayoutTest
 */

void LayoutTest::CreateControls()
{
////@begin LayoutTest content construction

    LayoutTest* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(true);
    wxStaticText* item3 = new wxStaticText( item1, wxID_STATIC, _("lbaaaaaa"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->SetBackgroundColour(wxColour(139, 139, 139));
    item2->Add(item3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    wxPanel* item4 = new wxPanel( item1, ID_PANEL1, wxDefaultPosition, wxSize(100, 80), wxTAB_TRAVERSAL );
    item2->Add(item4, 0, wxGROW, 5);
    wxBoxSizer* item5 = new wxBoxSizer(wxVERTICAL);
    item4->SetSizer(item5);
    item4->SetAutoLayout(true);
    wxStaticText* item6 = new wxStaticText( item4, wxID_STATIC, _("Static text"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add(item6, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);
    wxButton* item7 = new wxButton( item4, ID_BUTTON, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add(item7, 0, wxALIGN_LEFT|wxALL, 5);
////@end LayoutTest content construction
}

/*!
 * Should we show tooltips?
 */

bool LayoutTest::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LayoutTest::GetBitmapResource( const wxString& WXUNUSED(name) )
{
    // Bitmap retrieval
////@begin LayoutTest bitmap retrieval
    return wxNullBitmap;
////@end LayoutTest bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LayoutTest::GetIconResource( const wxString& WXUNUSED(name) )
{
    // Icon retrieval
////@begin LayoutTest icon retrieval
    return wxNullIcon;
////@end LayoutTest icon retrieval
}
